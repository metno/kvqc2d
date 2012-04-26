/*
 * NeighborInterpolator.cpp
 *
 *  Created on: Apr 19, 2012
 *      Author: alexanderb
 */

#include "NeighborInterpolator.h"

#include "helpers/Akima.h"
#include "helpers/WeightedMean.h"
#include "foreach.h"

#include <cmath>

#define NDEBUG 1
#include "debug.h"

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
    DBGL;
    interpolateSimple();
    setupAkima();
    interpolateGaps();
    return result;
}

void NeighborImplementation::interpolateSimple()
{
    const unsigned int duration = data.duration();
    simpleInterpolations = SupportVector(duration);

    const int nc = data.neighbors();

    for(unsigned int t=0; t<duration; ++t) {
        Helpers::WeightedMean wm;
        for(int n=0; n<nc && wm.count() < MAX_NEIGHBORS; ++n) {
            const SupportData nd = data.transformedNeighbor(n, t);
            if( nd.usable() )
                wm.add(nd.value(), data.neighborWeight(n));
        }
        if( wm.valid() )
            simpleInterpolations[t] = SupportData(wm.mean());
    }
}

void NeighborImplementation::setupAkima()
{
    DBGL;
    const int d = data.duration();
    DBGV(d);
    for(int t = 0; t<d; ++t) {
        const SupportData obs = data.parameter(t);
        if( obs.usable() ) {
            DBG(DBG1(t) << DBG1(obs.value()));
            akima.add(t, obs.value());
        }
    }
}

void NeighborImplementation::interpolateGaps()
{
    DBGL;
    const int duration = data.duration();
    DBGV(duration);

    beforeGap = NeighborInterpolator::EXTRA_DATA - 1; // assume this is an observed value
    const int afterGaps = duration - NeighborInterpolator::EXTRA_DATA;
    DBG(DBG1(duration) << DBG1(beforeGap) << DBG1(afterGaps));
    while( beforeGap < afterGaps ) {
        if( !data.parameter(beforeGap).usable() ) {
            beforeGap += 1;
            DBGV(beforeGap);
            continue;
        }
        afterGap = beforeGap+1;
        DBGV(afterGap);
        while( afterGap < afterGaps && !data.parameter(afterGap).usable() ) {
            ++afterGap;
            DBGV(afterGap);
        }

        if( afterGap != beforeGap + 1 )
            interpolateSingleGap();
        beforeGap = afterGap;
    }
}

void NeighborImplementation::interpolateSingleGap()
{
    const int gap = afterGap - beforeGap - 1;
    DBGV(gap);

    calculateOffsets(beforeGap, afterGap);

    const int t0 = beforeGap + 1;
    for(int t=t0; t<t0+gap; ++t) {
        const SeriesData obs = data.parameter(t);
        DBG(DBG1(t) << DBG1(obs.value()) << DBG1(obs.needsInterpolation()));
        if( obs.needsInterpolation() )
            interpolateSinglePoint(t);
    }
}

OffsetCorrection NeighborImplementation::calculateOffset(const SupportData &o0, const SupportData &o1, int t0, int t1)
{
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
    ocObservations = calculateOffset(simpleInterpolations[t0], simpleInterpolations[t1], t0, t1);
    ocModel = calculateOffset(data.model(t0), data.model(t1), t0, t1);
    DBG(DBG1(ocObservations.at(t0)) << DBG1(ocObservations.at(t1)));
}

void NeighborImplementation::interpolateSinglePoint(int t)
{
    DBGV(t);
    Helpers::WeightedMean combi;

    const bool canUseAkima = (akima.distance(t) < 1.5);
    DBGV(canUseAkima);
    if( akimaFirst && canUseAkima ) {
        combi.add(akima.interpolate(t), AKIMAWEIGHT);
        DBG("akima first");
    }

    const SupportData& inter = simpleInterpolations[t];
    DBGV(inter.usable());
    if( !combi.valid() && inter.usable() ) {
        const float delta = ocObservations.at(t);
        DBG(DBG1(delta) << DBG1(data.maximumOffset()));
        if( std::fabs(delta) < data.maximumOffset()) {
            combi.add(inter.value() - delta, 1);
            DBG("combi" << DBG1(inter.value()));
        }
    }

    if( !akimaFirst && !combi.valid() && canUseAkima ) {
        combi.add(akima.interpolate(t), AKIMAWEIGHT);
        DBG("akima not first");
    }

    const SupportData& model = data.model(t);
    if( !combi.valid() && model.usable() ) {
        const float delta = ocModel.at(t);
        if( std::fabs(delta) < data.maximumOffset() ) {
            combi.add(model.value() - delta, 1);
            DBGL;
        }
    }

    if( combi.valid() ) {
        DBGV(combi.mean());
        const bool good = (t==beforeGap+1 || t==afterGap-1);
        data.setInterpolated(t, good ? GOOD : BAD, combi.mean());
        result.addOk();
    } else {
        DBGL;
        data.setInterpolated(t, Interpolation::FAILED, 0);
        result.addFailed();
    }
}

} // anonymous namespace

const int NeighborInterpolator::EXTRA_DATA = 3;

Interpolation::Summary NeighborInterpolator::interpolate(SingleParameterInterpolator::Data& data)
{
    return doInterpolate(static_cast<Data&>(data));
}

Interpolation::Summary NeighborInterpolator::doInterpolate(NeighborInterpolator::Data& data)
{
    NeighborImplementation i(data);
    return i.interpolate();
}

} // namespace Interpolation
