// -*- c++ -*-

#ifndef MINMAXINTERPOLATOR_H
#define MINMAXINTERPOLATOR_H 1

#include "NeighborInterpolation.h"

class MinMaxData : public NeighborInterpolation::Data {
public:
    virtual NeighborInterpolation::SeriesData minimum(int t) = 0;
    virtual NeighborInterpolation::SeriesData maximum(int t) = 0;
    virtual void setMinimum(int time, NeighborInterpolation::Quality q, float value) = 0;
    virtual void setMaximum(int time, NeighborInterpolation::Quality q, float value) = 0;
    virtual float fluctuationLevel() const = 0;
};

void MinMaxInterpolate(MinMaxData& data);

#endif /* MINMAXINTERPOLATOR_H */
