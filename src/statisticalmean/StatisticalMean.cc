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
#include "DailyValueExtractor.h"
#include "foreach.h"

#include <boost/make_shared.hpp>

#define NDEBUG
#include "debug.h"

StatisticalMean::StatisticalMean()
    : Qc2Algorithm("StatisticalMean")
    , mNeighbors(new NeighborsDistance2())
{
}

// ------------------------------------------------------------------------

std::list<int> StatisticalMean::findNeighbors(int stationID)
{
    if( !mNeighbors->hasStationList() ) {
        DBInterface::StationList   allStations; // actually only stationary norwegian stations
        DBInterface::StationIDList allStationIDs;
        fillStationLists(allStations, allStationIDs);
        mNeighbors->setStationList(allStations);
    }

    return mNeighbors->findNeighbors(stationID);
}

// ------------------------------------------------------------------------

void StatisticalMean::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    params.getFlagSetCU(ok_flags, "ok", "", "U2=0");

    mTolerance = params.getParameter<float>("tolerance", 10.0f);
    mDays      = params.getParameter<int>("days", 30);
    mDaysRequired = params.getParameter<int>("days_required", int(0.9*mDays + 0.5));
    mParamid   = params.getParameter<int>("ParamId");
    mTypeids   = params.getMultiParameter<int>("TypeIds");

    mUT0extended = UT0;
    mUT0extended.addDay(-mDays);

    mNeighbors->configure(params);
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
    typedef std::vector<kvalobs::kvData> dlist_t;
    typedef std::map<int, dlist_t > smap_t;
    smap_t smap;
    foreach(const kvalobs::kvData& d, sdata) {
        if( !Helpers::isMissingOrRejected(d) )
            smap[d.stationID()].push_back(d);
    }
    return smap;
}

// ------------------------------------------------------------------------

StatisticalMean::sdm_t StatisticalMean::findStationDailyMeans()
{
    const smap_t smap = fetchData();

    const miutil::miDate date0 = mUT0extended.date();
    const int day0 = date0.julianDay();
    sdm_t stationDailyMeans;
    DailyValueExtractor dve;

    // calculate daily mean values TODO skip for RR_x
    foreach(const smap_t::value_type& sd, smap) {
        const dlist_t& dl = sd.second;
        for(dlist_t::const_iterator itB = dl.begin(), itE=itB; itB != dl.end(); itB = itE) {
            dve.newDay();
            const int day = itB->obstime().date().julianDay() - day0;
            for( ; itE != dl.end() && (itE->obstime().date().julianDay() - day0) == day; itE++ )
                dve.addObservation(itE->obstime(), itE->original());
            if( dve.isCompleteDay() )
                stationDailyMeans[sd.first].push_back(DayMean(day, dve.value()));
        }
    }

    return stationDailyMeans;
}

// ------------------------------------------------------------------------

StatisticalMean::sd2_t StatisticalMean::findStationMeansPerDay(AccumulatorP accumulator)
{
    const sdm_t stationDailyMeans = findStationDailyMeans();

    sd2_t stationMeansPerDay;

    const miutil::miDate date0 = mUT0extended.date();
    const int day0 = date0.julianDay();

    foreach(const sdm_t::value_type& sd, stationDailyMeans) {
        accumulator->newStation();
        const dm_t& dml = sd.second;
        dm_t::const_iterator tail = dml.begin(), head = tail;

        const int d0 = UT0.date().julianDay() - day0, d1 = UT1.date().julianDay() - day0;
        for(int day=d0; day<=d1; ++day) {
            const int dfront = day - mDays;
            while( head != dml.end() && head->day() <= day ) {
                accumulator->push(head->mean());
                head++;
            }
            while( tail != head && tail->day() <= dfront ) {
                accumulator->pop(tail->mean());
                tail++;
            }
            AccumulatedValueP value = accumulator->value();
            if( value != 0 )
                stationMeansPerDay[sd.first][day] = value;
        }
    }

    return stationMeansPerDay;
}

// ------------------------------------------------------------------------

void StatisticalMean::checkAllMeanValues(CheckerP checker, const sd2_t& stationMeansPerDay)
{
    const miutil::miDate date0 = mUT0extended.date();

    foreach(const sd2_t::value_type& sd, stationMeansPerDay) {
        const int center = sd.first;
        foreach(const dm2_t::value_type& dm, sd.second) {
            const int day = dm.first;

            miutil::miDate date(date0);
            date.addDay(day);
            if( date < UT0.date() )
                continue;

            if( checker->newCenter(center, Helpers::normalisedDayOfYear(date), dm.second) )
                continue;

            std::list<int> neighbors = findNeighbors(center);
            foreach(int n, neighbors) {
                sd2_t::const_iterator itN = stationMeansPerDay.find(n);
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
                warning() << "statistical test failed for station " << center
                          << " for series ending at " << date;
            }
        }
    }
}

// ------------------------------------------------------------------------

void StatisticalMean::run()
{
    AccumulatorP accumulator;
    CheckerP checker;
    if( mParamid == 178 /* PR */ || mParamid == 211 /* TA */ ) {
        accumulator = boost::make_shared<AccumulatorMeanOrSum>(true, mDays, mDaysRequired);
        checker = boost::make_shared<CheckerMeanOrSum>(this, mTolerance);
    } else if( mParamid == 106 /* RR_1 */ || mParamid == 108 /* RR_6 */
               || mParamid == 109 /* RR_12 */ || mParamid == 110 /* RR_24 */ ) {
        accumulator = boost::make_shared<AccumulatorMeanOrSum>(false, mDays, mDaysRequired);
        checker = boost::make_shared<CheckerMeanOrSum>(this, mTolerance);
    } else if( mParamid == 262 /* UU */ || mParamid == 15 /* NN */ || mParamid == 55 /* HL */
               || mParamid == 273 /* VV */ || mParamid == 200 /* QO */ ) {
        accumulator = boost::make_shared<AccumulatorQuartiles>(mDays, mDaysRequired);
        checker = boost::make_shared<CheckerQuartiles>(this, mDays, std::vector<float>(6, mTolerance));
    } else {
        warning() << "Illegal paramid " << mParamid << " in StatisticalMean::run";
        return;
    }

    const sd2_t stationMeansPerDay = findStationMeansPerDay(accumulator);

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
                        acc.push(vPush);
                }
                for(int i=1; i<=365; ++i) {
                    const float vPush = rvpd[i-1];
                    if( vPush != missing )
                        acc.push(vPush);
                    AccumulatedValueP v = acc.value();
                    if( v ) {
                        float mean = boost::static_pointer_cast<AccumulatedFloat>(v)->value;
                        rvpd_mean[i-1] = mean;
                    }
                    const int iPop = (365+i-mDays-1) % 365;
                    const float vPop = rvpd[iPop];
                    if( vPop != missing )
                        acc.pop(vPop);
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
