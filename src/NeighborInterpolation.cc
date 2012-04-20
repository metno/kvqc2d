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

namespace {

std::vector<Data> interpolateSimple(InterpolationData& data)
{
    const unsigned int duration = data.duration();
    std::vector<Data> interpolated(duration);

    for(unsigned int t=0; t<duration; ++t) {
        WeightedMean wm;
        for(int n=0; n<data.neighbors(); ++n) {
            if( !data.neighborObservationUsable(n, t) )
                continue;
            wm.add(data.neighborTransformedValue(n, t), data.neighborWeight(n));
            if( wm.count() >= MAX_NEIGHBORS )
                break;
        }
        if( wm.valid() )
            interpolated[t] = Data(wm.mean());
    }
    return interpolated;
}

struct OffsetCorrection {
    float slope, offset;
    float at(float x) const
    { return offset + x*slope; }
    OffsetCorrection() : slope(0), offset(0) { }
};

OffsetCorrection calculateNeighborOffset(InterpolationData& data, const std::vector<Data>& si, int t0, int t1)
{
    OffsetCorrection oc;
    const Data &si0 = si[t0], &si1 = si[t1];
    const bool have0 = (data.centerObservationUsable(t0) && si0.usable), have1 = (data.centerObservationUsable(t1) && si1.usable);
    const float delta0 = si0.value - data.centerObservationValue(t0), delta1 = si1.value - data.centerObservationValue(t1);
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

OffsetCorrection calculateModelOffset(InterpolationData& data, int t0, int t1)
{
    OffsetCorrection oc;
    const bool have0 = (data.centerObservationUsable(t0) && data.centerModelUsable(t0)),
            have1 = (data.centerObservationUsable(t1) && data.centerModelUsable(t1));
    const float delta0 = data.centerModelValue(t0) - data.centerObservationValue(t0),
            delta1 = data.centerModelValue(t1) - data.centerObservationValue(t1);
    const int N = t1 - t0;
    if( have0 && have1 && N!=0 ) {
        oc.slope = (delta1 - delta0)/N;
        oc.offset = delta0 + oc.slope*t0;
    } else if( have0 ) {
        oc.offset = delta0;
    } else if( have1 ) {
        oc.offset = delta1;
    }

    return oc;
}

struct IData {
    std::vector<Data> simpleInterpolations;
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
        if( !data.centerObservationNeedsInterpolation(t) ) {
            interpolated[t] = Interpolation(data.centerObservationValue(t), Interpolation::OBSERVATION);
            continue;
        }

        WeightedMean combi;

        const bool canUseAkima = (idata.akima.distance(t) < 1.5);
        if( akimaFirst && canUseAkima )
            combi.add(idata.akima.interpolate(t), AKIMAWEIGHT);

        const Data& inter = idata.simpleInterpolations[t];
        if( !combi.valid() && inter.usable ) {
            const float delta = ocObservations.at(t);
            if( std::fabs(delta) < data.maximumOffset())
                combi.add(inter.value - delta, 1);
        }

        if( !akimaFirst && !combi.valid() && canUseAkima )
            combi.add(idata.akima.interpolate(t), AKIMAWEIGHT);

        if( !combi.valid() && data.centerModelUsable(t) ) {
            const float delta = ocModel.at(t);
            if( std::fabs(delta) < data.maximumOffset() )
                combi.add(data.centerModelValue(t) - delta, 1);
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
        if( data.centerObservationUsable(t) )
            idata.akima.add(t, data.centerObservationValue(t));
    }

    idata.beforeGap = extraData-1; // assume this is an observed value
    const int afterGaps = duration - extraData;
    while( idata.beforeGap < afterGaps ) {
        idata.afterGap = idata.beforeGap+1;
        while( idata.afterGap < afterGaps && !data.centerObservationUsable(idata.afterGap) )
            ++idata.afterGap;

        if( idata.afterGap != idata.beforeGap + 1 )
            interpolateSingleGap(data, idata, interpolated);
        if( idata.afterGap < afterGaps ) {
            DBG("adding original at afterGap=" << idata.afterGap);
            interpolated[idata.afterGap] = Interpolation(data.centerObservationValue(idata.afterGap), Interpolation::OBSERVATION);
        }
        idata.beforeGap = idata.afterGap;
    }
    return interpolated;
}

} // namespace NeighborInterpolation
