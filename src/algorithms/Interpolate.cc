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
#include "ProcessImpl.h"
#include "BasicStatistics.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "Qc2D.h"
#include "ReadProgramOptions.h"
#include <sstream>
#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>
#include <puTools/miTime.h>
#include <memory>
#include <stdexcept>

#include "CheckedDataCommandBase.h"
#include "CheckedDataHelper.h"
#include "ProcessControl.h"


using namespace kvalobs;
using namespace std;
using namespace miutil;


int 
ProcessImpl::
Interpolate( ReadProgramOptions params )
{

  miutil::miTime stime=params.UT0;
  miutil::miTime etime=params.UT1;
  const int pid=params.pid;
  const int tid=params.tid;

  std::vector<float> XP;
  std::vector<float> YP;

  kvalobs::kvControlInfo fixflags;
  ProcessControl CheckFlags;

  std::list<kvalobs::kvStation> StationList;
  std::list<kvalobs::kvStation> ActualStationList;
  std::list<int> StationIds;
  std::list<int> StationIdsActual;
  std::list<kvalobs::kvData> Qc2Data;
  std::list<kvalobs::kvData> ReturnData;
  bool result;

  kvalobs::kvStationInfoList  stList;
  CheckedDataHelper checkedDataHelper(app);

  kvalobs::kvDbGate dbGate( &con );

  miutil::miTime ProcessTime;

  GetStationList(StationList);  // StationList is all the possible stations
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     StationIds.push_back( sit->stationID() );
  }
  ProcessTime = stime;
  while (ProcessTime <= etime) {

             try {
              result = dbGate.select(Qc2Data, kvQueries::selectData(StationIds,pid,ProcessTime,ProcessTime));
              //result = dbGate.select(Qc2Data, kvQueries::selectData(StationIds,pid,tid,ProcessTime,ProcessTime));
              }
              catch ( dnmi::db::SQLException & ex ) {
                IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
              }
              catch ( ... ) {
                IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
              }

      if(!Qc2Data.empty()) {

                Qc2D GSW(Qc2Data,StationList, params);
                GSW.Qc2_interp(); 
                //GSW.SampleSemiVariogram();
                GSW.write_cdf(StationList);
       }
       ProcessTime.addDay();
     }
return 0;
}

