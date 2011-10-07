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

#include "SingleLinear_v32.h"

#include "Helpers.h"
#include "ProcessImpl.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "tround.h"
#include "scone.h"

#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>
#include <puTools/miTime.h>

#include <boost/foreach.hpp>
#include <memory>
#include <stdexcept>
#include <sstream>

using namespace kvalobs;
using namespace std;
using namespace miutil;

namespace {

const float NO_UPDATE = -99999.0;

// struct Helper {
//     
//     kvalobs::kvDbGate dbGate;
//     const ReadProgramOptions& params;
// 
//     Helper(const ReadProgramOptions& p, ProcessImpl* d)
//         : params(p) , dbGate(&d->getConnection())
//         { }
// 
//     miutil::miTime timeBefore, timeAfter;
// };

} // anonymous namespace

// ########################################################################

SingleLinearV32Algorithm::SingleLinearV32Algorithm(ProcessImpl* p)
    : Qc2Algorithm(p)
    , checkedDataHelper(dispatcher()->getApp())
{
    std::vector<std::string> setmissing_fmis;
    setmissing_fmis.push_back("1->3");
    setmissing_fmis.push_back("4->2");
    setmissing_chflag[6]=setmissing_fmis;
}

// ------------------------------------------------------------------------

bool SingleLinearV32Algorithm::isNeighborOk(const ReadProgramOptions& params, const kvalobs::kvData& n)
{
    return !CheckFlags.condition(n.controlinfo(), params.Notflag)
        &&  CheckFlags.condition(n.controlinfo(), params.Aflag)
        && !CheckFlags.condition(n.useinfo(),     params.NotUflag)
        &&  CheckFlags.condition(n.useinfo(),     params.Uflag)
        && n.original() != params.missing
        && n.original() != params.rejected;
}

// ------------------------------------------------------------------------

void SingleLinearV32Algorithm::run(const ReadProgramOptions& params)
{
    LOGINFO("Single Linear");
    std::list<kvalobs::kvData> Qc2Data;
    std::list<kvalobs::kvData> Qc2SeriesData;
 
    kvalobs::kvDbGate dbGate( &dispatcher()->getConnection() );

    // TODO station ids do not change very often, need not update list for each test and every minute
    std::list<int> StationIds;
    fillStationIDList(StationIds);

    const miutil::miTime earlyTime = params.UT0, lateTime = params.UT1;
    for(miutil::miTime ProcessTime = lateTime; ProcessTime >= earlyTime; ProcessTime.addHour(-1)) {

        // TODO maybe just run one query and update ProcessTime according to the results (obstime>=a and obstime<=b)?
        // TODO maybe use substr(controlinfo,7,1) IN ('1','2','3','4')?
        // substr counts from 1
        const miutil::miString filter = "WHERE (substr(controlinfo,7,1)='1' OR substr(controlinfo,7,1)='2' "
            "OR substr(controlinfo,7,1)='3' OR substr(controlinfo,7,1)='4') "
            "AND PARAMID="+StrmConvert(params.pid)+" AND obstime=\'"+ProcessTime.isoTime()+"\'";
        if( !dbGate.select(Qc2Data, filter) ) {
            IDLOGERROR( "html", "Database error finding middle points for linear interpolation: " << dbGate.getErrorStr() );
            continue;
        }
        if( Qc2Data.empty() )
            continue;

        miutil::miTime timeBefore = ProcessTime, timeAfter = ProcessTime;
        timeBefore.addHour(-1);
        timeAfter.addHour(1);

        BOOST_FOREACH(const kvalobs::kvData& d, Qc2Data) {
            if( !dbGate.select(Qc2SeriesData, kvQueries::selectData(d.stationID(), params.pid, timeBefore, timeAfter)) ) {
                IDLOGERROR( "html", "Database error finding neighbours for linear interpolation: " << dbGate.getErrorStr() );
                continue;
            }
            if( Qc2SeriesData.size() != 3 )
                continue;

            std::list<kvalobs::kvData>::const_iterator iter = Qc2SeriesData.begin();
            const kvalobs::kvData& before = *iter++, middle = *iter++, after = *iter;
            
            // Check that the 3 data points are valid
            if( middle.obstime().hour() != (before.obstime().hour() + 1) % 24
                || middle.obstime().hour() != (24 + (after.obstime().hour() - 1)) % 24
                || middle.typeID() != before.typeID()
                || middle.typeID() != after.typeID() )
            {
                continue;
            }

            const float NewCorrected = calculateCorrected(params, before, middle, after, d.stationID(), timeAfter);
            if( NewCorrected == middle.corrected() || NewCorrected == NO_UPDATE )
                continue;

            if( CheckFlags.true_nibble(d.controlinfo(), params.Wflag, 15, params.Wbool) )
                storeUpdate(params, middle, NewCorrected);
        }
    }
}

// ------------------------------------------------------------------------
            
float SingleLinearV32Algorithm::calculateCorrected(const ReadProgramOptions& params, const kvalobs::kvData& before,
                                                   const kvalobs::kvData& middle, const kvalobs::kvData& after,
                                                   const int stationID, const miutil::miTime& timeAfter)
{
    float NewCorrected = NO_UPDATE;
    kvalobs::kvDbGate dbGate;

    // check that the neighbours are good
    const int flag7 = middle.controlinfo().flag(7);
    if( isNeighborOk(params, before) && isNeighborOk(params, after) ) {
        if( flag7 == 0 || flag7 == 1 ) {
            const float interpolated = (before.original()+after.original())/2;
            NewCorrected = round<float,1>( interpolated );
            if( params.maxpid>0 && params.minpid>0 ) {
                std::list<kvalobs::kvData> MaxValue, MinValue;
                // FIXME why twice timeAfter ? how does this find the max/min ???
                if( dbGate   .select(MaxValue, kvQueries::selectData(stationID, params.maxpid, timeAfter, timeAfter))
                    && dbGate.select(MinValue, kvQueries::selectData(stationID, params.minpid, timeAfter, timeAfter))
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
        }
    } else {
        //NB for ftime=0 ... do nothing
        
        //For ftime=1, reset to missing value ...
        if( flag7 == 1 ) {
            const int flag6 = middle.controlinfo().flag(6);
            if( flag6 == 1 || flag6 == 3 )
                NewCorrected = params.missing;
            if( flag6 == 2 || flag6 == 4 )
                NewCorrected = params.rejected;
        }
    }
    return NewCorrected;
}

// ------------------------------------------------------------------------
            
void SingleLinearV32Algorithm::storeUpdate(const ReadProgramOptions& params, const kvalobs::kvData& middle, const float NewCorrected)
{
    kvalobs::kvControlInfo fixflags = middle.controlinfo();
    CheckFlags.setter(fixflags,params.Sflag);
    CheckFlags.conditional_setter(fixflags,params.chflag);
    if( NewCorrected == params.missing || NewCorrected == params.rejected )
        CheckFlags.conditional_setter(fixflags, setmissing_chflag);
    miutil::miString new_cfailed = middle.cfailed();
    if( new_cfailed.length() > 0 )
        new_cfailed += ",";
    new_cfailed += "QC2d-2";
    if( params.CFAILED_STRING.length() > 0 )
        new_cfailed += ","+params.CFAILED_STRING;
        
    kvalobs::kvData dwrite;
    dwrite.set(middle.stationID(), middle.obstime(), middle.original(), middle.paramID(),
               middle.tbtime(), middle.typeID(), middle.sensor(), middle.level(),
               NewCorrected, fixflags, middle.useinfo(), new_cfailed );
    kvUseInfo ui = dwrite.useinfo();
    ui.setUseFlags( dwrite.controlinfo() );
    dwrite.useinfo( ui );   
    LOGINFO( "SingleLinear_v32: " + Helpers::kvqc2logstring(dwrite) );
    kvalobs::kvDbGate dbGate;
    if( !dbGate.insert( dwrite, "data", true) ) {
        IDLOGERROR( "html", "Error updating database with interpolated value: " << dbGate.getErrorStr() );
        return; // FIXME what should be done here, actually?
    }
        
    // TODO why not accumlate a long list and send several updates at once?
    kvalobs::kvStationInfo::kvStationInfo DataToWrite(middle.stationID(), middle.obstime(), middle.typeID());
    kvalobs::kvStationInfoList stList;
    stList.push_back(DataToWrite);
    checkedDataHelper.sendDataToService(stList);
}
