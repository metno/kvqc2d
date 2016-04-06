
#ifndef GAPDATA_HH
#define GAPDATA_HH 1

#include "GapUpdate.hh"
#include "DBInterface.h"
#include "TimeRange.h"

#include <kvalobs/kvModelData.h>

#include <vector>

class GapInterpolationAlgorithm;
class Instrument;
class ParameterInfo;

class GapData {
public:
    typedef std::vector<GapUpdate> DataUpdates;

    GapData(const Instrument& i, const TimeRange& timeRange, const ParameterInfo& parameterInfo, GapInterpolationAlgorithm* algo);
    virtual ~GapData() { }
    
    virtual int duration() const
        { return mDataPar.size(); }
    virtual Interpolation::SeriesData parameter(int time) const
        { return asValue(mDataPar, time); }
    virtual Interpolation::SeriesData minimum(int time) const
        { return asValue(mDataMin, time); }
    virtual Interpolation::SeriesData maximum(int time) const
        { return asValue(mDataMax, time); }
    virtual Interpolation::SupportData model(int time) const;
    
    virtual void setParameter(int time, Interpolation::Quality q, float value)
        { return toValue(mDataPar, time, q, value); }
    virtual void setMinimum(int time, Interpolation::Quality q, float value)
        { return toValue(mDataMin, time, q, value); }
    virtual void setMaximum(int time, Interpolation::Quality q, float value)
        { return toValue(mDataMax, time, q, value); }

    virtual int neighborCount();
    virtual Interpolation::SupportData neighbor(int n, int time);
    virtual Interpolation::SupportData transformedNeighbor(int n, int t);
    virtual float neighborWeight(int n);
    
    virtual Interpolation::SeriesData asValue(const DataUpdates&, int) const;
    virtual void toValue(DataUpdates&, int time, Interpolation::Quality q, float value);

    bool hasMinMax() const;

    const Instrument& mInstrument;
    TimeRange mTimeRange;
    const ParameterInfo& mParameterInfo;

    DataUpdates mDataPar;
    DataUpdates mDataMin;
    DataUpdates mDataMax;
    const std::vector<kvalobs::kvModelData> mModelData;

private:
    void fetchNeighborCorrelations();
    bool mFetchedNeighborCorrelations;

protected:
    GapInterpolationAlgorithm* mAlgo;
    NeighborDataVector mNeighborCorrelations;
    typedef std::vector<Interpolation::SupportDataList> NeighborObservations;
    NeighborObservations mNeighborObservations;
};

// ========================================================================

class GapDataUU : public GapData {
public:
    GapDataUU(const Instrument& i, const TimeRange& timeRange, const ParameterInfo& parameterInfo, GapInterpolationAlgorithm* algo);

    virtual Interpolation::SupportData model(int time) const;
    virtual Interpolation::SeriesData asValue(const DataUpdates&, int ) const;
    virtual void toValue(DataUpdates&, int time, Interpolation::Quality q, float value);

public:
    std::vector<float> mDataTA;
    std::vector<float> mModelTA;
};

// ========================================================================

typedef std::shared_ptr<GapData>   GapDataPtr;
typedef std::shared_ptr<GapDataUU> GapDataUUPtr;

#endif /* GAPDATA_HH */
