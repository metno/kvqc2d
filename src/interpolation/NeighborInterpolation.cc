/*
 * NeighborInterpolator.cpp
 *
 *  Created on: Apr 19, 2012
 *      Author: alexanderb
 */

#include "NeighborInterpolation.h"

#include "helpers/Akima.h"
#include "foreach.h"
#include "helpers/WeightedMean.h"

#include <cmath>

#define NDEBUG 1
#include "debug.h"

namespace NeighborInterpolation {

const int extraData = 3;

namespace {

const int MAX_NEIGHBORS = 5;
const float AKIMAWEIGHT = 2;
const bool akimaFirst = false;

struct OffsetCorrection {
    float slope, offset;
    float at(float x) const
    { return offset + x*slope; }
    OffsetCorrection() : slope(0), offset(0) { }
};

class IData {
public:
    IData(InterpolationData& data);
    std::vector<Interpolation> interpolate();

private:
    void interpolateSimple();
    void setupAkima();
    void interpolateGaps();
    void interpolateSingleGap();
    void interpolateSinglePoint(int t);

    OffsetCorrection calculateOffset(const SupportData &o0, const SupportData &o1, int t0, int t1);
    void calculateOffsets(int t0, int t1);

private:
    InterpolationData& data;
    std::vector<Interpolation> interpolated;

    std::vector<SupportData> simpleInterpolations;
    Akima akima;

    int beforeGap, afterGap;
    OffsetCorrection ocModel;
    OffsetCorrection ocObservations;
};

IData::IData(InterpolationData& d)
: data(d)
{
}

std::vector<Interpolation> IData::interpolate()
{
    interpolated = std::vector<Interpolation>(data.duration());
    interpolateSimple();
    setupAkima();
    interpolateGaps();
    return interpolated;
}

void IData::interpolateSimple()
{
    const unsigned int duration = data.duration();
    simpleInterpolations = InterpolationData::SupportVector(duration);

    for(unsigned int t=0; t<duration; ++t) {
        Helpers::WeightedMean wm;
        for(int n=0; n<data.neighbors() && wm.count() < MAX_NEIGHBORS; ++n) {
            const SupportData nd = data.transformedNeighbor(n, t);
            if( nd.usable() )
                wm.add(nd.value(), data.neighborWeight(n));
        }
        if( wm.valid() )
            simpleInterpolations[t] = SupportData(wm.mean());
    }
}

void IData::setupAkima()
{
    for(int t = 0; t<data.duration(); ++t) {
        const SupportData obs = data.center(t);
        if( obs.usable() )
            akima.add(t, obs.value());
    }
}

void IData::interpolateGaps()
{
    const int duration = data.duration();

    beforeGap = extraData-1; // assume this is an observed value
    const int afterGaps = duration - extraData;
    while( beforeGap < afterGaps ) {
        if( !data.center(beforeGap).usable() ) {
            beforeGap += 1;
            continue;
        }
        afterGap = beforeGap+1;
        while( afterGap < afterGaps && !data.center(afterGap).usable() )
            ++afterGap;

        if( afterGap != beforeGap + 1 )
            interpolateSingleGap();
        if( afterGap < afterGaps ) {
            DBG("adding original at afterGap=" << afterGap);
            interpolated[afterGap] = Interpolation(data.center(afterGap).value(), Interpolation::OBSERVATION);
        }
        beforeGap = afterGap;
    }
}

void IData::interpolateSingleGap()
{
    const int gap = afterGap - beforeGap - 1;

    calculateOffsets(beforeGap, afterGap);

    const int t0 = beforeGap + 1;
    for(int t=t0; t<t0+gap; ++t) {
        const SeriesData obs = data.center(t);
        if( !obs.needsInterpolation() ) {
            interpolated[t] = Interpolation(obs.value(), Interpolation::OBSERVATION);
        } else {
            interpolateSinglePoint(t);
        }
    }
}

OffsetCorrection IData::calculateOffset(const SupportData &o0, const SupportData &o1, int t0, int t1)
{
    OffsetCorrection oc;
    const SeriesData &d0 = data.center(t0), &d1 = data.center(t1);
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

void IData::calculateOffsets(int t0, int t1)
{
    ocObservations = calculateOffset(simpleInterpolations[t0], simpleInterpolations[t1], t0, t1);
    ocModel = calculateOffset(data.model(t0), data.model(t1), t0, t1);
}

void IData::interpolateSinglePoint(int t)
{
    Helpers::WeightedMean combi;

    const bool canUseAkima = (akima.distance(t) < 1.5);
    if( akimaFirst && canUseAkima )
        combi.add(akima.interpolate(t), AKIMAWEIGHT);

    const SupportData& inter = simpleInterpolations[t];
    if( !combi.valid() && inter.usable() ) {
        const float delta = ocObservations.at(t);
        if( std::fabs(delta) < data.maximumOffset())
            combi.add(inter.value() - delta, 1);
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
        bool good = (t==beforeGap+1 || t==afterGap-1);
        interpolated[t] = Interpolation(combi.mean(), good ? Interpolation::GOOD : Interpolation::BAD);
    }
}

} // anonymous namespace

std::vector<Interpolation> interpolate(InterpolationData& data)
{
    IData i(data);
    return i.interpolate();
}

} // namespace NeighborInterpolation
