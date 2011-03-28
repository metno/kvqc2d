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
#include "algorithms/BasicStatistics.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "algorithms/Qc2D.h"
#include "ReadProgramOptions.h"
#include <sstream>
#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>
#include <puTools/miTime.h>
#include <memory>
#include <stdexcept>

#include "CheckedDataCommandBase.h"
#include "CheckedDataHelper.h"



using namespace kvalobs;
using namespace std;
using namespace miutil;

ProcessImpl::ProcessImpl( Qc2App &app_, dnmi::db::Connection & con_ )
    : app( app_ ), con( con_ )
{
}

void 
ProcessImpl::
GetStationList(std::list<kvalobs::kvStation>& StationList)
{

   bool result;
   kvalobs::kvDbGate dbGate( &con );
   std::list<kvalobs::kvStation> SL;

   try {
        //result = dbGate.select( StationList, kvQueries::selectAllStations("stationid"), "station" );
        result = dbGate.select( SL, kvQueries::selectAllStations("stationid"), "station" );
     }
     catch ( dnmi::db::SQLException & ex ) {
        IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
     }
     catch ( ... ) {
        IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
     }

// Make Qc2 specific selection on the StationList here
// Only use stations less than 100000 i.e. only Norwegian stations
// Also remove stations that are moving, e.g. ships.
    for ( std::list<kvalobs::kvStation>::const_iterator it = SL.begin(); it != SL.end(); ++it ) {
			if (it->stationID() < 100000  &&  it->maxspeed()==0.0) {
			    StationList.push_back(*it);
			}
    }
    //for ( std::list<kvalobs::kvStation>::const_iterator iq = StationList.begin(); iq != StationList.end(); ++iq ) {
    //     std::cout << iq->stationID() << std::endl;
    //}

}

void 
ProcessImpl::
GetStationList(std::list<kvalobs::kvStation>& StationList, miutil::miTime ProcessTime)
{

   bool result;
   kvalobs::kvDbGate dbGate( &con );

   try {
        result = dbGate.select( StationList, kvQueries::selectAllStations("stationid"), "station" );
     }
     catch ( dnmi::db::SQLException & ex ) {
        IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
     }
     catch ( ... ) {
        IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
     }
}

int 
ProcessImpl::
select(ReadProgramOptions params)
{
     std::map<std::string,int> ALG;
	 ALG["SingleLinear"]=1;
	 ALG["Redistribute"]=2;
	 ALG["DipTest"]=3;
	 ALG["Dummy"]=12;

     int AlgoCode = params.AlgoCode;
	 std::string Algorithm = params.Algorithm;

	 std::cout << "Algorithm ... " << ALG[Algorithm] << std::endl;

     switch (ALG[Algorithm]) {
     case 1:
         LOGINFO("Case "+Algorithm);
         SingleLinear_v32(params);
         LOGINFO(Algorithm+" Completed");
         break;
     case 2:
         LOGINFO("Case " + Algorithm);
         Redistribute(params);
         LOGINFO(Algorithm+" Completed");
         break;
     case 3:
         LOGINFO("Case " + Algorithm);
         DipTest(params);
         LOGINFO(Algorithm+" Completed");
         break;
     case 12:
         LOGINFO("Case " + Algorithm);
         LOGINFO("Case 12: Dummy");
         break;
     default:
         std::cout << "No valid Algorithm Code Provided. Case: " << AlgoCode << std::endl;
         LOGINFO("Case ??: No Valid Algorithm Specified");
         break;
     }
     return(0);
}

std::string   
ProcessImpl::
kvqc2logstring(kvalobs::kvData kd)
{
  std::string logstring;

  logstring=
      StrmConvert(kd.stationID())+" "
      +StrmConvert(kd.obstime().year())+"-"
      +StrmConvert(kd.obstime().month())+"-"
      +StrmConvert(kd.obstime().day())+" "
      +StrmConvert(kd.obstime().hour())+":"
      +StrmConvert(kd.obstime().min())+":"
      +StrmConvert(kd.obstime().sec())+" "
      +StrmConvert(kd.original())+" "
      +StrmConvert(kd.paramID())+" "
      +StrmConvert(kd.typeID())+" "
      +StrmConvert(kd.sensor())+" "
      +StrmConvert(kd.level())+" "
      +StrmConvert(kd.corrected());

return(logstring);
}
