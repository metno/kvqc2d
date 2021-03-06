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

#include "SumFactory.h"

#include "AlgorithmConfig.h"
#include "DayMeanExtractor.h"

void SumFactory::configure(StatisticalMean* stm, const AlgorithmConfig& params)
{
    const float tolerance = params.getParameter<float>("tolerance", 10.0f);
    const int days = params.getParameter<int>("days", 30);
    const int daysRequired = params.getParameter<int>("days_required", int(0.9*days + 0.5));

    mAccumulator = std::make_shared<AccumulatorMeanOrSum>(false, days, daysRequired);
    mChecker = std::make_shared<CheckerMeanOrSum>(stm, tolerance);
}

bool SumFactory::appliesTo(int paramid)
{
    return ( paramid == 106 /* RR_1 */ || paramid == 108 /* RR_6 */
            || paramid == 109 /* RR_12 */ || paramid == 110 /* RR_24 */ );
}

CheckerP SumFactory::checker(int /*paramid*/)
{
    return mChecker;
}

AccumulatorP SumFactory::accumulator(int /*paramid*/)
{
    return mAccumulator;
}

DayValueExtractorP SumFactory::dayValueExtractor(int paramid)
{
    return std::make_shared<DayMeanExtractor>(false, paramid);
}
