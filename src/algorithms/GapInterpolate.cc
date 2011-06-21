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
  miutil::miDate PDate;
  //int ngap=parama.ngaps;
  int ngap=params.Ngap;
  std::cout << " ############### " << ngap << std::endl;
  double JulDec;
  long StartDay;
  double HourDec;
  double lowHour;
  double highHour;

  PDate.setDate(stime.year(),stime.month(),stime.day());
  StartDay=PDate.julianDay();

  std::vector<double> xt,yt;           // For holding only good data
  std::vector<double> xat,yat;         // For holiding all the data series (missing values as well as good)

  std::list<kvalobs::kvStation> StationList;
  std::list<int> StationIds;
  std::list<kvalobs::kvData> Qc2SeriesData;
  std::list<kvalobs::kvData> ReturnData;
  bool result;

  kvalobs::kvStationInfoList  stList;
  CheckedDataHelper checkedDataHelper(app);

  kvalobs::kvDbGate dbGate( &con );



  std::vector<kvalobs::kvData> Tseries;

  GetStationList(StationList);  
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     StationIds.push_back( sit->stationID() );
  }

  /// LOOP THROUGH STATIONS
  for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
     //std::cout << sit->stationID() << std::endl;
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
         xt.clear();
         yt.clear();
         xat.clear();
         yat.clear();
// Go through the data and fit an Akima Spline to the good points
         for (std::list<kvalobs::kvData>::const_iterator id = Qc2SeriesData.begin(); id != Qc2SeriesData.end(); ++id) {
			if (id->useinfo().flag(2)==0) {
                               PDate.setDate(id->obstime().year(),id->obstime().month(),id->obstime().day() );
                               JulDec=PDate.julianDay()+id->obstime().hour()/24.0 + 
                                                           id->obstime().min()/(24.0*60)+id->obstime().sec()/(24.0*60.0*60.0);
                               HourDec=(PDate.julianDay()-StartDay)*24.0 + id->obstime().hour() +
                                                                             id->obstime().min()/60.0+id->obstime().sec()/3600.0;
                    xt.push_back(HourDec);
                    yt.push_back(id->original());
		    } 
		 }
// Calculate the Akima Spline if there are enough points and the fill missing points
		 if (xt.size() > 4) {
            AkimaSpline AkimaX(xt,yt);
            //AkimaX.AkimaPoints();
// Find the missing points and their distance from a good point etc ..
         for (std::list<kvalobs::kvData>::const_iterator id = Qc2SeriesData.begin(); id != Qc2SeriesData.end(); ++id) {
				  if (id->controlinfo().flag(6)==1 || id->controlinfo().flag(6)==2 || id->controlinfo().flag(6)==3 || id->controlinfo().flag(6)==4){
                     //value is missing so find the time stamp wrt Akima fit
                     PDate.setDate(id->obstime().year(),id->obstime().month(),id->obstime().day() );
                     JulDec=PDate.julianDay()+id->obstime().hour()/24.0 + 
                                             id->obstime().min()/(24.0*60)+id->obstime().sec()/(24.0*60.0*60.0);
                     HourDec=(PDate.julianDay()-StartDay)*24.0 + id->obstime().hour() +
                                             id->obstime().min()/60.0+id->obstime().sec()/3600.0;
                     // Check to see how many hours the point is away from good data ... in the past ... 
                     for (std::vector<double>::const_iterator iv=xt.begin(); iv !=xt.end(); ++iv) {
                         if (*iv < HourDec) lowHour=*iv;
                     }
					 // ... and in the future
                     for (std::vector<double>::const_iterator iv=xt.end(); iv !=xt.begin(); --iv) {
                         if (*iv > HourDec) highHour=*iv;
                     }
					 //std::cout << lowHour << " :: " << HourDec << " :: " << highHour << std::endl;
					 if ( ( std::find(xt.begin(), xt.end(), highHour+1) != xt.end() ) &&
						  ( std::find(xt.begin(), xt.end(), lowHour-1)  !=xt.end()  ) &&
						  ( std::find(xt.begin(), xt.end(), lowHour-2)  != xt.end() || std::find(xt.begin(), xt.end(), highHour+2)  != xt.end() ) &&
						  ( HourDec - lowHour <= ngap ) &&
						  ( highHour - HourDec  < ngap ) ) {

                      // Do Akima Interpolation
					    std::cout << id->stationID() << " " << id->obstime() << " " << id->original() << " " << id->corrected() << " Sub Akima " << AkimaX.AkimaPoint(HourDec) << std::endl;

		             }

                  } else {
					    std::cout << id->obstime() << " " << id->original() << " " << id->corrected() << " --------- " << std::endl;
				  }
        	 }
         } // end of IF enough points for AKima
  }  
return 0;
}

