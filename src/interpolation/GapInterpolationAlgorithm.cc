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
                                            const TimeRange& range, DBInterface::DataList& updates,
                                            const ParameterInfo& parameterInfo)
{
    ParamGroupMissingRange::MissingRange::const_iterator dit = mr.begin();
    foreach(const Interpolation::SimpleResult& sr, interpolated) {
        if( dit == mr.end() )
            break;

        const int t = miutil::miTime::hourDiff(dit->obstime(), range.t0);

        // TODO maybe move this to KvalobsNeighborData or so?
        const float c = dit->corrected();
        const bool sameNumericalValue = Helpers::equal(parameterInfo.toNumerical(c), parameterInfo.toNumerical(sr.value));
        const bool failedInterpolationForMissingCorrected = (sr.quality == Interpolation::FAILED && c < -32765);

        if( t == sr.time && !( sameNumericalValue || failedInterpolationForMissingCorrected ) ) {
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

        const Instrument instrumentTA(instrument.stationid, 211, DBInterface::INVALID_ID, DBInterface::INVALID_ID, DBInterface::INVALID_ID);

        foreach(ParamGroupMissingRange& pgmr, imr.second) {
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
