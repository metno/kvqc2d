// -*- c++ -*-

#ifndef CORRELATEDNEIGHBORINTERPOLATOR_H
#define CORRELATEDNEIGHBORINTERPOLATOR_H 1

#include "Interpolator.h"

#include "DBInterface.h"
#include "NeighborInterpolation.h"
#include <map>
#include <vector>

namespace CorrelatedNeighbors {

class DataAccess {
public:
    virtual ~DataAccess();
    virtual std::vector<float> fetchObservations(const Instrument& instrument, const TimeRange& t) = 0;
    virtual std::vector<float> fetchModelValues (const Instrument& instrument, const TimeRange& t) = 0;
    virtual NeighborDataVector findNeighbors(const Instrument& instrument, double maxsigma) = 0;
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

class DataFilter {
public:
    virtual ~DataFilter();
    virtual NeighborInterpolation::Data toNumerical(int paramid, float storage) const = 0;
    virtual float toStorage(int paramid, NeighborInterpolation::Interpolation::Quality q, float numerical) const = 0;
};
typedef boost::shared_ptr<DataFilter> DataFilterP;

class KvalobsFilter : public DataFilter {
public:
    virtual NeighborInterpolation::Data toNumerical(int paramid, float storage) const;
    virtual float toStorage(int paramid, NeighborInterpolation::Interpolation::Quality q, float numerical) const;
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

    DataFilterP getFilter() const
        { return mFilter; }

    void setFilter(DataFilterP f)
        { mFilter = f; }

private:
    typedef std::map<Instrument, NeighborDataVector, lt_Instrument> neighbor_map_t;

private:
    std::vector<float> interpolate_simple(const Instrument& instrument, const TimeRange& t);

    void calculate_delta(const float data0, const float dataN1, const float i0, const float iN1, int N,
                         float& slope, float& offset);

    const NeighborDataVector& find_neighbors(const Instrument& instrument, float maxsigma);

private:
    DataAccess* mDax;
    float mMaxSigma;

    typedef std::map<int,ParamInfo> ParamInfos;
    typedef ParamInfos::const_iterator ParamInfos_cit;
    ParamInfos mParamInfos;

    DataFilterP mFilter;

    neighbor_map_t neighbor_map;
};

} // namespace CorrelatedNeigbors

#endif /* CORRELATEDNEIGHBORINTERPOLATOR_H */
