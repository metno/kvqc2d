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

#include "ProcessControl.h"
#include "CheckedDataCommandBase.h"
#include "CheckedDataHelper.h"

#include "tround.h"

using namespace kvalobs;
using namespace std;
using namespace miutil;

int 
ProcessImpl::
DipTest( ReadProgramOptions params )
{
   LOGINFO("Dip Test");
   int pid=params.pid;
   float LinInterpolated;
   float AkimaInterpolated;
   float NewCorrected;
   miutil::miTime stime=params.UT0;
   miutil::miTime etime=params.UT1;
 
   std::list<kvalobs::kvStation> StationList;
   std::list<int> StationIds;
   std::list<kvalobs::kvData> Qc2Data;
   std::list<kvalobs::kvData> Qc2SeriesData;
   bool result;
 
   ProcessControl CheckFlags;
   kvalobs::kvControlInfo fixflags;

   kvalobs::kvStationInfoList  stList;
   CheckedDataHelper checkedDataHelper(app);

   kvalobs::kvDbGate dbGate( &con );

   miutil::miTime ProcessTime;
   miutil::miTime XTime;
   miutil::miTime YTime;
   ProcessTime = etime;

   std::vector<kvalobs::kvData> Tseries;
 
   GetStationList(StationList);  /// StationList is all the possible stations ... Check
   for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
      StationIds.push_back( sit->stationID() );
   } 

   while (ProcessTime >= stime) 
   {
      XTime=ProcessTime;
      XTime.addHour(-1);
      YTime=ProcessTime;
      YTime.addHour(1);
      Tseries.clear();
      try {
        /// Select all data for a given stationlist over three hours (T-1),(T),(T+1)
        result = dbGate.select(Qc2Data, kvQueries::selectData(StationIds,pid,XTime,YTime));
        //********* Change this to select the appropriate data
      }
      catch ( dnmi::db::SQLException & ex ) {
        IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
      }
      catch ( ... ) {
        IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
      }
      if(!Qc2Data.empty()) { 
         /// Now pack the data into a 3D object
         
         
         
      } // There is Qc2Data Loop
   ProcessTime.addHour(-1);
   } // TimeLoop
   return 0;
}

//2d interpolation code may be added ...
//std::list<kvalobs::kvData> Qc2InterpData;
//result = dbGate.select(Qc2InterpData, kvQueries::selectData(StationIds,pid,Tseries[1].obstime(),Tseries[1].obstime() ));
//Qc2D GSW(Qc2InterpData,StationList,params);
//GSW.Qc2_interp(); 
