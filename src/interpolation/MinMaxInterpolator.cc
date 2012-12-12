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

#include "MinMaxInterpolator.h"

#include "helpers/mathutil.h"
#include "helpers/timeutil.h"
#include "helpers/WeightedMean.h"
#include "helpers/mathutil.h"

#include "gdebug.h"

namespace Interpolation {

// reconstruct parameter from complete min and max
Summary MinMaxInterpolator::interpolateFromMinMax(Data& data)
{
    Summary results;
    DBGL;
    const int duration = data.duration();
    DBGV(duration);
    for(int t=0; t<duration-1; ++t) {
        if( data.parameter(t).needsInterpolation() ) {
            const SupportData max0 = data.maximum(t), max1 = data.maximum(t+1), min0 = data.minimum(t), min1 = data.minimum(t+1);
            DBG(DBG1(t) << DBG1(max0) << DBG1(max1) << DBG1(min0) << DBG1(min1));

            float mini, maxi;
            if( min0.usable() and min1.usable() )
                mini = std::max(min0.value(), min1.value());
            else if( min0.usable() )
                mini = min0.value();
            else if( min1.usable() )
                mini = min1.value();
            else
                continue;

            if( max0.usable() and max1.usable() )
                maxi = std::min(max0.value(), max1.value());
            else if( max0.usable() )
                maxi = max0.value();
            else if( max1.usable() )
                maxi = max1.value();
            else
                continue;
            
            const float value = (maxi + mini)/2;
            // if observations for min and max are inside the
            // allowed parameter min and max value, value
            // cannot be outside either
            
            DBG("reconstruction from ..N/..X t=" << t << " value=" << value);
            data.setParameter(t, BAD, value);
            results.addOk();
        }
    }
    if( data.parameter(duration-1).needsInterpolation() ) {
        data.setParameter(duration-1, FAILED, Interpolation::MISSING_VALUE);
        results.addFailed();
    }
    return results;
}

// ------------------------------------------------------------------------

Summary MinMaxInterpolator::run(Data& data)
{
    return interpolateFromMinMax(data);
}

} // namespace Interpolation
