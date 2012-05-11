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

#include "QuartilesFactory.h"

#include "AlgorithmConfig.h"
#include <boost/make_shared.hpp>

void QuartilesFactory::configure(StatisticalMean* stm, const AlgorithmConfig& params)
{
    const float tolerance = params.getParameter<float>("tolerance", 10.0f);
    const int days = params.getParameter<int>("days", 30);
    const int daysRequired = params.getParameter<int>("days_required", int(0.9*days + 0.5));

    mAccumulator = boost::make_shared<AccumulatorQuartiles>(days, daysRequired);
    mChecker = boost::make_shared<CheckerQuartiles>(stm, days, std::vector<float>(6, tolerance));
}

bool QuartilesFactory::appliesTo(int paramid)
{
    return ( paramid == 262 /* UU */ || paramid == 15 /* NN */ || paramid == 55 /* HL */
            || paramid == 273 /* VV */ || paramid == 200 /* QO */ );
}

CheckerP QuartilesFactory::checker(int /*paramid*/)
{
    return mChecker;
}

AccumulatorP QuartilesFactory::accumulator(int /*paramid*/)
{
    return mAccumulator;
}
