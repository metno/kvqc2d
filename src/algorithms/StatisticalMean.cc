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

#include "StatisticalMean.h"

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

StatisticalMean::StatisticalMean()
    : Qc2Algorithm("StatisticalMean")
    , mNeighbors(new NeighborsDistance2())
{
}

// ------------------------------------------------------------------------

std::list<int> StatisticalMean::findNeighbors(int stationID)
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

void StatisticalMean::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    params.getFlagSetCU(ok_flags,      "ok", "", "U2=0");

    mTolerance = params.getParameter<int>("tolerance", 10.0f);
    mDays      = params.getParameter<int>("days", 30);
    mDaysRequired = params.getParameter<int>("days_required", int(0.9*mDays + 0.5));
    mParamid   = params.getParameter<int>("ParamId");
    mTypeids   = params.getMultiParameter<int>("TypeIds");

    mUT0extended = UT0;
    mUT0extended.addDay(-mDays);

    mNeighbors->configure(params);
}

// ------------------------------------------------------------------------

class DayMean {
public:
    DayMean(int d, float m)
        : mDay(d), mMean(m) { }
    int day() const
        { return mDay; }
    float mean() const
        { return mMean; }

    int normalisedDayOfYear(const miutil::miDate& day0) const;
    
private:
    int mDay;
    float mMean;
};

int DayMean::normalisedDayOfYear(const miutil::miDate& day0) const
{
    miutil::miDate date(day0);
    date.addDay(mDay);
    return Helpers::normalisedDayOfYear(date);
}

void StatisticalMean::run()
{
    std::list<kvalobs::kvData> sdata;
    const C::DBConstraint cData = C::Paramid(mParamid) && C::Typeid(mTypeids)
            && C::Obstime(mUT0extended, UT1);
    database()->selectData(sdata, cData, O::Obstime().asc());
    
    typedef std::vector<kvalobs::kvData> dlist_t;
    typedef std::map<int, dlist_t > smap_t;
    smap_t smap;
    foreach(const kvalobs::kvData& d, sdata) {
        smap[d.stationID()].push_back(d);
    }

    sdata.clear(); // release memory

    typedef std::vector<DayMean> dm_t;
    typedef std::map<int, dm_t> sdm_t;
    sdm_t stationDailyMeans;

    const miutil::miDate date0 = mUT0extended.date();
    const int day0 = date0.julianDay();

    // calculate daily mean values
    foreach(const smap_t::value_type& sd, smap) {
        const dlist_t& dl = sd.second;
        for(dlist_t::const_iterator itB = dl.begin(), itE=itB; itB != dl.end(); itB = itE) {
            const int day = itB->obstime().date().julianDay() - day0;
            int hours = 0;
            for( ; itE != dl.end() && (itE->obstime().date().julianDay() - day0) == day; itE++ )
                hours |= (1 << itE->obstime().hour());
            float dayMean;
            if( hours == (1<<6) ) {
                dayMean = itB->original();
            } else {
                int n = 0;
                dayMean = 0;
                for(dlist_t::const_iterator itA = itB; itA != itE; itA++ ) {
                    const int h = itA->obstime().hour();
                    if( h == 6 || h == 12 || h == 18 ) {
                        n += 1;
                        dayMean += itA->original();
                    }
                }
                if( n != 3 ) {
                    // bad day
                    continue;
                } else {
                    dayMean /= 3;
                }
            }
            DayMean dm(day, dayMean);
            stationDailyMeans[sd.first].push_back(dm);
        }
    }

    smap.clear(); // release memory

    typedef std::map<int,float> dm2_t;
    typedef std::map<int, dm2_t> sd2_t;
    sd2_t stationMeansPerDay;

    foreach(const sdm_t::value_type& sd, stationDailyMeans) {
        const dm_t& dml = sd.second;
        float sum = 0;
        dm_t::const_iterator tail = dml.begin(), head = tail;
        while(head != dml.end()) {
            while( tail != head && head->day() - tail->day() >= mDays ) {
                if( tail->mean() > missing )
                    sum -= tail->mean();
                tail++;
            }
            int day = head->day();
            while( head != dml.end() && head->day() - tail->day() < mDays ) {
                if( head->mean() > missing )
                    sum += head->mean();
                day = head->day();
                head++;
            }
            const int nDays = head - tail;
            if( nDays>0 && nDays >= mDaysRequired ) {
                const float mean = sum / nDays;
                stationMeansPerDay[sd.first][day] = mean;
            }
        }
    }

    stationDailyMeans.clear(); // release memory

    foreach(const sd2_t::value_type& sd, stationMeansPerDay) {
        const int center = sd.first;
        foreach(const dm2_t::value_type& dm, sd.second) {
            const int day = dm.first;
            const float mean = dm.second;

            miutil::miDate date(date0);
            date.addDay(day);
            if( date < UT0.date() )
                continue;
            const int referenceDayOfYear = (Helpers::normalisedDayOfYear(date) + 365 - mDays/2) % 365;
            const float referenceMean = getStatisticalMean(center, referenceDayOfYear);
            if( fabs(mean - referenceMean) > mTolerance ) {
                std::list<int> neighbors = findNeighbors(center);
                int nNeighborsBelowTolerance = 0;
                foreach(int n, neighbors) {
                    sd2_t::const_iterator itN = stationMeansPerDay.find(n);
                    if( itN != stationMeansPerDay.end() ) {
                        const dm2_t& ndata = itN->second;
                        dm2_t::const_iterator itD = ndata.find(day);
                        if( itD != ndata.end() ) {
                            const float nReferenceMean = getStatisticalMean(n, referenceDayOfYear);
                            const float nMean = itD->second;
                            if( fabs(nMean - nReferenceMean) < mTolerance )
                                nNeighborsBelowTolerance += 1;
                        }
                    }
                }
                if( nNeighborsBelowTolerance >= 3 ) {
                    warning() << "tolerance exceeded for station " << center
                              << " for series ending at " << date
                              << " mean=" << mean << " ref=" << referenceMean;
                }
            }
        }
    }
}

// ------------------------------------------------------------------------

float StatisticalMean::getStatisticalMean(int station, int dayOfYear)
{
    return 1014;
}
