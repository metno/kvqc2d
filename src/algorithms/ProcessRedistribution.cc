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
Redistribute( ReadProgramOptions params )
{

  LOGINFO("Redistribute Accumulations");

  miutil::miTime stime=params.UT0;
  miutil::miTime etime=params.UT1;
  miutil::miTime PreviousCheck;
  const int pid=params.pid;
  const int tid=params.tid;
  int ignore_station=0;
  int NibbleIndex=params.nibble_index;
  const std::vector<int> tids=params.tids;

  ProcessControl CheckFlags;

  std::list<kvalobs::kvStation> StationList;
  std::list<kvalobs::kvStation> ActualStationList;
  std::list<int> StationIds;
  std::list<kvalobs::kvData> Qc2Data;
  std::list<kvalobs::kvData> CheckData;
  std::list<kvalobs::kvData> ReturnData;
  bool result;

  kvalobs::kvStationInfoList  stList;
  CheckedDataHelper checkedDataHelper(app);

  kvalobs::kvDbGate dbGate( &con );

  miutil::miTime ProcessTime;
  miutil::miString ladle;


  GetStationList(StationList);  /// StationList is all the possible stations
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     StationIds.push_back( sit->stationID() );
  }

  ProcessTime = stime;

  //std::cout << "ETIME TIME STAMP: " << etime << std::endl;
  //std::cout << "STIME TIME STAMP: " << stime << std::endl;
  //std::cout << "ProcessTIME TIME STAMP: " << ProcessTime << std::endl;



  while (ProcessTime <= etime) {

             try {

                //ostringstream ost;    /// later put this back in kvQueries ******************
                //ost << " WHERE stationid IN (";
                //for (std::list<int>::const_iterator sp= StationIds.begin(); sp!=StationIds.end(); sp++)
                    //ost << (sp==StationIds.begin() ? "" : ",") << *sp;
                //ost << ") and paramid="    << pid
                    //<< " and typeid IN (";
                //for (std::vector<int>::const_iterator tp= tids.begin(); tp!=tids.end(); tp++) 
                    //ost << (tp==tids.begin() ? "" : ",") << *tp;
                //ost << ") and obstime=\'"  << ProcessTime.isoTime() << "\'"
                    //<< " order by obstime";
                //ladle=ost.str();
                //result = dbGate.select(Qc2Data, ladle);

              //result = dbGate.select(Qc2Data, kvQueries::selectData(StationIds,pid,tid,ProcessTime,ProcessTime));
              /// TODO: interpolate across all type ids and check for effective duplicates.            
              result = dbGate.select(Qc2Data, kvQueries::selectData(StationIds,pid,ProcessTime,ProcessTime));  // Choose all the data
              }
              catch ( dnmi::db::SQLException & ex ) {
                IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
              }
              catch ( ... ) {
                IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
              }

      if(!Qc2Data.empty()) {

                Qc2D GSW(Qc2Data,StationList, params,"Generate Missing Rows");
                GSW.Qc2_interp(); 
                GSW.distributor(StationList,ReturnData,0);
                //GSW.write_cdf(StationList);
       }
              
       if(!ReturnData.empty()) {
          std::cout << "Not Empty" << std::endl;
          for (std::list<kvalobs::kvData>::const_iterator id = ReturnData.begin(); id != ReturnData.end(); ++id) {
				      //LOGINFO("---------------->: "+ kvqc2logstring(*id) );
                      PreviousCheck=id->obstime();
					  PreviousCheck.addDay(-1);
                      result = dbGate.select(CheckData, kvQueries::selectData(id->stationID(),pid,PreviousCheck,PreviousCheck) ); 
                      for (std::list<kvalobs::kvData>::const_iterator ic = CheckData.begin(); ic != CheckData.end(); ++ic) {
							  if (ic->corrected()==params.missing) {
									  ignore_station=ic->stationID(); 
				                      LOGWARN("Incomplete redistribution (skipped): "+ kvqc2logstring(*ic) );
                              }
                      }
                      try {
                           //if ( CheckFlags.condition(id->controlinfo(),params.Wflag) && id->stationID() != ignore_station) { 
                           if ( CheckFlags.true_nibble(id->controlinfo(),params.Wflag,NibbleIndex,params.Wbool) && 
								                                   id->stationID() != ignore_station ) {  
                           LOGINFO("Redistribution: "+kvqc2logstring(*id) );
                                        ////LOGINFO("Redistribute Precipitation Writing Data "
                                                                            //+StrmConvert(id->corrected())+" "
                                                                            //+StrmConvert(id->stationID())+" "
                                                                            //+StrmConvert(id->obstime().year())+"-"
                                                                            //+StrmConvert(id->obstime().month())+"-"
                                                                            //+StrmConvert(id->obstime().day())+" "
                                                                            //+StrmConvert(id->obstime().hour())+":"
                                                                            //+StrmConvert(id->obstime().min())+":"
                                                                            //+StrmConvert(id->obstime().sec()) );
                                kvData d = *id;
                                kvUseInfo ui = d.useinfo();
                                ui.setUseFlags( d.controlinfo() );
                                d.useinfo( ui );   
                                dbGate.insert( d, "data", true); 
                                kvalobs::kvStationInfo::kvStationInfo DataToWrite(id->stationID(),id->obstime(),id->typeID());
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
          }
		  ignore_station=0;
          if(!stList.empty()){
              checkedDataHelper.sendDataToService(stList);
              stList.clear();
          }
          ReturnData.clear();
       }
  ProcessTime.addDay();

  }
Qc2D GSW(Qc2Data,StationList,params);
GSW.distributor(StationList,ReturnData,1); /// solution for memory cleanup ... maybe needs to be improved.
return 0;
}

