
#include "CorrelatedNeighborInterpolator.h"

#include "Akima.h"
#include "AlgorithmHelpers.h"
#include "foreach.h"

#include <kvalobs/kvData.h>
#include <vector>

#define NDEBUG 1
#include "debug.h"

namespace CorrelatedNeighbors {

static const float INVALID = -32767.0f;

DataAccess::~DataAccess()
{
}

// ========================================================================

Interpolator::Interpolator(DataAccess* dax)
    : mDax(dax)
{
}

// ------------------------------------------------------------------------

::Interpolator::ValuesWithQualities_t Interpolator::interpolate(const Instrument& instrument, const TimeRange& t)
{
    ValuesWithQualities_t interpolated;

    float maxdelta, limit_low, limit_high;
    switch(instrument.paramid) {
    case 178: maxdelta =  5; limit_low = 800; limit_high = 1200; break;
    case 211: maxdelta = 15; limit_low = -80; limit_high =  100; break;
    case 217: maxdelta = 15; limit_low = -80; limit_high =  100; break;
    case 262: maxdelta =  5; limit_low =   0; limit_high =  100; break;
    default:
        return interpolated;
    }

    const int gapLength = t.hours() - 1;
    if( gapLength < 1 )
        return interpolated;

    const int NA = 3;
    const TimeRange tExtended(Helpers::plusHour(t.t0, -(NA-1)), Helpers::plusHour(t.t1, (NA-1)));
    DBGV(tExtended.t0);
    const std::vector<float> observations = mDax->fetchObservations(instrument, tExtended);
    const std::vector<float> modelvalues  = mDax->fetchModelValues (instrument, tExtended);
    const std::vector<float> interpolations  =  interpolate_simple(instrument, tExtended);
    DBGV(interpolations.size());

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
    
    const double data0 = observations[NA-1], dataN1 = observations[gapLength+NA];
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
        DBG("i=" << i << " inter=" << interI);
        
        double combiValue = 0, combiWeights = 0;
        
        const bool canUseAkima = haveAkima && (i == 0 || i == gapLength-1);
        const bool akimaFirst = false;
        if( akimaFirst && canUseAkima ) {
            const double akimaWeight = 2;
            combiValue += akimaWeight * akima.interpolate(i);
            combiWeights += akimaWeight;
        }
        if( combiWeights == 0 && interI > INVALID ) {
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

void Interpolator::configure(const AlgorithmConfig& config)
{
}

// ------------------------------------------------------------------------

std::vector<float> Interpolator::interpolate_simple(const Instrument& ctr, const TimeRange& tExtended)
{
    const int length = tExtended.hours() + 1;
    std::vector<float> interpolated(length, INVALID);

    const neighbors_t& neighbors = find_neighbors(ctr, 2.7);
    DBGV(neighbors.size());
    if( neighbors.empty() )
        return interpolated;

    std::vector< std::vector<float> > neighbor_observations(neighbors.size());
    int n = 0;
    foreach(const NeighborData& nd, neighbors) {
        const Instrument nbr(nd.neighborid, ctr.paramid, -1, -1, -1);
        neighbor_observations[n++] = mDax->fetchObservations(nbr, tExtended);
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

void Interpolator::calculate_delta(const double data0, const double dataN1, const double i0, const double iN1, int N,
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

const neighbors_t& Interpolator::find_neighbors(const Instrument& instrument, double maxsigma)
{
    neighbor_map_t::iterator it = neighbor_map.find(instrument.stationid);
    if( it != neighbor_map.end() )
        return it->second;
    
    neighbor_map[instrument.stationid] = mDax->findNeighbors(instrument, maxsigma);
    return neighbor_map[instrument.stationid];
}

} // namespace CorrelatedNeigbors