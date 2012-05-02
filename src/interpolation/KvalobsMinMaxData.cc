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

#include "KvalobsMinMaxData.h"

#include "helpers/AlgorithmHelpers.h"
#include "helpers/timeutil.h"
#include "AlgorithmConfig.h"
#include "foreach.h"

#define NDEBUG 1
#include "debug.h"

using Interpolation::SeriesData;
using Interpolation::SupportData;

KvalobsMinMaxData::KvalobsMinMaxData(KvalobsNeighborData& nd)
        : Interpolation::MinMaxInterpolator::Data(nd)
{
}

void KvalobsMinMaxData::setMinimum(int time, Interpolation::Quality q, float value)
{
    interpolationsMin.push_back(Interpolation::SimpleResult(time, q, value));
}


void KvalobsMinMaxData::setMaximum(int time, Interpolation::Quality q, float value)
{
    interpolationsMax.push_back(Interpolation::SimpleResult(time, q, value));
}

SeriesData KvalobsMinMaxData::minimum(int time)
{
    return minmax(time, neighborData().getParameterInfo().minParameter, minimumData);
}

SeriesData KvalobsMinMaxData::maximum(int time)
{
    return minmax(time, neighborData().getParameterInfo().maxParameter, maximumData);
}

Interpolation::SeriesData KvalobsMinMaxData::minmax(int time, int paramid, KvalobsSeriesDataList& data)
{
    const ParameterInfo& pi = neighborData().getParameterInfo();
    if (data.fetchRequired()) {
        FlagSetCU all;
        const Instrument& i = neighborData().getInstrument();
        data.set(database()->findDataMaybeTSLOrderObstime(i.stationid, paramid, i.type, i.sensor,
                                                               i.level, neighborData().getTimeRange(), all));
    }

    const miutil::miTime t = timeAtOffset(time);
    return data.find(t, pi);
}

float KvalobsMinMaxData::fluctuationLevel()
{
    return neighborData().getParameterInfo().fluctuationLevel;
}
