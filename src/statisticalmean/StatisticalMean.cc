/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: kvalobs-dev@met.no

  This file is part of KVALOBS

  KVALOBS is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  KVALOBS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with KVALOBS; if not, write to the Free Software Foundation Inc.,
  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <boost/version.hpp>
#if BOOST_VERSION >= 104000

#include "StatisticalMean.h"

#include "algorithms/NeighborsDistance2.h"
#include "helpers/AlgorithmHelpers.h"
#include "helpers/timeutil.h"
#include "AccumulatorQuartiles.h"
#include "AccumulatorMeanOrSum.h"
#include "CheckerMeanOrSum.h"
#include "CheckerQuartiles.h"
#include "DayMeanExtractor.h"
#include "MeanFactory.h"
#include "QuartilesFactory.h"
#include "SumFactory.h"
#include "foreach.h"

#ifndef NDEBUG
#define NDEBUG
#endif
#include "debug.h"

StatisticalMean::StatisticalMean()
    : Qc2Algorithm("StatisticalMean")
    , mNeighbors(new NeighborsDistance2())
    , mMeanFactory(new MeanFactory())
    , mQuartilesFactory(new QuartilesFactory())
    , mSumFactory(new SumFactory())
{
}

// ------------------------------------------------------------------------

std::list<int> StatisticalMean::findNeighbors(int stationID)
{
    if( !mNeighbors->hasStationList() ) {
        DBInterface::StationList   allStations;
        DBInterface::StationIDList allStationIDs;
        fillStationLists(allStations, allStationIDs);
        mNeighbors->setStationList(allStations);
    }

    return mNeighbors->findNeighbors(stationID);
}

// ------------------------------------------------------------------------

void StatisticalMean::configure(const AlgorithmConfig& config)
{
    Qc2Algorithm::configure(config);

    config.getFlagSetCU(ok_flags, "ok", "", "U2=0");

    mTolerance = config.getParameter<float>("tolerance", 10.0f);
    mDays      = config.getParameter<int>("days", 30);
    mDaysRequired = config.getParameter<int>("days_required", int(0.9*mDays + 0.5));
    mMaxBadRatePerDay = config.getParameter<float>("max_bad_rate_per_day", 0.15);
    mParamid   = config.getParameter<int>("ParamId");
    mTypeids   = config.getMultiParameter<int>("TypeIds");

    mUT0extended = UT0;
    kvtime::addDays(mUT0extended, -mDays);

    mNeighbors->configure(config);
    mMeanFactory->configure(this, config);
    mSumFactory->configure(this, config);
    mQuartilesFactory->configure(this, config);
}

// ------------------------------------------------------------------------

StatisticalMean::smap_t StatisticalMean::fetchData()
{
    // this fetches all data with this paramid for all stations at
    // once; this might be a lot, but we need all neighbors for each
    // station anyhow
    const DBInterface::StationIDList stationIDs(1, DBInterface::ALL_STATIONS);
    const std::vector<int> paramid(1, mParamid);
    DBInterface::DataList sdata
        = database()->findDataOrderStationObstime(stationIDs, paramid, mTypeids, TimeRange(mUT0extended, UT1), ok_flags);
    DBGV(sdata.size());

    // sort by station; as sdata is ordered by time, data for each
    // station will keep this ordering
    smap_t smap;
    foreach(const kvalobs::kvData& d, sdata) {
        if( !Helpers::isMissingOrRejected(d) )
            smap[Instrument(d)].push_back(d);
    }
    return smap;
}

// ------------------------------------------------------------------------

StatisticalMean::sdm_t StatisticalMean::findStationDailyMeans(DayValueExtractorP dve)
{
    const smap_t smap = fetchData();

    const kvtime::date date0 = mUT0extended.date();
    const int day0 = kvtime::julianDay(date0);
    sdm_t stationDailyMeans;

    // calculate daily mean values TODO skip for RR_x
    foreach(const smap_t::value_type& sd, smap) {
        const dlist_t& dl = sd.second;
        for(dlist_t::const_iterator itB = dl.begin(), itE=itB; itB != dl.end(); itB = itE) {
            dve->newDay();
            const int day = kvtime::julianDay(itB->obstime().date()) - day0;
            int obsCount = 0, badCount = 0;
            for( ; itE != dl.end() && (kvtime::julianDay(itE->obstime().date()) - day0) == day; itE++ ) {
                obsCount += 1;
                if( !ok_flags.matches( *itE ) || !dve->addObservation(itE->obstime(), itE->original()) )
                    badCount += 1;
            }
            if( obsCount>0 && badCount/float(obsCount) < mMaxBadRatePerDay && dve->isCompleteDay() ) {
                DayValueP dv = dve->value();
                dv->setDay(day);
                stationDailyMeans[sd.first].push_back(dv);
            } else {
                kvtime::date d = date0;
                kvtime::addDays(d, day);
                info() << "not enough ok data on " << d << " for statistical checks on " << sd.first;
            }
        }
    }

    return stationDailyMeans;
}

// ------------------------------------------------------------------------

StatisticalMean::sd2_t StatisticalMean::findStationMeansPerDay(DayValueExtractorP dve, AccumulatorP accumulator)
{
    const sdm_t stationDailyMeans = findStationDailyMeans(dve);

    sd2_t stationMeansPerDay;

    const kvtime::date date0 = mUT0extended.date();
    const int day0 = kvtime::julianDay(date0);

    foreach(const sdm_t::value_type& sd, stationDailyMeans) {
        accumulator->newStation();
        const dm_t& dml = sd.second;
        dm_t::const_iterator tail = dml.begin(), head = tail;

        const int d0 = kvtime::julianDay(UT0.date()) - day0, d1 = kvtime::julianDay(UT1.date()) - day0;
        int accumulatedDays = 0;
        for(int day=d0; day<=d1; ++day) {
            const int dfront = day - mDays;
            while( head != dml.end() && (*head)->day() <= day ) {
                accumulator->push(*head);
                accumulatedDays += 1;
                head++;
            }
            while( tail != head && (*tail)->day() <= dfront ) {
                accumulator->pop(*tail);
                accumulatedDays -= 1;
                tail++;
            }
            AccumulatedValueP value = accumulator->value();
            if( accumulatedDays >= mDaysRequired  && value != 0 ) {
                stationMeansPerDay[sd.first][day] = value;
            } else {
                kvtime::date d0 = date0, d1 = date0;
                kvtime::addDays(d0, dfront);
                kvtime::addDays(d1, day);
                warning() << "too few (" << accumulatedDays << ") days with ok data between "
                          << kvtime::iso(d0) << " and " << kvtime::iso(d1) << " for statistical checks on "
                          << sd.first;
            }
        }
    }

    return stationMeansPerDay;
}

namespace {
struct same_station : public std::unary_function<bool, StatisticalMean::sd2_t::value_type> {
    bool operator() (const StatisticalMean::sd2_t::value_type& i) const {
        return i.first.stationid == stationid;
    }
    int stationid;
    same_station(int s) : stationid(s) { }
};
}

// ------------------------------------------------------------------------

void StatisticalMean::checkAllMeanValues(CheckerP checker, const sd2_t& stationMeansPerDay)
{
    const kvtime::date date0 = mUT0extended.date();

    foreach(const sd2_t::value_type& sd, stationMeansPerDay) {
        const Instrument& center = sd.first;
        if (!Helpers::isNorwegianStationId(center.stationid))
            continue;
        foreach(const dm2_t::value_type& dm, sd.second) {
            const int day = dm.first;

            kvtime::date date(date0);
            kvtime::addDays(date, day);
            if( date < UT0.date() )
                continue;

            if( checker->newCenter(center.stationid, Helpers::normalisedDayOfYear(date), dm.second) )
                continue;

            const std::list<int> neighbors = findNeighbors(center.stationid);
            foreach(int n, neighbors) {
                sd2_t::const_iterator itN = std::find_if(stationMeansPerDay.begin(), stationMeansPerDay.end(), same_station(n));
                if( itN == stationMeansPerDay.end() )
                    continue;

                const dm2_t& ndata = itN->second;
                dm2_t::const_iterator itD = ndata.find(day);
                if( itD == ndata.end() )
                    continue;

                if( !checker->checkNeighbor(n, itD->second) )
                    break;
            }
            if( !checker->pass() ) {
                warning() << "statistical test triggered for " << center
                          << " for series ending at " << date;
            }
        }
    }
}

// ------------------------------------------------------------------------

void StatisticalMean::run()
{
    std::shared_ptr<Factory> factory;
    if( mMeanFactory->appliesTo(mParamid) ) {
        factory = mMeanFactory;
    } else if( mSumFactory->appliesTo(mParamid) ) {
        factory = mSumFactory;
    } else if( mQuartilesFactory->appliesTo(mParamid) ) {
        factory = mQuartilesFactory;
    } else {
        warning() << "Illegal paramid " << mParamid << " in StatisticalMean::run";
        return;
    }
    AccumulatorP accumulator = factory->accumulator(mParamid);
    CheckerP checker = factory->checker(mParamid);
    DayValueExtractorP dayValueExtractor = factory->dayValueExtractor(mParamid);

    const sd2_t stationMeansPerDay = findStationMeansPerDay(dayValueExtractor, accumulator);

    checkAllMeanValues(checker, stationMeansPerDay);

    mReferenceKeys.clear();
}

// ------------------------------------------------------------------------

float StatisticalMean::getReferenceValue(int station, int dayOfYear, const std::string& key, bool& valid)
{
    if( dayOfYear<=0 || dayOfYear>365 ) {
        valid = false;
        return missing;
    }

    valid = true;

    if( mParamid == 178 ) // PR
        return 1014;

    // for TA(211), calculate mean value of the last mDays days here;
    // for quartiles and PR, nothing like this needs to be done

    if( mReferenceKeys.find(key) == mReferenceKeys.end() ) {
        DBInterface::reference_value_map_t rvps = database()->findStatisticalReferenceValues(mParamid, key, missing);
        if( mParamid == 211 && key == "ref_value" ) {
            DBInterface::reference_value_map_t rvps_mean;
            AccumulatorMeanOrSum acc(true, mDays, mDaysRequired);
            foreach(DBInterface::reference_value_map_t::value_type s_rv, rvps) {
                const int station = s_rv.first;
                const DBInterface::reference_values_t& rvpd = s_rv.second;
                DBInterface::reference_values_t rvpd_mean(365, missing);

                acc.newStation();
                for(int i=365-mDays+1; i<365; ++i) {
                    const float vPush = rvpd[i-1];
                    if( vPush != missing )
                        acc.push(std::make_shared<DayMean>(vPush));
                }
                for(int i=1; i<=365; ++i) {
                    const float vPush = rvpd[i-1];
                    if( vPush != missing )
                        acc.push(std::make_shared<DayMean>(vPush));
                    AccumulatedValueP v = acc.value();
                    if( v ) {
                        float mean = std::static_pointer_cast<AccumulatedFloat>(v)->value;
                        rvpd_mean[i-1] = mean;
                    }
                    const int iPop = (365+i-mDays-1) % 365;
                    const float vPop = rvpd[iPop];
                    if( vPop != missing )
                        acc.pop(std::make_shared<DayMean>(vPop));
                }
                rvps_mean[station] = rvpd_mean;
            }
            rvps = rvps_mean;
        }
        mReferenceKeys[key] = rvps;
    }
    DBInterface::reference_value_map_t& sr = mReferenceKeys[key];
    DBInterface::reference_value_map_t::const_iterator it = sr.find(station);
    if( it == sr.end() ) {
        valid = false;
        return missing;
    }
    const float value = it->second[dayOfYear-1];
    valid = value != missing;
    return value;
}

#endif /* BOOST_VERSION >= 104000 */
