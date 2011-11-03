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

class NeighborFinder {
public:
    typedef std::map<int, double> stationsWithDistances_t;
    typedef std::list<kvalobs::kvStation> stations_t;
    typedef std::map<int, kvalobs::kvStation> stationsByID_t;

    virtual ~NeighborFinder() { }

    bool hasStationList() const
        { return !mStationsByID.empty(); }

    void setStationList(const stations_t& stations);

    virtual void findNeighbors(stationsWithDistances_t& neighbors, int aroundID, float maxdist);

private:
    stationsByID_t mStationsByID;
};

void NeighborFinder::setStationList(const stations_t& stations)
{
    foreach(const kvalobs::kvStation& s, stations) {
        mStationsByID[ s.stationID() ] = s;
    }
}

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

void RedistributionAlgorithm2::run(const ReadProgramOptions& params)
{
    using namespace kvQCFlagTypes;
    typedef std::list<kvalobs::kvData> dataList_t;

    FlagSetCU endpoint_flags, missingpoint_flags, before_flags, neighbor_flags;
    if( !params.getFlagSetCU(endpoint_flags, "endpoint") ) {
        LOGWARN("problem reading endpoint flags; giving up");
        return;
    }
    if( !params.getFlagSetCU(missingpoint_flags, "missingpoint") ) {
        LOGWARN("problem reading missingpoint flags; giving up");
        return;
    }
    if( !params.getFlagSetCU(before_flags, "before") ) {
        LOGWARN("problem reading before flags; giving up");
        return;
    }
    if( !params.getFlagSetCU(neighbor_flags, "neighbor") ) {
        LOGWARN("problem reading neighbor flags; giving up");
        return;
    }
    FlagChange update_flagchange;
    if( !params.getFlagChange(update_flagchange, "update_flagchange")) {
        LOGWARN("problem reading update_flagchange; giving up");
        return;
    }

    NeighborFinder nf;

    const O::DBOrdering order_by_time = O::Obstime().desc();
    const O::DBOrdering order_by_time_id = (order_by_time, O::Stationid());
    const C::DBConstraint flags_endpoint  = C::ControlUseinfo(endpoint_flags);
    const C::DBConstraint flags_missing   = C::ControlUseinfo(missingpoint_flags);
    const C::DBConstraint flags_neighbors = C::ControlUseinfo(neighbor_flags);
    const C::DBConstraint flags_before    = C::ControlUseinfo(before_flags);

    dataList_t edata;
    const C::DBConstraint cEndpoints = flags_endpoint
            && C::Paramid(params.pid)
            && C::Typeid(params.tids)
            && C::Obstime(params.UT0, params.UT1);
    database()->selectData(edata, cEndpoints);
    foreach(const kvalobs::kvData& d, edata) {
        dataList_t bdata;
        const C::DBConstraint cBefore = flags_missing
            && C::Paramid(params.pid)
            && C::Typeid(d.typeID())
            && C::Obstime(params.UT0, d.obstime())
            && C::Station(d.stationID());
        database()->selectData(bdata, cBefore, order_by_time);

        dataList_t before;
        before.push_back(d);
        miutil::miTime t = Helpers::plusDay(d.obstime(), -1);
        foreach(const kvalobs::kvData& b, bdata) {
            if( b.obstime() != t )
                break;
            before.push_back(b);
            t.addDay(-1);
        }
        if( before.size()<=1 ) {
            //std::cout << "no data before accumulated value" << std::endl;
            continue;
        }
        if( before.back().obstime() <= params.UT0 ) {
            //std::cout << "before starts at UT0, no idea about series length" << std::endl;
            continue;
        }

        dataList_t startdata;
        const C::DBConstraint cBeforeMissing = flags_before
                && C::Paramid(params.pid)
                && C::Typeid(d.typeID())
                && C::Obstime(t)
                && C::Station(d.stationID());
        database()->selectData(startdata, cBeforeMissing, order_by_time);
        if( startdata.size() != 1 || equal(startdata.front().original(), params.missing) || equal(startdata.front().original(), params.rejected) ) {
            LOGINFO("value before time series not existing/missing/rejected/not usable at t=" << t << " for accumulation in " << d);
            continue;
        }

        if( !nf.hasStationList() ) {
            std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
            std::list<int> allStationIDs;
            fillStationLists(allStations, allStationIDs);
            nf.setStationList(allStations);
        }

        NeighborFinder::stationsWithDistances_t neighbors;
        nf.findNeighbors(neighbors, d.stationID(), params.InterpolationLimit);
        if( neighbors.size() < MIN_NEIGHBORS ) {
            LOGINFO("less than " << MIN_NEIGHBORS << " neighbor(s) within " << params.InterpolationLimit
                    << " km for stationid=" << d.stationID());
            continue;
        }

        C::Station cNeighborStations;
        foreach(NeighborFinder::stationsWithDistances_t::value_type& v, neighbors)
            cNeighborStations.add( v.first );
        dataList_t ndata;
        const C::DBConstraint cNeighbors = flags_neighbors
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
