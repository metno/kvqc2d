// -*- c++ -*-

#ifndef GapInterpolationAlgorithm_H
#define GapInterpolationAlgorithm_H 1

#include "interpolation/CorrelatedNeighborInterpolator.h"
#include "interpolation/InterpolatorUU.h"
#include "interpolation/MinMaxInterpolator.h"
#include "Qc2Algorithm.h"

#include <memory>

class Interpolator;

class GapDataAccess : public CorrelatedNeighbors::DataAccess {
public:
    GapDataAccess(DBInterface* db)
        : mDB(db) { }

    std::vector<float> fetchObservations(const Instrument& instrument, const TimeRange& t);
    std::vector<float> fetchModelValues (const Instrument& instrument, const TimeRange& t);
    NeighborDataVector findNeighbors(const Instrument& instrument, double maxsigma);

    void setDatabase(DBInterface* db)
        { mDB = db; }

    void configure(const AlgorithmConfig& params);

private:
    DBInterface* mDB;
    FlagSetCU neighbor_flags;
};

// ========================================================================

class GapInterpolationAlgorithm : public Qc2Algorithm {
public:
    GapInterpolationAlgorithm();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    struct ParamGroupMissingRange {
        TimeRange range;
        typedef std::vector<kvalobs::kvData> MissingRange;
        typedef std::map<int, MissingRange> ParamMissingRanges;
        ParamMissingRanges paramMissingRanges;

        bool tryExtend(const kvalobs::kvData& missing);
        ParamGroupMissingRange(const kvalobs::kvData& missing);
    };

private:
    Instrument getMasterInstrument(const kvalobs::kvData& data);
    void makeUpdates(const ParamGroupMissingRange::MissingRange& mr, const Interpolator::ValuesWithQualities_t& interpolated,
                     const TimeRange& range, DBInterface::DataList& updates);

private:
    typedef DBInterface::DataList    DataList;
    typedef DataList::iterator       DataList_it;
    typedef DataList::const_iterator DataList_cit;

private:
    GapDataAccess* mDataAccess;
    CorrelatedNeighbors::Interpolator* mInterpolator;
    InterpolatorUU* mInterpolatorUU;

    typedef std::vector<ParameterInfo> ParameterInfos;
    typedef ParameterInfos::const_iterator ParameterInfos_it;
    ParameterInfos mParameterInfos;

    std::vector<int> tids;

    FlagSetCU missing_flags;
    FlagChange missing_flagchange;
};

#endif
