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

#include "AccumulatorQuartiles.h"

#include "AccumulatedQuartiles.h"
#include "DayMean.h"
#include "helpers/mathutil.h"

void AccumulatorQuartiles::push(DayValueP value)
{
    float v = std::static_pointer_cast<DayMean>(value)->mean();
    mValues.push_back(v);
}

void AccumulatorQuartiles::pop(DayValueP value)
{
    float v = std::static_pointer_cast<DayMean>(value)->mean();
    std::vector<float>::iterator it = std::find(mValues.begin(), mValues.end(), v);
    if( it != mValues.end() )
        mValues.erase(it);
}

AccumulatedValueP AccumulatorQuartiles::value()
{
    if( mValues.empty() || (int)mValues.size() < mDaysRequired )
        return AccumulatedValueP();

    double q1, q2, q3;
    Helpers::quartiles(mValues.begin(), mValues.end(), q1, q2, q3);

    return std::make_shared<AccumulatedQuartiles>(q1, q2, q3);
}
