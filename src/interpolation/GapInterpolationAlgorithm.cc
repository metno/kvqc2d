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

#include "GapData.hh"
#include "helpers/AlgorithmHelpers.h"
#include "helpers/FormulaUU.h"
#include "helpers/mathutil.h"
#include "helpers/timeutil.h"
#include "interpolation/InterpolationError.hh"
#include "interpolation/KvalobsMinMaxData.h"
#include "interpolation/KvalobsNeighborData.h"
#include "interpolation/MinMaxInterpolator.h"
#include "interpolation/MinMaxReconstruction.h"
#include "interpolation/NeighborInterpolator.h"
#include "foreach.h"

#include <kvalobs/kvDataOperations.h>

#include <boost/make_shared.hpp>

#include "gdebug.h"

static const int KVALOBS_PARAMID_RA = 104;
static const int KVALOBS_PARAMID_TA = 211;
static const int KVALOBS_PARAMID_UU = 262;

// ========================================================================

namespace /* anonymous */ {
struct HasParameter : public std::unary_function<bool, ParameterInfo> {
    bool operator() (const ParameterInfo& pi) const {
        return pi.parameter == parameter;
    }
    int parameter;
    HasParameter(int p) : parameter(p) { }
};

bool isFailed(const Interpolation::SeriesData& d)
{
    DBGV(d);
    return d.quality() == Interpolation::FAILED;
}

bool isReliableMinMax(const Interpolation::SeriesData& d)
{
    DBGV(d);
    return d.quality() < Interpolation::FAILED;
}

void discardUpdate(GapUpdate& gu) {
    if( not gu.isNew() ) {
        DBGV(gu);
        gu.update(Interpolation::FAILED, Interpolation::MISSING_VALUE, true);
    }
}

} // anonymous namespace

// ========================================================================

GapInterpolationAlgorithm::ParamGroupMissingRange::ParamGroupMissingRange(const kvalobs::kvData& missing)
    : range(missing.obstime(), missing.obstime())
{
    paramMissingRanges[missing.paramID()].push_back(missing);
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

// ========================================================================

GapInterpolationAlgorithm::GapInterpolationAlgorithm()
    : Qc2Algorithm("GapInterpolation")
{
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::configure( const AlgorithmConfig& params )
{
    Qc2Algorithm::configure(params);

    tids = params.getMultiParameter<int>("TypeId");

    mRAThreshold = params.getParameter("RA_threshold", 50.0f);

    params.getFlagSetCU(missing_flags,  "missing", "ftime=0&fmis=[123]&fhqc=0", "");
    params.getFlagSetCU(mNeighborFlags, "neighbors", "", "U2=0");
    params.getFlagSetCU(mDataFlagsUUTA, "data_UU_TA", "ftime=[012]&fmis=[024]", "");

    params.getFlagChange(missing_flagchange_good,   "missing_flagchange_good", "ftime=1");
    params.getFlagChange(missing_flagchange_bad,    "missing_flagchange_bad" , "ftime=2");
    params.getFlagChange(missing_flagchange_failed, "missing_flagchange_failed" , "ftime=3");
    params.getFlagChange(missing_flagchange_common, "missing_flagchange_common", "fmis=3->fmis=1;fmis=2->fmis=4");

    const std::vector<std::string> parameters = params.getMultiParameter<std::string>("Parameter");
    foreach(const std::string& pi, parameters)
        mParameterInfos.push_back(ParameterInfo(pi));
}

// ------------------------------------------------------------------------

NeighborDataVector GapInterpolationAlgorithm::findNeighborData(int stationid, int paramid, float maxsigma)
{
    DBG(DBG1(stationid) << DBG1(paramid) << DBG1(maxsigma));
    return database()->findNeighborData(stationid, paramid, maxsigma); 
}

// ------------------------------------------------------------------------

Interpolation::SupportDataList GapInterpolationAlgorithm::getNeighborData(const TimeRange& t, int neighborid, int paramid)
{
    DBGL;
    Interpolation::SupportDataList nd(t.hours() + 1);

    const DataList dl = database()->findDataMaybeTSLOrderObstime(neighborid, paramid,
                                                                 DBInterface::INVALID_ID, DBInterface::INVALID_ID,
                                                                 DBInterface::INVALID_ID, t, mNeighborFlags);

    if( paramid == KVALOBS_PARAMID_UU ) {
        const DataList dlTA = database()->findDataMaybeTSLOrderObstime(neighborid, KVALOBS_PARAMID_TA,
                                                                       DBInterface::INVALID_ID, DBInterface::INVALID_ID,
                                                                       DBInterface::INVALID_ID, t, mDataFlagsUUTA);

        std::vector<float> valueUU(nd.size(), Interpolation::MISSING_VALUE), valueTA(nd.size(), Interpolation::MISSING_VALUE);
        foreach(const kvalobs::kvData& d, dl) {
            const int time = miutil::miTime::hourDiff(d.obstime(), t.t0);
            valueUU.at(time) = d.original();
        }
        foreach(const kvalobs::kvData& d, dlTA) {
            const int time = miutil::miTime::hourDiff(d.obstime(), t.t0);
            valueTA.at(time) = d.corrected();
        }
        for(std::size_t t=0; t<nd.size(); ++t) {
            const float ta = valueTA.at(t), uu = valueUU.at(t);
            if( ta > Interpolation::INVALID_VALUE and uu > Interpolation::INVALID_VALUE )
                nd.at(t) = Interpolation::SupportData(Helpers::formulaTD(ta, uu));
        }
    } else {
        foreach(const kvalobs::kvData& d, dl) {
            const int time = miutil::miTime::hourDiff(d.obstime(), t.t0);
            nd.at(time) = Interpolation::SupportData(d.original());
        }
    }
    return nd;
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::makeUpdates(GapData::DataUpdates& dul, const ParameterInfo& pi, DBInterface::DataList& updates)
{
    foreach(GapUpdate& du, dul) {
        if( not du.isNew() and du.isUpdated() ) {
            const float v = pi.toStorage(du.value());
            const int q = du.quality();
            if( q == Interpolation::FAILED )  {
                du.flagchange(missing_flagchange_failed);
            } else if (q == Interpolation::GOOD or q == Interpolation::BAD) {
                du.corrected(v).flagchange(missing_flagchange_common);
                if (q == Interpolation::GOOD)
                    du.flagchange(missing_flagchange_good);
                else
                    du.flagchange(missing_flagchange_bad);
            } else {
                DBGV(du);
                continue;
            }
            if( du.needsWrite() ) {
                du.cfailed("QC2d-2-I");
                updates.push_back(du.data());
                DBG("update " << du);
            }
        }
    }
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::makeUpdates(GapData& data)
{
    DBInterface::DataList updates;
    makeUpdates(data.mDataPar, data.mParameterInfo, updates);
    makeUpdates(data.mDataMin, data.mParameterInfo, updates);
    makeUpdates(data.mDataMax, data.mParameterInfo, updates);
    if( not updates.empty() )
        storeData(updates);
}

// ------------------------------------------------------------------------

GapInterpolationAlgorithm::InstrumentMissingRanges GapInterpolationAlgorithm::findMissing()
{
    const DBInterface::StationIDList stationIDs(1, DBInterface::ALL_STATIONS);
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

    InstrumentMissingRanges instrumentMissingRanges;
    foreach(const kvalobs::kvData& d, missingData) {
        const Instrument i = getMasterInstrument(d);
        MissingRanges& mr = instrumentMissingRanges[i];
        if( mr.empty() or not mr.back().tryExtend(d) )
            mr.push_back(ParamGroupMissingRange(d));
    }
    return instrumentMissingRanges;
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

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::run()
{
    InstrumentMissingRanges instrumentMissingRanges = findMissing();

    foreach(InstrumentMissingRanges::value_type& imr, instrumentMissingRanges)
        if( imr.first.paramid == KVALOBS_PARAMID_TA )
            interpolateMissingRange(imr.first, imr.second);

    foreach(InstrumentMissingRanges::value_type& imr, instrumentMissingRanges)
        if( imr.first.paramid != KVALOBS_PARAMID_TA )
            interpolateMissingRange(imr.first, imr.second);
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::interpolateMissingRange(const Instrument& instrument, const MissingRanges& mr)
{
    DBG(DBG1(instrument.stationid) << DBG1(instrument.paramid));
    const ParameterInfo& pi = findParameterInfo(instrument.paramid);

    foreach(const ParamGroupMissingRange& pgmr, mr) {
        if( not checkTimeRangeLimits(instrument, pgmr) )
            continue;

        TimeRange missingTime = pgmr.range.extendedByHours(Interpolation::NeighborInterpolator::EXTRA_DATA);
        DBGV(missingTime);
        if( missingTime.t0 < UT0 )
            missingTime.t0 = UT0;
        if( missingTime.t1 > UT1 )
            missingTime.t1 = UT1;
        DBG(DBG1(pgmr.range) << DBG1(missingTime));

        GapDataPtr data = findSeriesData(instrument, missingTime, pi);

        if( not data or seriesHasMissingRows(*data) )
            continue;

        discardUnreliableMinMax(*data);

        if( instrument.paramid == KVALOBS_PARAMID_RA and hasRADownStep(*data) )
            continue;

        replaceFromOtherTypeid(*data);

        interpolateFromMinMax(*data);

        interpolateFromNeighbors(*data);

        reconstructMinMax(*data);
        
        makeUpdates(*data);
    }
}

// ------------------------------------------------------------------------

bool GapInterpolationAlgorithm::interpolateFromMinMax(GapData& data)
{
    DBGL;
    KvalobsMinMaxInterpolatorData mmiData(data);
    Interpolation::MinMaxInterpolator mmi;
    mmi.run(mmiData);
    return false;
}

// ------------------------------------------------------------------------

bool GapInterpolationAlgorithm::interpolateFromNeighbors(GapData& data)
{
    DBGL;
    KvalobsNeighborData nData(data);
    Interpolation::NeighborInterpolator ni;
    DBGL;
    ni.run(nData);
    DBGL;
    return false;
}

// ------------------------------------------------------------------------

const ParameterInfo& GapInterpolationAlgorithm::findParameterInfo(int parameter)
{
    const ParameterInfos_it pi = std::find_if(mParameterInfos.begin(), mParameterInfos.end(), HasParameter(parameter));
    if( pi == mParameterInfos.end() ) {
        std::ostringstream what;
        what << "parameter '" << parameter << " not found in parameter list";
        throw InterpolationError(Message::ERROR, what.str());
    }
    return *pi;
}

// ------------------------------------------------------------------------

bool GapInterpolationAlgorithm::checkTimeRangeLimits(const Instrument& instrument, const ParamGroupMissingRange& pgmr)
{
    const TimeRange timeIntervalShrinked = TimeRange(UT0, UT1).extendedByHours(-2);
    DBG(DBG1(pgmr.range) << DBG1(timeIntervalShrinked));
    if( pgmr.range.t0 < timeIntervalShrinked.t0 || pgmr.range.t1 > timeIntervalShrinked.t1 ) {
        info() << "missing range obstime BETWEEN '" << pgmr.range.t0 << "' AND '" << pgmr.range.t1
               << "' AND station=" << instrument.stationid << " AND paramid=" << instrument.paramid
               << " at start/end of time interval, skipping interpolation attempt";
        return false;
    } else {
        return true;
    }
}

// ------------------------------------------------------------------------

GapDataPtr GapInterpolationAlgorithm::findSeriesData(const Instrument& instrument, const TimeRange& range, const ParameterInfo& pi)
{
    DBG(DBG1(instrument.stationid) << DBG1(instrument.paramid) << DBG1(range));
    std::vector<int> pids(1, pi.parameter);
    if( pi.minParameter > 0 )
        pids.push_back(pi.minParameter);
    if( pi.maxParameter > 0 )
        pids.push_back(pi.maxParameter);
    if( pi.parameter == KVALOBS_PARAMID_UU )
        pids.push_back(KVALOBS_PARAMID_TA);

    GapDataPtr data;
    GapDataUUPtr dataUU;
    if( pi.parameter == KVALOBS_PARAMID_UU ) {
        data = dataUU = boost::make_shared<GapDataUU>(instrument, range, pi, this);
    } else {
        data = boost::make_shared<GapData>(instrument, range, pi, this);
    }

    const FlagSetCU all; // FIXME this sets too few constraints (none); must in any case be weaker than mDataFlagsUUTA

    const DataList series = database()->findDataOrderObstime(instrument.stationid, pids, std::vector<int>(1, instrument.type),
                                                             instrument.sensor, instrument.level, range, all);

    foreach(const kvalobs::kvData& d, series) {
        const int time = miutil::miTime::hourDiff(d.obstime(), range.t0), paramid = d.paramID();
        DBGV(d);
        Interpolation::Quality q = Interpolation::UNUSABLE;

        bool usable = false;
        float value = Interpolation::MISSING_VALUE;
        const kvalobs::kvUseInfo& useinfo = d.useinfo();
        const kvalobs::kvControlInfo& controlinfo = d.controlinfo();
        const int ftime = controlinfo.flag(kvalobs::flag::ftime), fmis = controlinfo.flag(kvalobs::flag::fmis);
        if( useinfo.flag(2) == 0 ) {
            q = Interpolation::OBSERVATION;
            value = d.original();
            usable = true;
        } else if( ftime == 1 ) {
            q = Interpolation::GOOD;
            value = d.corrected();
        } else if( ftime == 2 ) {
            q = Interpolation::BAD;
            value = d.corrected();
        } else if( ftime == 3 ) {
            q = Interpolation::FAILED;
            value = d.corrected();
        } else if( fmis == 1 or fmis == 3 ) {
            q = Interpolation::MISSING;
            value = d.original();
        } else if( useinfo.flag(2) == 9 ) { // not checked
            // FIXME this needs to be clarified
            q = Interpolation::UNUSABLE;
            value = d.original();
        } else {
            continue;
        }
        if( q <= Interpolation::FAILED and not pi.hasNumerical(value) ) {
            DBGL;
            value = Interpolation::MISSING_VALUE;
            q = Interpolation::UNUSABLE;
            usable = false;
        } else {
            value = pi.toNumerical(value);
        }

        GapUpdate gu(d, q, value, usable);
        DBG(DBG1(q) << DBG1(value) << DBG1(usable));
        if( paramid == pi.parameter )
            data->mDataPar.at(time) = gu;
        else if( paramid == pi.minParameter )
            data->mDataMin.at(time) = gu;
        else if( paramid == pi.maxParameter )
            data->mDataMax.at(time) = gu;
        else if( paramid == KVALOBS_PARAMID_TA and pi.parameter == KVALOBS_PARAMID_UU and q < Interpolation::FAILED )
            dataUU->mDataTA.at(time) = value;
    }
    return data;
}

// ------------------------------------------------------------------------

bool GapInterpolationAlgorithm::hasRADownStep(GapData& data)
{
    DBGL;
    if( data.duration() <= 1 )
        return false;

    float lastValue = data.parameter(0).value();
    bool lastUsable = data.parameter(0).usable();

    for(int t=1; t<data.duration(); ++t) {
        const float thisValue = data.parameter(t).value();
        const bool thisUsable = data.parameter(t).usable();
        if( thisUsable && lastUsable && (thisValue < lastValue - mRAThreshold) ) {
            info() << "RA decreasing around " //<< pgmr.range.t0 << " and " << pgmr.range.t1
                   << ", no interpolation attempted";
            return true;
        }
        if( thisUsable ) {
            lastValue = thisValue;
            lastUsable = thisUsable;
        }
    }
    return false;
}

// ------------------------------------------------------------------------

bool GapInterpolationAlgorithm::reconstructMinMax(GapData& data)
{
    DBGL;
    KvalobsMinMaxReconstructionData mmrData(data);
    Interpolation::MinMaxReconstruction mmr;
    mmr.run(mmrData);
    return false;
}

// ------------------------------------------------------------------------

bool GapInterpolationAlgorithm::seriesHasMissingRows(GapData& data)
{
    DBGL;
    for(int t=0; t<data.duration(); ++t) {
        if( data.parameter(t).quality() == Interpolation::NO_ROW ) {
            DBG(DBG1(t) << DBG1(data.mDataPar.at(t)));
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------------------

void GapInterpolationAlgorithm::discardUnreliableMinMax(GapData& data)
{
    DBGL;
    const int duration = data.duration();
    if( duration == 0 )
        return;
    bool last_t_ok = isReliableMinMax(data.minimum(0)) and isReliableMinMax(data.maximum(0));
    for(int t=1; t<duration; ++t) {
        const bool this_ok = isReliableMinMax(data.minimum(t)) and isReliableMinMax(data.maximum(t));
        DBG(DBG1(t) << DBG1(this_ok) << DBG1(last_t_ok));
        if( not last_t_ok ) {
            discardUpdate(data.mDataMin.at(t));
            discardUpdate(data.mDataMax.at(t));
        }
        last_t_ok = this_ok;
    }
}

// ------------------------------------------------------------------------

bool GapInterpolationAlgorithm::replaceFromOtherTypeid(GapData& data)
{
    DBGL;
    bool allok = true;
    const int duration = data.duration();
    for(int t=0; t<duration; ++t) {
        if( data.parameter(t).needsInterpolation() ) {
            DBGV(t);
            allok &= replaceFromOtherTypeid(data.mDataPar.at(t)/*, data.mParameterInfos.parameter*/);
        }
        if( data.hasMinMax() ) {
            if( data.minimum(t).needsInterpolation() ) {
                DBGV(t);
                allok &= replaceFromOtherTypeid(data.mDataMin.at(t)/*, data.mParameterInfos.minParameter*/);
            }
            if( data.maximum(t).needsInterpolation() ) {
                DBGV(t);
                allok &= replaceFromOtherTypeid(data.mDataMax.at(t)/*, data.mParameterInfos.maxParameter*/);
            }
        }
    }
    DBGV(allok);
    return allok;
}

// ------------------------------------------------------------------------

bool GapInterpolationAlgorithm::replaceFromOtherTypeid(GapUpdate& data/*, int paramID*/)
{
    const TimeRange obstime(data.obstime(), data.obstime());
    const FlagSetCU good_flags("", "U2=0");
    const kvalobs::kvData& d = data.data();
    DBGV(d.paramID());
    const DataList other = database()->findDataMaybeTSLOrderObstime(d.stationID(), d.paramID(), DBInterface::INVALID_ID,
                                                                    d.sensor(), d.level(), obstime, good_flags);
    if( other.empty() )
        return false;
    DBG(DBG1(data) << DBG1(other.size()));
#ifndef NDEBUG
    foreach(const kvalobs::kvData& od, other)
        DBGV(od);
#endif

    int max_typeid = 0; // initial value 0 to avoid aggregated values which have typeid < 0
    float best_value = Interpolation::MISSING_VALUE;
    foreach(const kvalobs::kvData& d, other) {
        const int type = d.typeID();
        if( type != data.data().typeID() and type > max_typeid ) {
            best_value = d.original();
            max_typeid = type;
        }
    }
    if( max_typeid > 0 ) {
        data.update(Interpolation::GOOD, best_value);
        return true;
    } else {
        return false;
    }
}
