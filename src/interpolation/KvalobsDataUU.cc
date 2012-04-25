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

#include "KvalobsDataUU.h"

#include "helpers/AlgorithmHelpers.h"
#include "helpers/FormulaUU.h"
#include "foreach.h"

using Interpolation::SeriesData;
using Interpolation::SupportData;

KvalobsUUNeighborData::KvalobsUUNeighborData(KvalobsNeighborData& dUU, KvalobsNeighborData& dTA)
        : dataUU(dUU), dataTA(dTA)
{
}

SeriesData KvalobsUUNeighborData::parameter(int time)
{
    const SeriesData sdUU = dataUU.parameter(time), sdTA = dataTA.parameter(time);
    if( !sdUU.usable() || !sdTA.usable() )
        return sdUU;

    return SeriesData(Helpers::formulaTD(sdTA.value(), sdUU.value()));
}

SupportData KvalobsUUNeighborData::transformedNeighbor(int n, int time)
{
    const SupportData sdUU = dataUU.neighbor(n, time), sdTA = dataTA.neighbor(n, time);
    if( !sdUU.usable() || !sdTA.usable() )
        return sdUU;

    const NeighborData& nd = dataUU.getNeighborData(n);
    const float value = nd.offset +
            (nd.slope * Helpers::formulaTD(sdTA.value(), sdUU.value()));
    return SupportData(value);
}

void KvalobsUUNeighborData::setInterpolated(int time, Interpolation::Quality q, float value)
{
    const SupportData sdTA = dataTA.parameter(time);
    value = Helpers::formulaUU(sdTA.value(), value);
    dataUU.setInterpolated(time, q, value);
}



KvalobsUUMinMaxData::KvalobsUUMinMaxData(KvalobsUUNeighborData& ndata, KvalobsMinMaxData& dUU, KvalobsMinMaxData& dTA)
        : Interpolation::MinMaxInterpolator::Data(ndata), dataUU(dUU), dataTA(dTA)
{
}

SeriesData KvalobsUUMinMaxData::minimum(int time)
{
    const SeriesData sdUU = dataUU.minimum(time), sdTA = dataTA.minimum(time);
    if( !sdUU.usable() || !sdTA.usable() )
        return sdUU;

    return SeriesData(Helpers::formulaTD(sdTA.value(), sdUU.value()));
}

SeriesData KvalobsUUMinMaxData::maximum(int time)
{
    const SeriesData sdUU = dataUU.maximum(time), sdTA = dataTA.maximum(time);
    if( !sdUU.usable() || !sdTA.usable() )
        return sdUU;

    return SeriesData(Helpers::formulaTD(sdTA.value(), sdUU.value()));
}

void KvalobsUUMinMaxData::setMinimum(int time, Interpolation::Quality q, float value)
{
    const SupportData sdTA = dataTA.parameter(time);
    value = Helpers::formulaUU(sdTA.value(), value);
    dataUU.setMinimum(time, q, value);
}

void KvalobsUUMinMaxData::setMaximum(int time, Interpolation::Quality q, float value)
{
    const SupportData sdTA = dataTA.parameter(time);
    value = Helpers::formulaUU(sdTA.value(), value);
    dataUU.setMaximum(time, q, value);
}

