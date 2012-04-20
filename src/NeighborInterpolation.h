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

    float transformed(float value) const
    { return offset + value*slope; }
};
class InterpolationData {
public:
    int duration()
    { return centerObservations.size(); }

    int neighbors()
    { return neighborCorrelations.size(); }

    float maximumOffset()
    { return maxOffset; }

    bool centerObservationNeedsInterpolation(int time)
    { return centerObservations[time].needsInterpolation; }

    bool centerObservationUsable(int time)
    { return centerObservations[time].usable; }

    float centerObservationValue(int time)
    { return centerObservations[time].value; }

    bool centerModelUsable(int time)
    { return centerModel[time].usable; }

    float centerModelValue(int time)
    { return centerModel[time].value; }

    bool neighborObservationUsable(int neighbor, int time)
    { return neighborObservations[neighbor][time].usable; }

    float neighborObservationValue(int neighbor, int time)
    { return neighborObservations[neighbor][time].value; }

    float neighborTransformedValue(int neighbor, int time)
    { return neighborCorrelations[neighbor].transformed(neighborObservationValue(neighbor, time)); }

    float neighborWeight(int neighbor)
    { float s = neighborCorrelations[neighbor].sigma; return (s*s*s); }

    std::vector<Data>& co() { return centerObservations; }
    std::vector<Data>& cm() { return centerModel; }
    std::vector<Correlation>& nc() { return neighborCorrelations; }
    std::vector<std::vector<Data> >& no() { return neighborObservations; }
    float& mo() { return maxOffset; }

private:
    std::vector<Data> centerObservations;
    std::vector<Data> centerModel;
    std::vector<Correlation> neighborCorrelations;
    std::vector<std::vector<Data> > neighborObservations;
    float maxOffset;
};

/** amount of extra data required before and after the gap for Akima interpolation. */
extern const int extraData;

std::vector<Interpolation> interpolate(InterpolationData& data);

} // namespace NeighborInterpolator

#endif /* NEIGHBORINTERPOLATION_H_ */
