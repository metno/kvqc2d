// -*- c++ -*-

#ifndef GapInterpolationAlgorithm_H
#define GapInterpolationAlgorithm_H 1

#include "Qc2Algorithm.h"
#include "CorrelatedNeighborInterpolator.h"
#include <memory>

class Interpolator;

class GapDataAccess : public CorrelatedNeighbors::DataAccess {
public:
    GapDataAccess(DBInterface* db)
        : mDB(db) { }

    const std::vector<float> fetchObservations(const Instrument& instrument, const TimeRange& t);
    const std::vector<float> fetchModelValues (const Instrument& instrument, const TimeRange& t);
    const CorrelatedNeighbors::neighbors_t findNeighbors(const Instrument& instrument, double maxsigma);

    void setDatabase(DBInterface* db)
        { mDB = db; }
private:
    DBInterface* mDB;
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
    ::Interpolator* mInterpolator;

    std::vector<int> pids;
    std::vector<int> tids;

    FlagSetCU missing_flags, neighbor_flags;
    FlagChange missing_flagchange;
};

#endif
