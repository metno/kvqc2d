/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011 met.no

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
#include "DBConstraints.h"
#include "DBInterface.h"
#include "foreach.h"

#include <milog/milog.h>

#define NDEBUG
#include "debug.h"

namespace C = Constraint;
namespace O = Ordering;
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

bool RedistributionAlgorithm::findMissing(const kvalobs::kvData& endpoint, const miutil::miTime& earliest, updateList_t& mdata)
{
    dataList_t mdatao;
    const C::DBConstraint cMissing = C::SameDevice(endpoint)
        && C::Obstime(stepTime(earliest), stepTime(endpoint.obstime()));
    database()->selectData(mdatao, cMissing, O::Obstime().desc());
    DBGV(cMissing.sql());
    mdata = updateList_t(mdatao.begin(), mdatao.end());

#ifndef NDEBUG
    DBG("  missingdata.size=" << mdata.size());
    foreach(const RedisUpdate& md, mdata)
        DBG("    missingdata md=" << md);
#endif

    updateList_it it = mdata.begin();
    for(; it != mdata.end(); ++it ) {
        const RedisUpdate& m = *it;
        if( m.obstime().hour() != mMeasurementHour ) {
            warning() << "expected obstime hour "  << std::setw(2) << std::setfill('0')
                      << mMeasurementHour << " not found in missing point " << m
                      << " for accumulation ending in " << endpoint;
            return false;
        }
        if( !missingpoint_flags.matches(m.data()) ) {
            DBG("no missingpoint_flags match: " << m);
            break;
        }
        if( warn_and_stop_flags.matches(m.data()) ) {
            warning() << "missing point " << m << " matches warn_and_stop_flags for accumulation ending in "
                      << endpoint;
            return false;
        }

    }
    if( it == mdata.end() ) {
        info() << "database starts with accumulation ending in " << endpoint;
        return false;
    }
    if( equal(it->original(), missing) || equal(it->original(), rejected) ) {
        warning() << "could not find non-missing data point before accumulation ending in " << endpoint
                  << "; candidate is " << *it;
        return false;
    }
    mdata.erase(it, mdata.end());


    miutil::miTime t = stepTime(endpoint.obstime()), now = miutil::miTime::nowTime(), beforeTime = it->obstime();
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

    if( mdata.empty() ) {
        DBG("no missing data rows");
        return false;
    }
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
        warning() << "too few neighbor stations for accumulation ending in " << before.front();
        return false;
    }

    const C::DBConstraint cNeighbors = C::ControlUseinfo(neighbor_flags)
        && C::Paramid(endpoint.paramID()) && C::Typeid(endpoint.typeID())
        && C::Obstime(before.back().obstime(), endpoint.obstime())
        && C::Station(neighbors);
    database()->selectData(ndata, cNeighbors, (O::Obstime().desc(), O::Stationid()));

    foreach(const kvalobs::kvData& n, ndata) {
        if( n.obstime().hour() != mMeasurementHour ) {
            warning() << "expected obstime hour " << std::setw(2) << std::setfill('0') << mMeasurementHour
                      << " not seen in neighbor " << n << " for accumulation ending in " << endpoint;
            return false;
        }
    }

    return true;
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    params.getFlagSetCU(endpoint_flags,      "endpoint",            "fmis=4&fd=2", "");
    params.getFlagSetCU(missingpoint_flags,  "missingpoint",        "fmis=3&fd=2", "");
    params.getFlagSetCU(neighbor_flags,      "neighbor",            "fd=1",        "U2=0");
    params.getFlagSetCU(warn_and_stop_flags, "warn_and_stop_flags", "fhqc=)0(",    "");
    params.getFlagChange(update_flagchange,  "update_flagchange",   "fd=7;fmis=3->fmis=1");

    mMinNeighbors = params.getParameter<int>("min_neighbors", 1);
    mDaysBeforeNoNeighborWarning = params.getParameter<int>("days_before_no_neighbor_warning", 28);
    pids = params.getMultiParameter<int>("ParamId");
    tids = params.getMultiParameter<int>("TypeIds");
    mMeasurementHour = params.getParameter<int>("measurement_hour", 6);

    mNeighbors->configure(params);
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::run()
{
    dataList_t edata;
    const C::DBConstraint cEndpoints = C::ControlUseinfo(endpoint_flags)
            && C::Paramid(pids) && C::Typeid(tids)
            && C::Obstime(UT0, UT1);
    database()->selectData(edata, cEndpoints, (O::Stationid(), O::Obstime().asc()));
    
    int lastStationId = -1;
    miutil::miTime lastObstime = UT0;
    foreach(const kvalobs::kvData& endpoint, edata) {
        if( equal(endpoint.original(), missing) || equal(endpoint.original(), rejected) )
            continue;
        if( endpoint.obstime().hour() != mMeasurementHour ) {
            warning() << "expected obstime hour " << std::setw(2) << std::setfill('0') << mMeasurementHour
                      << " not seen in accumulation endpoint "  << endpoint;
            continue;
        }

        const miutil::miTime earliestPossibleMissing = ( endpoint.stationID() != lastStationId ) ? UT0 : lastObstime;
        lastStationId = endpoint.stationID();
        lastObstime   = endpoint.obstime();

        if( warn_and_stop_flags.matches(endpoint) ) {
            warning() << "endpoint matches warn_and_stop_flags: " << endpoint;
            continue;
        }

        // get series back in time from endpoint to first missing
        updateList_t accumulation;
        if( !findMissing(endpoint, earliestPossibleMissing, accumulation) )
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
    const float accumulated = before.front().original();
    const bool accumulationIsDry = equal(accumulated, 0.0f);

    dataList_t ndata;
    if( !getNeighborData(before, ndata) && !accumulationIsDry )
        return false;

    // neighbor data are in ndata like this:
    // [endpoint] n3_1 n3_2 n3_3
    // [miss 1]   n2_1 n2_2
    // [miss 2]   n1_1 n1_2 n3_4
    // where nT_S has time T and stationid S
    // forward iterating through ndata yields n3_1 n3_2 n3_3 n2_1 n2_2 n1_1 n1_2 n1_4 (backwards in time)
    
    float weightedNeighborsAccumulated = 0;
    dataList_t::const_iterator itN = ndata.begin();
    foreach(RedisUpdate& b, before) {
        std::ostringstream cfailed;
        cfailed << "QC2N";
        float sumWeights = 0.0, sumWeightedValues = 0.0;
        int goodNeighbors = 0;
        bool allNeighborsBoneDry = true; // true to assume dry if no neighbors
        for( ; itN != ndata.end() && itN->obstime() == b.obstime(); ++itN ) {
            const float neighborValue = dry2real(itN->original());
            if( neighborValue <= -2.0f )
                continue;
            const double weight = mNeighbors->getWeight(itN->stationID());
            if( weight > 0 ) {
                if( !equal(itN->original(), -1.0f) )
                    allNeighborsBoneDry = false;
                if( neighborValue > 0.15f )
                    b.setHasNeighborsWithPrecipitation();
                sumWeights        += weight;
                sumWeightedValues += weight * neighborValue;
                goodNeighbors += 1;
                cfailed << "_" << itN->stationID();
            }
        }
        b.setHasAllNeighborsBoneDry(allNeighborsBoneDry);
        if( goodNeighbors < mMinNeighbors && !accumulationIsDry ) {
            const int ageInDays = miutil::miDate::today().julianDay() - b.obstime().date().julianDay();
            (ageInDays > mDaysBeforeNoNeighborWarning ? warning() : info())
                << "not enough good neighbors at t=" << b.obstime()
                << " for accumulation ending in " << before.front();
            return false;
        }
        const float weightedNeighbors = sumWeightedValues/sumWeights;
        weightedNeighborsAccumulated += weightedNeighbors;
    
        cfailed << ",QC2-redist";
        b.corrected(weightedNeighbors)
            .cfailed(accumulationIsDry && goodNeighbors==0 ? "QC2-redist-dry-no-neighbors" : cfailed.str(), CFAILED_STRING)
            .controlinfo(update_flagchange.apply(b.controlinfo()));
    }

    float corrected_sum = 0;
    const float scale = ( weightedNeighborsAccumulated > 0.0f )
        ? accumulated / weightedNeighborsAccumulated : 0.0f;
    foreach(RedisUpdate& b, before) {
        const float corr = Helpers::round(scale * b.corrected());
        corrected_sum += corr;
        if( b.hasAllNeighborsBoneDry() )
            b.corrected(-1.0f);
        else
            b.corrected(corr);
    }

    // make sure that sum of re-distributed is the same as the original accumulated value
    float delta = Helpers::round(corrected_sum - accumulated);
    if( !accumulationIsDry ) {
        foreach(RedisUpdate& b, before) {
            if( fabs(delta) <= 0.05f )
                break;
            const float corr = b.corrected(), threshold = b.hasNeighborsWithPrecipitation() ? (delta+0.1) : 0.05;
            if( corr >= threshold ) {
                const float newCorr = std::max(Helpers::round(corr - delta), 0.0f);
                b.corrected(newCorr);
                delta = Helpers::round(delta - (corr - newCorr));
            }
        }
    }
    if( fabs(delta) > 0.05 ) {
        warning() << "could not avoid difference of " << fabs(delta) << " between distributed sum "
                  << " and accumulated value at endpoint=" << before.front();
    }
    return true;
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::updateOrInsertData(const updateList_t& data)
{
    bool hasChanges = false;
#if 0
    // perok 2011-11-21 store only rows with modified flags/corrected value
    foreach(const RedisUpdate& du, data) {
        if( du.isModified() ) {
            hasChanges = true;
            break;
        }
    }
#endif

    dataList_t toInsert, toUpdate;
    foreach(const RedisUpdate& du, data) {
        if( hasChanges || du.isModified() ) {
            DBG(du);
            if( du.isNew() )
                toInsert.push_front(du.data());
            else
                toUpdate.push_front(du.data());
        }
    }
#ifndef NDEBUG
    if( toInsert.empty() && toUpdate.empty() )
        DBG("no updates/inserts");
#endif
    storeData(toUpdate, toInsert);
}

