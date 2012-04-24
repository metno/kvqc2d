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

#include "KvalobsDataFilter.h"

using Interpolation::Quality;
using Interpolation::SeriesData;
using Interpolation::SupportData;

KvalobsDataFilter::~KvalobsDataFilter()
{
}

SeriesData KvalobsDataFilter::toSeries(int paramid, float storage) const
{
    return SeriesData(toNumerical(paramid, storage));
}

SupportData KvalobsDataFilter::toNumerical(int paramid, float storage) const
{
    if( storage <= -32766 )
        return SupportData();
    if( paramid == 112 && (storage==-1 || storage==-3) )
        storage = 0;
    return SupportData(storage);
}

float KvalobsDataFilter::toStorage(int paramid, Quality quality, float numerical) const
{
    if( quality == Interpolation::FAILED )
        return -32766;
    if( paramid == 112 && numerical < 0.5 )
        return -1;
    return numerical;
}



