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

static const unsigned int MIN_NEIGHBORS = 1;

class NeighboringStationFinder {
public:
    typedef std::map<int, double> stationsWithDistances_t;
    typedef std::list<kvalobs::kvStation> stations_t;
    typedef std::map<int, kvalobs::kvStation> stationsByID_t;

    virtual ~NeighboringStationFinder() { }

    bool hashStationList() const
        { return !mStationsByID.empty(); }

    void setStationList(const stations_t& stations);

    virtual void findNeighbors(stationsWithDistances_t& neighbors, int aroundID, float maxdist);

private:
    stationsByID_t mStationsByID;
};

void NeighboringStationFinder::setStationList(const stations_t& stations)
{
    foreach(const kvalobs::kvStation& s, stations) {
        mStationsByID[ s.stationID() ] = s;
    }
}

void NeighboringStationFinder::findNeighbors(stationsWithDistances_t& neighbors, int aroundID, float maxdist)
{
    neighbors.clear();
    if( mStationsByID.find(aroundID) == mStationsByID.end() )
        return;
    const kvalobs::kvStation& around = mStationsByID.at(aroundID);
    foreach(stationsByID_t::value_type& v, mStationsByID) {
        const kvalobs::kvStation& station = v.second;
        if( station.stationID() == aroundID )
            continue;
        const double distance = Helpers::distance(around.lon(), around.lat(), station.lon(), station.lat());
        if( distance > 0 && distance < maxdist ) {
            neighbors[ station.stationID() ] = distance;
        }
    }
}

void RedistributionAlgorithm2::run(const ReadProgramOptions& params)
{
    using namespace kvQCFlagTypes;
    typedef std::list<kvalobs::kvData> dataList_t;

    NeighboringStationFinder nf;

    const O::DBOrdering order_by_time = O::Obstime().desc();
    const O::DBOrdering order_by_time_id = (order_by_time, O::Stationid());
    const C::DBConstraint controli_endpoint = C::Controlinfo(FlagMatcher().permit(f_fmis, 4).permit(f_fd, 2).permit(f_fhqc, 0));
    const C::DBConstraint controli_missing = C::Controlinfo(FlagMatcher().permit(f_fmis, 3).permit(f_fd, 2).permit(f_fhqc, 0));
    const C::DBConstraint usei_neighbors = C::Useinfo(FlagMatcher().permit(2, 0));

    dataList_t allDataOneTime;
    database()->selectData(allDataOneTime,
            controli_endpoint
                    && C::Paramid(params.pid)
                    && C::Typeid(params.tids)
                    && C::Obstime(params.UT0, params.UT1));
    foreach(const kvalobs::kvData& d, allDataOneTime) {
        //std::cout << "center=" << d << std::endl;

        dataList_t bdata;
        database()->selectData(bdata,
                controli_missing
                        && C::Paramid(params.pid)
                        && C::Typeid(d.typeID())
                        && C::Obstime(params.UT0, d.obstime())
                        && C::Station(d.stationID()),
                order_by_time);

        // FIXME check for start of database, too
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
        database()->selectData(startdata,
                /*usei_neighbors // need to define some constraints on this value
                        &&*/ C::Paramid(params.pid)
                        && C::Typeid(d.typeID())
                        && C::Obstime(t)
                        && C::Station(d.stationID()),
                order_by_time);
        if( startdata.size() != 1 || startdata.front().original() == params.missing || startdata.front().original() == params.rejected ) {
            LOGINFO("value before time series not existing/missing/rejected/not usable at t=" << t << " for accumulation in " << d);
            continue;
        }

        if( !nf.hashStationList() ) {
            std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
            std::list<int> allStationIDs;
            fillStationLists(allStations, allStationIDs);
            nf.setStationList(allStations);
        }

        NeighboringStationFinder::stationsWithDistances_t neighbors;
        nf.findNeighbors(neighbors, d.stationID(), params.InterpolationLimit);
        if( neighbors.size() < MIN_NEIGHBORS ) {
            LOGINFO("less than " << MIN_NEIGHBORS << " neighbor(s) within " << params.InterpolationLimit
                    << " km for stationid=" << d.stationID());
            continue;
        }

        C::Station sc;
        foreach(NeighboringStationFinder::stationsWithDistances_t::value_type& v, neighbors)
            sc.add( v.first );
        dataList_t ndata;
        database()->selectData(ndata,
                usei_neighbors
                        && C::Paramid(params.pid)
                        && C::Typeid(d.typeID())
                        && C::Obstime(before.back().obstime(), d.obstime())
                        && sc,
                order_by_time_id);

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
                if (data_point == -1)
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
        if( d.original() == -1 )
            accumulated = 0;
        for(; itB != before.end() && itN != ndata.end(); ++itB ) {
            hasNeigboursWithPrecipitation.push_back(false);
            std::ostringstream cfailed;
            cfailed << "QC2N";
            float sumWeights = 0.0, sumWeightedValues = 0.0;
            for( ; itN->obstime() == itB->obstime(); ++itN ) {
                float data_point = itN->original();
                if (data_point == -1)
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
            if( d.original() == -1 || itB_corrected == 0 )
                itB_corrected = -1; // bugzilla 1304: by default assume dry

            kvalobs::kvControlInfo fixflags = itB->controlinfo();
            checkFlags().setter(fixflags, params.Sflag);
            checkFlags().conditional_setter(fixflags, params.chflag);

            kvalobs::kvData correctedData(*itB);
            correctedData.corrected(itB_corrected);
            correctedData.controlinfo(fixflags);
            Helpers::updateUseInfo(correctedData);
            Helpers::updateCfailed(correctedData, cfailed.str(), params.CFAILED_STRING);
            toWrite.push_back(correctedData);
        }

        // make sure that sum of re-distributed is the same as the original accumulated value
        int idxNeighboursWithPrecipitation = hasNeigboursWithPrecipitation.size()-1;
        float delta = round<float,1>(corrected_sum - accumulated);
        std::list<kvalobs::kvData>::reverse_iterator itS = toWrite.rbegin();
        for( ; delta > 0 && itS != toWrite.rend(); ++itS, --idxNeighboursWithPrecipitation ) {
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

        // we accumulated the corrections in time-reversed order, while tests expect them the other way around => foreach_r
        database()->insertData(toWrite, true);
        foreach_r(const kvalobs::kvData& corr, toWrite)
            broadcaster()->queueChanged(corr);
        broadcaster()->sendChanges();
    }
}
