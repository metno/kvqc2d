/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

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

#include "NeighborInterpolator.h"

#include "helpers/Akima.h"
#include "helpers/WeightedMean.h"
#include "foreach.h"

#include <cmath>
#include <stdexcept>

#include "gdebug.h"

namespace Interpolation {

namespace {

const int MAX_NEIGHBORS = 5;
const float AKIMAWEIGHT = 2;
const bool akimaFirst = false;
typedef std::vector<SupportData> SupportVector;

struct OffsetCorrection {
    float slope, offset;
    float at(float x) const
        { return offset + x*slope; }
    OffsetCorrection()
        : slope(0), offset(0) { }
};

class NeighborImplementation {
public:
    NeighborImplementation(NeighborInterpolator::Data& data);
    Interpolation::Summary interpolate();

private:
    void interpolateSimple();
    void setupAkima();
    void interpolateGaps();
    void interpolateSingleGap();
    void interpolateSinglePoint(int t);

    OffsetCorrection calculateOffset(const SupportData &o0, const SupportData &o1, int t0, int t1);
    void calculateOffsets(int t0, int t1);
    void failure(int time);
    void success(int time, Interpolation::Quality q, float value);

private:
    NeighborInterpolator::Data& data;

    std::vector<SupportData> simpleInterpolations;
    Akima akima;

    int beforeGap, afterGap;
    OffsetCorrection ocModel;
    OffsetCorrection ocObservations;
    Interpolation::Summary result;
};

NeighborImplementation::NeighborImplementation(NeighborInterpolator::Data& d)
: data(d)
{
}

Interpolation::Summary NeighborImplementation::interpolate()
{
    interpolateSimple();
    setupAkima();
    interpolateGaps();
    DBG(DBG1(result.nOk()) << DBG1(result.nFailed()));
    return result;
}

void NeighborImplementation::interpolateSimple()
{
    const unsigned int duration = data.duration();
    simpleInterpolations = SupportVector(duration);

    const int nc = data.neighborCount();
    DBGV(nc);

    for(unsigned int t=0; t<duration; ++t) {
        Helpers::WeightedMean wm;
        for(int n=0; n<nc && wm.count() < MAX_NEIGHBORS; ++n) {
            const SupportData nd = data.transformedNeighbor(n, t);
            if( nd.usable() )
                wm.add(nd.value(), data.neighborWeight(n));
        }
        if( wm.valid() ) {
            simpleInterpolations[t] = SupportData(wm.mean());
            DBG(DBG1(t) << DBG1(simpleInterpolations[t].value()));
        }
    }
}

void NeighborImplementation::setupAkima()
{
    DBGL;
    const int d = data.duration();
    for(int t = 0; t<d; ++t) {
        const SupportData obs = data.parameter(t);
        DBG(DBG1(t) << DBG1(obs.value()) << DBG1(obs.usable()));
        if( obs.usable() ) {
            akima.add(t, obs.value());
            DBGL;
        }
    }
}

void NeighborImplementation::interpolateGaps()
{
    DBGL;
    const int duration = data.duration();

// FIXME this is wrong
    //beforeGap = NeighborInterpolator::EXTRA_DATA - 1; // assume this is an observed value
    beforeGap = 0;
// FIXME this is wrong
    //const int afterGaps = duration - NeighborInterpolator::EXTRA_DATA;
    const int afterGaps = duration;
    while( beforeGap < afterGaps ) {
        if( !data.parameter(beforeGap).usable() ) {
            beforeGap += 1;
            continue;
        }
        afterGap = beforeGap+1;
        while( afterGap < afterGaps && !data.parameter(afterGap).usable() )
            ++afterGap;

        if( afterGap < afterGaps && afterGap != beforeGap + 1 )
            interpolateSingleGap();
        beforeGap = afterGap;
    }
}

void NeighborImplementation::interpolateSingleGap()
{
    DBGL;
    const int gap = afterGap - beforeGap - 1;

    calculateOffsets(beforeGap, afterGap);

    const int t0 = beforeGap + 1;
    for(int t=t0; t<t0+gap; ++t) {
        const SeriesData obs = data.parameter(t);
        DBG(DBG1(t) << DBG1(obs.needsInterpolation()));
        if( obs.needsInterpolation() )
            interpolateSinglePoint(t);
    }
}

OffsetCorrection NeighborImplementation::calculateOffset(const SupportData &o0, const SupportData &o1, int t0, int t1)
{
    DBG(DBG1(t0) << DBG1(t1) << DBG1(data.duration()));
    OffsetCorrection oc;
    const SeriesData &d0 = data.parameter(t0), &d1 = data.parameter(t1);
    const bool have0 = (d0.usable() && o0.usable()), have1 = (d1.usable() && o1.usable());
    const float delta0 = o0.value() - d0.value(), delta1 = o1.value() - d1.value();
    const int N = t1 - t0;
    if( have0 && have1 && N!=0 ) {
        oc.slope = (delta1 - delta0)/N;
        oc.offset = delta0 - oc.slope*t0;
    } else if( have0 ) {
        oc.offset = delta0;
    } else if( have1 ) {
        oc.offset = delta1;
    }
    return oc;
}

void NeighborImplementation::calculateOffsets(int t0, int t1)
{
    DBGL;
    ocObservations = calculateOffset(simpleInterpolations[t0], simpleInterpolations[t1], t0, t1);
    ocModel = calculateOffset(data.model(t0), data.model(t1), t0, t1);
}

void NeighborImplementation::interpolateSinglePoint(int t)
{
    DBGL;
    const bool atStartOrEnd = (t==beforeGap+1 || t==afterGap-1);
    const int gap = afterGap - beforeGap - 1;
    const bool longGap = (gap>12);
    const bool canUseAkima = (akima.distance(t) < 1.5);
    DBG(DBG1(t) << DBG1(akima.distance(t)) << DBG1(canUseAkima));
    if( longGap ) {
        if( atStartOrEnd and canUseAkima )
            success(t, BAD, akima.interpolate(t));
        else
            failure(t);
        return;
    }

    Helpers::WeightedMean combi;

    if( akimaFirst && canUseAkima )
        combi.add(akima.interpolate(t), AKIMAWEIGHT);

    const SupportData& inter = simpleInterpolations[t];
    DBG(DBG1(simpleInterpolations[t].value()) << DBG1(simpleInterpolations[t].usable()));
    if( !combi.valid() && inter.usable() ) {
        DBGL;
        const float delta = ocObservations.at(t);
        if( std::fabs(delta) < data.maximumOffset()) {
            DBGV(inter.value());
            combi.add(inter.value() - delta, 1);
        }
    }

    if( !akimaFirst && !combi.valid() && canUseAkima )
        combi.add(akima.interpolate(t), AKIMAWEIGHT);

    const SupportData& model = data.model(t);
    if( !combi.valid() && model.usable() ) {
        const float delta = ocModel.at(t);
        if( std::fabs(delta) < data.maximumOffset() )
            combi.add(model.value() - delta, 1);
    }

    if( combi.valid() ) {
        success(t, atStartOrEnd ? GOOD : BAD, combi.mean());
    } else {
        failure(t);
    }
}

void NeighborImplementation::failure(int time)
{
    DBGV(time);
    data.setParameter(time, Interpolation::FAILED, 0);
    result.addFailed();
}

void NeighborImplementation::success(int time, Interpolation::Quality q, float value)
{
    DBGV(time);
    data.setParameter(time, q, value);
    result.addOk();
}

} // anonymous namespace

const int NeighborInterpolator::EXTRA_DATA = 3;

Interpolation::Summary NeighborInterpolator::run(NeighborInterpolator::Data& data)
{
    NeighborImplementation i(data);
    return i.interpolate();
}

} // namespace Interpolation
