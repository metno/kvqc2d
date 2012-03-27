// -*- c++ -*-

#ifndef CORRELATEDNEIGHBORINTERPOLATOR_H
#define CORRELATEDNEIGHBORINTERPOLATOR_H 1

#include "Interpolator.h"

#include <map>
#include <vector>

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
    virtual const std::vector<float> fetchObservations(int stationid, int paramid, const miutil::miTime& t0, const miutil::miTime& t1) = 0;
    virtual const std::vector<float> fetchModelValues (int stationid, int paramid, const miutil::miTime& t0, const miutil::miTime& t1) = 0;
    virtual const neighbors_t findNeighbors(int stationid, int paramid, double maxsigma) = 0;
};

// ========================================================================

class CorrelatedNeighborInterpolator : public Interpolator {
public:
    CorrelatedNeighborInterpolator(DataAccess* dax);

    ValuesWithQualities_t interpolate(const miutil::miTime& beforeGap, const miutil::miTime& afterGap,
                                      int stationid, int paramid);

    void configure(const AlgorithmConfig& config);

private:
    typedef std::map<int, neighbors_t> neighbor_map_t;

private:
    std::vector<float> interpolate_simple(int stationid, int paramid, const miutil::miTime& t0, const miutil::miTime& t1);
    
    void calculate_delta(const double data0, const double dataN1, const double i0, const double iN1, int N,
                         double& slope, double& offset);

    const neighbors_t& find_neighbors(int stationid, int paramid, double maxsigma);

private:
    DataAccess* mDax;

    neighbor_map_t neighbor_map;
};

#endif /* CORRELATEDNEIGHBORINTERPOLATOR_H */
