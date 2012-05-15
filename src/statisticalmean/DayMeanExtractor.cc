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

DayMeanExtractor::DayMeanExtractor(bool cm, int paramid)
    : mCalculateMean(cm)
    , mParamid(paramid)
{
}

void DayMeanExtractor::newDay()
{
    mHours = 0;
    mCountHours = 0;
    mSum = 0;
}

void DayMeanExtractor::addObservation(const miutil::miTime& obstime, float original)
{
    const int h = obstime.hour(), hBit = (1<<h), hp = hourPattern();
    if( hp & hBit ) {
        mHours |= hBit;
        mCountHours += 1;
        mSum += original;
    }
}

int DayMeanExtractor::hourPattern()
{
    if( mParamid == 106 /* RR_1 */ )
        return (1<<24)-1;
    if( mParamid == 108 /* RR_6 */ )
        return (1<<0 | 1<<6 | 1<<12 | 1<<18);
    if( mParamid == 109 /* RR_12 */ )
        return (1<<6 | 1<<18);
    if( mParamid == 110 /* RR_24 */ )
        return 1<<6;
    // otherwise allow everything
    return (1<<24)-1;
}

bool DayMeanExtractor::isCompleteDay()
{
    return (mHours & hourPattern()) == mHours;
}

DayValueP DayMeanExtractor::value()
{
    float v = mSum;
    if( mCalculateMean )
        v /= mCountHours;
    return boost::make_shared<DayMean>(v);
}
