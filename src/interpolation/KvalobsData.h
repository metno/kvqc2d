// -*- c++ -*-

#ifndef KVALOBSDATA_H_
#define KVALOBSDATA_H_

#include "AlgorithmConfig.h"
#include "DBInterface.h"
#include "FlagPatterns.h"
#include "Interpolator.h"
#include "NeighborInterpolation.h"
#include "SimpleData.h"

#include <kvalobs/kvData.h>
#include <kvalobs/kvModelData.h>

class KvalobsData: public NeighborInterpolation::Data {
public:
    KvalobsData(DBInterface* db, const Instrument& i, const TimeRange& t);

    virtual ~KvalobsData();

    virtual int duration() const
    { return mTimeRangeExtended.hours() + 1; }

    virtual int neighbors() const
    { return neighborCorrelations.size(); }

    virtual float maximumOffset() const
    { return mMaxOffset; }

    virtual NeighborInterpolation::SeriesData center(int time);

    virtual NeighborInterpolation::SupportData model(int time);

    virtual NeighborInterpolation::SupportData transformedNeighbor(int n, int time);

    virtual float neighborWeight(int neighbor);

    void configure(const AlgorithmConfig& params);

    virtual NeighborInterpolation::SupportData neighbor(int n, int time);

    const NeighborData& getNeighborData(int n) const
    { return neighborCorrelations[n]; }

    virtual void setInterpolated(int time, NeighborInterpolation::Quality q, float value)
    { interpolations.push_back(NeighborInterpolation::Result(time, q, value)); }

protected:
    miutil::miTime timeAtOffset(int time) const;

protected:
    DBInterface* mDB;

    const TimeRange& mTimeRangeExtended;
    const Instrument& mInstrument;

    DBInterface::DataList centerObservations;
    DBInterface::ModelDataList centerModel;
    NeighborDataVector neighborCorrelations;
    std::vector<DBInterface::DataList> neighborObservations;
    float mMaxOffset;
    FlagSetCU mNeighborFlags;
    NeighborInterpolation::SimpleData::InterpolationVector interpolations;
};

#endif /* KVALOBSDATA_H_ */
