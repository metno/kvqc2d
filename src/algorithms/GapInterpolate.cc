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
#include <memory>
#include <stdexcept>

#include "CheckedDataCommandBase.h"
#include "CheckedDataHelper.h"

#include <math.h>
//GNU Statistical library
//#include <gsl/gsl_errno.h>
//#include <gsl/gsl_spline.h>

#include "AkimaSpline.h"

using namespace kvalobs;
using namespace std;
using namespace miutil;

int 
ProcessImpl::
GapInterpolate( ReadProgramOptions params )
{
  int pid=params.pid;
  int tid=params.tid;
  miutil::miTime stime=params.UT0;
  miutil::miTime etime=params.UT1;
  miutil::miTime reftime;


  std::list<kvalobs::kvStation> StationList;
  std::list<int> StationIds;
  std::list<kvalobs::kvData> Qc2SeriesData;
  std::list<kvalobs::kvData> ReturnData;
  bool result;

  kvalobs::kvStationInfoList  stList;
  CheckedDataHelper checkedDataHelper(app);

  kvalobs::kvDbGate dbGate( &con );

  double JulDec;
  long StartDay;
  double HourDec;

  int MissingVal;
  int AfterGap;

  std::vector<kvalobs::kvData> Tseries;

  GetStationList(StationList);  
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     StationIds.push_back( sit->stationID() );
  }

  /// LOOP THROUGH STATIONS
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     std::cout << sit->stationID() << std::endl;
     try {
            result = dbGate.select(Qc2SeriesData, kvQueries::selectData(sit->stationID(),pid,tid,stime,etime));
         }
         catch ( dnmi::db::SQLException & ex ) {
           IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
         }
         catch ( ... ) {
           IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
         }
         /// ANALYSE RESULTS FOR ONE STATIONS
		 MissingVal=0;
		 AfterGap=0;
		 reftime=stime;
         for (std::list<kvalobs::kvData>::const_iterator id = Qc2SeriesData.begin(); id != Qc2SeriesData.end(); ++id) {
            if (id->controlinfo().flag(7)==1 || id->controlinfo().flag(7)==2 || id->controlinfo().flag(7)==3 || id->controlinfo().flag(7)==4) MissingVal=1;
            if (id->obstime().hour() != ((24 + reftime.hour() - 1)) % 24) AfterGap=1;
		    std::cout << id->obstime() << " : " << id->original() << " : " << id->controlinfo() << " " << id->useinfo() << " : " << MissingVal << " : " << AfterGap << " : " << id->controlinfo();
			reftime=id->obstime();
		    MissingVal=0;
		    AfterGap=0;
			if (id->useinfo().flag(2)==0) {
					Tseries.push_back(*id);
					std::cout << "<-------------";
		    }
			std::cout << std::endl;
	     }
         AkimaSpline AkimaX(Tseries[0].obstime().hour(),Tseries[0].original());
		 Tseries.clear();
		 
  }  


return 0;
}

