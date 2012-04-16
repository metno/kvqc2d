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
    virtual std::vector<float> fetchObservations(const Instrument& instrument, const TimeRange& t) = 0;
    virtual std::vector<float> fetchModelValues (const Instrument& instrument, const TimeRange& t) = 0;
    virtual neighbors_t findNeighbors(const Instrument& instrument, double maxsigma) = 0;
};

// ========================================================================

struct ParamInfo : public BasicParameterInfo {

    /** maximum difference between interpolated and observed value
     * before / after a gap which is permitted to apply offset
     * correction */
    float offsetCorrectionLimit;

    ParamInfo(const std::string& info);
};

// ========================================================================

class Interpolator : public ::Interpolator {
public:
    Interpolator(DataAccess* dax);

    ValuesWithQualities_t interpolate(const Instrument& instrument, const TimeRange& t);

    void configure(const AlgorithmConfig& config);

    DataAccess* getDataAccess() const
        { return mDax; }

    void setDataAccess(DataAccess* dax)
        { mDax = dax; }

private:
    typedef std::map<Instrument, neighbors_t, lt_Instrument> neighbor_map_t;

private:
    std::vector<float> interpolate_simple(const Instrument& instrument, const TimeRange& t);

    void calculate_delta(const float data0, const float dataN1, const float i0, const float iN1, int N,
                         float& slope, float& offset);

    const neighbors_t& find_neighbors(const Instrument& instrument, float maxsigma);

private:
    DataAccess* mDax;
    float mMaxSigma;

    typedef std::map<int,ParamInfo> ParamInfos;
    typedef ParamInfos::const_iterator ParamInfos_cit;
    ParamInfos mParamInfos;

    neighbor_map_t neighbor_map;
};

} // namespace CorrelatedNeigbors

#endif /* CORRELATEDNEIGHBORINTERPOLATOR_H */
