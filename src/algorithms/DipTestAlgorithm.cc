/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2007-2012 met.no

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

#include "DipTestAlgorithm.h"

#include "helpers/AkimaSpline.h"
#include "helpers/AlgorithmHelpers.h"
#include "helpers/mathutil.h"
#include "DBInterface.h"
#include "foreach.h"
#include "GetStationParam.h"
#include "ParseParValFile.h"

#include <kvalobs/kvDataFlag.h>
#include <milog/milog.h>
#include <puTools/miTime.h>

#include <boost/algorithm/string/predicate.hpp>

#define NDEBUG
#include "debug.h"

using namespace kvQCFlagTypes;

bool DipTestAlgorithm::fillParameterDeltaMap(const AlgorithmConfig& params, std::map<int, float>& map)
{
    const std::string pvf = params.getParameter<std::string>("ParValFilename");

    if( boost::algorithm::starts_with(pvf, "list:") )
        return Helpers::fillMapFromList(pvf.substr(5), map, ',');

    if( pvf == "NotSet")
        return false;

    map = ParseParValFile(pvf).ReturnMap();
    return true;
}

float DipTestAlgorithm::fetchDelta(const miutil::miTime& time, int pid)
{
    std::ostringstream qcx;
    qcx << "QC1-3a-" << pid;
    DBInterface::StationParamList splist = database()->findStationParams(0, time, qcx.str()); /// FIXME what time to use here?
    if( splist.empty() ) {
        error() << "empty station_param for stationid=0 list";
        return -1e8; // FIXME throw an exception or so
    }
    const float DeltaCheck = std::atof(GetStationParam(splist).ValueOf("max").c_str());
    info() << "delta from station_params (valid for t=" << time << ") = " << DeltaCheck;
    return DeltaCheck;
}

void DipTestAlgorithm::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    fillParameterDeltaMap(params, PidValMap);

    params.getFlagSetCU(akima_flags,          "akima",          "", "U2=0");
    params.getFlagSetCU(candidate_flags,      "candidate",      "fs=2&fhqc=0", "");
    params.getFlagSetCU(dip_before_flags,     "dip_before",     "fs=1", "");
    params.getFlagSetCU(dip_after_flags,      "dip_after"   ,   "fs=2&fhqc=0", "");
    params.getFlagSetCU(message_before_flags, "message_before", "fs=)0129(", "");
    params.getFlagSetCU(message_after_flags,  "message_after",  "fs=)01(|fhqc=)0(", "");

    params.getFlagChange(dip_flagchange,      "dip_flagchange",      "fs=9");
    params.getFlagChange(afterdip_flagchange, "afterdip_flagchange", "fs=4");
}

void DipTestAlgorithm::run()
{
    const DBInterface::StationIDList stationIDs(1, DBInterface::ALL_STATIONS);

    for (std::map<int, float>::const_iterator it=PidValMap.begin(); it!=PidValMap.end(); ++it) {
        const int pid = it->first, delta = it->second;

        const DBInterface::DataList candidates = database()->findDataOrderObstime(stationIDs, pid, TimeRange(UT0, UT1), candidate_flags);
        DBGV(candidates.size());
        foreach(const kvalobs::kvData& c, candidates) {
            if( c.original() > missing )
                checkDipAndInterpolate(c, delta);
        }
    }
}

void DipTestAlgorithm::checkDipAndInterpolate(const kvalobs::kvData& candidate, float delta)
{
    miutil::miTime linearStart = candidate.obstime(), linearStop = candidate.obstime();
    linearStart.addHour(-1);
    linearStop.addHour(1);
    if( linearStart < UT0 || linearStop > UT1 )
        return;

    const DBInterface::DataList seriesLinear
        = database()->findDataOrderObstime(candidate.stationID(), candidate.paramID(), candidate.typeID(), TimeRange(linearStart, linearStop));
    if( seriesLinear.size() != 3 ) {
        warning() << "did not find rows before or after (latter needs fhqc=0) potential dip"
                  << Helpers::datatext(candidate, 1);
        return;
    }

    const kvalobs::kvData& before = seriesLinear.front(), after = seriesLinear.back();
    const bool dip_before = dip_before_flags.matches(before), dip_after = dip_after_flags.matches(after);
    if( !(dip_before && dip_after) ) {
        if( message_before_flags.matches(before) || message_after_flags.matches(after) ) {
            info() << "flag pattern mismatch for rows before/after potential dip "
                   << Helpers::datatext(candidate);
        }
        return;
    }
    if( before.original() <= missing || after.original() <= missing )
        return;

#if 0
    // FIXME which delta -- DeltaCheck or delta?
    const float deltaCheck = fetchDelta(miutil::miTime::nowTime(), candidate.paramID());
#endif

    //            x
    //      x           x     -> time
    //     A(0)  A(1)  A(2)
    const float ABS20 = fabs( after    .original() - before.original() );
    const float ABS10 = fabs( candidate.original() - before.original() );
    if( !(ABS20 < ABS10 && ABS20 < delta) ) {
        info() << "not a dip/spike around candidate " << Helpers::datatext(candidate);
        return;
    }

    float interpolated = Helpers::round1( 0.5*(before.original() + after.original()) );

    const bool AkimaPresent = tryAkima(candidate, interpolated);

    writeChanges(candidate, after, interpolated, AkimaPresent);
}

bool DipTestAlgorithm::tryAkima(const kvalobs::kvData& candidate, float& interpolated)
{
    const int N_BEFORE = 3, N_AFTER = 2, N_AKIMA = N_BEFORE + N_AFTER;
    miutil::miTime akimaStart = candidate.obstime(), akimaStop = candidate.obstime();
    akimaStart.addHour(-N_BEFORE);
    akimaStop .addHour( N_AFTER );

    const DBInterface::DataList seriesAkima
        = database()->findDataOrderObstime(candidate.stationID(), candidate.paramID(), candidate.typeID(), TimeRange(akimaStart, akimaStop));
    DBGV(seriesAkima.size());

    if( (int)seriesAkima.size() != N_AKIMA+1 )
        return false;

    // check that the akima support points are in the right order and with the right time
    miutil::miTime akimaTime = akimaStart;
    std::vector<double> xt,yt;
    int i = 0;
    foreach(const kvalobs::kvData& a, seriesAkima) {
        if( i != N_BEFORE ) {
            if( a.original() <= missing || a.obstime() != akimaTime || !(akima_flags.matches(a) || i == N_BEFORE-1 || i == N_BEFORE+1) )
                return false;
            xt.push_back(i);
            yt.push_back(a.original());
        }
        i += 1;
        akimaTime.addHour(1);
    }
    DBGV(i);
    if( i < N_AKIMA )
        return false;

    const AkimaSpline AkimaX(xt,yt);
    const float AkimaInterpolated = Helpers::round1( AkimaX.AkimaPoint(N_BEFORE) );

    std::ostringstream qcx;
    qcx << "QC1-1-" << candidate.paramID();
    const DBInterface::StationParamList splist
        = database()->findStationParams(candidate.stationID(), candidate.obstime(), qcx.str());
    if( splist.empty() ) {
        error() << "no station params for akima MinimumCheck for candidate " << Helpers::datatext(candidate) << ". Assuming no akima interpolation.";
        return false;
    }
    const float MinimumCheck = std::atof(GetStationParam(splist).ValueOf("min").c_str());
    if( AkimaInterpolated < MinimumCheck ) {
        LOGDEBUG("akima < mini for candidate " << Helpers::datatext(candidate));
        return false;
    }

    interpolated = AkimaInterpolated;
    return true;
}

void DipTestAlgorithm::writeChanges(const kvalobs::kvData& dip, const kvalobs::kvData& after, const float interpolated, bool haveAkima)
{
    kvalobs::kvData wdip(dip);
    wdip.corrected(interpolated);
    wdip.controlinfo(dip_flagchange.apply(wdip.controlinfo()));
    Helpers::updateCfailed(wdip, haveAkima ? "QC2d-1-A" : "QC2d-1-L", CFAILED_STRING);
    Helpers::updateUseInfo(wdip);

    kvalobs::kvData wafter(after);
    wafter.controlinfo(afterdip_flagchange.apply(wafter.controlinfo()));
    Helpers::updateCfailed(wafter, "QC2d-1", CFAILED_STRING);
    Helpers::updateUseInfo(wafter);

    std::list<kvalobs::kvData> write;
    write.push_back(wdip);
    write.push_back(wafter);
    storeData(write);
}
