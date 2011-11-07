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
#include "Helpers.h"
#include "tround.h"

#include <milog/milog.h>
#include "foreach.h"

namespace C = Constraint;
namespace O = Ordering;
using Helpers::equal;

static const unsigned int MIN_NEIGHBORS = 1;

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

void RedistributionAlgorithm::getMissingBefore(const kvalobs::kvData& endpoint, const miutil::miTime& earliest, dataList_t& bdata)
{
    const C::DBConstraint cBefore = C::ControlUseinfo(missingpoint_flags)
        && C::Station(endpoint.stationID()) && C::Paramid(endpoint.paramID()) && C::Typeid(endpoint.typeID())
        && C::Obstime(earliest, stepTime(endpoint.obstime()));
    database()->selectData(bdata, cBefore, O::Obstime().desc());
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::checkAndTrimSeries(dataList_t& bdata)
{
    // walk back in time in 1-day steps until jump/end
    dataList_t::iterator it = bdata.begin();
    miutil::miTime t = it->obstime();
    for(; it != bdata.end() && it->obstime() == t; ++it)
        t = stepTime(t);

    // erase data before time jump (or erase nothing)
    bdata.erase(it, bdata.end());

    if( bdata.size()<=1 ) {
        // std::cout << "no data before accumulated value" << std::endl;
        return false;
    }
    if( bdata.back().obstime() <= UT0 ) {
        // std::cout << "before starts at UT0, no idea about series length" << std::endl;
        return false;
    }
    return true;
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::checkPointBeforeMissing(const dataList_t& accumulation)
{
    const kvalobs::kvData& earliestMissing = accumulation.back();
    dataList_t startdata;
    const C::DBConstraint cBeforeMissing = C::ControlUseinfo(before_flags)
        && C::Station(earliestMissing.stationID()) && C::Paramid(earliestMissing.paramID()) && C::Typeid(earliestMissing.typeID())
        && C::Obstime(stepTime(earliestMissing.obstime()));

    database()->selectData(startdata, cBeforeMissing, O::Obstime());
    return startdata.size() == 1
        && !equal(startdata.front().original(), missing)
        && !equal(startdata.front().original(), rejected);
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::getNeighborData(const dataList_t& before, dataList_t& ndata)
{
    const kvalobs::kvData& endpoint = before.front();

    const std::list<int> neighbors = findNeighbors(endpoint.stationID());
    if( neighbors.size() < MIN_NEIGHBORS )
        return false;

    const C::DBConstraint cNeighbors = C::ControlUseinfo(neighbor_flags)
        && C::Paramid(endpoint.paramID()) && C::Typeid(endpoint.typeID())
        && C::Obstime(before.back().obstime(), endpoint.obstime())
        && C::Station(neighbors);
    database()->selectData(ndata, cNeighbors, (O::Obstime().desc(), O::Stationid()));

    return ndata.size() >= MIN_NEIGHBORS * before.size();
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::configure(const ReadProgramOptions& params)
{
    params.getFlagSetCU(endpoint_flags, "endpoint");
    params.getFlagSetCU(missingpoint_flags, "missingpoint");
    params.getFlagSetCU(before_flags, "before");
    params.getFlagSetCU(neighbor_flags, "neighbor");
    params.getFlagChange(update_flagchange, "update_flagchange");
    UT0      = params.UT0;
    missing  = params.missing;
    rejected = params.rejected;
    CFAILED_STRING = params.CFAILED_STRING;

    mNeighbors->configure(params);
}

// ------------------------------------------------------------------------

// void RedistributionAlgorithm::checkRedistributedSum()
// {
//     // make sure that sum of re-distributed is the same as the original accumulated value
//     int idxNeighboursWithPrecipitation = hasNeigboursWithPrecipitation.size()-1;
//     float delta = round<float,1>(corrected_sum - accumulated);
//     std::list<kvalobs::kvData>::iterator itS = toWrite.begin();
//     for( ; delta > 0 && itS != toWrite.end(); ++itS, --idxNeighboursWithPrecipitation ) {
//         kvalobs::kvData& w = *itS;
//         bool nb = hasNeigboursWithPrecipitation[idxNeighboursWithPrecipitation];
//         const float oc = w.corrected();
//         if( (nb && oc >= delta+0.1) || (!nb && oc>0.05) ) {
//             const float nc = std::max(round<float, 1>(oc - delta), 0.0f);
//             w.corrected(nc);
//             delta = round<float, 1>(delta - (oc - nc));
//         }
//     }
//     if( delta > 0.05 ) {
//         LOGWARN("Could not avoid difference between distributed sum and accumulated value at endpoint=" << endpoint);
//     }
// }

// ------------------------------------------------------------------------

void RedistributionAlgorithm::run(const ReadProgramOptions& params)
{
    configure(params);

    dataList_t edata;
    const C::DBConstraint cEndpoints = C::ControlUseinfo(endpoint_flags)
            && C::Paramid(params.pid) && C::Typeid(params.tids)
            && C::Obstime(UT0, params.UT1);
    database()->selectData(edata, cEndpoints, (O::Stationid(), O::Obstime().asc()));
    
    int lastStationId = -1;
    miutil::miTime lastObstime = UT0;
    foreach(const kvalobs::kvData& endpoint, edata) {
        const miutil::miTime earliestPossibleMissing = ( endpoint.stationID() != lastStationId ) ? UT0 : lastObstime;
        lastStationId = endpoint.stationID();
        lastObstime   = endpoint.obstime();

        // get series back in time from endpoint to last missing
        dataList_t accumulation;
        getMissingBefore(endpoint, earliestPossibleMissing, accumulation);
        accumulation.push_front(endpoint);
        if( !checkAndTrimSeries(accumulation) )
            continue;
        
        if( !checkPointBeforeMissing(accumulation) ) {
            LOGINFO("value before accumulation start missing/... for endpoint " << endpoint);
            continue;
        }

        if( equal(endpoint.original(), -1.0f) ) {
            redistributeDry(accumulation);
        } else {
            redistributePrecipitation(accumulation);
        }
   }
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::redistributeDry(dataList_t& accumulation)
{
    // accumulated value is -1 => nothing to redistribute, all missing values must be dry (-1), too
    std::list<kvalobs::kvData> toWrite;
    foreach(kvalobs::kvData& b, accumulation) {
        b.corrected(-1);
        b.controlinfo(update_flagchange.apply(b.controlinfo()));
        Helpers::updateUseInfo(b);
        Helpers::updateCfailed(b, "Qc2-redist-dry", CFAILED_STRING);
        toWrite.push_front(b);
    }
    updateData(toWrite);
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::redistributePrecipitation(dataList_t& before)
{
    dataList_t ndata;
    if( !getNeighborData(before, ndata) ) {
        LOGINFO("too few valid neighbors for accumulation series ending with " << before.front() << ", giving up");
        return;
    }
    // neighbor data are in ndata like this:
    // [endpoint] n3_1 n3_2 n3_3
    // [miss 1]   n2_1 n2_2
    // [miss 2]   n1_1 n1_2 n3_4
    // where nT_S has time T and stationid S
    // forward iterating through ndata yields n3_1 n3_2 n3_3 n2_1 n2_2 n1_1 n1_2 n1_4 (backwards in time)
    
    std::vector<bool> hasNeigboursWithPrecipitation;
    float weightedNeighborsAccumulated = 0;
    dataList_t::const_iterator itN = ndata.begin();
    dataList_t::iterator itB = before.begin();
    for(; itB != before.end(); ++itB) {
        hasNeigboursWithPrecipitation.push_back(false);
        std::ostringstream cfailed;
        cfailed << "QC2N";
        float sumWeights = 0.0, sumWeightedValues = 0.0;
        unsigned int goodNeighbors = 0;
        for( ; itN != ndata.end() && itN->obstime() == itB->obstime(); ++itN ) {
            const float neighborValue = dry2real(itN->original());
            if( neighborValue <= -1.0f )
                continue;
            const double weight = mNeighbors->getWeight(itN->stationID());
            if( weight > 0 ) {
                if( neighborValue > 0.15f )
                    hasNeigboursWithPrecipitation.back() = true;
                sumWeights        += weight;
                sumWeightedValues += weight * neighborValue;
                goodNeighbors += 1;
                cfailed << "_" << itN->stationID();
            }
        }
        if( goodNeighbors < MIN_NEIGHBORS ) {
            LOGINFO("not enough good neighbors for accumulation ending in " << before.front() << " at t=" << itB->obstime());
            return;
        }
        const float weightedNeighbors = sumWeightedValues/sumWeights;
        weightedNeighborsAccumulated += weightedNeighbors;
    
        itB->corrected(weightedNeighbors);
        itB->controlinfo(update_flagchange.apply(itB->controlinfo()));
        Helpers::updateUseInfo(*itB);
        cfailed << ",QC2-redist";
        Helpers::updateCfailed(*itB, cfailed.str(), CFAILED_STRING);
    }
    
    float corrected_sum = 0;
    const float scale = dry2real(before.front().original()) / weightedNeighborsAccumulated;
    foreach(kvalobs::kvData& b, before) {
        const float b_corrected = round<float, 1>(b.corrected() * scale);
        corrected_sum += b_corrected;
        b.corrected(real2dry(b_corrected)); // bugzilla 1304: by default assume dry
    }

    // make sure that sum of re-distributed is the same as the original accumulated value
    std::vector<bool>::const_iterator itNP = hasNeigboursWithPrecipitation.begin();
    float delta = round<float,1>(corrected_sum - dry2real(before.front().original()));
    itB = before.begin();
    for( ; delta > 0.0f && itB != before.end(); ++itB, ++itNP ) {
        const float oc = itB->corrected();
        if( (*itNP && oc >= delta+0.1) || (!*itNP && oc>0.05) ) {
            const float nc = std::max(round<float, 1>(oc - delta), 0.0f);
            itB->corrected(nc);
            delta = round<float, 1>(delta - (oc - nc));
        }
    }
    if( delta > 0.05 ) {
        LOGWARN("Could not avoid difference between distributed sum and accumulated value at endpoint=" << before.front());
    }

    // we accumulated the corrections in time-reversed order, while we want to send it with increasing time => push_front
    std::list<kvalobs::kvData> toWrite(before.rbegin(), before.rend());
    updateData(toWrite);
}
