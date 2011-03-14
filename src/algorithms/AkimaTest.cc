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

#include "AkimaSpline.h"

#include "tround.h"

using namespace kvalobs;
using namespace std;
using namespace miutil;

int 
ProcessImpl::
AkimaTest( ReadProgramOptions params )
{
   LOGINFO("Akima Test");
   int pid=params.pid;
   int Maxpid=params.maxpid;
   int Minpid=params.minpid;
   float LinInterpolated;
   float MaxMinInterpolated;
   float NewCorrected;
   miutil::miTime stime=params.UT0;
   miutil::miTime etime=params.UT1;
 
   std::list<kvalobs::kvStation> StationList;
   std::list<int> StationIds;
   std::list<kvalobs::kvData> Qc2Data;
   std::list<kvalobs::kvData> Qc2SeriesData;
   bool result, resultMin, resultMax;
 
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
   std::list<kvalobs::kvData> MaxValue;
   std::list<kvalobs::kvData> MinValue;
 
				 std::vector<double> xt,yt;
				 //for (int i=0;i<7;i++){
						 //xt.push_back(i*1.0);
						 //yt.push_back( xt[i]*xt[i] - xt[i]);
                 //}
				 //for (int i=8;i<12;i++){
						 //xt.push_back(i*1.0);
						 //yt.push_back( xt[i-1]*xt[i-1] - 7.0);
                 //}
//
                 //AkimaSpline AkimaY(xt,yt);
				 //AkimaY.AkimaPoints();
				 //std::cout << "0.5 " << AkimaY.AkimaPoint(0.5) << std::endl;
				 //std::cout << "7.0 " << AkimaY.AkimaPoint(7.0) << std::endl;
				 xt.clear();
				 yt.clear();

   //return 0;

   GetStationList(StationList);  /// StationList is all the possible stations ... Check
   for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
      StationIds.push_back( sit->stationID() );
   } 

   while (ProcessTime >= stime) 
   {
      XTime=ProcessTime;
      XTime.addHour(-3);
      YTime=ProcessTime;
      YTime.addHour(2);
      Tseries.clear();
      try {
        result = dbGate.select(Qc2Data, kvQueries::selectMissingData(params.missing,pid,ProcessTime));
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

                //std::cout << Tseries.size()                                                     << " " <<
                //Tseries[1].corrected()                                                          << " " <<
                //(Tseries[1].obstime().hour() == (Tseries[0].obstime().hour() + 1) % 24)         << " " <<
                //(Tseries[1].obstime().hour() == (24 + (Tseries[2].obstime().hour() - 1)) % 24)  << " " <<
                //(Tseries[1].typeID() == Tseries[0].typeID())                                    << " " <<
                //(Tseries[1].typeID() == Tseries[2].typeID())                                    << " " <<
                //!CheckFlags.condition(Tseries[0].controlinfo(),params.Notflag)                  << " " <<
                //!CheckFlags.condition(Tseries[2].controlinfo(),params.Notflag)                  << " " <<
                //CheckFlags.condition(Tseries[0].controlinfo(),params.Aflag)                     << " " <<
                //CheckFlags.condition(Tseries[2].controlinfo(),params.Aflag)                     << " " <<
                //!CheckFlags.condition(Tseries[0].useinfo(),params.NotUflag)                     << " " <<
                //!CheckFlags.condition(Tseries[2].useinfo(),params.NotUflag)                     << " " <<
                //CheckFlags.condition(Tseries[0].useinfo(),params.Uflag)                         << " " <<
                //CheckFlags.condition(Tseries[2].useinfo(),params.Uflag)  << " " << std::endl;

				// Need 5 points for AKima. One point in the t-series is missing so use
				// three in the past and two in the future. Therefore need a good run of
				// 6 points.
				//
				//  x  x  x  M  x x
				//
				//

            if (Tseries.size()==6                                                            &&
                Tseries[3].corrected() == params.missing                                     &&
                Tseries[1].obstime().hour() == (Tseries[0].obstime().hour() + 1) % 24        &&
                Tseries[2].obstime().hour() == (Tseries[1].obstime().hour() + 1) % 24        &&
                Tseries[3].obstime().hour() == (Tseries[2].obstime().hour() + 1) % 24        &&
                Tseries[4].obstime().hour() == (Tseries[3].obstime().hour() + 1) % 24        &&
                Tseries[5].obstime().hour() == (Tseries[4].obstime().hour() + 1) % 24        &&
                Tseries[1].typeID() == Tseries[0].typeID()                                   &&
                Tseries[2].typeID() == Tseries[1].typeID()                                   &&
                Tseries[3].typeID() == Tseries[2].typeID()                                   &&
                Tseries[4].typeID() == Tseries[3].typeID()                                   &&
                Tseries[5].typeID() == Tseries[4].typeID()                                   &&
                CheckFlags.condition(Tseries[0].useinfo(),params.Uflag)                  &&
                CheckFlags.condition(Tseries[1].useinfo(),params.Uflag)                  &&
                CheckFlags.condition(Tseries[2].useinfo(),params.Uflag)                  &&
                CheckFlags.condition(Tseries[4].useinfo(),params.Uflag)                  &&
                CheckFlags.condition(Tseries[5].useinfo(),params.Uflag) ) {
				xt.clear();
				yt.clear();;
                if (Tseries[0].original() > params.missing && 
                    Tseries[1].original() > params.missing && 
                    Tseries[2].original() > params.missing && 
                    Tseries[4].original() > params.missing && 
                    Tseries[5].original() > params.missing && 
					Tseries[3].original()==params.missing ){
                 std::cout << Tseries[3].obstime() << " " << Tseries[3].stationID() << " " << Tseries[3].paramID() << "********************************" << std::endl;
                 LinInterpolated=0.5*(Tseries[2].original()+Tseries[4].original() );
				 std::cout << "LinearInterpolated for point 3: " << Tseries[3].obstime() << " : " << LinInterpolated << std::endl;
				 std::vector<double> xt,yt;
				 for (int i=0;i<6;i++){
						 if (i != 3 ) {
						   xt.push_back(i*1.0);
						   yt.push_back( Tseries[i].original() );
						   std::cout << i << ": "<< Tseries[i].obstime() << " " << Tseries[i].original() << std::endl;
						 }
                 }
                 AkimaSpline AkimaX(xt,yt);
				 AkimaX.AkimaPoints();
				 AkimaX.AkimaPoint(3.0);
				 // first argument only needs to be 1,2,3,4,5 ... maybe do not need
                 // If NewCorrected has not been set then use the LinInerpolated Result

                 ////// try{
                     ////// if (Tseries[1].corrected() != NewCorrected && NewCorrected != -99999.0 && CheckFlags.true_nibble(id->controlinfo(),params.Wflag,15,params.Wbool) ) {  
                        ////// fixflags=Tseries[1].controlinfo();
                        ////// CheckFlags.setter(fixflags,params.Sflag);
                        ////// CheckFlags.conditional_setter(fixflags,params.chflag);
                        ////// kvData d;                                                   
                        ////// // Round the value to the correct precision before writing back
                        ////// NewCorrected=round<float,1>(NewCorrected);
                        ////// d.set(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].original(),Tseries[1].paramID(),Tseries[1].tbtime(),
                              ////// Tseries[1].typeID(),Tseries[1].sensor(), Tseries[1].level(),NewCorrected,fixflags,Tseries[1].useinfo(),
                              ////// Tseries[1].cfailed()+" QC2d-2 "+params.CFAILED_STRING );
                        ////// kvUseInfo ui = d.useinfo();
                        ////// ui.setUseFlags( d.controlinfo() );
                        ////// d.useinfo( ui );   
                        ////// LOGINFO("ProcessUnitT:"+kvqc2logstring(d) );
                        ////// //LOGINFO("ProcessUnitT Writing Data "+StrmConvert(d.corrected())+" " +StrmConvert(Tseries[1].stationID())+" " +StrmConvert(Tseries[1].obstime().year())+"-" +StrmConvert(Tseries[1].obstime().month())+"-" +StrmConvert(Tseries[1].obstime().day())+" " +StrmConvert(Tseries[1].obstime().hour())+":" +StrmConvert(Tseries[1].obstime().min())+":" +StrmConvert(Tseries[1].obstime().sec()) );
                        ////// // TEMP //dbGate.insert( d, "data", true); 
                        ////// // TEMP //kvalobs::kvStationInfo::kvStationInfo DataToWrite(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].typeID());
                        ////// // TEMP //stList.push_back(DataToWrite);
                     ////// }
                  ////// }
                  ////// catch ( dnmi::db::SQLException & ex ) {
                     ////// IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
                     ////// std::cout<<"INSERTO> CATCH ex" << result <<std::endl;
                  ////// }
                  ////// catch ( ... ) {
                     ////// IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
                     ////// std::cout<<"INSERTO> CATCH ..." << result <<std::endl;
                  ////// }
                  ////// if(!stList.empty()){
                     ////// checkedDataHelper.sendDataToService(stList);
                     ////// stList.clear();
                  ////// }

               } // Contents of the points are valid
            } //Three points to work with
         } // Loopthrough all stations at the given time
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
