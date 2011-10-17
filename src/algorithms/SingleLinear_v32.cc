/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  Copyright (C) 2011 met.no

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

#include "SingleLinear_v32.h"

#include "AlgorithmHelpers.h"
#include "Helpers.h"
#include "tround.h"
#include "scone.h"

#include <milog/milog.h>
#include <puTools/miTime.h>

#include "foreach.h"

namespace {

const float NO_UPDATE = -99999.0;

} // anonymous namespace

// ########################################################################

SingleLinearV32Algorithm::SingleLinearV32Algorithm()
    : Qc2Algorithm()
{
    std::vector<std::string> setmissing_fmis;
    setmissing_fmis.push_back("1->3");
    setmissing_fmis.push_back("4->2");
    setmissing_chflag[6]=setmissing_fmis;
}

// ------------------------------------------------------------------------

bool SingleLinearV32Algorithm::isNeighborOk(const ReadProgramOptions& params, const kvalobs::kvData& n)
{
    return !checkFlags().condition(n.controlinfo(), params.Notflag)
        &&  checkFlags().condition(n.controlinfo(), params.Aflag)
        && !checkFlags().condition(n.useinfo(),     params.NotUflag)
        &&  checkFlags().condition(n.useinfo(),     params.Uflag)
        && n.original() != params.missing
        && n.original() != params.rejected;
}

// ------------------------------------------------------------------------

void SingleLinearV32Algorithm::run(const ReadProgramOptions& params)
{
    LOGINFO("Single Linear");
    std::list<kvalobs::kvData> Qc2Data;
    std::list<kvalobs::kvData> Qc2SeriesData;
 
    // TODO station ids do not change very often, need not update list for each test and every minute
    std::list<int> StationIds;
    fillStationIDList(StationIds);

    for(miutil::miTime ProcessTime = params.UT1; ProcessTime >= params.UT0; ProcessTime.addHour(-1)) {

        // TODO maybe just run one query and update ProcessTime according to the results (obstime>=a and obstime<=b)?
        // substr counts from 1
        const miutil::miString filter = "WHERE (substr(controlinfo,7,1) IN ('1', '2', '3', '4')) "
            "AND paramid="+StrmConvert(params.pid)+" AND obstime=\'"+ProcessTime.isoTime()+"\'";
        if( !database()->selectData(Qc2Data, filter) ) {
            LOGERROR( "Database error finding middle points for linear interpolation");
            continue;
        }
        if( Qc2Data.empty() )
            continue;

        miutil::miTime timeBefore = ProcessTime, timeAfter = ProcessTime;
        timeBefore.addHour(-1);
        timeAfter.addHour(1);

        foreach(const kvalobs::kvData& d, Qc2Data) {
            if( !database()->dataForStationParamTimerange(Qc2SeriesData, d.stationID(), params.pid, timeBefore, timeAfter) ) {
                LOGERROR( "Database error finding neighbours for linear interpolation");
                continue;
            }
            if( Qc2SeriesData.size() != 3 )
                continue;

            std::vector<kvalobs::kvData> Tseries(Qc2SeriesData.begin(), Qc2SeriesData.end());
            if( !Helpers::checkContinuousHourAndSameTypeID(Tseries) )
                continue;

            const float NewCorrected = calculateCorrected(params, Tseries, d.stationID(), timeAfter);
            if( NewCorrected == NO_UPDATE )
                continue;

            if( checkFlags().true_nibble(d.controlinfo(), params.Wflag, 15, params.Wbool) )
                storeUpdate(params, Tseries[1], NewCorrected);
        }
    }
}

// ------------------------------------------------------------------------
            
float SingleLinearV32Algorithm::calculateCorrected(const ReadProgramOptions& params, const std::vector<kvalobs::kvData>& Tseries,
                                                   const int /*stationID*/, const miutil::miTime& /*timeAfter*/)
{
    float NewCorrected = NO_UPDATE;
    const kvalobs::kvData &before = Tseries[0], &middle = Tseries[1], &after = Tseries[2];

    // check that the neighbours are good
    const int flag7 = middle.controlinfo().flag(7);
    if( isNeighborOk(params, before) && isNeighborOk(params, after) ) {
        if( flag7 == 0 || flag7 == 1 ) {
            NewCorrected = round<float,1>( 0.5*(before.original()+after.original()) );
#if 0 // not used according to paule@met.no (20111009-0230) (also removed in v33)
            if( params.maxpid>0 && params.minpid>0 ) {
                std::list<kvalobs::kvData> MaxValue, MinValue;
                // FIXME why twice timeAfter ? how does this find the max/min ???
                if( database()->dataForStationParamTimerange(MaxValue, stationID, params.maxpid, timeAfter, timeAfter)
                    && database()->dataForStationParamTimerange(MinValue, stationID, params.minpid, timeAfter, timeAfter)
                    && MaxValue.size()==1 && MinValue.size()==1 )
                {
                    const float maxi = MaxValue.begin()->original(), mini = MinValue.begin()->original();
                    if( maxi > -99.9 && mini > -99.9 ) {
                        if( interpolated > maxi )
                            NewCorrected = maxi;
                        if( interpolated < mini )
                            NewCorrected = mini;
                    }
                    //NB if a corrected value exists and it is already between the min and max then do not overwrite
                    if( middle.corrected() >= mini && middle.corrected() <= maxi )
                        NewCorrected = NO_UPDATE; 
                }
            }
#endif
            if( flag7 == 1 && NewCorrected == middle.corrected() )
                NewCorrected = NO_UPDATE;
        }
    } else {
        //NB for ftime=0 ... do nothing
        
        //For ftime=1, reset to missing value ...
        if( flag7 == 1 ) {
            const int flag6 = middle.controlinfo().flag(6);
            if( flag6 == 1 || flag6 == 3 )
                NewCorrected = params.missing;
            else if( flag6 == 2 || flag6 == 4 )
                NewCorrected = params.rejected;
            else {
                NewCorrected = round<float,1>( (before.original()+after.original())/2 );
            }
        }
    }
    return NewCorrected;
}

// ------------------------------------------------------------------------
            
void SingleLinearV32Algorithm::storeUpdate(const ReadProgramOptions& params, const kvalobs::kvData& middle, const float NewCorrected)
{
    kvalobs::kvControlInfo fixflags = middle.controlinfo();
    checkFlags().setter(fixflags, params.Sflag);
    checkFlags().conditional_setter(fixflags, params.chflag);
    if( NewCorrected == params.missing || NewCorrected == params.rejected )
        checkFlags().conditional_setter(fixflags, setmissing_chflag);
        
    kvalobs::kvData dwrite(middle);
    dwrite.corrected(NewCorrected);
    dwrite.controlinfo(fixflags);
    Helpers::updateCfailed(dwrite, "QC2d-2", params.CFAILED_STRING);
    Helpers::updateUseInfo(dwrite);

    LOGINFO( "SingleLinear_v32: " + Helpers::kvqc2logstring(dwrite) );
    if( !database()->insertData(dwrite, true) ) {
        LOGERROR( "Error updating database with interpolated value");
        return; // FIXME what should be done here, actually?
    }

    // TODO why not accumulate a long list and send several updates at once?
    broadcaster()->queueChanged(middle);
    broadcaster()->sendChanges();
}
