/*
 * KvalobsInterpolationData.cc
 *
 *  Created on: Apr 23, 2012
 *      Author: alexanderb
 */

#include "KvalobsData.h"

#include "helpers/AlgorithmHelpers.h"
#include "helpers/timeutil.h"
#include "foreach.h"

using NeighborInterpolation::SeriesData;
using NeighborInterpolation::SupportData;

namespace {
const int NA = NeighborInterpolation::extraData;
const float maxsigma = 3;
}

KvalobsData::KvalobsData(DBInterface* db, const Instrument& instrument, const TimeRange& t)
        : mDB(db), mTimeRangeExtended(TimeRange(Helpers::plusHour(t.t0, -NA), Helpers::plusHour(t.t1, NA))), mInstrument(instrument)
{
}

KvalobsData::~KvalobsData()
{
}

SeriesData KvalobsData::center(int time)
{
    if( centerObservations.empty() ) {
        FlagSetCU all;
        centerObservations = mDB->findDataMaybeTSLOrderObstime(mInstrument.stationid,
                mInstrument.paramid, mInstrument.type, mInstrument.sensor,
                mInstrument.level, mTimeRangeExtended, all);
    }

    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvData& d, centerObservations) {
        if( d.obstime() == t && !Helpers::isMissingOrRejected(d)) {
            return SeriesData(d.original());
        }
    }
    return SeriesData();
}

SupportData KvalobsData::model(int time)
{
    if( centerModel.empty() ) {
        centerModel = mDB->findModelData(mInstrument.stationid, mInstrument.paramid,
                mInstrument.level, mTimeRangeExtended);
    }
    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvModelData& m, centerModel) {
        if( m.obstime() == t ) {
            return SupportData(m.original());
        }
    }
    return SupportData();
}

SupportData KvalobsData::neighbor(int n, int time)
{
    if( neighborCorrelations.empty() ) {
        neighborCorrelations = mDB->findNeighborData(mInstrument.stationid,
                mInstrument.paramid, maxsigma);
        neighborObservations = std::vector<DBInterface::DataList>(neighbors());
    }
    if (n >= neighbors())
        return SupportData();

    const DBInterface::DataList& no = neighborObservations[n];
    if( no.empty() ) {
        const NeighborData& nd = getNeighborData(n);
        FlagSetCU all;
        neighborObservations[n] =
                mDB->findDataMaybeTSLOrderObstime(nd.neighborid, mInstrument.paramid,
                        DBInterface::INVALID_ID, DBInterface::INVALID_ID,
                        DBInterface::INVALID_ID, mTimeRangeExtended, all);
    }

    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvData& d, neighborObservations[n]) {
        if( d.obstime() == t ) {
            if( mNeighborFlags.matches(d) )
                return SupportData(d.original());
            else
                return SupportData();
        }
    }
    return SupportData();
}

SupportData KvalobsData::transformedNeighbor(int n, int time)
{
    const SupportData sd = neighbor(n, time);
    if (sd.usable()) {
        const NeighborData& nd = neighborCorrelations[n];
        return SupportData(nd.offset + nd.slope * sd.value());
    } else {
        return sd;
    }
}

float KvalobsData::neighborWeight(int neighbor)
{
    if( neighborCorrelations.empty() ) {
        neighborCorrelations = mDB->findNeighborData(mInstrument.stationid,
                mInstrument.paramid, maxsigma);
    }
    const float s = neighborCorrelations[neighbor].sigma;
    return 1 / (s * s * s);
}

void KvalobsData::configure(const AlgorithmConfig& params)
{
    params.getFlagSetCU(mNeighborFlags, "neighbor", "fmis=0", "U0=[37]&U2=0");
}

miutil::miTime KvalobsData::timeAtOffset(int time) const
{
    return Helpers::plusHour(mTimeRangeExtended.t0, time);
}
