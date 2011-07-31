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
#include "Plumatic.h"
#include "GetStationParam.h"

using namespace kvalobs;
using namespace std;
using namespace miutil;

int 
ProcessImpl::
ProcessPlumatic( ReadProgramOptions params )
{

  LOGINFO("Plumatic Control");

  miutil::miTime stime=params.UT0;
  miutil::miTime etime=params.UT1;
  miutil::miTime PreviousCheck;
  const int pid=params.pid;
  const int tid=params.tid;
  int ignore_station=0;
  const std::vector<int> tids=params.tids;

  ProcessControl CheckFlags;
  miutil::miString new_cfailed;
  kvalobs::kvControlInfo fixflags;

  std::list<kvalobs::kvStation> StationList;
  std::list<kvalobs::kvStation> ActualStationList;
  std::list<int> StationIds;
  std::list<int> TestStation;
  std::list<miutil::miTime> TimeList;
  std::list<miutil::miTime>::iterator iTime;
  std::list<kvalobs::kvData> PluviData;
  std::list<kvalobs::kvData> CheckData;
  std::list<kvalobs::kvData> ReturnData;
  bool result;

  kvalobs::kvStationInfoList  stList;
  CheckedDataHelper checkedDataHelper(app);

  kvalobs::kvDbGate dbGate( &con );

  miutil::miTime ProcessTime;
  miutil::miString ladle;

  kvalobs::kvData dwrite;                                                   

  GetStationList(StationList);  /// StationList is all the possible stations
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     StationIds.push_back( sit->stationID() );
  }

   std::list<kvalobs::kvStationParam> splist,resultlist;
   std::ostringstream query;

  /// LOOP THROUGH STATIONS
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++sit) {
     try {
			 ladle="WHERE STATIONID="+StrmConvert(sit->stationID())+" AND PARAMID="+StrmConvert(pid)+" AND obstime BETWEEN \'"+stime.isoTime()+"\' AND \'"+etime.isoTime()+"\'";
			 result = dbGate.select(PluviData, ladle);
         }
         catch ( dnmi::db::SQLException & ex ) {
           IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
         }
         catch ( ... ) {
           IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
         }

      if(!PluviData.empty()) {

		 Plumatic PL(PluviData,params);
		 std::cout << " --------------- " << std::endl;
		 PL.aggregate_window(params, TimeList);
		 for (std::list<kvalobs::kvData>::const_iterator idata=PluviData.begin(); idata!=PluviData.end(); ++idata)
		 {
            try{
		       iTime=std::find(TimeList.begin(), TimeList.end(), idata->obstime()); 
			   if (iTime != TimeList.end() && (std::find(iTime, TimeList.end(), idata->obstime()) != TimeList.end() ) ) {
                    std::cout << "Flag This: " << *idata << std::endl;
                    // UPDATE FLAG 
                    fixflags=idata->controlinfo();
                    CheckFlags.setter(fixflags,params.Sflag);
                    CheckFlags.conditional_setter(fixflags,params.chflag);
                    // SET CFAILED
                    new_cfailed=idata->cfailed();
                    if (new_cfailed.length() > 0) new_cfailed += ",";
                    new_cfailed += "PLX";
                    if (params.CFAILED_STRING.length() > 0) new_cfailed += ","+params.CFAILED_STRING;
                    // PREPARE KVALOBS DATA 
                    dwrite.clean();
                    dwrite.set(idata->stationID(),idata->obstime(),idata->original(),idata->paramID(),idata->tbtime(),
                      idata->typeID(),idata->sensor(), idata->level(),idata->corrected(),fixflags,idata->useinfo(),
                      new_cfailed );
                    // UPDATE USEINFO BASED ON NEW CONTROLINFO 
                    kvUseInfo ui = dwrite.useinfo();
                    ui.setUseFlags( dwrite.controlinfo() );
                    dwrite.useinfo( ui );   
					// WRITE TO LOG, DB  	
                    LOGINFO("Pluviometer Aggregation Check: "+kvqc2logstring(dwrite) );
                    dbGate.insert( dwrite, "data", true); 
					// PREPARE kvServiced SIGNAL
                    kvalobs::kvStationInfo::kvStationInfo DataToWrite(idata->stationID(),idata->obstime(),idata->typeID());
                    stList.push_back(DataToWrite);
			   }
		    }

            catch ( dnmi::db::SQLException & ex ) {
               IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
               std::cout<<"INSERTO> CATCH ex" << result <<std::endl;
            }
            catch ( ... ) {
               IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
               std::cout<<"INSERTO> CATCH ..." << result <<std::endl;
            }
			// SEND kvServiced SIGNAL
            if(!stList.empty()){
               checkedDataHelper.sendDataToService(stList);
               stList.clear();
		    }
       }
   }
}
return 0;
}

