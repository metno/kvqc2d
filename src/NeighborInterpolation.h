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
    bool usable, needsInterpolation;
    float value;
    Data() : usable(false), needsInterpolation(true), value(-32766.5) { }
    Data(float v) : usable(true), needsInterpolation(false), value(v) { }
};
struct Interpolation {
    enum Quality { OBSERVATION, GOOD, BAD, FAILED };
    Quality quality;
    float value;

    Interpolation() : quality(FAILED), value(-32767) { }
    Interpolation(float v, Quality q) : quality(q), value(v) { }
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