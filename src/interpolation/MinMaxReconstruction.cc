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

#include "MinMaxReconstruction.h"

#include "helpers/Akima.h"
#include "helpers/mathutil.h"
#include "helpers/timeutil.h"
#include "helpers/WeightedMean.h"
#include "helpers/mathutil.h"

#include "gdebug.h"

namespace Interpolation {

void MinMaxReconstruction::failMinMaxIfNeeded(Data& data, int time, Summary& results)
{
    DBGV(time);
    if(data.minimum(time).needsInterpolation()) {
        data.setMinimum(time, FAILED, Interpolation::MISSING_VALUE);
        results.addFailed();
    }
    if(data.maximum(time).needsInterpolation()) {
        data.setMaximum(time, FAILED, Interpolation::MISSING_VALUE);
        results.addFailed();
    }
}

#define AKIMA_MIN_MAX 1

Summary MinMaxReconstruction::reconstructMinMax(Data& data)
{
    Summary results;
    const int duration = data.duration();

    DBG("reconstruction ..N/..X");
    Akima akima;
#ifdef AKIMA_MIN_MAX
    Akima akimaMin, akimaMax;
#endif
    for(int t=0; t<duration; ++t) {
        const SupportData i = data.parameter(t);
        if( i.usable() )
            akima.add(t, i.value());
#ifdef AKIMA_MIN_MAX
        const SeriesData smin = data.minimum(t), smax = data.maximum(t);
        DBG(DBG1(t) << DBG1(smin) << DBG1(smax));
        if (smin.usable() and not smin.needsInterpolation())
            akimaMin.add(t, smin.value());
        if (smax.usable() and not smax.needsInterpolation())
            akimaMax.add(t, smax.value());
#endif
    }

    // we do not have data for t=-1, so first point always fails if it needs interpolation
    failMinMaxIfNeeded(data, 0, results);

    for(int t=1; t<duration; ++t) {
        const bool minNeeded = data.minimum(t).needsInterpolation();
        const bool maxNeeded = data.maximum(t).needsInterpolation();
        DBG(DBG1(t)
            << DBG1(minNeeded) << "(q=" << data.minimum(t).quality() << ')'
            << DBG1(maxNeeded) << "(q=" << data.maximum(t).quality() << ')');
        if( !minNeeded && !maxNeeded )
            continue;

        // TODO maybe find a more efficient algorithm
        float diff_min = 1.5e6, diff_max = 1.5e6;
        for(int t_before = t-1; t_before >= 0; --t_before) {
            const SupportData d = data.parameter(t_before);
            if( d.usable() )
                continue;
            const SeriesData smin = data.minimum(t_before), smax = data.maximum(t_before);
            if( !smin.needsInterpolation() )
                Helpers::minimize(diff_min, d.value() - smin.value());
            if( !smax.needsInterpolation() )
                Helpers::minimize(diff_max, smax.value() - d.value());
        }
        for(int t_after = t+1; t_after < duration; ++t_after) {
            const SupportData d = data.parameter(t_after);
            if( d.usable() )
                continue;
            const SeriesData smin = data.minimum(t_after), smax = data.maximum(t_after);
            if( !smin.needsInterpolation() )
                Helpers::minimize(diff_min, d.value() - smin.value());
            if( !smax.needsInterpolation() )
                Helpers::minimize(diff_max, smax.value() - d.value());
        }
        if( diff_min >= 1e6 )
            diff_min = 0;
        if( diff_max >= 1e6 )
            diff_max = 0;

        const SupportData i0 = data.parameter(t-1), i1 = data.parameter(t);
        const bool canUseAkima = (akima.distance(t+0.5) < 1.5);
        DBG(DBG1(i0.usable()) << DBG1(i1.usable()) << DBG1(canUseAkima ));
        if (not (i0.usable() and i1.usable() and canUseAkima)) {
            DBGL;
            failMinMaxIfNeeded(data, t, results);
            continue;
        }

        float mini = std::min(i0.value(), i1.value());
        float maxi = std::max(i0.value(), i1.value());
        DBG(DBG1(mini) << DBG1(maxi));
        const int Nbetween = 20;
        for(int j=1; j<Nbetween; ++j) {
            const float x = t-1 + j/float(Nbetween);
            const float noise = 0;
            const float akimaValue = akima.interpolate(x);
            const float value = akimaValue + noise;
            Helpers::minimize(mini, value-diff_min);
            Helpers::maximize(maxi, value+diff_max);
        }
#ifdef AKIMA_MIN_MAX
        if( akimaMin.distance(t) < 1.5 )
            Helpers::minimize(mini, static_cast<float>(akimaMin.interpolate(t)));
        if( akimaMax.distance(t) < 1.5 )
            Helpers::maximize(maxi, static_cast<float>(akimaMax.interpolate(t)));
#endif
        if( minNeeded ) {
            DBGV(mini);
            data.setMinimum(t, BAD, mini);
            results.addOk();
        }
        if( maxNeeded ) {
            DBGV(maxi);
            data.setMaximum(t, BAD, maxi);
            results.addOk();
        }
    }
    return results;
}


// ########################################################################

Summary MinMaxReconstruction::run(Data& data)
{
    return reconstructMinMax(data);
}

} // namespace Interpolation
