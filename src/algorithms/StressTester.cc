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
#include "scone.h"
#include "epsx.h"
#include "../StopWatch.h"


using namespace kvalobs;
using namespace std;
using namespace miutil;


int 
ProcessImpl::
StressTester( ReadProgramOptions params )
{

  LOGINFO("StressTester");
  miutil::miTime stime=params.UT0;
  miutil::miTime etime=params.UT1;
  const int pid=params.pid;
  const int tid=params.tid;

  miutil::miTime fixtime;
  long LoopCounter=0;
  double CpuUsage;


  std::vector<float> XP;
  std::vector<float> YP;

  kvalobs::kvControlInfo fixflags;
  ProcessControl CheckFlags;

  std::list<kvalobs::kvStation> StationList;
  std::list<kvalobs::kvStation> ActualStationList;
  std::list<int> StationIds;
  std::list<int> StationIdsActual;
  std::list<kvalobs::kvData> Qc2Data;
  kvalobs::kvData DataToWrite;
  bool result;

  kvalobs::kvStationInfoList  stList;
  CheckedDataHelper checkedDataHelper(app);

  kvalobs::kvDbGate dbGate( &con );

  miutil::miTime ProcessTime;

  GetStationList(StationList);  /// StationList is all the possible stations
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     StationIds.push_back( sit->stationID() );
  }
  ProcessTime = stime;
  stopwatch watchdog;
  while (ProcessTime <= etime) {  //START MAIN LOOP


             try {
              result = dbGate.select(Qc2Data, kvQueries::selectData(StationIds,pid,ProcessTime,ProcessTime));
              //std::cout << kvQueries::selectData(StationIds,pid,ProcessTime,ProcessTime) << std::endl;
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

       // PRINT OUT OR WRITE BACK SOME DATA
       //
            //std::cout << GSW.stid_.size() << std::endl;  //NB GSW holds the same as Qc2Data but with geo-location
            //std::cout << Qc2Data.size() << std::endl;    //information as well as interp, variability parameters etc ...

       // ADD circa 10 years (3650 days) to the data and write it back to the database ...
       
       for (std::list<kvalobs::kvData>::const_iterator id = Qc2Data.begin(); id != Qc2Data.end(); ++id) {
                      try {
                                fixtime=id->obstime();
                                fixtime.addDay(3650);
                                //can also work with GSW !!!!!!!!!!!
                                DataToWrite.set(id->stationID(),fixtime,id->original(),id->paramID(),
                                      id->tbtime(),id->typeID(), id->sensor(),
                                      id->level(), id->corrected(),id->controlinfo(), 
                                      id->useinfo(), id->cfailed()+" Qc2-R Loop=" + StrmConvert(LoopCounter));
                                //LOGINFO("Writing Data ");
                                dbGate.insert( DataToWrite, "data", true);
                                kvalobs::kvStationInfo::kvStationInfo DataToWrite(id->stationID(),id->obstime(),id->paramID());
                                stList.push_back(DataToWrite);
                       }
                       catch ( dnmi::db::SQLException & ex ) {
                         IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
                         std::cout<<"INSERTO> CATCH ex" << result <<std::endl;
                       }
                       catch ( ... ) {
                         IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
                         std::cout<<"INSERTO> CATCH ..." << result <<std::endl;
                       }
          }

       //
       }

       if (ProcessTime==etime) {  ///This will never stop!
          ProcessTime  = stime;
          LOGINFO("Stress test time loop completed. "+StrmConvert(LoopCounter));
          CpuUsage=watchdog.snapshot();
          LOGINFO("Elapsed CPU: "+StrmConvert(CpuUsage));
          std::cout << "Loop reset ... " << std::cout;
          ++LoopCounter;
       }

       ProcessTime.addDay();
     }  //END OF MAIN LOOP

return 0;
}

