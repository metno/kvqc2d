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
    if (minimumData.empty()) {
        FlagSetCU all;
        const Instrument& i = neighborData().getInstrument();
        minimumData = database()->findDataMaybeTSLOrderObstime(i.stationid, i.paramid, i.type, i.sensor,
                                                               i.level, neighborData().getTimeRange(), all);
    }

    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvData& d, minimumData) {
        if( d.obstime() == t && !Helpers::isMissingOrRejected(d)) {
            return SeriesData(d.original());
        }
    }
    return SeriesData();
}

SeriesData KvalobsMinMaxData::maximum(int time)
{
    if (maximumData.empty()) {
        FlagSetCU all;
        const Instrument& i = neighborData().getInstrument();
        minimumData = database()->findDataMaybeTSLOrderObstime(i.stationid, i.paramid, i.type, i.sensor,
                                                               i.level, neighborData().getTimeRange(), all);
    }

    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvData& d, maximumData) {
        if( d.obstime() == t && !Helpers::isMissingOrRejected(d)) {
            return SeriesData(d.original());
        }
    }
    return SeriesData();
}

float KvalobsMinMaxData::fluctuationLevel() const
{
    return 0.5; // TODO
}
