/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2012 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
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

#include "InterpolatorUU.h"

#include "helpers/Akima.h"
#include "helpers/AlgorithmHelpers.h"
#include "helpers/FormulaUU.h"
#include "helpers/Helpers.h"

#define NDEBUG 1
#include "debug.h"

using Helpers::formulaTD;
using Helpers::formulaUU;

// ########################################################################

std::vector<float> DataAccessUU::fetchObservations(const Instrument& instrumentUU, const TimeRange& t)
{
    Instrument instrumentTA = instrumentUU;
    instrumentTA.paramid = 211;

    const std::vector<float> seriesUU = mDax->fetchObservations(instrumentUU, t);
    const std::vector<float> seriesTA = mDax->fetchObservations(instrumentTA, t);

    std::vector<float> series(seriesUU.size());
    for(unsigned int i=0; i<series.size(); ++i) {
        series[i] = formulaTD(seriesTA[i], seriesUU[i]);
        DBG("UU=" << seriesUU[i] << " TA=" << seriesTA[i] << " => UU=" << series[i]);
    }

    return series;
}

// ------------------------------------------------------------------------

std::vector<float> DataAccessUU::fetchModelValues (const Instrument&, const TimeRange& t)
{
    return std::vector<float>(t.hours()+1, ::Interpolator::INVALID);
}

// ------------------------------------------------------------------------

NeighborDataVector DataAccessUU::findNeighbors(const Instrument& instrument, double maxsigma)
{
    return mDax->findNeighbors(instrument, maxsigma);
}

// ========================================================================

Interpolator::ValuesWithQualities_t InterpolatorUU::interpolate(const Instrument& instrumentUU, const TimeRange& t)
{
    CorrelatedNeighbors::DataAccess* dax = mInterpolator->getDataAccess();
    mInterpolator->setDataAccess(mDataAccess);
    ValuesWithQualities_t interpolatedTD = mInterpolator->interpolate(instrumentUU, t);
    DBGV(interpolatedTD.size());
    mInterpolator->setDataAccess(dax);

    Instrument instrumentTA = instrumentUU;
    instrumentTA.paramid = 211;

    const std::vector<float> seriesTA = dax->fetchObservations(instrumentTA, t);
    DBGV(seriesTA.size());
    for(unsigned int i=0; i<interpolatedTD.size(); ++i) {
        const float TD = interpolatedTD[i].value, TA = seriesTA[i+1];
        const float UU = formulaUU(TA, TD);
        interpolatedTD[i].value = UU;
        DBG("TD=" << TD << " TA=" << TA << " => UU=" << UU);
    }

    return interpolatedTD;
}
