// -*- c++ -*-

#ifndef MINMAXINTERPOLATOR_H
#define MINMAXINTERPOLATOR_H 1

#include "CorrelatedNeighborInterpolator.h"

#include <vector>

struct MinMaxValuesWithQualities_t {
    Interpolator::ValuesWithQualities_t par, min, max;
};

MinMaxValuesWithQualities_t MinMaxInterpolate(Interpolator* inter, CorrelatedNeighbors::DataAccess* dax, const Instrument& instrument,
                                              const ParameterInfo& parameterInfo, const TimeRange& t);

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

    std::vector<float> fetchObservations(const Instrument& instrument, const TimeRange& t);
    std::vector<float> fetchModelValues (const Instrument& instrument, const TimeRange& t);
    CorrelatedNeighbors::neighbors_t findNeighbors(const Instrument& instrument, double maxsigma);

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
