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

#define NDEBUG
#include "debug.h"

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

bool RedistributionAlgorithm::findMissing(const kvalobs::kvData& endpoint, const kvalobs::kvData& beforeMissing, updateList_t& mdata)
{
    dataList_t mdatao;
    const C::DBConstraint cMissing = C::SameDevice(endpoint)
        && C::Obstime(Helpers::plusDay(beforeMissing.obstime(), 1), stepTime(endpoint.obstime()));
    database()->selectData(mdatao, cMissing, O::Obstime().desc());
    mdata = updateList_t(mdatao.begin(), mdatao.end());

#ifndef NDEBUG
    DBG("  missingdata.size=" << mdata.size());
    foreach(const RedisUpdate& md, mdata)
        DBG("    missingdata md=" << md);
#endif

    foreach(const RedisUpdate& m, mdata) {
        if( !missingpoint_flags.matches(m.data()) ) {
            DBG("'missing' value " << m << " does not match 'missingpoint' flags between before=" << beforeMissing << " and endpoint=" << endpoint);
            return false;
        }
    }

    miutil::miTime t = stepTime(endpoint.obstime()), now = miutil::miTime::nowTime();
    for(updateList_it it = mdata.begin(); t > beforeMissing.obstime(); ++it ) {
        const miutil::miTime tdata = (it != mdata.end()) ? it->obstime() : beforeMissing.obstime();
        DBG("tdata=" << tdata << " t=" << t);
        while( t > tdata ) {
            const RedisUpdate fake(endpoint, t, now, missing, "0000003000002000");
            mdata.insert(it, fake);
            DBG("insert missing data " << fake);
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

bool RedistributionAlgorithm::findPointBeforeMissing(const kvalobs::kvData& endpoint, const miutil::miTime& earliest, kvalobs::kvData& latestBefore)
{
    DBG("findPointBeforeMissing: endpoint = " << endpoint);
    dataList_t startdata;
    const C::DBConstraint cBeforeMissing = C::ControlUseinfo(before_flags)
        && C::SameDevice(endpoint)
        && C::Obstime(earliest, stepTime(endpoint.obstime()));
    DBG("cBeforeMissing.sql='" << cBeforeMissing.sql() << "'");

    // TODO use MAX(...) in SQL, we are not interested in the others anyhow
    database()->selectData(startdata, cBeforeMissing, O::Obstime().desc());

#ifndef NDEBUG
    DBG("  startdata.size=" << startdata.size());
    foreach(const kvalobs::kvData& sd, startdata)
        DBG("    startdata sd=" << sd);
#endif

    if( !startdata.empty() ) {
        latestBefore = startdata.front();
        DBG("latestBefore=" << latestBefore);
        return( !equal(latestBefore.original(), missing)
                && !equal(latestBefore.original(), rejected) );
    } else {
        return false;
    }
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::getNeighborData(const updateList_t& before, dataList_t& ndata)
{
    const kvalobs::kvData& endpoint = before.front().data();

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
    Qc2Algorithm::configure(params);

    params.getFlagSetCU(endpoint_flags, "endpoint");
    params.getFlagSetCU(missingpoint_flags, "missingpoint");
    params.getFlagSetCU(before_flags, "before");
    params.getFlagSetCU(neighbor_flags, "neighbor");
    params.getFlagChange(update_flagchange, "update_flagchange");
    pids = params.getMultiParameter<int>("ParamId");
    tids = params.getMultiParameter<int>("TypeIds");

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
        const miutil::miTime earliestPossibleMissing = ( endpoint.stationID() != lastStationId ) ? UT0 : lastObstime;
        lastStationId = endpoint.stationID();
        lastObstime   = endpoint.obstime();

        // find the oldest point before the accumulated value which is "good"
        kvalobs::kvData beforeMissing;
        if( !findPointBeforeMissing(endpoint, earliestPossibleMissing, beforeMissing) ) {
            INF("no non-missing value before endpoint " << endpoint);
            continue;
        }

        // get series back in time from endpoint to last missing
        updateList_t accumulation;
        if( !findMissing(endpoint, beforeMissing, accumulation) )
            continue;

        if( equal(endpoint.original(), -1.0f) ) {
            redistributeDry(accumulation);
        } else {
            if( !redistributePrecipitation(accumulation) )
                continue;
        }

        updateOrInsertData(accumulation);
   }
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::redistributeDry(updateList_t& accumulation)
{
    // accumulated value is -1 => nothing to redistribute, all missing values must be dry (-1), too
    foreach(RedisUpdate& a, accumulation) {
        a.corrected(-1)
            .controlinfo(update_flagchange.apply(a.controlinfo()))
            .cfailed("Qc2-redist-dry", CFAILED_STRING);
    }
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::redistributePrecipitation(updateList_t& before)
{
    dataList_t ndata;
    if( !getNeighborData(before, ndata) ) {
        LOGINFO("too few valid neighbors for accumulation series ending with " << before.front() << ", giving up");
        return false;
    }
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
        unsigned int goodNeighbors = 0;
        for( ; itN != ndata.end() && itN->obstime() == b.obstime(); ++itN ) {
            const float neighborValue = dry2real(itN->original());
            if( neighborValue <= -1.0f )
                continue;
            const double weight = mNeighbors->getWeight(itN->stationID());
            if( weight > 0 ) {
                if( neighborValue > 0.15f )
                    b.setHasNeighborsWithPrecipitation();
                sumWeights        += weight;
                sumWeightedValues += weight * neighborValue;
                goodNeighbors += 1;
                cfailed << "_" << itN->stationID();
            }
        }
        if( goodNeighbors < MIN_NEIGHBORS ) {
            INF("not enough good neighbors for accumulation ending in " << before.front() << " at t=" << b.obstime());
            return false;
        }
        const float weightedNeighbors = sumWeightedValues/sumWeights;
        weightedNeighborsAccumulated += weightedNeighbors;
    
        cfailed << ",QC2-redist";
        b.corrected(weightedNeighbors)
            .cfailed(cfailed.str(), CFAILED_STRING)
            .controlinfo(update_flagchange.apply(b.controlinfo()));
    }
    
    float corrected_sum = 0;
    const float scale = dry2real(before.front().original()) / weightedNeighborsAccumulated;
    foreach(RedisUpdate& b, before) {
        const float corr = round<float, 1>(scale * b.corrected());
        corrected_sum += corr;
        b.corrected(real2dry(corr)); // bugzilla 1304: by default assume dry
    }

    // make sure that sum of re-distributed is the same as the original accumulated value
    float delta = round<float,1>(corrected_sum - dry2real(before.front().original()));
    foreach(RedisUpdate& b, before) {
        if( delta <= 0.0f )
            break;
        const float corr = b.corrected(), threshold = b.hasNeighborsWithPrecipitation() ? (delta+0.1) : 0.05;
        if( corr >= threshold ) {
            const float newCorr = std::max(round<float, 1>(corr - delta), 0.0f);
            b.corrected(newCorr);
            delta = round<float, 1>(delta - (corr - newCorr));
        }
    }
    if( delta > 0.05 ) {
        LOGWARN("Could not avoid difference between distributed sum and accumulated value at endpoint=" << before.front());
    }
    return true;
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::updateOrInsertData(const updateList_t& data)
{
    bool hasChanges = false;
#if 1
    // XXX it is not specified if the whole series or just the modified rows should be stored
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

