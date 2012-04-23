/*
 * SimpleInterpolationData.cc
 *
 *  Created on: Apr 23, 2012
 *      Author: alexanderb
 */

#include "SimpleData.h"

namespace NeighborInterpolation {

SupportData SimpleData::transformedNeighbor(int n, int time)
{
    SupportData sd = neighbor(n, time);
    if (sd.usable())
        return SupportData(neighborCorrelations[n].transformed(sd.value()));
    else
        return sd;
}

float SimpleData::neighborWeight(int neighbor)
{
    float s = neighborCorrelations[neighbor].sigma;
    return 1 / (s * s * s);
}

} // namespace NeighborInterpolation
