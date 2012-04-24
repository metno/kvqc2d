// -*- c++ -*-

#ifndef GapInterpolationAlgorithm_H
#define GapInterpolationAlgorithm_H 1

#include "interpolation/Instrument.h"
#include "interpolation/NeighborInterpolator.h"
#include "interpolation/MinMaxInterpolator.h"
#include "interpolation/ParameterInfo.h"
#include "interpolation/SimpleInterpolationResult.h"
#include "Qc2Algorithm.h"

#include <boost/shared_ptr.hpp>

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
    void makeUpdates(const ParamGroupMissingRange::MissingRange& mr, const Interpolation::SimpleResultVector& interpolated,
                     const TimeRange& range, DBInterface::DataList& updates);

private:
    typedef DBInterface::DataList    DataList;
    typedef DataList::iterator       DataList_it;
    typedef DataList::const_iterator DataList_cit;

private:
    boost::shared_ptr<Interpolation::NeighborInterpolator> mNeighborInterpolator;
    boost::shared_ptr<Interpolation::MinMaxInterpolator> mMinMaxInterpolator;

    typedef std::vector<ParameterInfo> ParameterInfos;
    typedef ParameterInfos::const_iterator ParameterInfos_it;
    ParameterInfos mParameterInfos;

    std::vector<int> tids;

    FlagSetCU missing_flags;
    FlagChange missing_flagchange;
};

#endif
