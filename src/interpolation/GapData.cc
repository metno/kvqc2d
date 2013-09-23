
#include "GapData.hh"

#include "GapInterpolationAlgorithm.h"
#include "helpers/FormulaUU.h"
#include "Instrument.h"
#include "ParameterInfo.h"

#include "gdebug.h"

using Interpolation::SeriesData;
using Interpolation::SupportData;

GapData::GapData(const Instrument& i, const TimeRange& timeRange, const ParameterInfo& parameterInfo, GapInterpolationAlgorithm* algo)
    : mInstrument(i)
    , mTimeRange(timeRange)
    , mParameterInfo(parameterInfo)
    , mDataPar(timeRange.hours()+1)
    , mDataMin(timeRange.hours()+1)
    , mDataMax(timeRange.hours()+1)
    , mModelData(timeRange.hours()+1)
    , mFetchedNeighborCorrelations(false)
    , mAlgo(algo)
{
}

SupportData GapData::model(int time) const
{
    const float mv = mModelData.at(time).original();
    if( mv >= Interpolation::INVALID_VALUE )
        return SupportData(mv);
    else
        return SupportData();
}

SeriesData GapData::asValue(const DataUpdates& u, int time) const
{
    const GapUpdate& d = u.at(time);
    if( d.isNew() )
        return SeriesData(Interpolation::MISSING_VALUE, Interpolation::NO_ROW, false);
    return SeriesData(d.value(), d.quality(), d.usable());
}

void GapData::toValue(DataUpdates& u, int time, Interpolation::Quality q, float value)
{
    u.at(time).update(q, value);
}

void GapData::fetchNeighborCorrelations()
{
    DBGL;
    mNeighborCorrelations = mAlgo->findNeighborData(mInstrument.stationid, mInstrument.paramid, mParameterInfo.maxSigma);
    DBGL;
    mNeighborObservations = NeighborObservations(mNeighborCorrelations.size());
    DBGL;
    mFetchedNeighborCorrelations = true;
}

int GapData::neighborCount()
{
    if( !mFetchedNeighborCorrelations )
        fetchNeighborCorrelations();
    return mNeighborCorrelations.size();
}

SupportData GapData::neighbor(int n, int time)
{
    //DBG(DBG1(n) << DBG1(mNeighborCorrelations.size()));
    if (n >= neighborCount())
        return SupportData();

    Interpolation::SupportDataList& no = mNeighborObservations[n];
    if( no.empty() ) {
        const NeighborData& nd = mNeighborCorrelations[n];
        no = mAlgo->getNeighborData(mTimeRange, nd.neighborid, mInstrument.paramid);
    }

    return no.at(time);
}

SupportData GapData::transformedNeighbor(int n, int time)
{
    const SupportData sd = neighbor(n, time);
    if (sd.usable()) {
        const NeighborData& nd = mNeighborCorrelations[n];
        return SupportData(nd.offset + nd.slope * sd.value());
    } else {
        return sd;
    }
}

float GapData::neighborWeight(int neighbor)
{
    if( !mFetchedNeighborCorrelations )
        fetchNeighborCorrelations();
    const float s = mNeighborCorrelations[neighbor].sigma;
    return 1 / (s * s * s);
}

bool GapData::hasMinMax() const
{
    return (mParameterInfo.minParameter > 0) or (mParameterInfo.maxParameter > 0);
}

// ========================================================================

GapDataUU::GapDataUU(const Instrument& i, const TimeRange& timeRange, const ParameterInfo& parameterInfo, GapInterpolationAlgorithm* algo)
    : GapData(i, timeRange, parameterInfo, algo)
    , mDataTA(timeRange.hours()+1)
    , mModelTA(timeRange.hours()+1)
{
}

Interpolation::SupportData GapDataUU::model(int time) const
{
    const float TA = mModelTA.at(time);
    const float mv = mModelData.at(time).original();
    if( mv >= Interpolation::INVALID_VALUE )
        return Interpolation::SupportData(Helpers::formulaTD(TA, mv));
    else
        return Interpolation::SupportData();
}

SeriesData GapDataUU::asValue(const DataUpdates& u, int time) const
{
    const float TA = mDataTA.at(time);
    DBG(DBG1(TA) << DBG1(time));
    if( TA >= Interpolation::INVALID_VALUE ) {
        const GapUpdate& gu = u.at(time);
        DBG(DBG1(gu) << DBG1(gu.value()) << DBG1(gu.quality()));
        const float v = Helpers::formulaTD(TA, gu.value());
        const bool u = gu.usable() and (v > Interpolation::INVALID_VALUE);
        return SeriesData(v, gu.quality(), u);
    } else {
        return SeriesData();
    }
}

void GapDataUU::toValue(DataUpdates& u, int time, Interpolation::Quality q, float value)
{
    const float TA = mDataTA.at(time);
    if( TA >= Interpolation::INVALID_VALUE )
        u.at(time).update(q, Helpers::formulaUU(TA, value));
}
