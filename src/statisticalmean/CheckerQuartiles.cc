/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011 met.no

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

#include "CheckerQuartiles.h"

#include "AccumulatedQuartiles.h"
#include <boost/format.hpp>
#include <cmath>

CheckerQuartiles::CheckerQuartiles(StatisticalMean* sm, int days, const std::vector<float>& tolerances)
: Checker(sm), mDays(days), mTolerances(tolerances)
{ }

// ------------------------------------------------------------------------

bool CheckerQuartiles::calculateDiffsToReferences(int station, int dOy, float diffs[3], AccumulatedValueP accumulated)
{
    AccumulatedQuartilesP quartiles = boost::static_pointer_cast<AccumulatedQuartiles>(accumulated);
    bool valid[3] = { false, false, false };
    for(int i=0; i<3; ++i) {
        const float ref = getReference(station, dOy, (boost::format("ref_q%1$d") % (i+1)).str(), valid[i]);
        diffs[i] = valid[i] ? std::fabs(quartiles->q(i) - ref) : 0;
    }
    if( !valid[0] || !valid[1] || !valid[2] ) {
        Message w = warning();
        w << "Missing quartile reference values ";
        for(int i=0; i<3; ++i) {
            if( !valid[i] )
                w << (i+1) << ' ';
        }
        w << "for station " << station;
    }
    return (valid[0] || valid[1] || valid[2]);
}

// ------------------------------------------------------------------------

bool CheckerQuartiles::newCenter(int id, int dayOfYear, AccumulatedValueP accumulated)
{
    mCenter = id;
    mDayOfYear = dayOfYear;
    mCountNeighborsWithError = 0;

    if( calculateDiffsToReferences(mCenter, dayOfYear, mDiffsQ, accumulated) ) {
        return mDiffsQ[0] <= mTolerances[1] && mDiffsQ[1] <= mTolerances[3] && mDiffsQ[2] <= mTolerances[5];
    } else {
        return true;
    }
}

// ------------------------------------------------------------------------

bool CheckerQuartiles::checkNeighbor(int nbr, AccumulatedValueP accumulated)
{
    float nDiffsQ[3];
    if( calculateDiffsToReferences(nbr, mDayOfYear, nDiffsQ, accumulated) ) {
        const bool error = (mDiffsQ[0] > mTolerances[1] && mDiffsQ[0] - nDiffsQ[0] > mTolerances[0])
            || (mDiffsQ[1] > mTolerances[3] && mDiffsQ[1] - nDiffsQ[1] > mTolerances[2])
            || (mDiffsQ[2] > mTolerances[5] && mDiffsQ[2] - nDiffsQ[2] > mTolerances[4]);
        if( error )
            mCountNeighborsWithError += 1;
    }
    return mCountNeighborsWithError <= 3;
}

// ------------------------------------------------------------------------

bool CheckerQuartiles::pass()
{
    return mCountNeighborsWithError < 3;
}
