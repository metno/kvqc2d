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

#define DBG(x) do { LOGDEBUG(x); /*std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl;*/ } while(false);
#define INF(x) do { LOGINFO(x);  /*std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl;*/ } while(false);

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

bool RedistributionAlgorithm::findMissing(const kvalobs::kvData& endpoint, const kvalobs::kvData& beforeMissing, const miutil::miTime& fakeTableTime, dataList_t& mdata)
{
    const C::DBConstraint cMissing = C::SameDevice(endpoint)
        && C::Obstime(Helpers::plusDay(beforeMissing.obstime(), 1), stepTime(endpoint.obstime()));
    database()->selectData(mdata, cMissing, O::Obstime().desc());

    DBG("  missingdata.size=" << mdata.size());
    foreach(const kvalobs::kvData& md, mdata)
        DBG("    missingdata md=" << md);

    foreach(const kvalobs::kvData& m, mdata) {
        if( !missingpoint_flags.matches(m) ) {
            DBG("'missing' value " << m << " does not match 'missingpoint' flags between before=" << beforeMissing << " and endpoint=" << endpoint);
            return false;
        }
    }

    miutil::miTime t = stepTime(endpoint.obstime());
    for(dataList_t::iterator it = mdata.begin(); t > beforeMissing.obstime(); ++it ) {
        const miutil::miTime tdata = (it != mdata.end()) ? it->obstime() : beforeMissing.obstime();
        DBG("tdata=" << tdata << " t=" << t);
        while( t > tdata ) {
            const kvalobs::kvData fake(endpoint.stationID(), t, missing, endpoint.paramID(), fakeTableTime, endpoint.typeID(),
                                       endpoint.sensor(), endpoint.level(), missing,
                                       kvalobs::kvControlInfo("0000003000002000"), kvalobs::kvUseInfo("7899900000000000"),
                                       miutil::miString("QC2-missing-row"));
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

    foreach(const kvalobs::kvData& md, mdata)
        DBG("  final redistribution series: " << md);

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
    DBG("  startdata.size=" << startdata.size());
    foreach(const kvalobs::kvData& sd, startdata)
        DBG("    startdata sd=" << sd);
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
    pids = params.getMultiParameter<int>("ParamId");
    tids = params.getMultiParameter<int>("TypeIds");

    mNeighbors->configure(params);
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::run(const ReadProgramOptions& params)
{
    configure(params);

    dataList_t edata;
    const C::DBConstraint cEndpoints = C::ControlUseinfo(endpoint_flags)
            && C::Paramid(pids) && C::Typeid(tids)
            && C::Obstime(UT0, params.UT1);
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
        dataList_t accumulation;
        const miutil::miTime fakeTableTime("2121-01-01 00:00:00.12345");
        if( !findMissing(endpoint, beforeMissing, fakeTableTime, accumulation) )
            continue;

        dataList_t toStore;
        if( equal(endpoint.original(), -1.0f) ) {
            redistributeDry(accumulation, toStore);
        } else {
            redistributePrecipitation(accumulation, toStore);
        }
        updateOrInsertData(toStore, fakeTableTime);
   }
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::redistributeDry(const dataList_t& accumulation, dataList_t& toWrite)
{
    // accumulated value is -1 => nothing to redistribute, all missing values must be dry (-1), too
    bool hasChanges = false;
    foreach(const kvalobs::kvData& a, accumulation) {
        kvalobs::kvData w(a);
        w.corrected(-1);
        w.controlinfo(update_flagchange.apply(a.controlinfo()));
        if( w.corrected() != a.corrected() || w.controlinfo() != a.controlinfo() )
            hasChanges = true;
        Helpers::updateUseInfo(w);
        Helpers::updateCfailed(w, "Qc2-redist-dry", CFAILED_STRING);
        toWrite.push_front(w);
    }
    if( !hasChanges )
        toWrite.clear();
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::redistributePrecipitation(dataList_t& before, dataList_t& toWrite)
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
    
    std::vector<float> corrected;
    corrected.reserve(before.size());

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
            INF("not enough good neighbors for accumulation ending in " << before.front() << " at t=" << itB->obstime());
            return;
        }
        const float weightedNeighbors = sumWeightedValues/sumWeights;
        weightedNeighborsAccumulated += weightedNeighbors;
    
        corrected.push_back(weightedNeighbors);
        cfailed << ",QC2-redist";
        Helpers::updateCfailed(*itB, cfailed.str(), CFAILED_STRING);
    }
    DBG("corrected: " << corrected.size() << " before:" << before.size());
    
    bool hasChanges = false;
    float corrected_sum = 0;
    const float scale = dry2real(before.front().original()) / weightedNeighborsAccumulated;
    std::vector<float>::iterator itC = corrected.begin();
    foreach(kvalobs::kvData& b, before) {
        *itC = round<float, 1>(*itC * scale);
        const kvalobs::kvControlInfo b_controlinfo(update_flagchange.apply(b.controlinfo()));
        if( b_controlinfo.flagstring() != b.controlinfo().flagstring() ) {
            DBG("b_controlinfo['" << b_controlinfo.flagstring() << " != b.controlinfo[" << b.controlinfo().flagstring() << "]");
            hasChanges = true;
        }
        corrected_sum += *itC;
        *itC = real2dry(*itC); // bugzilla 1304: by default assume dry
        b.controlinfo(b_controlinfo);
        Helpers::updateUseInfo(b);
        ++itC;
    }

    // make sure that sum of re-distributed is the same as the original accumulated value
    std::vector<bool>::const_iterator itNP = hasNeigboursWithPrecipitation.begin();
    itC = corrected.begin();
    float delta = round<float,1>(corrected_sum - dry2real(before.front().original()));
    for( ; delta > 0.0f && itC != corrected.end(); ++itC, ++itNP ) {
        const float oc = *itC;
        DBG("oc=" << oc);
        if( (*itNP && oc >= delta+0.1) || (!*itNP && oc>0.05) ) {
            const float nc = std::max(round<float, 1>(oc - delta), 0.0f);
            *itC = nc;
            delta = round<float, 1>(delta - (oc - nc));
        }
    }
    if( delta > 0.05 ) {
        LOGWARN("Could not avoid difference between distributed sum and accumulated value at endpoint=" << before.front());
    }

    itC = corrected.begin();
    foreach(kvalobs::kvData& b, before) {
        DBG("*itC[" << *itC << "], b.corrected[" << b.corrected() << "], equal? " << equal(*itC, b.corrected()) );
        if( !equal(*itC, b.corrected()) )
            hasChanges = true;
        b.corrected(*itC);
        ++itC;
    }

    DBG("hasChanges=" << hasChanges);
    if( hasChanges )
        // we accumulated the corrections in time-reversed order, while we want to send it with increasing time => push_front
        toWrite = dataList_t(before.rbegin(), before.rend());
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::updateOrInsertData(const dataList_t& toStore, const miutil::miTime& fakeTableTime)
{
    const miutil::miTime now = miutil::miTime::nowTime();
    dataList_t toUpdate, toInsert;
    foreach(const kvalobs::kvData& d, toStore) {
        if( d.tbtime() == fakeTableTime ) {
            // cannot set tbtime in libkvcpp-dev 2.2.0-lucid2
            kvalobs::kvData dd(d.stationID(), d.obstime(), d.original(), d.paramID(), now, d.typeID(), d.sensor(),
                               d.level(), d.corrected(), d.controlinfo(), d.useinfo(), d.cfailed());
            toInsert.push_back(dd);
        } else {
            toUpdate.push_back(d);
        }
    }
    storeData(toUpdate, toInsert);
}
