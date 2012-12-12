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

#include "KvalobsNeighborData.h"

#include "GapInterpolationAlgorithm.h"
#include "helpers/AlgorithmHelpers.h"
#include "helpers/timeutil.h"
#include "AlgorithmConfig.h"
#include "FlagPatterns.h"
#include "foreach.h"

using Interpolation::SeriesData;
using Interpolation::SupportData;

KvalobsNeighborData::KvalobsNeighborData(GapData& data)
    : mData(data)
{
}

float KvalobsNeighborData::maximumOffset()
{
    return mData.mParameterInfo.maxOffset;
}

int KvalobsNeighborData::duration()
{
    return mData.duration();
}

SeriesData KvalobsNeighborData::parameter(int time)
{
    return mData.parameter(time);
}

void KvalobsNeighborData::setParameter(int time, Interpolation::Quality q, float value)
{
    mData.setParameter(time, q, value);
}

SupportData KvalobsNeighborData::model(int time)
{
    return mData.model(time);
}

int KvalobsNeighborData::neighborCount()
{
    return mData.neighborCount();
}

SupportData KvalobsNeighborData::transformedNeighbor(int n, int time)
{
    return mData.transformedNeighbor(n, time);
}

float KvalobsNeighborData::neighborWeight(int neighbor)
{
    return mData.neighborWeight(neighbor);
}
