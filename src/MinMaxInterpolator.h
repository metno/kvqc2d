// -*- c++ -*-

#ifndef MINMAXINTERPOLATOR_H
#define MINMAXINTERPOLATOR_H 1

#include "CorrelatedNeighborInterpolator.h"

#include <vector>

struct MinMaxValuesWithQualities_t {
    Interpolator::ValuesWithQualities_t par, min, max;
};

MinMaxValuesWithQualities_t MinMaxInterpolate(Interpolator* inter, CorrelatedNeighbors::DataAccess* dax, const Instrument& instrument,
                                              int minPar, int maxPar, float noiseLevel, const TimeRange& t);

// ========================================================================

#if 0
class MinMaxInterpolator : public Interpolator {
public:
    MinMaxInterpolator(CorrelatedNeighbors::DataAccess* dax);

    ValuesWithQualities_t interpolate(const Instrument& instrument, const TimeRange& t);

    void configure(const AlgorithmConfig& config);

private:
    struct MinMaxParameters {
        int minParameter, maxParameter;
        float noiseLevel;
        MinMaxParameters(int mi, int ma, float n)
            : minParameter(mi), maxParameter(ma), noiseLevel(n) { }
    };
    typedef std::map<int, MinMaxParameters> minmaxpar_t;

private:
    DataAccess* mDax;

    minmaxpar_t mMinMaxParameters;
};
#endif

// ========================================================================

class DataAccessUU : public CorrelatedNeighbors::DataAccess {
public:
    DataAccessUU(DataAccess* dax)
        : mDax(dax) { }

    const std::vector<float> fetchObservations(const Instrument& instrument, const TimeRange& t);
    const std::vector<float> fetchModelValues (const Instrument& instrument, const TimeRange& t);
    const CorrelatedNeighbors::neighbors_t findNeighbors(const Instrument& instrument, double maxsigma);

    void setDataAccess(DataAccess* dax)
        { mDax = dax; }

private:
    CorrelatedNeighbors::DataAccess* mDax;
};

// ========================================================================

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

#endif /* MINMAXINTERPOLATOR_H */
