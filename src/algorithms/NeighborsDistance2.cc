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

#include "NeighborsDistance2.h"

#include "AlgorithmHelpers.h"
#include "DBConstraints.h"
#include "ReadProgramOptions.h"
#include "foreach.h"

namespace C = Constraint;
namespace O = Ordering;

// ------------------------------------------------------------------------

NeighborsDistance2::stationIDs_t NeighborsDistance2::findNeighbors(int aroundID)
{
    stationIDs_t neighborIDs;

    mDistances.clear();
    const stationsByID_t::const_iterator itAround = mStationsByID.find(aroundID);
    if( itAround != mStationsByID.end() ) {
        const kvalobs::kvStation& around = itAround->second;
        foreach(stationsByID_t::value_type& v, mStationsByID) {
            const kvalobs::kvStation& neighbor = v.second;
            if( neighbor.stationID() == aroundID )
                continue;
            const double distance = Helpers::distance(around.lon(), around.lat(), neighbor.lon(), neighbor.lat());
            if( distance > 0 && distance < mInterpolationLimit ) {
                mDistances[ neighbor.stationID() ] = distance;
                neighborIDs.push_back( neighbor.stationID() );
            }
        }
    }
    return neighborIDs;
}

// ------------------------------------------------------------------------

void NeighborsDistance2::configure(const ReadProgramOptions& params)
{
    mStationsByID.clear();
    mInterpolationLimit = params.getParameter("InterpolationDistance", 25);
}

// ------------------------------------------------------------------------

double NeighborsDistance2::getWeight(int neighborID)
{
    const double dist = mDistances.at( neighborID );
    return 1.0/(dist*dist);
}
