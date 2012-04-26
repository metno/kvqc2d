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
using Interpolation::SupportData;

namespace {

const float MAX_SIGMA = 3;

} // anonymous namespace

KvalobsNeighborData::KvalobsNeighborData(DBInterface* db, const Instrument& instrument, const TimeRange& t, const ParameterInfo& pi)
        : mDB(db), mTimeRange(t), mInstrument(instrument), mParameterInfo(pi)
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

Interpolation::SeriesData KvalobsNeighborData::parameter(int time)
{
    if (centerObservations.empty()) {
        FlagSetCU all;
        centerObservations = mDB->findDataMaybeTSLOrderObstime(mInstrument.stationid, mInstrument.paramid, mInstrument.type, mInstrument.sensor,
                mInstrument.level, mTimeRange, all);
    }

    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvData& d, centerObservations) {
        if( d.obstime() == t && !Helpers::isMissingOrRejected(d)) {
            return SeriesData(mParameterInfo.toNumerical(d.original()));
        }
    }
    return SeriesData();
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

Interpolation::SimpleResult KvalobsNeighborData::getInterpolated(int time)
{
    for(unsigned int i=0; i<interpolations.size(); ++i)
        if( interpolations[i].time == time )
            return interpolations[i];
    const SeriesData sd = parameter(time);
    return Interpolation::SimpleResult(time, Interpolation::OBSERVATION ,sd.value());
}

SupportData KvalobsNeighborData::model(int time)
{
    if( centerModel.empty() ) {
        centerModel = mDB->findModelData(mInstrument.stationid, mInstrument.paramid, mInstrument.level, mTimeRange);
    }
    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvModelData& m, centerModel) {
        if( m.obstime() == t ) {
            return SupportData(mParameterInfo.toNumerical(m.original()));
        }
    }
    return SupportData();
}

int KvalobsNeighborData::neighbors()
{
    if( neighborCorrelations.empty() ) {
        neighborCorrelations = mDB->findNeighborData(mInstrument.stationid, mInstrument.paramid, MAX_SIGMA);
        neighborObservations = std::vector<DBInterface::DataList>(neighborCorrelations.size());
    }
    return neighborCorrelations.size();
}

SupportData KvalobsNeighborData::neighbor(int n, int time)
{
    DBG(DBG1(n) << DBG1(neighborCorrelations.size()));
    if (n >= neighbors())
        return SupportData();

    DBInterface::DataList& no = neighborObservations[n];
    if( no.empty() ) {
        const NeighborData& nd = getNeighborData(n);
        FlagSetCU all;
        const Instrument& i = mInstrument;
        no = mDB->findDataMaybeTSLOrderObstime(nd.neighborid, i.paramid,
                DBInterface::INVALID_ID, DBInterface::INVALID_ID,
                DBInterface::INVALID_ID, mTimeRange, all);
    }

    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvData& d, no) {
        if( d.obstime() == t ) {
            if( mNeighborFlags.matches(d) )
                return SupportData(mParameterInfo.toNumerical(d.original()));
            else
                return SupportData();
        }
    }
    return SupportData();
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
    if( neighborCorrelations.empty() ) {
        const Instrument& i = mInstrument;
        neighborCorrelations = mDB->findNeighborData(i.stationid, i.paramid, MAX_SIGMA);
    }
    const float s = neighborCorrelations[neighbor].sigma;
    return 1 / (s * s * s);
}

miutil::miTime KvalobsNeighborData::timeAtOffset(int time) const
{
    return Helpers::plusHour(mTimeRange.t0, time);
}

