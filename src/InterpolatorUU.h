// -*- c++ -*-

#ifndef INTERPOLATORUU_H
#define INTERPOLATORUU_H 1

#include "CorrelatedNeighborInterpolator.h"

#include <vector>

// ########################################################################

class DataAccessUU : public CorrelatedNeighbors::DataAccess {
public:
    DataAccessUU(DataAccess* dax)
        : mDax(dax) { }

    std::vector<float> fetchObservations(const Instrument& instrument, const TimeRange& t);
    std::vector<float> fetchModelValues (const Instrument& instrument, const TimeRange& t);
    NeighborDataVector findNeighbors(const Instrument& instrument, double maxsigma);

    void setDataAccess(DataAccess* dax)
        { mDax = dax; }

private:
    CorrelatedNeighbors::DataAccess* mDax;
};

// ########################################################################

class InterpolatorUU : public Interpolator {
public:
    InterpolatorUU(CorrelatedNeighbors::Interpolator* i)
        : mDataAccess(new DataAccessUU(i->getDataAccess())), mInterpolator(i) { }

    ValuesWithQualities_t interpolate(const Instrument& instrument, const TimeRange& t);

    void configure(const AlgorithmConfig&)
        { }

private:
    DataAccessUU* mDataAccess;
    CorrelatedNeighbors::Interpolator* mInterpolator;
};

#endif /* INTERPOLATORUU_H */
