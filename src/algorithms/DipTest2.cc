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
#include "ParseParValFile.h"
#include "kvMetadataTable.h"
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
   float LinInterpolated;
   float AkimaInterpolated;
   float ABS20, ABS10, ABS21;
   float delta;
   int pid;
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

   std::vector<kvalobs::kvData> Tseries;

   std::map<int, float>  PidValMap; 

   GetStationList(StationList);  /// StationList is all the possible stations ... Check
   for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
      StationIds.push_back( sit->stationID() );
   } 


////
// Look into the station_param metadata , this will be moved eslewhere later
// code lifted from .../kvQabased/kvQABaseDBConnection.cc
////


{
  bool result;
  std::string data;

  //****************//

  int sid=15890;
  miutil::miTime otime=stime;
  std::string qcx="QC1-1-211";
  std::list<kvMetadataTable> tables;

  //***************//

  // fetch metadata from table 'station_param'

  std::list<int> slist; // list of stations
  //slist = StationIds;
  slist.push_back( 0 );
  //slist.push_back( sid );

  std::list<kvalobs::kvStationParam> splist;

  try
  {
    std::cout << "trying" << std::endl;
    std::cout <<  kvQueries::selectStationParam( slist, otime, qcx ) << std::endl;
    result = dbGate.select( splist,
                            kvQueries::selectStationParam( slist, otime, qcx ) );
  }
  catch ( dnmi::db::SQLException & ex )
  {
    IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
  }
  catch ( ... )
  {
    IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
  }

  if ( !result )
    std::cout << "no result" << std::endl;
    return false;

  std::list<kvalobs::kvStationParam>::const_iterator it = splist.begin();

  for ( ;it != splist.end(); it++ )
  {
    IDLOGDEBUG( "html", "Found StationParams:" << *it << std::endl );
    std::cout << "... result" << std::endl;

	std::cout << it->paramID() << std::endl;
	std::cout << it->metadata() << std::endl;

    // get info about parameter
    kvalobs::kvParam kvparam;
    //if ( !getParameter( it->paramID(), kvparam ) )
    //{
      //IDLOGWARN( "html", "kvQABaseDBConnection::getMetadata WARNING "
                 //<< " getParameter failed, paramid:"
                 //<< it->paramID() << std::endl );
      //return false;
    //}

    //std::ostringstream ost;
    //ost << kvparam.name() << "&"
    //<< it->level() << "&"
    //<< it->sensor();

    // unpack metadata-string to table-structures
    //result &= kvMetadataTable::processString( ost.str(),
              //it->metadata(), tables );
    //if ( !result )
    //{
      //IDLOGWARN( "html", "kvQABaseDBConnection::getMetadata WARNING "
                 //<< " kvMetadataTable::processString with name:"
                 //<< ost.str() << " and data:" << it->metadata() << std::endl );
    //}
  }

  std::cout << "Result: " << result << std::endl;
}

////
////
////

   //int pid=params.pid;
   if (params.ParValFile != "NotSet") {
          ParseParValFile ParValues(params.ParValFile);   
          PidValMap=ParValues.ReturnMap(); 
   }
 
 
   for (std::map<int, float>::const_iterator it=PidValMap.begin(); it!=PidValMap.end(); ++it) {
      std::cout << (*it).first << " " << (*it).second << std::endl;
	  pid=(*it).first;
      delta=(*it).second;
	  std::cout << pid << " " << delta << std::endl;
      ProcessTime = etime;
	  std::cout << "------------------" << std::endl;
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
                   Tseries[0].controlinfo().flag(3) == 1	                                    &&
                   Tseries[1].controlinfo().flag(3) == 2	                                    &&
                   Tseries[2].controlinfo().flag(3) == 2 ) {
   
   //            x
   //             
   //      x           x     -> time
   //
   //     A(0)  A(1)  A(2)
   //
				   ABS20 = fabs( Tseries[2].original()-Tseries[0].original() );
				   ABS10 = fabs( Tseries[1].original()-Tseries[0].original() );
                   ABS21 = fabs( Tseries[2].original()-Tseries[1].original() );
   
   
				   if (ABS20 < ABS10 && ABS21>delta) {
   
                       LinInterpolated=round<float,1>( 0.5*(Tseries[0].original()+Tseries[2].original()) );
                                       
                       try{
                           if ( CheckFlags.true_nibble(id->controlinfo(),params.Wflag,15,params.Wbool) ) {  // check for HQC action already
                             
					          fixflags1=Tseries[1].controlinfo(); // later control this from the config file
					          fixflags1.set(3,9); // later control this from the config file
                              new_cfailed1=Tseries[1].cfailed();
                              if (new_cfailed1.length() > 0) new_cfailed1 += ",";
                              new_cfailed1 += "QC2d-1";
                              if (params.CFAILED_STRING.length() > 0) new_cfailed1 += ","+params.CFAILED_STRING;
                              
					          fixflags2=Tseries[1].controlinfo(); // later control this from the config file
					          fixflags2.set(3,4); // later control this from the config file
						      new_cfailed2=Tseries[2].cfailed();
                              if (new_cfailed2.length() > 0) new_cfailed2 += ",";
                              new_cfailed2 += "QC2d-1";
                              if (params.CFAILED_STRING.length() > 0) new_cfailed2 += ","+params.CFAILED_STRING;
      
                              dwrite1.clean();
                              dwrite1.set(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].original(),Tseries[1].paramID(),Tseries[1].tbtime(),
                                    Tseries[1].typeID(),Tseries[1].sensor(), Tseries[1].level(),LinInterpolated,fixflags1,Tseries[1].useinfo(),
                                    new_cfailed1 );
                              kvUseInfo ui1 = dwrite1.useinfo();
                              ui1.setUseFlags( dwrite1.controlinfo() );
                              dwrite1.useinfo( ui1 );   
                              dbGate.insert( dwrite1, "data", true); 
      
                              dwrite2.clean();
                              dwrite2.set(Tseries[2].stationID(),Tseries[2].obstime(),Tseries[2].original(),Tseries[2].paramID(),Tseries[2].tbtime(),
                                    Tseries[2].typeID(),Tseries[2].sensor(), Tseries[2].level(),Tseries[2].corrected(),fixflags2,Tseries[2].useinfo(),
                                    new_cfailed2 );
                              kvUseInfo ui2 = dwrite2.useinfo();
                              ui2.setUseFlags( dwrite2.controlinfo() );
                              dwrite2.useinfo( ui2 );   
                              dbGate.insert( dwrite2, "data", true); 
      
                              kvalobs::kvStationInfo::kvStationInfo DataToWrite1(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].typeID());
                              stList.push_back(DataToWrite1);
                              kvalobs::kvStationInfo::kvStationInfo DataToWrite2(Tseries[2].stationID(),Tseries[2].obstime(),Tseries[2].typeID());
                              stList.push_back(DataToWrite2);
      
                              LOGINFO("DipTest: "+kvqc2logstring(dwrite1) );
                              LOGINFO("DipTest: "+kvqc2logstring(dwrite2) );
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
				     }
               } // Three points to work with   
            } // Loopthrough all stations at the given time
         } // There is Qc2Data Loop
      ProcessTime.addHour(-1);
      } // TimeLoop
   } // parameter loop
   return 0;
}