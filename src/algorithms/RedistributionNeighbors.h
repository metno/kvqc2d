/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

  Contact information:
  Norwegian Meteorological Institute
  Postboks 43 Blindern
  N-0313 OSLO
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

#ifndef RedistributionNeighbors_H
#define RedistributionNeighbors_H 1

#include <kvalobs/kvStation.h>
#include <list>
#include <map>

class AlgorithmConfig;

class RedistributionNeighbors {
public:
    typedef std::list<int> stationIDs_t;
    typedef std::list<kvalobs::kvStation> stations_t;

protected:
    typedef std::map<int, kvalobs::kvStation> stationsByID_t;

public:
    virtual ~RedistributionNeighbors() { }

    bool hasStationList() const
        { return !mStationsByID.empty(); }

    void setStationList(const stations_t& stations);

    virtual void configure(const AlgorithmConfig& params) = 0;

    virtual stationIDs_t findNeighbors(int aroundID) = 0;

    /** Return weight for a neighbor. If weight < 1, the neighbor is
     * not really good -- weights need to be scaled accordingly.
     */
    virtual double getWeight(int neighborID) = 0;

protected:
    stationsByID_t mStationsByID;
};

#endif
