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
//#include <maths.h>

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
   float ABS20, ABS10;
   kvalobs::kvData dwrite1;                                                   
   kvalobs::kvData dwrite2;                                                   
   miutil::miString new_cfailed1;
   miutil::miString new_cfailed2;
   miutil::miTime stime=params.UT0;
   miutil::miTime etime=params.UT1;
 
   std::list<kvalobs::kvStation> StationList;
   std::list<int> StationIds;
   std::list<kvalobs::kvData> Qc2Data;
   std::list<kvalobs::kvData> Qc2SeriesData;
   bool result;
   bool HOLDING=false;
 
   ProcessControl CheckFlags;
   kvalobs::kvControlInfo fixflags1;
   kvalobs::kvControlInfo fixflags2;

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
		result = dbGate.select(Qc2Data, kvQueries::selectData(StationIds,pid,ProcessTime,ProcessTime));
        //********* Change this to select the appropriate data
      }
      catch ( dnmi::db::SQLException & ex ) {
        IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
      }
      catch ( ... ) {
        IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
      }
      if(!Qc2Data.empty()) { 
         for (std::list<kvalobs::kvData>::const_iterator id = Qc2Data.begin(); id != Qc2Data.end(); ++id) {
            Tseries.clear();  
            result = dbGate.select(Qc2SeriesData, kvQueries::selectData(id->stationID(),pid,XTime,YTime));

            for (std::list<kvalobs::kvData>::const_iterator is = Qc2SeriesData.begin(); is != Qc2SeriesData.end(); ++is) {
                     Tseries.push_back(*is);
            }
            
            if  (Tseries.size()==3 &&  // Check that we have three valid data points and no mixing of typeids
                Tseries[1].obstime().hour() == (Tseries[0].obstime().hour() + 1) % 24        &&
                Tseries[1].obstime().hour() == (24 + (Tseries[2].obstime().hour() - 1)) % 24 &&      
                Tseries[1].typeID() == Tseries[0].typeID()                                   &&
                Tseries[1].typeID() == Tseries[2].typeID()                                   &&
                Tseries[0].controlinfo().flag(3) == 1	                                     &&
                Tseries[1].controlinfo().flag(3) == 2	                                     &&
                Tseries[2].controlinfo().flag(3) == 2 ){


				ABS20 = fabs( Tseries[2].original()-Tseries[0].original() );
				ABS10 = fabs( Tseries[1].original()-Tseries[0].original() );

				if (ABS20 < ABS10) {
                    std::cout << Tseries[0].obstime() << " " << Tseries[0].original() << " " << Tseries[0].corrected() << " " << Tseries[0].controlinfo() << std::endl;
                    std::cout << Tseries[1].obstime() << " " << Tseries[1].original() << " " << Tseries[1].corrected() << " " << Tseries[1].controlinfo() << std::endl;
                    std::cout << Tseries[2].obstime() << " " << Tseries[2].original() << " " << Tseries[2].corrected() << " " << Tseries[2].controlinfo() << std::endl;
				    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << std::endl;
				    std::cout << Tseries[0].original() << " " <<Tseries[1].original() << " " << Tseries[2].original() << std::endl;
				    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << std::endl;
                    LinInterpolated=0.5*(Tseries[0].original()+Tseries[2].original());
				}
                                 
                 try{
                     if ( CheckFlags.true_nibble(id->controlinfo(),params.Wflag,15,params.Wbool) ) {  // check for HQC action already
                        fixflags1=Tseries[1].controlinfo();
                        fixflags2=Tseries[2].controlinfo();
                        CheckFlags.setter(fixflags,params.Sflag);
                        CheckFlags.setter(fixflags,params.Sflag);
                        CheckFlags.conditional_setter(fixflags,params.chflag);
                        new_cfailed=Tseries[1].cfailed();
                        if (new_cfailed.length() > 0) new_cfailed += ",";
                        new_cfailed += "QC2d-2";
                        if (params.CFAILED_STRING.length() > 0) new_cfailed += ","+params.CFAILED_STRING;
                        dwrite.clean();
                        dwrite.set(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].original(),Tseries[1].paramID(),Tseries[1].tbtime(),
                              Tseries[1].typeID(),Tseries[1].sensor(), Tseries[1].level(),Tseries[1].corrected(),fixflags,Tseries[1].useinfo(),
                              new_cfailed );
                        kvUseInfo ui = dwrite.useinfo();
                        ui.setUseFlags( dwrite.controlinfo() );
                        dwrite.useinfo( ui );   
                        LOGINFO("DipTest: "+kvqc2logstring(dwrite) );
                        dbGate.insert( dwrite, "data", true); 
                        kvalobs::kvStationInfo::kvStationInfo DataToWrite(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].typeID());
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
                  if(!stList.empty()){
                     checkedDataHelper.sendDataToService(stList);
                     stList.clear();
                  }
            } // Three points to work with   
         } // Loopthrough all stations at the given time
      } // There is Qc2Data Loop
   ProcessTime.addHour(-1);
   } // TimeLoop
   return 0;
}
