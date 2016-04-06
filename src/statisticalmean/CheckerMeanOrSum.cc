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

#include "CheckerMeanOrSum.h"
#include <cmath>

bool CheckerMeanOrSum::newCenter(int id, int dayOfYear, AccumulatedValueP accumulated)
{
    mCenter = id;
    mDayOfYear = dayOfYear;
    mCountNeighborsBelowTolerance = 0;

    bool referenceValid;
    const float reference = getReference(id, dayOfYear, "ref_value", referenceValid);
    //bool toleranceValid;
    //const float tolerance = getReference(id, dayOfYear, "tolerance", referenceValid);
    const float value = std::static_pointer_cast<AccumulatedFloat>(accumulated)->value;
    return !referenceValid || std::fabs(value - reference) < mTolerance;
}

// ------------------------------------------------------------------------

bool CheckerMeanOrSum::checkNeighbor(int nbr, AccumulatedValueP accumulated)
{
    bool referenceValid;
    const float reference = getReference(nbr, mDayOfYear, "ref_value", referenceValid);
    const float value = std::static_pointer_cast<AccumulatedFloat>(accumulated)->value;
    if( referenceValid && std::fabs(value - reference) < mTolerance )
        mCountNeighborsBelowTolerance += 1;
    return mCountNeighborsBelowTolerance <= 3;
}

// ------------------------------------------------------------------------

bool CheckerMeanOrSum::pass()
{
    return mCountNeighborsBelowTolerance < 3;
}
