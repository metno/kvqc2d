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

std::vector<Data> interpolateSimple(const InterpolationData& data)
{
    const unsigned int duration = data.centerObservations.size();
    DBG(DBG1(data.neighborCorrelations.size()) << DBG1(duration));
    std::vector<Data> interpolated(duration);

    for(unsigned int t=0; t<duration; ++t) {
        float sumNeighbors = 0, sumWeights = 0;
        int countGoodNeighbors = 0;
        for(unsigned int n=0; n<data.neighborCorrelations.size(); ++n) {
            //DBG(DBG1(n) << DBG1(data.neighborObservations[n].size()));

            const Data& neighborObs = data.neighborObservations[n][t];
            if( !neighborObs.usable )
                continue;
            const Correlation& nc = data.neighborCorrelations[n];
            const float nv = nc.transform(neighborObs.value);
            const float weight = 1/(nc.sigma*nc.sigma*nc.sigma);
            sumWeights += weight;
            sumNeighbors += nv * weight;
            countGoodNeighbors += 1;
            if( countGoodNeighbors >= 5 )
                break;
        }
        if( sumWeights > 0 )
            interpolated[t] = Data(sumNeighbors / sumWeights);
    }
    return interpolated;
}

struct OffsetCorrection {
    float slope, offset;
    float at(float x) const
    { return offset + x*slope; }
    OffsetCorrection() : slope(0), offset(0) { }
};

OffsetCorrection calculateOffset(const std::vector<Data>& centerObs, const std::vector<Data>& otherObs, int i0, int i1)
{
    OffsetCorrection oc;
    const Data &c0 = centerObs[i0], &c1 = centerObs[i1], &o0 = otherObs[i0], &o1 = otherObs[i1];
    const bool have0 = (c0.usable && o0.usable), have1 = (c1.usable && o1.usable);
    const float delta0 = o0.value - c0.value, delta1 = o1.value - c1.value;
    const int N = i1 - i0;
    if( have0 && have1 && N!=0 ) {
        oc.slope = (delta1 - delta0)/N;
        oc.offset = delta0 + oc.slope;
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

void interpolateSingleGap(const InterpolationData& data, const IData& idata, std::vector<Interpolation>& interpolated)
{
    const int gap = idata.afterGap - idata.beforeGap - 1;

    const OffsetCorrection ocModel = calculateOffset(data.centerObservations, data.centerModel, idata.beforeGap, idata.afterGap);
    DBG(DBG1(ocModel.at(0)) << DBG1(ocModel.at(gap)));
    const OffsetCorrection ocObservations= calculateOffset(data.centerObservations, idata.simpleInterpolations, idata.beforeGap, idata.afterGap);
    DBG(DBG1(ocObservations.at(0)) << DBG1(ocObservations.at(gap)));

    const int t0 = idata.beforeGap + 1;
    DBG(DBG1(idata.beforeGap) << DBG1(idata.afterGap) << DBG1(gap) << DBG1(idata.simpleInterpolations.size()) << DBG1(t0));
    for(int t=0; t<gap; ++t) {
        if( !data.centerObservations[t0+t].needsInterpolation ) {
            interpolated[t0+t] = Interpolation(data.centerObservations[t0+t].value, Interpolation::OBSERVATION);
            continue;
        }
        const Data& model = data.centerModel[t0+t];
        const Data& inter = idata.simpleInterpolations[t0+t];
        DBG("t=" << t << " inter=" << inter.value << "/" << inter.usable);

        float combiValue = 0, combiWeights = 0;
        const bool canUseAkima = (idata.akima.distance(t0+t) < 1.5);
        DBG(DBG1(idata.akima.distance(t0+t)) << DBG1(canUseAkima));
        const bool akimaFirst = false;
        if( akimaFirst && canUseAkima ) {
            const float akimaWeight = 2, akimaValue = idata.akima.interpolate(t0+t);
            combiValue += akimaWeight * akimaValue;
            combiWeights += akimaWeight;
        }
        if( combiWeights == 0 && inter.usable ) {
            const float delta = ocObservations.at(t);
            DBGV(delta);
            if( std::fabs(delta) < data.maxOffset ) {
                combiValue += inter.value - delta;
                combiWeights += 1;
            }
        }
        if( !akimaFirst && combiWeights == 0 && canUseAkima ) {
            const float akimaWeight = 2, akimaValue = idata.akima.interpolate(t0+t);
            DBG("using AKIMA " << DBG1(akimaValue));
            combiValue += akimaWeight * akimaValue;
            combiWeights += akimaWeight;
        }
        if( combiWeights == 0 && model.usable ) {
            const float delta = ocModel.at(t);
            DBGV(delta);
            if( std::fabs(delta) < data.maxOffset ) {
                combiValue += model.value - delta;
                combiWeights += 1;
            }
        }
        if( combiWeights > 0 ) {
            const Interpolation::Quality quality = (t==0 || t==gap-1) ? Interpolation::GOOD : Interpolation::BAD;
            const float combi = combiValue / combiWeights;
            DBG(DBG1(combi) << DBG1(quality));
            interpolated[t0 + t] = Interpolation(combi, quality);
        }
    }
}

} // anonymous namespace

std::vector<Interpolation> interpolate(const InterpolationData& data)
{
    const int duration = data.centerObservations.size();
    std::vector<Interpolation> interpolated(duration);

    IData idata;
    idata.simpleInterpolations = interpolateSimple(data);
    int t = 0;
    foreach(const Data& d, data.centerObservations) {
        if( d.usable ) {
            DBG("adding to akima: " << DBG1(t) << DBG1(d.value));
            idata.akima.add(t, d.value);
        }
        t += 1;
    }

    idata.beforeGap = extraData-1; // assume this is an observed value
    const int afterGaps = duration - extraData;
    while( idata.beforeGap < afterGaps ) {
        idata.afterGap = idata.beforeGap+1;
        while( idata.afterGap < afterGaps && !data.centerObservations[idata.afterGap].usable ) {
            DBG(DBG1(data.centerObservations[idata.afterGap].value) << DBG1(data.centerObservations[idata.afterGap].usable));
            ++idata.afterGap;
        }

        DBG(DBG1(idata.beforeGap) << DBG1(idata.afterGap) << DBG1(afterGaps));
        if( idata.afterGap != idata.beforeGap + 1 )
            interpolateSingleGap(data, idata, interpolated);
        if( idata.afterGap < afterGaps ) {
            DBG("adding original at afterGap=" << idata.afterGap);
            interpolated[idata.afterGap] = Interpolation(data.centerObservations[idata.afterGap].value, Interpolation::OBSERVATION);
        }
        idata.beforeGap = idata.afterGap;
    }
    return interpolated;
}

} // namespace NeighborInterpolation
