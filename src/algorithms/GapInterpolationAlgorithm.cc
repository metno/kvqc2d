/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2012 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
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

#include "GapInterpolationAlgorithm.h"

#include "AlgorithmHelpers.h"
#include "DBInterface.h"
#include "foreach.h"

#define NDEBUG 1
#include "debug.h"

// ########################################################################

const std::vector<float> GapDataAccess::fetchObservations(const Instrument& instrument, const TimeRange& t)
{
    FlagSetCU neighbor_flags;
    neighbor_flags.setC(FlagPatterns("fmis=0", FlagPattern::CONTROLINFO));
    neighbor_flags.setU(FlagPatterns("U0=[37]&U2=0", FlagPattern::USEINFO));

    const DBInterface::DataList obs
        = mDB->findDataMaybeTSLOrderObstime(instrument.stationid, instrument.paramid, instrument.type,
                                            instrument.sensor, instrument.level, t, neighbor_flags);
    DBGV(t.t0);
    DBGV(t.t1);
    DBGV(obs.size());

    std::vector<float> series(t.hours()+1, ::Interpolator::INVALID);
    foreach(const kvalobs::kvData& d, obs) {
        const int hour = miutil::miTime::hourDiff(d.obstime(), t.t0);
        series[hour] = d.original();
    }
    DBGV(series.size());
    return series;
}

const std::vector<float> GapDataAccess::fetchModelValues (const Instrument& instrument, const TimeRange& t)
{
    const DBInterface::ModelDataList modelData
        = mDB->findModelData(instrument.stationid, instrument.paramid, instrument.level, t);
    DBGV(modelData.size());

    std::vector<float> series(t.hours()+1, ::Interpolator::INVALID);
    foreach(const kvalobs::kvModelData& d, modelData) {
        const int hour = miutil::miTime::hourDiff(d.obstime(), t.t0);
        series[hour] = d.original();
    }
    DBGV(series.size());
    return series;
}

const CorrelatedNeighbors::neighbors_t GapDataAccess::findNeighbors(const Instrument& instrument, double maxsigma)
{
    DBGV(instrument.stationid);
    DBGV(instrument.paramid);
    CorrelatedNeighbors::neighbors_t n = mDB->findNeighborData(instrument.stationid, instrument.paramid);
    DBGV(n.size());
    return n;
}

// ========================================================================

GapInterpolationAlgorithm::GapInterpolationAlgorithm()
    : Qc2Algorithm("GapInterpolate")
    , mDataAccess(new GapDataAccess(0))
    , mInterpolator(new CorrelatedNeighbors::Interpolator(mDataAccess))
{
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::configure( const AlgorithmConfig& params )
{
    Qc2Algorithm::configure(params);

    pids = params.getMultiParameter<int>("ParamId");
    tids = params.getMultiParameter<int>("TypeId");

    params.getFlagSetCU(missing_flags,  "missing", "ftime=0&fmis=[1234]&fhqc=0", "");
    params.getFlagSetCU(neighbor_flags, "neighbor", "fmis=0", "U0=[37]&U2=0");
    params.getFlagChange(missing_flagchange, "missing_flagchange", "ftime=1;fmis=3->fmis=1;fmis=2->fmis=4");

    mInterpolator->configure(params);
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::run()
{
    mDataAccess->setDatabase(database());
    DBInterface::StationIDList StationIds;
    fillStationIDList(StationIds);
    DBGV(StationIds.size());

    typedef std::vector<kvalobs::kvData> MissingRange;
    typedef std::vector<MissingRange> MissingRanges;
    typedef std::map<Instrument, MissingRanges, lt_Instrument> InstrumentMissingRanges;
    InstrumentMissingRanges instrumentMissingRanges;

    const DBInterface::DataList missingData
        = database()->findDataOrderStationObstime(StationIds, pids, tids, TimeRange(UT0, UT1), missing_flags);
    DBGV(missingData.size());

    foreach(const kvalobs::kvData& d, missingData) {
        const Instrument i(d);
        MissingRanges& mr = instrumentMissingRanges[i];
        if( mr.empty() || miutil::miTime::hourDiff(d.obstime(), mr.back().back().obstime()) > 1 ) {
            mr.push_back(MissingRange(1, d));
            DBG("new time range at " << d);
        } else {
            mr.back().push_back(d);
            DBG("extending time range with " << d);
        }
    }

    DataList_t updates;
    foreach(InstrumentMissingRanges::value_type& imr, instrumentMissingRanges) {
        const Instrument& d = imr.first;
        foreach(MissingRange& mr, imr.second) {

            const TimeRange missingTime(Helpers::plusHour(mr.front().obstime(), -1), Helpers::plusHour(mr.back().obstime(), 1));
            DBGV(missingTime.t0);
            DBGV(missingTime.t1);
            const Instrument instrument(d);
            const Interpolator::ValuesWithQualities_t interpolated  = mInterpolator->interpolate(instrument, missingTime);
            DBGV(interpolated.size());

            MissingRange::const_iterator dit = mr.begin();
            foreach(const ::Interpolator::ValueWithQuality vq, interpolated) {
                kvalobs::kvData dwrite(*dit++);
                DBGV(dwrite);
                DBGV(vq.value);
                if( Helpers::equal(dwrite.corrected(), vq.value) )
                    continue;
                dwrite.corrected(vq.value);
                dwrite.controlinfo(missing_flagchange.apply(dwrite.controlinfo()));
                Helpers::updateCfailed(dwrite, "QC2d-2-I", CFAILED_STRING);
                Helpers::updateUseInfo(dwrite);
                updates.push_back(dwrite);
            }
        }
    }
    if( !updates.empty() )
        storeData(updates);
}
