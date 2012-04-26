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

#ifndef NeighborsDistance2_H
#define NeighborsDistance2_H 1

#include "RedistributionNeighbors.h"

class NeighborsDistance2 : public RedistributionNeighbors {
public:
    typedef std::map<int, double> stationsWithDistances_t;

    virtual ~NeighborsDistance2() { }

    virtual void configure(const AlgorithmConfig& params);

    virtual stationIDs_t findNeighbors(int aroundID);

    virtual double getWeight(int neighborID);

private:
    float mInterpolationLimit;
    float mWarnClosest;
    stationsWithDistances_t mDistances;
};

#endif
