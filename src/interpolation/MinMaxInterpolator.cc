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

#include "helpers/Akima.h"
#include "helpers/mathutil.h"
#include "helpers/timeutil.h"

#define NDEBUG 1
#include "debug.h"

namespace Interpolation {

namespace {
void mmwq_helper(const SeriesData& sd, bool& usable, bool& needsInterpolation)
{
    usable &= sd.usable();
    needsInterpolation |= sd.needsInterpolation();
}

class MinMaxImplementation {
public:
    MinMaxImplementation(MinMaxInterpolator::Data& d, SingleParameterInterpolator& spi)
        : data(d), mSingleParInterpolator(spi) { }

    void interpolateFromMinMax();
    void failMinMaxIfNeeded(int time);
    void reconstructMinMax();
    Summary interpolate();

private:
    MinMaxInterpolator::Data& data;
    SingleParameterInterpolator& mSingleParInterpolator;
    Summary results;
};

// reconstruct parameter from complete min and max
void MinMaxImplementation::interpolateFromMinMax()
{
    DBGL;
    const int duration = data.duration();
    DBGV(duration);
    for(int t=0; t<duration-1; ++t) {
        if( data.parameter(t).needsInterpolation() ) {
            const float maxi = std::min(data.maximum(t).value(), data.maximum(t+1).value());
            const float mini = std::max(data.minimum(t).value(), data.minimum(t+1).value());

            const float value = (maxi + mini)/2;
            // if observations for min and max are inside the
            // allowed parameter min and max value, value
            // cannot be outside either

            DBG("reconstruction from ..N/::X t=" << t << " value=" << value);
            data.setInterpolated(t, BAD, value);
        }
    }
    if( data.parameter(duration-1).needsInterpolation() )
        data.setInterpolated(duration-1, FAILED, 0);
}

void MinMaxImplementation::failMinMaxIfNeeded(int time)
{
    DBGV(time);
    if(data.minimum(time).needsInterpolation()) {
        data.setMinimum(time, FAILED, 0);
        results.addFailed();
    }
    if(data.maximum(time).needsInterpolation()) {
        data.setMaximum(time, FAILED, 0);
        results.addFailed();
    }
}

void MinMaxImplementation::reconstructMinMax()
{
    const int duration = data.duration();
    SingleParameterInterpolator::Data& cd = data.centerData();

    DBG("reconstruction ..N/..X");
    Akima akima;
    for(int t=0; t<duration; ++t)
        akima.add(t, cd.getInterpolated(t).value);

    failMinMaxIfNeeded(0);
    for(int t=1; t<duration; ++t) {
        const bool minNeeded = data.minimum(t).needsInterpolation();
        const bool maxNeeded = data.maximum(t).needsInterpolation();
        DBG(DBG1(t) << DBG1(minNeeded) << DBG1(maxNeeded));
        if( !minNeeded && !maxNeeded )
            continue;

        const SimpleResult i0 = cd.getInterpolated(t-1), i1 = cd.getInterpolated(t);
        const bool canUseAkima = ( akima.interpolate(t+0.5) != Akima::INVALID );
        DBG(DBG1(i0.quality) << DBG1(i1.quality) << DBG1(canUseAkima ));
        if( i0.quality == FAILED || i1.quality == FAILED || !canUseAkima ) {
            failMinMaxIfNeeded(t);
            continue;
        }

        float mini = std::min(i0.value, i1.value);
        float maxi = std::max(i0.value, i1.value);
        const int Nbetween = 20;
        for(int j=1; j<Nbetween; ++j) {
            const float x = t + j/float(Nbetween);
            const float noise = data.fluctuationLevel() * Helpers::randNormal();
            const float akimaValue = akima.interpolate(x);
            const float value = akimaValue + noise;
            Helpers::minimize(mini, value);
            Helpers::maximize(maxi, value);
        }
        DBG(DBG1(mini) << DBG1(maxi));
        if( minNeeded ) {
            data.setMinimum(t, BAD, mini);
            results.addOk();
        }
        if( maxNeeded ) {
            data.setMaximum(t, BAD, maxi);
            results.addOk();
        }
    }
}

} // anonymous namespace

// ########################################################################

Summary MinMaxImplementation::interpolate()
{
    DBGL;
    const int duration = data.duration();

    bool needsPar = false, needsMin = false, needsMax = false;
    bool usablePar = true, usableMin = true, usableMax = true;
    for(int t=0; t<duration; ++t) {
        mmwq_helper(data.parameter(t), usablePar, needsPar);
        mmwq_helper(data.minimum(t), usableMin, needsMin);
        mmwq_helper(data.maximum(t), usableMax, needsMax);
    }
    DBG("needs par=" << needsPar << " min=" << needsMin << " max=" << needsMax);

    if( !needsPar and !needsMin and !needsMax ) {
        // nothing to do
        return results;
    }

    if( needsPar and usableMin and usableMax ) {
        interpolateFromMinMax();
        return results;
    }

    if( needsPar ) {
        // parameter and min and max incomplete => interpolate
        // parameter with other interpolator

        SingleParameterInterpolator::Data& cd = data.centerData();
        mSingleParInterpolator.interpolate(cd);

        bool newCompletePar = true;
        for(int t=0; t<duration; ++t) {
            const SimpleResult sr = cd.getInterpolated(t);
            DBG(DBG1(t) << DBG1(sr.quality));
            newCompletePar &= ( sr.quality != FAILED );
        }
        DBGV(newCompletePar);
        if( !newCompletePar ) {
            for(int t=0; t<duration; ++t)
                failMinMaxIfNeeded(t);
            return results;
        }
    }
    // here, mmwq.par is complete, although maybe not so good quality

    if( needsMin || needsMax )
        reconstructMinMax();
    return results;
}

Summary MinMaxInterpolator::interpolate(Data& data, SingleParameterInterpolator& spi)
{
    DBGL;
    MinMaxImplementation mm(data, spi);
    return mm.interpolate();
}

} // namespace Interpolation
