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

void NeighborFinder::setStationList(const stations_t& stations)
{
    foreach(const kvalobs::kvStation& s, stations) {
        mStationsByID[ s.stationID() ] = s;
    }
}

// ------------------------------------------------------------------------

void NeighborFinder::findNeighbors(stationsWithDistances_t& neighbors, int aroundID, float maxdist)
{
    neighbors.clear();
    const stationsByID_t::const_iterator itAround = mStationsByID.find(aroundID);
    if( itAround == mStationsByID.end() )
        return;
    const kvalobs::kvStation& around = itAround->second;
    foreach(stationsByID_t::value_type& v, mStationsByID) {
        const kvalobs::kvStation& neighbor = v.second;
        if( neighbor.stationID() == aroundID )
            continue;
        const double distance = Helpers::distance(around.lon(), around.lat(), neighbor.lon(), neighbor.lat());
        if( distance > 0 && distance < maxdist ) {
            neighbors[ neighbor.stationID() ] = distance;
        }
    }
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::findNeighbors(int stationID, NeighborFinder::stationsWithDistances_t& neighbors)
{
    if( !nf.hasStationList() ) {
        std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
        std::list<int> allStationIDs;
        fillStationLists(allStations, allStationIDs);
        nf.setStationList(allStations);
    }
    
    nf.findNeighbors(neighbors, stationID, mInterpolationLimit);
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::getSeriesBefore(const kvalobs::kvData& endpoint, std::list<kvalobs::kvData>& bdata)
{
    const C::DBConstraint cBefore = C::ControlUseinfo(missingpoint_flags)
        && C::Station(endpoint.stationID()) && C::Paramid(endpoint.paramID()) && C::Typeid(endpoint.typeID())
        && C::Obstime(UT0, Helpers::plusDay(endpoint.obstime(), -1));
    database()->selectData(bdata, cBefore, O::Obstime().desc());
}

// ------------------------------------------------------------------------

bool RedistributionAlgorithm::checkAndTrimSeriesBefore(std::list<kvalobs::kvData>& bdata, std::list<kvalobs::kvData>& before, const kvalobs::kvData& endpoint)
{
    before.push_back(endpoint);
    miutil::miTime t = Helpers::plusDay(endpoint.obstime(), -1);
    foreach(const kvalobs::kvData& b, bdata) {
        if( b.obstime() != t )
            break;
        before.push_back(b);
        t.addDay(-1);
    }
    if( before.size()<=1 ) {
        //std::cout << "no data before accumulated value" << std::endl;
        return false;
    }
    if( before.back().obstime() <= UT0 ) {
        //std::cout << "before starts at UT0, no idea about series length" << std::endl;
        return false;
    }
    return true;
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::configure(const ReadProgramOptions& params)
{
    params.getFlagSetCU(endpoint_flags, "endpoint");
    params.getFlagSetCU(missingpoint_flags, "missingpoint");
    params.getFlagSetCU(before_flags, "before");
    params.getFlagSetCU(neighbor_flags, "neighbor");
    params.getFlagChange(update_flagchange, "update_flagchange");
    mInterpolationLimit = params.getParameter("InterpolationDistance", 25);
    UT0 = params.UT0;
}

// ------------------------------------------------------------------------

void RedistributionAlgorithm::run(const ReadProgramOptions& params)
{
    typedef std::list<kvalobs::kvData> dataList_t;

    configure(params);

    const O::DBOrdering order_by_time_id = (O::Obstime().desc(), O::Stationid());

    dataList_t edata;
    const C::DBConstraint cEndpoints = C::ControlUseinfo(endpoint_flags)
            && C::Paramid(params.pid) && C::Typeid(params.tids)
            && C::Obstime(UT0, params.UT1);
    database()->selectData(edata, cEndpoints);
    foreach(const kvalobs::kvData& d, edata) {

        dataList_t bdata;
        getSeriesBefore(d, bdata);

        dataList_t before;
        if( !checkAndTrimSeriesBefore(bdata, before, d) )
            continue;

        dataList_t startdata;
        const C::DBConstraint cBeforeMissing = C::ControlUseinfo(before_flags)
            && C::Station(d.stationID()) && C::Paramid(d.paramID()) && C::Typeid(d.typeID())
            && C::Obstime(Helpers::plusDay(before.back().obstime(), -1));
        database()->selectData(startdata, cBeforeMissing, O::Obstime());
        if( startdata.size() != 1 || equal(startdata.front().original(), params.missing) || equal(startdata.front().original(), params.rejected) ) {
            LOGINFO("value before time series not existing/missing/rejected/not usable for accumulation ending in " << d);
            continue;
        }

        NeighborFinder::stationsWithDistances_t neighbors;
        findNeighbors(d.stationID(), neighbors);
        if( neighbors.size() < MIN_NEIGHBORS ) {
            LOGINFO("only " << MIN_NEIGHBORS << " neighbor(s) for d=" << d << ", giving up");
            continue;
        }

        C::Station cNeighborStations;
        foreach(NeighborFinder::stationsWithDistances_t::value_type& v, neighbors)
            cNeighborStations.add( v.first );
        dataList_t ndata;
        const C::DBConstraint cNeighbors = C::ControlUseinfo(neighbor_flags)
                && C::Paramid(params.pid)
                && C::Typeid(d.typeID())
                && C::Obstime(before.back().obstime(), d.obstime())
                && cNeighborStations;
        database()->selectData(ndata, cNeighbors, order_by_time_id);

        bool neighborsMissing = false;
        float sumint = 0;
        dataList_t::const_iterator itN = ndata.begin(), itB = before.begin();
        for(; itB != before.end(); ++itB ) {
            //std::cout << "itb obstime=" << itB->obstime() << " itN obstime=" << itN->obstime() << std::endl;
            float sumWeights = 0.0, sumWeightedValues = 0.0;
            unsigned int n = 0;
            for( ; itN != ndata.end() && itN->obstime() == itB->obstime(); ++itN ) {
                //std::cout << "neighbour id = " << itN->stationID() << std::endl;
                float data_point = itN->original();
                if( equal(data_point, -1.0f) )
                    data_point = 0; // These are bone dry measurements as opposed to days when there may have been rain but none was measurable
                if( data_point <= -1 )
                    continue;
                const double dist = neighbors.at(itN->stationID()), invDist2 = 1.0/(dist*dist);
                //std::cout << "distance =" << dist << " km" << std::endl;
                sumWeights += invDist2;
                sumWeightedValues += data_point*invDist2;
                n += 1;
            }
            if( n < MIN_NEIGHBORS ) {
                //LOGERROR("not enough good neighbors n=" << n << " d=" << d);
                neighborsMissing = true;
                break;
            }
            sumint += sumWeightedValues/sumWeights;
        }
        if( neighborsMissing ) {
            LOGERROR("not enough good neighbors d=" << d);
            continue;
        }
        std::vector<bool> hasNeigboursWithPrecipitation;
        std::list<kvalobs::kvData> toWrite;
        //std::cout << "accval = " << d.original() << " sumint=" << sumint << std::endl;
        itN = ndata.begin(), itB = before.begin();
        float accumulated = d.original(), corrected_sum = 0;
        if( equal(d.original(), -1.0f) )
            accumulated = 0;
        for(; itB != before.end() && itN != ndata.end(); ++itB ) {
            hasNeigboursWithPrecipitation.push_back(false);
            std::ostringstream cfailed;
            cfailed << "QC2N";
            float sumWeights = 0.0, sumWeightedValues = 0.0;
            for( ; itN->obstime() == itB->obstime(); ++itN ) {
                float data_point = itN->original();
                if( equal(data_point, -1.0f) )
                    data_point = 0; // These are bone dry measurements as opposed to days when there may have been rain but none was measurable
                if( data_point < -1 )
                    continue;
                if( data_point>0.15 )
                    hasNeigboursWithPrecipitation.back() = true;
                const double dist = neighbors.at(itN->stationID()), invDist2 = 1.0/(dist*dist);
                sumWeights += invDist2;
                sumWeightedValues += data_point*invDist2;
                cfailed << "_" << itN->stationID();
            }
            cfailed << ",QC2-redist";
            float itB_corrected = round<float, 1>((sumWeightedValues/sumWeights) * (accumulated / sumint));
            corrected_sum += itB_corrected;
            if( equal(d.original(), -1.0f) || equal(itB_corrected, 0.0f) )
                itB_corrected = -1; // bugzilla 1304: by default assume dry

            kvalobs::kvData correctedData(*itB);
            correctedData.corrected(itB_corrected);
            correctedData.controlinfo(update_flagchange.apply(correctedData.controlinfo()));
            Helpers::updateUseInfo(correctedData);
            Helpers::updateCfailed(correctedData, cfailed.str(), params.CFAILED_STRING);
            // we accumulated the corrections in time-reversed order, while we want to send it with increasing time => push_front
            toWrite.push_front(correctedData);
        }

        // make sure that sum of re-distributed is the same as the original accumulated value
        int idxNeighboursWithPrecipitation = hasNeigboursWithPrecipitation.size()-1;
        float delta = round<float,1>(corrected_sum - accumulated);
        std::list<kvalobs::kvData>::iterator itS = toWrite.begin();
        for( ; delta > 0 && itS != toWrite.end(); ++itS, --idxNeighboursWithPrecipitation ) {
            kvalobs::kvData& w = *itS;
            bool nb = hasNeigboursWithPrecipitation[idxNeighboursWithPrecipitation];
            const float oc = w.corrected();
            if( (nb && oc >= delta+0.1) || (!nb && oc>0.05) ) {
                const float nc = std::max(round<float, 1>(oc - delta), 0.0f);
                w.corrected(nc);
                delta = round<float, 1>(delta - (oc - nc));
            }
        }
        if( delta > 0.05 ) {
            LOGWARN("Could not avoid difference between distributed sum and accumulated value at d=" << d);
        }

        updateData(toWrite);
    }
}
