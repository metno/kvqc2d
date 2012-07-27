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

    mRAThreshold = params.getParameter("RA_threshold", 50.0f);

    params.getFlagSetCU(missing_flags,  "missing", "ftime=0&fmis=[123]&fhqc=0", "");
    params.getFlagChange(missing_flagchange_good, "missing_flagchange_good", "ftime=1");
    params.getFlagChange(missing_flagchange_bad,  "missing_flagchange_bad" , "ftime=2");
    params.getFlagChange(missing_flagchange_common, "missing_flagchange_common", "fmis=3->fmis=1;fmis=2->fmis=4");

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
                                            const TimeRange& range, DBInterface::DataList& updates,
                                            const ParameterInfo& parameterInfo)
{
    ParamGroupMissingRange::MissingRange::const_iterator dit = mr.begin();
    Interpolation::SimpleResultVector::const_iterator rit = interpolated.begin();
    for(; rit != interpolated.end() && dit != mr.end(); ++rit, ++dit) {
        const Interpolation::SimpleResult& sr = *rit;
        const int t = miutil::miTime::hourDiff(dit->obstime(), range.t0);
        if( t != sr.time || sr.quality == Interpolation::OBSERVATION || sr.quality == Interpolation::FAILED )
            continue;

        // TODO maybe move this to KvalobsNeighborData or so?
        const float oc = dit->corrected();
        const float nc = Helpers::round(sr.value, parameterInfo.roundingFactor);
        const bool sameNumericalValue = Helpers::equal(parameterInfo.toNumerical(oc), parameterInfo.toNumerical(nc));
        if( sameNumericalValue )
            continue;

        kvalobs::kvData dwrite(*dit);
        DBGV(sr.value);
        dwrite.corrected(nc);
        kvalobs::kvControlInfo ci = dwrite.controlinfo();
        if( sr.quality == Interpolation::GOOD )
            ci = missing_flagchange_good.apply(ci);
        else if( sr.quality == Interpolation::BAD )
            ci = missing_flagchange_good.apply(ci);
        dwrite.controlinfo(missing_flagchange_common.apply(ci));
        Helpers::updateCfailed(dwrite, "QC2d-2-I", CFAILED_STRING);
        Helpers::updateUseInfo(dwrite);
        DBG("update=" << dwrite << " quality=" << sr.quality);
        updates.push_back(dwrite);
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
    const DBInterface::StationIDList stationIDs(1, DBInterface::ALL_STATIONS);

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

    const DBInterface::DataList missingData
        = database()->findDataOrderStationObstime(stationIDs, pids, tids, TimeRange(UT0, UT1), missing_flags);

    foreach(const kvalobs::kvData& d, missingData) {
        const Instrument i = getMasterInstrument(d);
        MissingRanges& mr = instrumentMissingRanges[i];
        if( mr.empty() || !mr.back().tryExtend(d) )
            mr.push_back(ParamGroupMissingRange(d));
    }

    const ParameterInfos_it piTA = std::find_if(mParameterInfos.begin(), mParameterInfos.end(), HasParameter(211));
    const TimeRange timeIntervalShrinked = TimeRange(UT0, UT1).extendedByHours(-1);

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

        const Instrument instrumentTA(instrument.stationid, 211, DBInterface::INVALID_ID, instrument.type, DBInterface::INVALID_ID);

        foreach(ParamGroupMissingRange& pgmr, imr.second) {
            if( pgmr.range.t0 < timeIntervalShrinked.t0 || pgmr.range.t1 > timeIntervalShrinked.t1 ) {
                info() << "missing range obstime BETWEEN '" << pgmr.range.t0 << "' AND '" << pgmr.range.t1
                       << "' AND station=" << instrument.stationid << " AND paramid=" << instrument.paramid
                       << " at start/end of time interval, skipping interpolation attempt";
                continue;
            }
            const TimeRange missingTime = pgmr.range.extendedByHours(Interpolation::NeighborInterpolator::EXTRA_DATA);
            KvalobsNeighborData knd(database(), instrument, missingTime, *pi);
            if( minmax ) {
                KvalobsMinMaxData mmd(knd);
                if( parameter == 262 ) {
                    if( piTA == mParameterInfos.end() ) {
                        error() << "no TA (211) parameter info found while interpolating UU (262); please fix configuration files";
                        break;
                    }
                    KvalobsNeighborData kndTA(database(), instrumentTA, missingTime, *piTA);
                    KvalobsUUNeighborData kndUU(knd, kndTA);

                    KvalobsMinMaxData mmdTA(kndTA);
                    KvalobsUUMinMaxData mmdUU(kndUU, mmd, mmdTA);

                    const Interpolation::Summary s = mMinMaxInterpolator->interpolate(mmdUU, *mNeighborInterpolator);
                } else {
                    const Interpolation::Summary s = mMinMaxInterpolator->interpolate(mmd, *mNeighborInterpolator);
                }

                makeUpdates(pgmr.paramMissingRanges[parameter       ], knd.getInterpolated(),    missingTime, updates, *pi);
                makeUpdates(pgmr.paramMissingRanges[pi->minParameter], mmd.getInterpolatedMin(), missingTime, updates, *pi);
                makeUpdates(pgmr.paramMissingRanges[pi->maxParameter], mmd.getInterpolatedMax(), missingTime, updates, *pi);
            } else {
                if( parameter == 104 /*RA*/) {
                    DBGV( pgmr.paramMissingRanges.size() );
                    if( pgmr.paramMissingRanges.size() == 1 ) {
                        const int t0 = Interpolation::NeighborInterpolator::EXTRA_DATA-1, t1 = t0 + 1 + pgmr.range.hours() + 1;
                        DBG(DBG1(t0) << DBG1(t1));
                        const Interpolation::SeriesData sd0 = knd.parameter(t0), sd1 = knd.parameter(t1);
                        DBG(DBG1(sd0.usable()) << DBG1(sd1.usable()) << DBG1(sd0.usable()) << DBG1(sd1.usable()));
                        if( sd0.usable() && sd1.usable() && (sd1.value() < sd0.value() - mRAThreshold) ) {
                            info() << "RA decreasing around " << pgmr.range.t0 << " and " << pgmr.range.t1
                                   << ", no interpolation attempted";
                            continue;
                        }
                    }
                }
                foreach(ParamGroupMissingRange::ParamMissingRanges::value_type& pr, pgmr.paramMissingRanges) {
                    const Interpolation::Summary s  = mNeighborInterpolator->interpolate(knd);
                    makeUpdates(pr.second, knd.getInterpolated(), missingTime, updates, *pi);
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
