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

class CatchUpdate : public MinMaxInterpolator::Data {
public:
    CatchUpdate(MinMaxInterpolator::Data& w);

    virtual int duration() const
    { return wrapped().duration(); }

    virtual SeriesData parameter(int time)
    { return wrapped().parameter(time); }

    virtual void setInterpolated(int time, Quality q, float value);

    virtual SeriesData minimum(int t)
    { return wrapped().minimum(t); }

    virtual SeriesData maximum(int t)
    { return wrapped().maximum(t); }

    virtual void setMinimum(int time, Quality q, float value)
    { wrapped().setMinimum(time, q, value); }

    virtual void setMaximum(int time, Quality q, float value)
    { wrapped().setMaximum(time, q, value); }

    virtual float fluctuationLevel() const
    { return wrapped().fluctuationLevel(); }

    struct Update {
        int quality;
        float value;
        Update() : quality(FAILED) { }
    };

    const Update& getUpdate(int time) const
        { return interpolatedCenter[time]; }

    MinMaxInterpolator::Data& wrapped()
        { return mWrapped; }

    const MinMaxInterpolator::Data& wrapped() const
        { return mWrapped; }

private:
    MinMaxInterpolator::Data& mWrapped;
    std::vector<Update> interpolatedCenter;
};

CatchUpdate::CatchUpdate(MinMaxInterpolator::Data& w)
        : MinMaxInterpolator::Data(w.centerData()), mWrapped(w), interpolatedCenter(w.duration())
{
    for(int t=0; t<duration(); ++t) {
        const SeriesData sd = parameter(t);
        if( sd.usable() ) {
            interpolatedCenter[t].value = sd.value();
            interpolatedCenter[t].quality = -1;
        }
    }
}

void CatchUpdate::setInterpolated(int time, Quality q, float value)
{
    SeriesData mini = minimum(time);
    if( mini.usable() && value < mini.value() )
        value = mini.value();
    SeriesData maxi = maximum(time);
    if( maxi.usable() && value > maxi.value() )
        value = maxi.value();

    interpolatedCenter[time].value = value;
    interpolatedCenter[time].quality = q;
    wrapped().setInterpolated(time, q, value);
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
    CatchUpdate data;
    SingleParameterInterpolator& mSingleParInterpolator;
    Summary results;
};

// reconstruct parameter from complete min and max
void MinMaxImplementation::interpolateFromMinMax()
{
    const int duration = data.duration();
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

    DBG("reconstruction ..N/..X");
    Akima akima;
    for(int t=0; t<duration; ++t)
        akima.add(t, data.getUpdate(t).value);

    failMinMaxIfNeeded(0);
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
            if( data.minimum(t).needsInterpolation() ) {
                data.setMinimum(t, BAD, mini);
                results.addOk();
            }
            if( data.maximum(t).needsInterpolation() ) {
                data.setMaximum(t, BAD, maxi);
                results.addOk();
            }
        } else {
            failMinMaxIfNeeded(t);
        }
    }
}

} // anonymous namespace

// ########################################################################

Summary MinMaxImplementation::interpolate()
{
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

        mSingleParInterpolator.interpolate(data.centerData());

        bool newCompletePar = false;
        for(int t=0; t<duration; ++t) {
            const CatchUpdate::Update& u = data.getUpdate(t);
            newCompletePar &= ( u.quality != FAILED );
        }
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
    MinMaxImplementation mm(data, spi);
    return mm.interpolate();
}

} // namespace Interpolation
