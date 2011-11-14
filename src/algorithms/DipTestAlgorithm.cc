/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  $Id$

  Copyright (C) 2007 met.no

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

#include "AkimaSpline.h"
#include "AlgorithmHelpers.h"
#include "DBConstraints.h"
#include "GetStationParam.h"
#include "Helpers.h"
#include "ParseParValFile.h"
#include "ProcessControl.h"
#include "ReadProgramOptions.h"
#include "scone.h"
#include "tround.h"

#include <kvalobs/kvDataFlag.h>
#include <milog/milog.h>
#include <puTools/miTime.h>
#include "foreach.h"

namespace C = Constraint;
namespace O = Ordering;
using namespace kvQCFlagTypes;

bool DipTestAlgorithm::fillParameterDeltaMap(const ReadProgramOptions& params, std::map<int, float>& map)
{
    const std::string pvf = params.getParameter<std::string>("ParValFilename");

    if( Helpers::startsWith(pvf, "list:") )
        return Helpers::fillMapFromList(pvf.substr(5), map, ',');

    if( pvf == "NotSet")
        return false;

    map = ParseParValFile(pvf).ReturnMap();
    return true;
}

float DipTestAlgorithm::fetchDelta(const miutil::miTime& time, int pid)
{
    DBInterface::kvStationParamList_t splist;
    database()->selectStationparams(splist, 0, time, "QC1-3a-"+StrmConvert(pid) ); /// FIXME what time to use here?
    if( splist.empty() ) {
        LOGERROR("Empty station_param for stationid=0 list.");
        return -1e8; // FIXME throw an exception or so
    }
    const float DeltaCheck = GetStationParam(splist).ValueOf("max").toFloat();
    LOGINFO("Delta from station_params (valid for t=" << time << ") = " << DeltaCheck);
    return DeltaCheck;
}

void DipTestAlgorithm::configure(const ReadProgramOptions& params)
{
    fillParameterDeltaMap(params, PidValMap);
    fillStationIDList(StationIds);

    params.getFlagSetCU(akima_flags, "akima");
    params.getFlagSetCU(candidate_flags, "candidate");
    params.getFlagSetCU(linear_before_flags, "linear_before");
    params.getFlagSetCU(linear_after_flags, "linear_after");

    params.getFlagChange(dip_flagchange, "dip_flagchange");
    params.getFlagChange(afterdip_flagchange, "afterdip_flagchange");

    CFAILED_STRING = params.CFAILED_STRING;
    missing = params.missing;
}

void DipTestAlgorithm::run(const ReadProgramOptions& params)
{
    configure(params);

    for (std::map<int, float>::const_iterator it=PidValMap.begin(); it!=PidValMap.end(); ++it) {
        const int pid = it->first, delta = it->second;

        const C::DBConstraint cDipCandidate = C::ControlUseinfo(candidate_flags)
            && C::Station(StationIds) && C::Paramid(pid) && C::Obstime(params.UT0, params.UT1);

        std::list<kvalobs::kvData> candidates;
        database()->selectData(candidates, cDipCandidate);

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

    const C::DBConstraint cDipAroundL = C::Station(candidate.stationID()) && C::Paramid(candidate.paramID()) && C::Typeid(candidate.typeID())
        && ((   C::ControlUseinfo(linear_before_flags) && C::Obstime(linearStart))
            || (C::ControlUseinfo(linear_after_flags ) && C::Obstime(linearStop)));
    
    std::list<kvalobs::kvData> seriesLinear;
    database()->selectData(seriesLinear, cDipAroundL, O::Obstime());
    if( seriesLinear.size() != 2 ) {
        LOGINFO("no neighbors (may be bad flags) around candidate=" << candidate);
        return;
    }

    const kvalobs::kvData& before = seriesLinear.front(), after = seriesLinear.back();
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
        LOGINFO("not a dip/spike around candidate=" << candidate);
        return;
    }

    float interpolated = round<float,1>( 0.5*(before.original() + after.original()) );

    const bool AkimaPresent = tryAkima(candidate, interpolated);

    writeChanges(candidate, after, interpolated, AkimaPresent);
}

bool DipTestAlgorithm::tryAkima(const kvalobs::kvData& candidate, float& interpolated)
{
    const int N_BEFORE = 3, N_AFTER = 2, N_AKIMA = N_BEFORE + N_AFTER;
    miutil::miTime akimaStart = candidate.obstime(), akimaStop = candidate.obstime();
    akimaStart.addHour(-N_BEFORE);
    akimaStop .addHour( N_AFTER );
    miutil::miTime linearStart = candidate.obstime(), linearStop = candidate.obstime();
    linearStart.addHour(-1);
    linearStop.addHour(1);

    const C::DBConstraint cDipAroundA = C::Station(candidate.stationID()) && C::Paramid(candidate.paramID()) && C::Typeid(candidate.typeID())
        && C::Obstime(akimaStart, akimaStop) && (!C::Obstime(candidate.obstime()))
        && (C::ControlUseinfo(akima_flags) || C::Obstime(linearStart) || C::Obstime(linearStop));
    
    std::list<kvalobs::kvData> seriesAkima;
    database()->selectData(seriesAkima, cDipAroundA, O::Obstime());

    if( (int)seriesAkima.size() != N_AKIMA )        
        return false;

    // check that the akima support points are in the right order and with the right time
    miutil::miTime akimaTime = akimaStart;
    std::vector<double> xt,yt;
    int i = 0;
    foreach(const kvalobs::kvData& a, seriesAkima) {
        if( a.original() <= missing || a.obstime() != akimaTime )
            return false;
        xt.push_back(i);
        yt.push_back(a.original());
        i += 1;
        akimaTime.addHour(1);
        if( i == N_BEFORE ) {
            i += 1;
            akimaTime.addHour(1);
        }
    }

    const AkimaSpline AkimaX(xt,yt);
    const float AkimaInterpolated = round<float,1>( AkimaX.AkimaPoint(N_BEFORE) );

    DBInterface::kvStationParamList_t splist;
    database()->selectStationparams(splist, candidate.stationID(), candidate.obstime(), "QC1-1-"+StrmConvert(candidate.paramID()));
    if( splist.empty() ) {
        LOGERROR("No station params for akima MinimumCheck for candidate=" << candidate << ". Assuming no akima interpolation.");
        return false;
    }
    const float MinimumCheck = GetStationParam(splist).ValueOf("min").toFloat();
    if( AkimaInterpolated < MinimumCheck ) {
        LOGDEBUG("akima < mini for candidate=" << candidate);
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
