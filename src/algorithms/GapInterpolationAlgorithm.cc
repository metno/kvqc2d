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
#include "MinMaxInterpolator.h"
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

    std::vector<float> series(t.hours()+1, ::Interpolator::INVALID);
    foreach(const kvalobs::kvData& d, obs) {
        const int hour = miutil::miTime::hourDiff(d.obstime(), t.t0);
        series[hour] = d.original();
    }
    return series;
}

const std::vector<float> GapDataAccess::fetchModelValues (const Instrument& instrument, const TimeRange& t)
{
    const DBInterface::ModelDataList modelData
        = mDB->findModelData(instrument.stationid, instrument.paramid, instrument.level, t);

    std::vector<float> series(t.hours()+1, ::Interpolator::INVALID);
    foreach(const kvalobs::kvModelData& d, modelData) {
        const int hour = miutil::miTime::hourDiff(d.obstime(), t.t0);
        series[hour] = d.original();
    }
    return series;
}

const CorrelatedNeighbors::neighbors_t GapDataAccess::findNeighbors(const Instrument& instrument, double maxsigma)
{
    return mDB->findNeighborData(instrument.stationid, instrument.paramid, maxsigma);
}

// ========================================================================

GapInterpolationAlgorithm::GapInterpolationAlgorithm()
    : Qc2Algorithm("GapInterpolate")
    , mDataAccess(new GapDataAccess(0))
    , mInterpolator(new CorrelatedNeighbors::Interpolator(mDataAccess))
    , mInterpolatorUU(new InterpolatorUU(mInterpolator))
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

bool GapInterpolationAlgorithm::ParamGroupMissingRange::tryExtend(const kvalobs::kvData& missing)
{
    const miutil::miTime obstime = missing.obstime();
    const int hd = miutil::miTime::hourDiff(obstime, range.t1);
    assert(hd>=0);
    if( hd > 3 )
        return false;
    range.t1 = obstime;
    paramMissingRanges[missing.paramID()].push_back(missing);
    return true;
}

GapInterpolationAlgorithm::ParamGroupMissingRange::ParamGroupMissingRange(const kvalobs::kvData& missing)
    : range(missing.obstime(), missing.obstime())
{
    paramMissingRanges[missing.paramID()].push_back(missing);
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::makeUpdates(const ParamGroupMissingRange::MissingRange& mr, const Interpolator::ValuesWithQualities_t& interpolated,
                                            const TimeRange& range, DBInterface::DataList& updates)
{
    ParamGroupMissingRange::MissingRange::const_iterator dit = mr.begin();
    int h = 0;
    foreach(const ::Interpolator::ValueWithQuality& vq, interpolated) {
        if( dit == mr.end() )
            break;
        if( dit->obstime() != Helpers::plusHour(range.t0, h++) )
            continue;
        kvalobs::kvData dwrite(*dit++);
        if( Helpers::equal(dwrite.corrected(), vq.value) )
            continue;
        dwrite.corrected(vq.value);
        dwrite.controlinfo(missing_flagchange.apply(dwrite.controlinfo()));
        Helpers::updateCfailed(dwrite, "QC2d-2-I", "");//CFAILED_STRING);
        Helpers::updateUseInfo(dwrite);
        DBG("update=" << dwrite << " quality=" << vq.quality);
        updates.push_back(dwrite);
    }
}

void GapInterpolationAlgorithm::run()
{
    mDataAccess->setDatabase(database());
    DBInterface::StationIDList StationIds;
    fillStationIDList(StationIds);

    typedef std::vector<ParamGroupMissingRange> MissingRanges;
    typedef std::map<Instrument, MissingRanges, lt_Instrument> InstrumentMissingRanges;
    InstrumentMissingRanges instrumentMissingRanges;

    const DBInterface::DataList missingData
        = database()->findDataOrderStationObstime(StationIds, pids, tids, TimeRange(UT0, UT1), missing_flags);

    foreach(const kvalobs::kvData& d, missingData) {
        const Instrument i = getMasterInstrument(d);
        MissingRanges& mr = instrumentMissingRanges[i];
        if( mr.empty() || !mr.back().tryExtend(d) )
            mr.push_back(ParamGroupMissingRange(d));
    }

    DataList updates;
    foreach(InstrumentMissingRanges::value_type& imr, instrumentMissingRanges) {
        const Instrument& instrument = imr.first;
        DBGV(instrument.paramid);
        Interpolator* interpolator = mInterpolator;

        const int par = instrument.paramid;
        int minPar, maxPar;
        bool minmax = false;
        if( par == 211 ) {
            minmax = true;
            minPar = 213;
            maxPar = 215;
        } else if( par == 262 ) {
            minmax = true;
            minPar = 264;
            maxPar = 265;
            interpolator = mInterpolatorUU;
        }

        foreach(ParamGroupMissingRange& pgmr, imr.second) {

            // TODO need to split the time range again such that
            // events like missing
            // master-master-master-slave-master-master use three
            // interpolations (3*master, 1*slave, 2*master)

            const TimeRange missingTime(Helpers::plusHour(pgmr.range.t0, -1), Helpers::plusHour(pgmr.range.t1, 1));
            if( minmax ) {
                MinMaxValuesWithQualities_t mmwq = MinMaxInterpolate(interpolator, mDataAccess, instrument, minPar, maxPar, 0.5, missingTime);

                makeUpdates(pgmr.paramMissingRanges[par   ], mmwq.par, pgmr.range, updates);
                makeUpdates(pgmr.paramMissingRanges[minPar], mmwq.min, pgmr.range, updates);
                makeUpdates(pgmr.paramMissingRanges[maxPar], mmwq.max, pgmr.range, updates);
            } else {
                foreach(ParamGroupMissingRange::ParamMissingRanges::value_type& pr, pgmr.paramMissingRanges) {
                    const Interpolator::ValuesWithQualities_t interpolated  = mInterpolator->interpolate(Instrument(pr.second.front()), missingTime);
                    makeUpdates(pr.second, interpolated, pgmr.range, updates);
                }
            }
        }
    }
    if( !updates.empty() )
        storeData(updates);
}

// ------------------------------------------------------------------------

Instrument GapInterpolationAlgorithm::getMasterInstrument(const kvalobs::kvData& data)
{
    Instrument master(data);
    if( master.paramid == 213 || master.paramid == 215 )
        master.paramid = 211;
    else if( master.paramid == 264 || master.paramid == 265 )
        master.paramid = 262;
    return master;
}
