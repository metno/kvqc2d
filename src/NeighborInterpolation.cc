/*
 * NeighborInterpolator.cpp
 *
 *  Created on: Apr 19, 2012
 *      Author: alexanderb
 */

#include "NeighborInterpolation.h"

#include "Akima.h"
#include "foreach.h"

#include <cmath>

#define NDEBUG 1
#include "debug.h"

namespace NeighborInterpolation {

const int extraData = 3;

namespace {

const int MAX_NEIGHBORS = 5;
const float AKIMAWEIGHT = 2;
const bool akimaFirst = false;

class WeightedMean {
public:
    WeightedMean() : mSumWeights(0), mSumValues(0), mCount(0) { }
    void add(float value, float weight) { if(weight>0) { mSumValues += value*weight; mSumWeights += weight; mCount += 1;} }
    bool valid() const { return mCount>0; }
    float mean() const { if( valid() ) return mSumValues / mSumWeights; else return 0; }
    int count() const { return mCount; }
private:
    float mSumWeights, mSumValues;
    int mCount;
};

InterpolationData::SupportVector interpolateSimple(InterpolationData& data)
{
    const unsigned int duration = data.duration();
    InterpolationData::SupportVector interpolated(duration);

    for(unsigned int t=0; t<duration; ++t) {
        WeightedMean wm;
        for(int n=0; n<data.neighbors(); ++n) {
            const SupportData nd = data.transformedNeighbor(n, t);
            if( !nd.usable() )
                continue;
            wm.add(nd.value(), data.neighborWeight(n));
            if( wm.count() >= MAX_NEIGHBORS )
                break;
        }
        if( wm.valid() )
            interpolated[t] = SupportData(wm.mean());
    }
    return interpolated;
}

struct OffsetCorrection {
    float slope, offset;
    float at(float x) const
    { return offset + x*slope; }
    OffsetCorrection() : slope(0), offset(0) { }
};

OffsetCorrection calculateOffset(const SupportData &o0, const SupportData &o1, const SeriesData &d0, const SeriesData &d1, int t0, int t1)
{
    OffsetCorrection oc;
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

OffsetCorrection calculateNeighborOffset(InterpolationData& data, const std::vector<SupportData>& o, int t0, int t1)
{
    return calculateOffset(o[t0], o[t1], data.center(t0), data.center(t1), t0, t1);
}

OffsetCorrection calculateModelOffset(InterpolationData& data, int t0, int t1)
{
    return calculateOffset(data.model(t0), data.model(t1), data.center(t0), data.center(t1), t0, t1);
}

struct IData {
    std::vector<SupportData> simpleInterpolations;
    Akima akima;
    int beforeGap, afterGap;
};

void interpolateSingleGap(InterpolationData& data, const IData& idata, std::vector<Interpolation>& interpolated)
{
    const int gap = idata.afterGap - idata.beforeGap - 1;

    const OffsetCorrection ocModel = calculateModelOffset(data, idata.beforeGap, idata.afterGap);
    const OffsetCorrection ocObservations = calculateNeighborOffset(data, idata.simpleInterpolations, idata.beforeGap, idata.afterGap);

    const int t0 = idata.beforeGap + 1;
    for(int t=t0; t<t0+gap; ++t) {
        const SeriesData obs = data.center(t);
        if( !obs.needsInterpolation() ) {
            interpolated[t] = Interpolation(obs.value(), Interpolation::OBSERVATION);
            continue;
        }

        WeightedMean combi;

        const bool canUseAkima = (idata.akima.distance(t) < 1.5);
        if( akimaFirst && canUseAkima )
            combi.add(idata.akima.interpolate(t), AKIMAWEIGHT);

        const SupportData& inter = idata.simpleInterpolations[t];
        if( !combi.valid() && inter.usable() ) {
            const float delta = ocObservations.at(t);
            if( std::fabs(delta) < data.maximumOffset())
                combi.add(inter.value() - delta, 1);
        }

        if( !akimaFirst && !combi.valid() && canUseAkima )
            combi.add(idata.akima.interpolate(t), AKIMAWEIGHT);

        const SupportData& model = data.model(t);
        if( !combi.valid() && model.usable() ) {
            const float delta = ocModel.at(t);
            if( std::fabs(delta) < data.maximumOffset() )
                combi.add(model.value() - delta, 1);
        }

        if( combi.valid() ) {
            const Interpolation::Quality quality = (t==t0 || t==t0+gap-1) ? Interpolation::GOOD : Interpolation::BAD;
            interpolated[t] = Interpolation(combi.mean(), quality);
        }
    }
}

} // anonymous namespace

std::vector<Interpolation> interpolate(InterpolationData& data)
{
    const int duration = data.duration();
    std::vector<Interpolation> interpolated(duration);

    IData idata;
    idata.simpleInterpolations = interpolateSimple(data);
    for(int t = 0; t<duration; ++t) {
        const SupportData obs = data.center(t);
        if( obs.usable() )
            idata.akima.add(t, obs.value());
    }

    idata.beforeGap = extraData-1; // assume this is an observed value
    const int afterGaps = duration - extraData;
    while( idata.beforeGap < afterGaps ) {
        idata.afterGap = idata.beforeGap+1;
        while( idata.afterGap < afterGaps && !data.center(idata.afterGap).usable() )
            ++idata.afterGap;

        if( idata.afterGap != idata.beforeGap + 1 )
            interpolateSingleGap(data, idata, interpolated);
        if( idata.afterGap < afterGaps ) {
            DBG("adding original at afterGap=" << idata.afterGap);
            interpolated[idata.afterGap] = Interpolation(data.center(idata.afterGap).value(), Interpolation::OBSERVATION);
        }
        idata.beforeGap = idata.afterGap;
    }
    return interpolated;
}

} // namespace NeighborInterpolation
