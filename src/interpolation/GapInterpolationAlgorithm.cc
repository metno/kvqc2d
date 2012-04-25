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

#include "helpers/AlgorithmHelpers.h"
#include "helpers/mathutil.h"
#include "helpers/timeutil.h"
#include "interpolation/KvalobsDataUU.h"
#include "interpolation/KvalobsMinMaxData.h"
#include "foreach.h"

#define NDEBUG 1
#include "debug.h"

GapInterpolationAlgorithm::GapInterpolationAlgorithm()
    : Qc2Algorithm("GapInterpolation")
    , mNeighborInterpolator(new Interpolation::NeighborInterpolator())
    , mMinMaxInterpolator(new Interpolation::MinMaxInterpolator())
{
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::configure( const AlgorithmConfig& params )
{
    Qc2Algorithm::configure(params);

    tids = params.getMultiParameter<int>("TypeId");

    params.getFlagSetCU(missing_flags,  "missing", "ftime=0&fmis=[23]&fhqc=0", "");
    params.getFlagChange(missing_flagchange, "missing_flagchange", "ftime=1");

    const std::vector<std::string> parameters = params.getMultiParameter<std::string>("Parameter");
    foreach(const std::string& pi, parameters)
        mParameterInfos.push_back(ParameterInfo(pi));
}

// ------------------------------------------------------------------------

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

// ------------------------------------------------------------------------

GapInterpolationAlgorithm::ParamGroupMissingRange::ParamGroupMissingRange(const kvalobs::kvData& missing)
    : range(missing.obstime(), missing.obstime())
{
    paramMissingRanges[missing.paramID()].push_back(missing);
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::makeUpdates(const ParamGroupMissingRange::MissingRange& mr,
                                            const Interpolation::SimpleResultVector& interpolated,
                                            const TimeRange& range, DBInterface::DataList& updates)
{
    DBGL;
    ParamGroupMissingRange::MissingRange::const_iterator dit = mr.begin();
    foreach(const Interpolation::SimpleResult& sr, interpolated) {
        if( dit == mr.end() ) {
            DBGL;
            break;
        }
        const int t = miutil::miTime::hourDiff(dit->obstime(), range.t0);
        const float c = dit->corrected();
        DBG(DBG1(sr.time) << DBG1(sr.quality) << DBG1(sr.value) << DBG1(dit->obstime()) << DBG1(t));
        if( t == sr.time && !( Helpers::equal(c, sr.value) || (sr.quality == Interpolation::FAILED && c < -32765) ) ) {
            DBGL;
            kvalobs::kvData dwrite(*dit);
            dwrite.corrected(sr.value);
            dwrite.controlinfo(missing_flagchange.apply(dwrite.controlinfo()));
            Helpers::updateCfailed(dwrite, "QC2d-2-I", CFAILED_STRING);
            Helpers::updateUseInfo(dwrite);
            DBG("update=" << dwrite << " quality=" << sr.quality);
            updates.push_back(dwrite);
        }
        ++dit;
    }
}

// ------------------------------------------------------------------------

struct HasParameter : public std::unary_function<bool, ParameterInfo> {
    bool operator() (const ParameterInfo& pi) const {
        return pi.parameter == parameter;
    }
    int parameter;
    HasParameter(int p) : parameter(p) { }
};

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::run()
{
    DBInterface::StationIDList StationIds;
    fillStationIDList(StationIds);

    typedef std::vector<ParamGroupMissingRange> MissingRanges;
    typedef std::map<Instrument, MissingRanges, lt_Instrument> InstrumentMissingRanges;
    InstrumentMissingRanges instrumentMissingRanges;

    std::vector<int> pids;
    foreach(const ParameterInfo& pi, mParameterInfos) {
        pids.push_back(pi.parameter);
        if( pi.minParameter > 0 )
            pids.push_back(pi.minParameter);
        if( pi.maxParameter > 0 )
            pids.push_back(pi.maxParameter);
    }
    DBGV(pids.size());

    const DBInterface::DataList missingData
        = database()->findDataOrderStationObstime(StationIds, pids, tids, TimeRange(UT0, UT1), missing_flags);
    DBGV(missingData.size());

    foreach(const kvalobs::kvData& d, missingData) {
        const Instrument i = getMasterInstrument(d);
        MissingRanges& mr = instrumentMissingRanges[i];
        if( mr.empty() || !mr.back().tryExtend(d) )
            mr.push_back(ParamGroupMissingRange(d));
    }

    DataList updates;
    foreach(InstrumentMissingRanges::value_type& imr, instrumentMissingRanges) {
        const Instrument& instrument = imr.first;
        const int parameter = instrument.paramid;

        const ParameterInfos_it pi = std::find_if(mParameterInfos.begin(), mParameterInfos.end(), HasParameter(parameter));
        if( pi == mParameterInfos.end() ) {
            error() << "parameter '" << instrument.paramid << " not found in parameter list";
            continue;
        }
        const bool minmax = (pi->minParameter > 0) && (pi->maxParameter > 0);

        foreach(ParamGroupMissingRange& pgmr, imr.second) {
            const TimeRange missingTime(Helpers::plusHour(pgmr.range.t0, -3), Helpers::plusHour(pgmr.range.t1, 3));
            KvalobsNeighborData knd(database(), instrument, missingTime, *pi);
            if( minmax ) {
                KvalobsMinMaxData mmd(knd);
                const Interpolation::Summary s = mMinMaxInterpolator->interpolate(mmd, *mNeighborInterpolator);

                makeUpdates(pgmr.paramMissingRanges[parameter       ], knd.getInterpolated(),    missingTime, updates);
                makeUpdates(pgmr.paramMissingRanges[pi->minParameter], mmd.getInterpolatedMin(), missingTime, updates);
                makeUpdates(pgmr.paramMissingRanges[pi->maxParameter], mmd.getInterpolatedMax(), missingTime, updates);
            } else {
                foreach(ParamGroupMissingRange::ParamMissingRanges::value_type& pr, pgmr.paramMissingRanges) {
                    const Interpolation::Summary s  = mNeighborInterpolator->interpolate(knd);
                    makeUpdates(pr.second, knd.getInterpolated(), missingTime, updates);
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
    foreach(const ParameterInfo& pi, mParameterInfos) {
        if( pi.minParameter == master.paramid || pi.maxParameter == master.paramid )
            master.paramid = pi.parameter;
    }
    return master;
}
