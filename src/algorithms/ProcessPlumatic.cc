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

  std::list<kvalobs::kvStation> StationList;
  std::list<kvalobs::kvStation> ActualStationList;
  std::list<int> StationIds;
  std::list<int> TestStation;
  std::list<kvalobs::kvData> PluviData;
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

   std::list<kvalobs::kvStationParam> splist;
   std::ostringstream query;

   int sid=15890;
   TestStation.push_back( sid );
   miutil::miTime otime=stime;
   std::string qcx="QC1-1-211";

       //query << "SELECT metadata FROM station_param WHERE "
	            //"stationid in (0, " << si.stationID() << ") AND "
		        //"paramid=(SELECT paramid FROM param WHERE name='" << parameter << "') AND "
		        //"fromday<=" << dayNumber << " AND " << dayNumber << "<=today AND "
		        //"qcx='" << qcx << "' AND "
		        //"level=0 AND sensor='0' AND "
		        //"fromtime<='" << si.obstime() << "' "
		        //"ORDER BY stationid DESC, fromtime DESC "
		        //"LIMIT 1;";
	//result = dbGate.select(splist , query, "station_param")
	//kvQueries::selectStationParam( slist, otime, qcx )|
    std::ostringstream ozst;
    std::list<kvMetadataTable> tables;
	std::string data_value;
    result = dbGate.select( splist, kvQueries::selectStationParam( TestStation, otime, qcx ) );
    for (std::list<kvalobs::kvStationParam>::const_iterator lst=splist.begin(); lst!=splist.end(); ++lst) {
			std::cout << lst->metadata() << std::endl;
			std::cout << lst->descMetadata() << std::endl;
			std::cout << otime << std::endl;
			std::cout << qcx << std::endl;
			std::cout << "...................." << std::endl;

       miString dougal=lst->metadata(); 
	   vector<miString> vs,names,vs2;
	   vs = dougal.split("\n");

       for (size_t i=0; i<vs.size(); i++){
         miString t = vs[i];
		 std::cout << t << std::endl;
		 std::cout <<  "..."  << std::endl;
		 vs2=t.split(";");
         for (size_t j=0; j<vs2.size(); j++){
				 std::cout << vs2[j] << std::endl;
	     }
	   }





		 //ozst << kvparam.name() << "&"
		 ozst << "RR_01" << "&"
		     << 0 << "&"
		     << 0;
			 std::string vname= "RR_01";

//  HERE HERE HERE
		  // // unpack metadata-string to table-structures
			     result &= kvMetadataTable::processString( ozst.str(),
					               lst->metadata(), tables );
				 std::list<kvMetadataTable>::iterator mp= tables.begin();
				       for (; mp != tables.end(); mp++){
							   	if (mp->findEntry(vname, data_value)){
										std::cout << ".X." << std::endl;
						  	    }
				      }

				 //std::cout << tables.value_["RR_01"] << std::endl;
				 //tables.findEntry("RR_01",&data_value);
    }
// 

    return 0;
  /// LOOP THROUGH STATIONS
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     try {
			 ladle="WHERE STATIONID="+StrmConvert(sit->stationID())+" AND PARAMID="+StrmConvert(pid)+" AND obstime BETWEEN \'"+stime.isoTime()+"\' AND \'"+etime.isoTime()+"\'";
			 //std::cout << ladle << std::endl;
			 result = dbGate.select(PluviData, ladle);
         }
         catch ( dnmi::db::SQLException & ex ) {
           IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
         }
         catch ( ... ) {
           IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
         }
         /// ANALYSE RESULTS FOR ONE STATIONS

      if(!PluviData.empty()) {

		 Plumatic PL(PluviData,params);
		 std::cout << " --------------- " << std::endl;
       }
  }

return 0;
}

