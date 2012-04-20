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

class SupportData {
public:
    SupportData() : mUsable(false), mValue(-32766.5) { }
    SupportData(float v) : mUsable(true), mValue(v) { }
    bool usable() const { return mUsable; }
    float value() const { return mValue; }
private:
    bool mUsable;
    float mValue;
};
class SeriesData : public SupportData {
public:
    SeriesData() : SupportData(), mNeedsInterpolation(true) { }
    SeriesData(float v) : SupportData(v), mNeedsInterpolation(false) { }
    explicit SeriesData(const SupportData& sd) : SupportData(sd), mNeedsInterpolation(!sd.usable()) { }
    bool needsInterpolation() const { return mNeedsInterpolation; }
private:
    bool mNeedsInterpolation;
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

    SeriesData center(int time)
    { return centerObservations[time]; }

    SupportData model(int time)
    { return centerModel[time]; }

    SupportData neighbor(int n, int time)
    { return neighborObservations[n][time]; }

    SupportData transformedNeighbor(int n, int time)
    { SupportData sd = neighbor(n, time); if( sd.usable() ) return SupportData(neighborCorrelations[n].transformed(sd.value())); else return sd; }

    float neighborWeight(int neighbor)
    { float s = neighborCorrelations[neighbor].sigma; return 1/(s*s*s); }

    typedef std::vector<SeriesData> SeriesVector;
    typedef std::vector<SupportData> SupportVector;
    typedef std::vector<Correlation> CorrelationVector;

    SeriesVector& co() { return centerObservations; }
    SupportVector& cm() { return centerModel; }
    CorrelationVector& nc() { return neighborCorrelations; }
    std::vector<SupportVector>& no() { return neighborObservations; }
    float& mo() { return maxOffset; }

private:
    SeriesVector centerObservations;
    SupportVector centerModel;
    CorrelationVector neighborCorrelations;
    std::vector<SupportVector> neighborObservations;
    float maxOffset;
};

/** amount of extra data required before and after the gap for Akima interpolation. */
extern const int extraData;

std::vector<Interpolation> interpolate(InterpolationData& data);

} // namespace NeighborInterpolator

#endif /* NEIGHBORINTERPOLATION_H_ */
