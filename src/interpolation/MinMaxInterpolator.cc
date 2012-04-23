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

using NeighborInterpolation::SeriesData;
using NeighborInterpolation::SupportData;

// ########################################################################

namespace {
void mmwq_helper(const SeriesData& sd, bool& usable, bool& needsInterpolation)
{
    usable &= sd.usable();
    needsInterpolation |= sd.needsInterpolation();
}

// reconstruct parameter from complete min and max
void interpolateFromMinMax(MinMaxData& data)
{
    const int duration = data.duration();
    for(int t=0; t<duration-1; ++t) {
        if( data.center(t).needsInterpolation() ) {
            const float maxi = std::min(data.maximum(t).value(), data.maximum(t+1).value());
            const float mini = std::max(data.minimum(t).value(), data.minimum(t+1).value());

            const float value = (maxi + mini)/2;
            // if observations for min and max are inside the
            // allowed parameter min and max value, value
            // cannot be outside either

            DBG("reconstruction from ..N/::X t=" << t << " value=" << value);
            data.setInterpolated(t, NeighborInterpolation::BAD, value);
        }
    }
    if( data.center(duration-1).needsInterpolation() )
        data.setInterpolated(duration-1, NeighborInterpolation::FAILED, 0);
}

void failMinMaxIfNeeded(MinMaxData& data, int time)
{
    if(data.minimum(time).needsInterpolation())
        data.setMinimum(time, NeighborInterpolation::FAILED, 0);
    if(data.maximum(time).needsInterpolation())
        data.setMaximum(time, NeighborInterpolation::FAILED, 0);
}

class CatchUpdate : public MinMaxData {
public:
    CatchUpdate(MinMaxData& w);

    virtual int duration() const
    { return wrapped.duration(); }

    virtual int neighbors() const
    { return wrapped.neighbors(); }

    virtual float maximumOffset() const
    { return wrapped.maximumOffset(); }

    virtual SeriesData center(int time)
    { return wrapped.center(time); }

    virtual SupportData model(int time)
    { return wrapped.model(time); }

    virtual SupportData transformedNeighbor(int n, int time)
    { return wrapped.transformedNeighbor(n, time); }

    virtual float neighborWeight(int neighbor)
    { return wrapped.neighborWeight(neighbor); }

    virtual void setInterpolated(int time, NeighborInterpolation::Quality q, float value);

    virtual NeighborInterpolation::SeriesData minimum(int t)
    { return wrapped.minimum(t); }

    virtual NeighborInterpolation::SeriesData maximum(int t)
    { return wrapped.maximum(t); }

    virtual void setMinimum(int time, NeighborInterpolation::Quality q, float value)
    { wrapped.setMinimum(time, q, value); }

    virtual void setMaximum(int time, NeighborInterpolation::Quality q, float value)
    { wrapped.setMaximum(time, q, value); }

    virtual float fluctuationLevel() const
    { return wrapped.fluctuationLevel(); }

    struct Update {
        int quality;
        float value;
        Update() : quality(NeighborInterpolation::FAILED) { }
    };

    const Update& getUpdate(int time) const
    { return interpolatedCenter[time]; }

private:
    MinMaxData& wrapped;
    std::vector<Update> interpolatedCenter;
};

CatchUpdate::CatchUpdate(MinMaxData& w)
: wrapped(w), interpolatedCenter(wrapped.duration())
{
    for(int t=0; t<duration(); ++t) {
        const SeriesData sd = center(t);
        if( sd.usable() ) {
            interpolatedCenter[t].value = sd.value();
            interpolatedCenter[t].quality = -1;
        }
    }
}

void CatchUpdate::setInterpolated(int time, NeighborInterpolation::Quality q, float value)
{
    SeriesData mini = wrapped.minimum(t);
    if( minimum.usable() && value < minimum.value() )
        value = minimum.value();
    SeriesData maxi = wrapped.maximum(t);
    if( maximum.usable() && value > maximum.value() )
        value = maximum.value();

    interpolatedCenter[time].value = value;
    interpolatedCenter[time].quality = q;
    wrapped.setInterpolated(time, q, value);
}

void reconstructMinMax(CatchUpdate& data)
{
    const int duration = data.duration();

    DBG("reconstruction ..N/..X");
    Akima akima;
    for(int t=0; t<duration; ++t)
        akima.add(t, data.getUpdate(t).value);

    failMinMaxIfNeeded(data, 0);
    for(int t=1; t<duration; ++t) {
        float mini = std::min(data.getUpdate(t-1).value, data.getUpdate(t).value);
        float maxi = std::max(data.getUpdate(t-1).value, data.getUpdate(t).value);
        if( akima.interpolate(t+0.5) != Akima::INVALID ) {
            const int Nbetween = 20;
            for(int j=1; j<Nbetween; ++j) {
                const float x = t + j/float(Nbetween);
                const float noise = data.fluctuationLevel() * Helpers::randNormal();
                const float akimaValue = akima.interpolate(x);
                const float value = akimaValue + noise;
                Helpers::minimize(mini, value);
                Helpers::maximize(maxi, value);
            }
            if( data.minimum(t).needsInterpolation() )
                data.setMinimum(t, NeighborInterpolation::BAD, mini);
            if( data.maximum(t).needsInterpolation() )
                data.setMaximum(t, NeighborInterpolation::BAD, maxi);
        } else {
            failMinMaxIfNeeded(data, t);
        }
    }
}

} // anonymous namespace

// ########################################################################

void MinMaxInterpolate(MinMaxData& data)
{
    const int duration = data.duration();

    bool needsPar = false, needsMin = false, needsMax = false;
    bool usablePar = true, usableMin = true, usableMax = true;
    for(int t=0; t<duration; ++t) {
        mmwq_helper(data.center(t), usablePar, needsPar);
        mmwq_helper(data.minimum(t), usableMin, needsMin);
        mmwq_helper(data.maximum(t), usableMax, needsMax);
    }
    DBG("needs par=" << needsPar << " min=" << needsMin << " max=" << needsMax);

    if( !needsPar and !needsMin and !needsMax ) {
        // nothing to do
        return;
    }

    if( needsPar and usableMin and usableMax ) {
        interpolateFromMinMax(data);
        return;
    }

    CatchUpdate cu(data);
    if( needsPar ) {
        // parameter and min and max incomplete => interpolate
        // parameter with other interpolator

        NeighborInterpolation::interpolate(cu);

        bool newCompletePar = false;
        for(int t=0; t<duration; ++t) {
            const CatchUpdate::Update& u = cu.getUpdate(t);
            newCompletePar &= ( u.quality != NeighborInterpolation::FAILED );
        }
        if( !newCompletePar ) {
            for(int t=0; t<duration; ++t)
                failMinMaxIfNeeded(cu, t);
            return;
        }
    }
    // here, mmwq.par is complete, although maybe not so good quality

    if( needsMin || needsMax )
        reconstructMinMax(cu);
}
