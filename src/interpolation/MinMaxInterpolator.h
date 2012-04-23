// -*- c++ -*-

#ifndef MINMAXINTERPOLATOR_H
#define MINMAXINTERPOLATOR_H 1

#include "CorrelatedNeighborInterpolator.h"

struct MinMaxValuesWithQualities_t {
    Interpolator::ValuesWithQualities_t par, min, max;
};

MinMaxValuesWithQualities_t MinMaxInterpolate(Interpolator* inter, CorrelatedNeighbors::DataAccess* dax, const Instrument& instrument,
                                              const ParameterInfo& parameterInfo, const TimeRange& t);


#endif /* MINMAXINTERPOLATOR_H */
