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

#include "RedistributionAlgorithm.h"

#include "AlgorithmHelpers.h"
#include "algorithms/NeighborsDistance2.h"
#include "DBInterface.h"
#include "foreach.h"

#include <kvalobs/kvQCFlagTypes.h>
#include <milog/milog.h>

#include <boost/bind.hpp>

#define NDEBUG
#include "debug.h"

using Helpers::equal;

// ------------------------------------------------------------------------

inline float dry2real(float original)
{ return Helpers::equal(original, -1.0f) ? 0.0f : original; }

// ------------------------------------------------------------------------

inline float real2dry(float real)
{ return Helpers::equal(real, 0.0f) ? -1.0f : real; }

// ------------------------------------------------------------------------

RedistributionAlgorithm::RedistributionAlgorithm()
    : Qc2Algorithm("Redistribute")
    , mNeighbors(new NeighborsDistance2())
{
}

// ------------------------------------------------------------------------

miutil::miTime RedistributionAlgorithm::stepTime(const miutil::miTime& time)
{
    return Helpers::plusDay(time, -1);
}

// ------------------------------------------------------------------------

std::list<int> RedistributionAlgorithm::findNeighbors(int stationID)
{
    if( !mNeighbors->hasStationList() ) {
        std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
        std::list<int> allStationIDs;
        fillStationLists(allStations, allStationIDs);
        mNeighbors->setStationList(allStations);
    }

    return mNeighbors->findNeighbors(stationID);
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::checkEndpoint(const kvalobs::kvData& endpoint)
{
    DBG("endpoint=" << endpoint);
    if( Helpers::isMissingOrRejected(endpoint) ) {
        warning() << "endpoint missing/rejected: " << Helpers::datatext(endpoint);
        return false;
    }
    if( endpoint.obstime().hour() != mMeasurementHour ) {
        warning() << "expected obstime hour " << std::setw(2) << std::setfill('0') << mMeasurementHour
                  << " not seen in endpoint " << Helpers::datatext(endpoint);
        return false;
    }
    if( endpoint.controlinfo().flag(kvQCFlagTypes::f_fmis) == 0 && !equal(endpoint.original(), -1.0f) ) {
        warning() << "fmis=0 and original!=-1 for endpoint " << Helpers::datatext(endpoint);
        return false;
    }
    if( endpoint.controlinfo().flag(kvQCFlagTypes::f_fd) == 2 && !equal(endpoint.original(), endpoint.corrected()) ) {
        warning() << "fd=2 and original!=corrected for endpoint " << Helpers::datatext(endpoint);
        return false;
    }
    if( warn_and_stop_flags.matches(endpoint) ) {
        warning() << "endpoint matches warn_and_stop_flags: " << Helpers::datatext(endpoint);
        return false;
    }
    return true;
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::insertMissingRows(const kvalobs::kvData& endpoint, updateList_t& mdata, const miutil::miTime& beforeTime)
{
    miutil::miTime t = stepTime(endpoint.obstime()), now = miutil::miTime::nowTime();
    for(updateList_it it = mdata.begin(); t > beforeTime; ++it ) {
        const miutil::miTime tdata = (it != mdata.end()) ? it->obstime() : beforeTime;
        DBG("tdata=" << tdata << " t=" << t);
        while( t > tdata ) {
            const RedisUpdate fake(endpoint, t, now, missing, missing, "0000003000002000");
            mdata.insert(it, fake);
            DBG("about to insert missing row " << fake);
            t = stepTime(t);
        }
        t = stepTime(tdata);
    }
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::checkAccumulationPeriod(const updateList_t& mdata)
{
    const int length = mdata.size();
    DBGV(length);
    if( length == 0 ) {
        error() << "zero-length accumulation";
        return false;
    }
    const RedisUpdate& endpoint = mdata.front();
    if( length == 1 ) {
        warning() << "accumulation without missing rows in " << endpoint;
        return false;
    }

    bool stop = false;
    float redistributed_sum = 0.0f;
    int count_fhqc_0 = 0, count_corrected = 0;
    const int endpoint_fd = endpoint.controlinfo().flag(kvQCFlagTypes::f_fd);

    for(updateList_cit it = mdata.begin(); it != mdata.end(); ++it ) {
        const RedisUpdate& m = *it;
        if( m.obstime().hour() != mMeasurementHour ) {
            warning() << "expected obstime hour "  << std::setw(2) << std::setfill('0')
                      << mMeasurementHour << " not found in missing point " << m
                      << " for accumulation ending in " << endpoint.text(m.obstime());
            stop = true;
        }
        const bool m_missOrRej = equal(m.corrected(), missing) || equal(m.corrected(), rejected);
        if( !m_missOrRej ) {
            redistributed_sum = Helpers::round1(redistributed_sum + dry2real(m.corrected()));
            count_corrected += 1;
        }
        const int m_fhqc = m.controlinfo().flag(kvQCFlagTypes::f_fhqc);
        if( m_fhqc == 0 )
            count_fhqc_0 += 1;
        if( it != mdata.begin() ) {
            if( !equal(m.original(), missing) ) {
                warning() << "missing point " << m << " has original!=missing for endpoint "
                          << endpoint.text(m.obstime());
                stop = true;
            }
            const int m_fmis = m.controlinfo().flag(kvQCFlagTypes::f_fmis);
            const int m_fd   = m.controlinfo().flag(kvQCFlagTypes::f_fd  );
            if( m_fd != endpoint_fd ) {
                warning() << "missing point " << m << " has different fd flag than endpoint "
                          << endpoint.text(m.obstime());
                stop = true;
            }
            if( endpoint_fd == 2 && m_fmis != 3 ) {
                warning() << "missing point " << m << " has fmis!=3 while fd=2 for endpoint "
                          << endpoint.text(m.obstime());
                stop = true;
            } else if( (endpoint_fd == 7||endpoint_fd==6) && m_fmis != 1 ) {
                warning() << "missing point " << m << " has fmis!=1 while fd=6/7 for endpoint "
                          << endpoint.text(m.obstime());
                stop = true;
            }
        }
        if( warn_and_stop_flags.matches(m.data()) ) {
            warning() << "missing point " << m << " matches warn_and_stop_flags for accumulation ending in "
                      << endpoint.text(m.obstime());
            stop = true;
        }
    }
    const miutil::miTime acc_start = mdata.back().obstime();
    if( (endpoint_fd == 2 && count_corrected != 1)
        || ((endpoint_fd == 7||endpoint_fd==6) && count_corrected != length) )
    {
        warning() << "found " << count_corrected << ", but expected " << (endpoint_fd == 2 ? 1 : length)
                  << " rows with corrected values for accumulation from "
                  << acc_start << " to endpoint " << endpoint.text(acc_start);
        stop = true;
    }
    if( !equal(redistributed_sum, dry2real(endpoint.original())) ) {
        const bool fix = endpoint_fd == 7 && count_fhqc_0 == length;
        (fix ? info() : warning())
            << "redistributed sum " << redistributed_sum
            << " starting " << acc_start
            << " differs from original in endpoint " << endpoint.text(acc_start)
            << (fix ? "; will try to fix it" : "; will not fix");
        stop = !fix;
    }
    if( count_fhqc_0 != 0 && count_fhqc_0 != length ) {
        warning() << "fhqc mess for accumulation from " << acc_start
                  << " to endpoint " << endpoint;
        stop = true;
    }
    if( !stop && count_fhqc_0 != length ) {
        info() << "stop because fhqc != 0 somewhere for accumulation ending in " << endpoint.text(acc_start);
        stop = true;
    }
    DBGV(stop);
    return !stop;
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::findMissing(const kvalobs::kvData& endpoint, const miutil::miTime& earliest, updateList_t& mdata)
{
    const DBInterface::DataList mdatao
        = database()->findDataOrderObstime(endpoint.stationID(), endpoint.paramID(), endpoint.typeID(), TimeRange(stepTime(earliest), stepTime(endpoint.obstime())));
    DBGV(mdatao.size());
    mdata = updateList_t(mdatao.rbegin(), mdatao.rend());

#ifndef NDEBUG
    DBG("  missingdata.size=" << mdata.size());
    foreach(const RedisUpdate& md, mdata)
        DBG("    missingdata md=" << md);
#endif

    updateList_it it = mdata.begin();
    while(it != mdata.end() && missingpoint_flags.matches(it->data()) )
        ++it;

    if( it == mdata.end() ) {
        info() << "database starts with accumulation ending in " << Helpers::datatext(endpoint);
        return false;
    }
    if( Helpers::isMissingOrRejected(it->data()) ) {
        warning() << "suspicious (missing/rejected) row " << *it
                  << " before endpoint " << Helpers::datatext(endpoint, it->obstime())
                  << "; giving up";
        return false;
    }
    const miutil::miTime beforeTime = it->obstime();
    mdata.erase(it, mdata.end());

    insertMissingRows(endpoint, mdata, beforeTime);

    mdata.push_front(endpoint);

#ifndef NDEBUG
    foreach(const RedisUpdate& md, mdata)
        DBG("  final redistribution series: " << md);
#endif

    return true;
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::getNeighborData(const updateList_t& before, dataList_t& ndata)
{
    const kvalobs::kvData& endpoint = before.front().data();

    const std::list<int> neighbors = findNeighbors(endpoint.stationID());
    if( (int)neighbors.size() < mMinNeighbors ) {
        warning() << "too few neighbor stations for accumulation ending in "
                  << before.front().text(before.back().obstime());
        return false;
    }

    const dataList_t ndata_r = database()->findDataOrderObstime(neighbors, endpoint.paramID(), endpoint.typeID(),
                                                                TimeRange(before.back().obstime(), endpoint.obstime()), neighbor_flags);
    // reverse ordering
    ndata = dataList_t(ndata_r.rbegin(), ndata_r.rend());

    foreach(const kvalobs::kvData& n, ndata) {
        if( n.obstime().hour() != mMeasurementHour ) {
            warning() << "expected obstime hour " << std::setw(2) << std::setfill('0') << mMeasurementHour
                      << " not seen in neighbor " << Helpers::datatext(n)
                      << " for accumulation ending in " << Helpers::datatext(endpoint, n.obstime());
            return false;
        }
    }

    return true;
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    params.getFlagSetCU(endpoint_flags,      "endpoint",          "fmis=[04]&fd=[267]", "");
    params.getFlagSetCU(missingpoint_flags,  "missingpoint",      "fmis=3&fd=2|fmis=1&fd=[67]", "");
    params.getFlagSetCU(neighbor_flags,      "neighbor",          "fd=1", "U2=0");
    params.getFlagSetCU(warn_and_stop_flags, "warn_and_stop",     "never", "");
    params.getFlagChange(update_flagchange,  "update_flagchange", "fd=7;fmis=3->fmis=1");

    mMinNeighbors = params.getParameter<int>("min_neighbors", 1);
    mMaxNeighbors = params.getParameter<int>("max_neighbors", 5);
    mDaysBeforeNoNeighborWarning = params.getParameter<int>("days_before_no_neighbor_warning", 14);
    mDaysBeforeRedistributingZeroesWarning = params.getParameter<int>("days_before_redistributing_zeroes_warning", 14);
    pids = params.getMultiParameter<int>("ParamId");
    tids = params.getMultiParameter<int>("TypeIds");
    mMeasurementHour = params.getParameter<int>("measurement_hour", 6);

    mNeighbors->configure(params);
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::run()
{
    const DBInterface::StationIDList stations = database()->findNorwegianFixedStationIDs();
    const DBInterface::DataList edata
        = database()->findDataOrderStationObstime(stations, pids, tids, TimeRange(UT0, UT1), endpoint_flags);

    int lastStationId = -1;
    miutil::miTime lastObstime = UT0;
    foreach(const kvalobs::kvData& endpoint, edata) {
        if( !checkEndpoint(endpoint) )
            continue;

        const miutil::miTime earliestPossibleMissing = ( endpoint.stationID() != lastStationId ) ? UT0 : lastObstime;
        lastStationId = endpoint.stationID();
        lastObstime   = endpoint.obstime();

        // get series back in time from endpoint to first missing
        updateList_t accumulation;
        if( !findMissing(endpoint, earliestPossibleMissing, accumulation) )
            continue;

        if( !checkAccumulationPeriod(accumulation) )
            continue;

        if( equal(endpoint.original(), -1.0f) ) {
            redistributeBoneDry(accumulation);
        } else {
            if( !redistributePrecipitation(accumulation) )
                continue;
        }

        updateOrInsertData(accumulation);
   }
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::redistributeBoneDry(updateList_t& accumulation)
{
    DBGV(accumulation.size());
    // accumulated value is -1 => nothing to redistribute, all missing values must be dry (-1), too
    foreach(RedisUpdate& a, accumulation) {
        a.corrected(-1)
            .controlinfo(update_flagchange.apply(a.controlinfo()))
            .cfailed("QC2-redist-bonedry", CFAILED_STRING);
    }
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::redistributePrecipitation(updateList_t& before)
{
    DBGV(before.size());
    const float accumulated = before.front().original();
    const bool accumulationIsDry = equal(accumulated, 0.0f);

    dataList_t ndata;
    if( !getNeighborData(before, ndata) && !accumulationIsDry ) {
        DBG("no nieghbor and not dry");
        return false;
    }
    DBGV(ndata.size());

    // neighbor data are in ndata like this:
    // [endpoint] n3_1 n3_2 n3_3
    // [miss 1]   n2_1 n2_2
    // [miss 2]   n1_1 n1_2 n1_4
    // where nT_S has time T and stationid S
    // forward iterating through ndata yields n3_1 n3_2 n3_3 n2_1 n2_2 n1_1 n1_2 n1_4 (backwards in time)

    float weightedNeighborsAccumulated = 0;
    dataList_t::const_iterator itN = ndata.begin();
    foreach(RedisUpdate& b, before) {
        std::vector<kvalobs::kvData> neighbors;
        DBGV(b.obstime());
        DBGV(itN->obstime());
        for( ; itN != ndata.end() && itN->obstime() == b.obstime(); ++itN ) {
            if( dry2real(itN->original()) <= -2.0f )
                continue;
            const double weight = mNeighbors->getWeight(itN->stationID());
            DBGV(weight);
            if( weight > 0 )
                neighbors.push_back(*itN);
        }
        // sort neighbors by decreasing weight
        std::sort(neighbors.begin(), neighbors.end(),
                  boost::bind( &RedistributionNeighbors::getWeight, mNeighbors, boost::bind( &kvalobs::kvData::stationID, _1 ))
                  > boost::bind( &RedistributionNeighbors::getWeight, mNeighbors, boost::bind( &kvalobs::kvData::stationID, _2 )));
        DBGV(neighbors.size());

        std::ostringstream cfailed;
        cfailed << "QC2N";
        float sumWeights = 0.0, sumWeightedValues = 0.0;
        int usedNeighbors = 0, warnNeighbors = 0;
        bool allNeighborsBoneDry = true; // true to assume dry if no neighbors
        foreach(const kvalobs::kvData& neighbor, neighbors) {
            if( !equal(neighbor.original(), -1.0f) )
                allNeighborsBoneDry = false;
            const float neighborValue = dry2real(neighbor.original());
            if( neighborValue > 0.15f )
                b.setHasNeighborsWithPrecipitation();
            const float weight = mNeighbors->getWeight(neighbor.stationID());
            sumWeights        += weight;
            sumWeightedValues += weight * neighborValue;
            usedNeighbors += 1;
            if( weight < 1 )
                warnNeighbors += 1;
            cfailed << "_" << neighbor.stationID();
            if( usedNeighbors >= mMaxNeighbors )
                break;
        }
        b.setHasAllNeighborsBoneDry(allNeighborsBoneDry);
        if( usedNeighbors > 0 && warnNeighbors == usedNeighbors ) {
            info() << "no really good neighbors at obstime=" << b.obstime()
                   << " for accumulation ending in " << before.front().text(before.back().obstime(), false);
        }
        if( usedNeighbors < mMinNeighbors && !accumulationIsDry ) {
            DBG("not enough neighbors");
            const int ageInDays = miutil::miDate::today().julianDay() - b.obstime().date().julianDay();
            const bool doWARN = ageInDays > mDaysBeforeNoNeighborWarning;
            (doWARN ? warning() : info())
                << "not enough good neighbors at t=" << b.obstime()
                << " for accumulation ending in " << before.front().text(before.back().obstime(), false);
            return false;
        }
        const float weightedNeighbors = (sumWeights > 0.0f)
            ? sumWeightedValues/sumWeights : 0.0f;
        weightedNeighborsAccumulated += weightedNeighbors;

        cfailed << ",QC2-redist";
        b.corrected(weightedNeighbors)
            .cfailed(accumulationIsDry && usedNeighbors==0 ? "QC2-redist-dry-no-neighbors" : cfailed.str(), CFAILED_STRING)
            .controlinfo(update_flagchange.apply(b.controlinfo()));
    }
    DBGV(weightedNeighborsAccumulated);

    const float scale = ( weightedNeighborsAccumulated > 0.0f )
        ? accumulated / weightedNeighborsAccumulated : 0.0f;
    if( scale < 0.001f && accumulated > 0.05f ) {
        const int ageInDays = miutil::miDate::today().julianDay() - before.front().obstime().date().julianDay();
        const bool doWARN = ageInDays > mDaysBeforeNoNeighborWarning;
        (doWARN ? warning() : info())
            << "accumulation " << accumulated << " > 0 would be redistributed to zeros for endpoint "
            << before.front().text(before.back().obstime(), false);
        return false;
    }
    float corrected_sum = 0;
    foreach(RedisUpdate& b, before) {
        const float corr = Helpers::round1(scale * b.corrected());
        corrected_sum += corr;
        if( b.hasAllNeighborsBoneDry() )
            b.corrected(-1.0f);
        else
            b.corrected(corr);
    }

    // make sure that sum of re-distributed is the same as the original accumulated value
    float delta = Helpers::round1(corrected_sum - accumulated);
    if( !accumulationIsDry ) {
        foreach(RedisUpdate& b, before) {
            if( fabs(delta) <= 0.05f )
                break;
            const float corr = b.corrected(), threshold = b.hasNeighborsWithPrecipitation() ? (delta+0.1) : 0.05;
            if( corr >= threshold ) {
                const float newCorr = std::max(Helpers::round1(corr - delta), 0.0f);
                b.corrected(newCorr);
                delta = Helpers::round1(delta - (corr - newCorr));
            }
        }
    }
    if( fabs(delta) > 0.05 ) {
        warning() << "could not avoid difference of " << fabs(delta) << " between distributed sum "
                  << " and accumulated value at endpoint=" << before.front().text(before.back().obstime());
    }
    return true;
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::updateOrInsertData(const updateList_t& data)
{
    dataList_t toInsert, toUpdate;
    foreach(const RedisUpdate& du, data) {
        if( du.needsWrite() ) {
            if( du.isNew() )
                toInsert.push_front(du.data());
            else
                toUpdate.push_front(du.data());
        }
    }
    storeData(toUpdate, toInsert);
}
