/*
 * SimpleInterpolationData.h
 *
 *  Created on: Apr 23, 2012
 *      Author: alexanderb
 */

#ifndef SIMPLEINTERPOLATIONDATA_H_
#define SIMPLEINTERPOLATIONDATA_H_

#include "NeighborInterpolation.h"

namespace NeighborInterpolation {

struct Correlation {
    float slope, offset, sigma;

    float transformed(float value) const
    { return offset + value*slope; }
};

struct Result {
    int time;
    NeighborInterpolation::Quality quality;
    float value;

    Result(int t) : time(t), quality(NeighborInterpolation::FAILED), value(-32767) { }
    Result(int t, NeighborInterpolation::Quality q, float v) : time(t), quality(q), value(v) { }
};

class SimpleData : public NeighborInterpolation::Data {
public:
    virtual int duration() const
    { return centerObservations.size(); }

    virtual int neighbors() const
    { return neighborCorrelations.size(); }

    virtual float maximumOffset() const
    { return maxOffset; }

    virtual SeriesData center(int time)
    { return centerObservations[time]; }

    virtual SupportData model(int time)
    { return centerModel[time]; }

    virtual SupportData neighbor(int n, int time)
    { return neighborObservations[n][time]; }

    virtual SupportData transformedNeighbor(int n, int time);

    virtual float neighborWeight(int neighbor);

    virtual void setInterpolated(int time, Quality q, float value)
    { interpolations.push_back(Result(time, q, value)); }

    typedef std::vector<SeriesData> SeriesVector;
    typedef std::vector<SupportData> SupportVector;
    typedef std::vector<Correlation> CorrelationVector;
    typedef std::vector<Result> InterpolationVector;

    SeriesVector& co() { return centerObservations; }
    SupportVector& cm() { return centerModel; }
    CorrelationVector& nc() { return neighborCorrelations; }
    std::vector<SupportVector>& no() { return neighborObservations; }
    float& mo() { return maxOffset; }

    SeriesVector centerObservations;
    SupportVector centerModel;
    CorrelationVector neighborCorrelations;
    std::vector<SupportVector> neighborObservations;
    float maxOffset;

    InterpolationVector interpolations;
};

} // namespace NeighborInterpolation

#endif /* SIMPLEINTERPOLATIONDATA_H_ */
