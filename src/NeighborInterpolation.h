/* -*- c++ -*-
 * NeighborInterpolator.h
 *
 *  Created on: Apr 19, 2012
 *      Author: alexanderb
 */

#ifndef NEIGHBORINTERPOLATION_H_
#define NEIGHBORINTERPOLATION_H_

#include <vector>

namespace NeighborInterpolation {

struct Data {
    bool usable;
    float value;
    Data() : usable(false), value(-32766.5) { }
    Data(float v) : usable(true), value(v) { }
};
struct Interpolation {
    enum Quality { OBSERVATION, GOOD, BAD, FAILED };
    float value;
    Quality quality;

    Interpolation() : quality(FAILED) { }
    Interpolation(float v, Quality q) : value(v), quality(q) { }
};
struct Correlation {
    float slope, offset, sigma;

    float transform(float value) const
    { return offset + value*slope; }
};
struct InterpolationData {
    std::vector<Data> centerObservations;
    std::vector<Data> centerModel;
    std::vector<Correlation> neighborCorrelations;
    std::vector<std::vector<Data> > neighborObservations;
    float maxOffset;
};

/** amount of extra data required before and after the gap for Akima interpolation. */
extern const int extraData;

std::vector<Interpolation> interpolate(const InterpolationData& data);

} // namespace NeighborInterpolator

#endif /* NEIGHBORINTERPOLATION_H_ */
