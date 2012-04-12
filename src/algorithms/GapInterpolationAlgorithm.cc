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

const std::vector<float> GapDataAccess::fetchObservations(const Instrument& instrument, const TimeRange& t)
{
    FlagSetCU neighbor_flags;
    neighbor_flags.setC(FlagPatterns("fmis=0", FlagPattern::CONTROLINFO));
    neighbor_flags.setU(FlagPatterns("U0=[37]&U2=0", FlagPattern::USEINFO));

    const DBInterface::DataList obs
        = mDB->findDataMaybeTSLOrderObstime(instrument.stationid, instrument.paramid, instrument.type,
                                            instrument.sensor, instrument.level, t, neighbor_flags);
    DBGV(obs.size());

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
    return mDB->findNeighborData(instrument.stationid, instrument.paramid);
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
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::run()
{
    mDataAccess->setDatabase(database());
    DBInterface::StationList StationList;
    DBInterface::StationIDList StationIds;
    fillStationLists(StationList, StationIds);
    DBGV(StationIds.size());

    foreach(const kvalobs::kvStation& station, StationList) {
        // FIXME this does not work as expected if there are several
        // parameter ids; the data will be ordered by obstime, so
        // parameters will be mixed
        const DBInterface::DataList missingData
            = database()->findDataOrderObstime(station.stationID(), pids, tids, DBInterface::INVALID_ID,
                                               DBInterface::INVALID_ID, TimeRange(UT0, UT1), missing_flags);
        DBGV(missingData.size());

        if( missingData.empty() )
            continue;

        DataList_t updates;
        for(DataList_cit mark = missingData.begin(); mark != missingData.end(); /* */ ) {

            // find a series of continuous hours with missing data
            DataList_cit start = mark;
            DBGV(*start);
            miutil::miTime t = start->obstime();
            do {
                t.addHour(1);
                ++mark;
                DBGV(t);
            } while( mark != missingData.end() && mark->obstime() == t );
            DataList_cit end = mark;
            --end;
            DBGV(*end);

            const TimeRange missingTime(Helpers::plusHour(start->obstime(), -1), Helpers::plusHour(end->obstime(), 1));
            DBGV(missingTime.t0);
            DBGV(missingTime.t1);
            const Instrument instrument(*start);
            DBGV(instrument.stationid);
            const Interpolator::ValuesWithQualities_t interpolated  = mInterpolator->interpolate(instrument, missingTime);
            DBGV(interpolated.size());

            foreach(const ::Interpolator::ValueWithQuality vq, interpolated) {
                kvalobs::kvData dwrite(*start++);
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
        if( !updates.empty() )
            storeData(updates);
    }
}
