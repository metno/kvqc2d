
#include "CorrelatedNeighborInterpolator.h"

#include "Akima.h"
#include "AlgorithmHelpers.h"
#include "foreach.h"

#include <puTools/miTime.h>
#include <kvalobs/kvData.h>
#include <vector>

#define NDEBUG 1
#include "debug.h"

DataAccess::~DataAccess()
{
}

// ========================================================================

CorrelatedNeighborInterpolator::CorrelatedNeighborInterpolator(DataAccess* dax)
    : mDax(dax)
{
}

static const float INVALID = -32767.0f;

// ------------------------------------------------------------------------

Interpolator::ValuesWithQualities_t CorrelatedNeighborInterpolator::interpolate(const miutil::miTime& beforeGap, const miutil::miTime& afterGap,
                                                                                int stationid, int paramid)
{
    ValuesWithQualities_t interpolated;

    float maxdelta, limit_low, limit_high;
    switch(paramid) {
    case 178: maxdelta =  5; limit_low = 800; limit_high = 1200; break;
    case 211: maxdelta = 15; limit_low = -80; limit_high =  100; break;
    case 217: maxdelta = 15; limit_low = -80; limit_high =  100; break;
    case 262: maxdelta =  5; limit_low =   0; limit_high =  100; break;
    default:
        return interpolated;
    }

    const int gapLength = miutil::miTime::hourDiff(afterGap, beforeGap) - 1;
    if( gapLength < 1 )
        return interpolated;

    const int NA = 3;
    const miutil::miTime t0 = Helpers::plusHour(beforeGap, -(NA-1)), tN1 = Helpers::plusHour(afterGap, (NA-1));
    const std::vector<float> observations = mDax->fetchObservations(stationid, paramid, t0, tN1);
    const std::vector<float> modelvalues  = mDax->fetchModelValues (stationid, paramid, t0, tN1);
    const std::vector<float> interpolations  =  interpolate_simple(stationid, paramid, t0, tN1);

#if 1
    Akima akima;
    int nBefore = 0, nAfter = 0;
    for(int x=0; x<NA; ++x) {
        float yb = observations[x];
        if( yb > INVALID ) {
            akima.add(x-NA, yb);
            nBefore += 1;
        }
        const float ya = observations[gapLength + NA + x];
        if( ya > INVALID ) {
            akima.add(gapLength + x, ya);
            nAfter += 1;
        }
    }
    const bool haveAkima = ( nBefore >= NA && nAfter >= NA );
#endif
    
    const double data0 = observations[NA-1], dataN1 = modelvalues[gapLength+NA];
    double model_slope, model_offset, inter_slope, inter_offset;
    calculate_delta(data0, dataN1,
                    modelvalues[NA-1], modelvalues[gapLength+NA],
                    gapLength, model_slope, model_offset);
    calculate_delta(data0, dataN1,
                    interpolations[NA-1], interpolations[gapLength+NA],
                    gapLength, inter_slope, inter_offset);
    
    for(int i=0; i<gapLength; ++i) {
        const double modelI = modelvalues[NA+i];
        const double interI = interpolations[NA+i];
        
        double combiValue = 0, combiWeights = 0;
        
        const bool canUseAkima = haveAkima && (i == 0 || i == gapLength-1);
        const bool akimaFirst = false;
        if( akimaFirst && canUseAkima ) {
            const double akimaWeight = 2;
            combiValue += akimaWeight * akima.interpolate(i);
            combiWeights += akimaWeight;
        }
        if( combiWeights == 0 && interI > -1000 ) {
            const double delta = inter_offset + i * inter_slope;
            if( fabs(delta) < maxdelta ) {
                combiValue += interI - delta;
                combiWeights += 1;
            }
        }
        if( !akimaFirst && combiWeights == 0 && canUseAkima ) {
            const double akimaWeight = 2;
            combiValue += akimaWeight * akima.interpolate(i);
            combiWeights += akimaWeight;
        }
        if( combiWeights == 0 && modelI > -1000 ) {
            const double delta = model_offset + i * model_slope;
            if( interI <= -1000 && fabs(delta) < maxdelta ) {
                combiValue += modelI - delta;
                combiWeights += 1;
            }
        }
        double combi;
        if( combiWeights > 0 ) {
            combi = combiValue / combiWeights;
            if( combi < limit_low )
                combi = limit_low;
            else if( combi > limit_high )
                combi = limit_high;
        } else {
            combi = INVALID;
        }
        interpolated.push_back(ValueWithQuality(combi, 0));
    }
    return interpolated;
}

// ------------------------------------------------------------------------

void CorrelatedNeighborInterpolator::configure(const AlgorithmConfig& config)
{
}

// ------------------------------------------------------------------------

std::vector<float> CorrelatedNeighborInterpolator::interpolate_simple(int ctr, int paramid,
                                                                      const miutil::miTime& t0, const miutil::miTime& t1)
{
    const int length = miutil::miTime::hourDiff(t1, t0) + 1;
    std::vector<float> interpolated(length, INVALID);

    const neighbors_t& neighbors = find_neighbors(ctr, paramid, 2.7);
    if( neighbors.empty() )
        return interpolated;

    std::vector< std::vector<float> > neighbor_observations(neighbors.size());
    int n = 0;
    foreach(const NeighborData& nd, neighbors) {
        neighbor_observations[n++] = mDax->fetchObservations(nd.neighborid, paramid, t0, t1);
    }
    for(int t=0; t<length; ++t) {
        double sn = 0, sw = 0;
        int count = 0;
        n = 0;
        foreach(const NeighborData& nd, neighbors) {
            const double nObs = neighbor_observations[n++][t];
            if( nObs <= INVALID )
                continue;
            const double nInter = nd.offset + nObs * nd.slope;
            if( nInter < -100 )
                continue;
            const double weight = 1/(nd.sigma*nd.sigma*nd.sigma);
            sw += weight;
            sn += nInter*weight;
            count += 1;
            if( count >= 5 )
                break;
        }
        if( sw > 0 )
            interpolated[t] = sn / sw;
    }
    return interpolated;
}

// ------------------------------------------------------------------------

void CorrelatedNeighborInterpolator::calculate_delta(const double data0, const double dataN1, const double i0, const double iN1, int N,
                                                     double& slope, double& offset)
{
    const bool have0 = (i0 > INVALID && data0 > INVALID), haveN1 = (iN1 > INVALID && dataN1 > INVALID);
    const double delta0 = i0 - data0, deltaN1 = iN1 - dataN1;
    if( have0 && haveN1 ) {
        slope = (deltaN1 - delta0)/(N+1);
        offset = delta0 + slope;
    } else {
        slope = 0;
        if( have0 ) {
            offset = delta0;
        } else if( haveN1 ) {
            offset = deltaN1;
        } else {
            offset = 0;
        }
    }
}

// ------------------------------------------------------------------------

const neighbors_t& CorrelatedNeighborInterpolator::find_neighbors(int stationid, int paramid, double maxsigma)
{
    neighbor_map_t::iterator it = neighbor_map.find(stationid);
    if( it != neighbor_map.end() )
        return it->second;
    
    neighbor_map[stationid] = mDax->findNeighbors(stationid, paramid, maxsigma);
    return neighbor_map[stationid];
}


