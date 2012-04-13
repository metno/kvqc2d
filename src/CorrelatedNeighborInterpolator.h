// -*- c++ -*-

#ifndef CORRELATEDNEIGHBORINTERPOLATOR_H
#define CORRELATEDNEIGHBORINTERPOLATOR_H 1

#include "Interpolator.h"

#include <map>
#include <vector>

namespace CorrelatedNeighbors {

struct NeighborData {
    int neighborid;
    double offset, slope, sigma;
    NeighborData(int xid, double xoffset, double xslope, double xsigma)
        : neighborid(xid), offset(xoffset), slope(xslope), sigma(xsigma) { }
};

typedef std::vector<NeighborData> neighbors_t;

// ========================================================================

class DataAccess {
public:
    virtual ~DataAccess();
    virtual const std::vector<float> fetchObservations(const Instrument& instrument, const TimeRange& t) = 0;
    virtual const std::vector<float> fetchModelValues (const Instrument& instrument, const TimeRange& t) = 0;
    virtual const neighbors_t findNeighbors(const Instrument& instrument, double maxsigma) = 0;
};

// ========================================================================

class Interpolator : public ::Interpolator {
public:
    Interpolator(DataAccess* dax);

    ValuesWithQualities_t interpolate(const Instrument& instrument, const TimeRange& t);

    void configure(const AlgorithmConfig& config);

private:
    typedef std::map<Instrument, neighbors_t, lt_Instrument> neighbor_map_t;

private:
    std::vector<float> interpolate_simple(const Instrument& instrument, const TimeRange& t);

    void calculate_delta(const double data0, const double dataN1, const double i0, const double iN1, int N,
                         double& slope, double& offset);

    const neighbors_t& find_neighbors(const Instrument& instrument, double maxsigma);

private:
    DataAccess* mDax;
    float mMaxSigma;

    neighbor_map_t neighbor_map;
};

} // namespace CorrelatedNeigbors

#endif /* CORRELATEDNEIGHBORINTERPOLATOR_H */
