/* -*- c++ -*-
 Kvalobs - Free Quality Control Software for Meteorological Observations

 Copyright (C) 2011 met.no

 Contact information:
 Norwegian Meteorological Institute
 Postboks 43 Blindern
 N-0313 OSLO
 NORWAY
 email: kvalobs-dev@met.no

 This file is part of KVALOBS

 KVALOBS is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 KVALOBS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with KVALOBS; if not, write to the Free Software Foundation Inc.,
 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "KvalobsNeighborData.h"

#include "helpers/AlgorithmHelpers.h"
#include "helpers/timeutil.h"
#include "AlgorithmConfig.h"
#include "FlagPatterns.h"
#include "foreach.h"

#define NDEBUG 1
#include "debug.h"

using Interpolation::SeriesData;
using Interpolation::SimpleResult;
using Interpolation::SupportData;

KvalobsNeighborData::KvalobsNeighborData(DBInterface* db, const Instrument& instrument, const TimeRange& t, const ParameterInfo& pi)
    : mDB(db)
    , mTimeRange(t)
    , mInstrument(instrument)
    , mParameterInfo(pi)
    , mFetchedNeighborCorrelations(false)
{
}

float KvalobsNeighborData::maximumOffset()
{
    return mParameterInfo.maxOffset;
}

int KvalobsNeighborData::duration()
{
    DBG("calc duration = " << mTimeRange.hours() + 1 << DBG1(mTimeRange.t0) << DBG1(mTimeRange.t1));
    return mTimeRange.hours() + 1;
}

SeriesData KvalobsNeighborData::parameter(int time)
{
    if (centerObservations.fetchRequired()) {
        FlagSetCU all;
        centerObservations.set(mDB->findDataMaybeTSLOrderObstime(mInstrument.stationid, mInstrument.paramid, mInstrument.type, mInstrument.sensor,
                mInstrument.level, mTimeRange, all));
    }

    const miutil::miTime t = timeAtOffset(time);
    return centerObservations.find(t, mParameterInfo);
}

void KvalobsNeighborData::setInterpolated(int time, Interpolation::Quality q, float value)
{
    DBG(DBG1(time) << DBG1(q) << DBG1(value));
    Interpolation::SimpleResult sr(time, q, mParameterInfo.toStorage(value));
    for(unsigned int i=0; i<interpolations.size(); ++i)
        if( interpolations[i].time == time ) {
            interpolations[i] = sr;
            return;
        }
    interpolations.push_back(sr);
}

SimpleResult KvalobsNeighborData::getInterpolated(int time)
{
    for(unsigned int i=0; i<interpolations.size(); ++i)
        if( interpolations[i].time == time )
            return interpolations[i];
    const SeriesData sd = parameter(time);
    if( sd.usable() && !sd.needsInterpolation() )
        return Interpolation::SimpleResult(time, Interpolation::OBSERVATION, sd.value());
    else
        return Interpolation::SimpleResult(time, Interpolation::FAILED, 0);
}

SupportData KvalobsNeighborData::model(int time)
{
    if( centerModel.fetchRequired() )
        centerModel.set(mDB->findModelData(mInstrument.stationid, mInstrument.paramid, mInstrument.level, mTimeRange));

    const miutil::miTime t = timeAtOffset(time);
    return centerModel.find(t, mParameterInfo);
}

void KvalobsNeighborData::fetchNeighborCorrelations()
{
    neighborCorrelations = mDB->findNeighborData(mInstrument.stationid, mInstrument.paramid, mParameterInfo.maxSigma);
    neighborObservations = NeighborObservations(neighborCorrelations.size());
    mFetchedNeighborCorrelations = true;
}

int KvalobsNeighborData::neighbors()
{
    if( !mFetchedNeighborCorrelations )
        fetchNeighborCorrelations();
    return neighborCorrelations.size();
}

SupportData KvalobsNeighborData::neighbor(int n, int time)
{
    DBG(DBG1(n) << DBG1(neighborCorrelations.size()));
    if (n >= neighbors())
        return SupportData();

    KvalobsSupportDataList& no = neighborObservations[n];
    if( no.fetchRequired() ) {
        const NeighborData& nd = getNeighborData(n);
        const Instrument& i = mInstrument;
        no.set(mDB->findDataMaybeTSLOrderObstime(nd.neighborid, i.paramid,
                DBInterface::INVALID_ID, DBInterface::INVALID_ID,
                DBInterface::INVALID_ID, mTimeRange, mNeighborFlags));
    }

    const miutil::miTime t = timeAtOffset(time);
    return no.find(t, mParameterInfo);
}

SupportData KvalobsNeighborData::transformedNeighbor(int n, int time)
{
    const SupportData sd = neighbor(n, time);
    if (sd.usable()) {
        const NeighborData& nd = neighborCorrelations[n];
        return SupportData(nd.offset + nd.slope * sd.value());
    } else {
        return sd;
    }
}

float KvalobsNeighborData::neighborWeight(int neighbor)
{
    if( !mFetchedNeighborCorrelations )
        fetchNeighborCorrelations();
    const float s = neighborCorrelations[neighbor].sigma;
    return 1 / (s * s * s);
}

miutil::miTime KvalobsNeighborData::timeAtOffset(int time) const
{
    return Helpers::plusHour(mTimeRange.t0, time);
}
