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

#include "DayMeanExtractor.h"

#include <puTools/miTime.h>
#include <boost/make_shared.hpp>

void DayMeanExtractor::newDay()
{
    mHours = 0;
    mCountHours = 0;
    mSum = 0;
}

// ------------------------------------------------------------------------

void DayMeanExtractor::addObservation(const miutil::miTime& obstime, float original)
{
    int h = obstime.hour();
    if( h == 6 || h == 12 || h == 18 ) {
        mHours |= (1 << h);
        mCountHours += 1;
        mSum += original;
    }
}

// ------------------------------------------------------------------------

bool DayMeanExtractor::isCompleteDay()
{
    return (mHours == 1<<6 || mHours == (1<<6 | 1<<12 | 1<<18));
}

// ------------------------------------------------------------------------

DayValueP DayMeanExtractor::value()
{
    return boost::make_shared<DayMean>(mSum / mCountHours);
}
