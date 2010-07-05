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

#include "scone.h"
#include "tround.h"

using namespace kvalobs;
using namespace std;
using namespace miutil;

int 
ProcessImpl::
SingleLinear( ReadProgramOptions params )
{
   LOGINFO("Single Linear");
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
               if  ( !CheckFlags.condition(is->controlinfo(),params.Notflag) ) {  //Actually here that we check that this has not been 
                                                                                //done before by checking for set flags ...
                     Tseries.push_back(*is);
               }
            }
            if (Tseries.size()==3) {

               if (Tseries[0].original() > params.missing && Tseries[1].original()==params.missing && Tseries[2].original() > params.missing){

                 NewCorrected=-99999.0;
                 LinInterpolated=0.5*(Tseries[0].original()+Tseries[2].original());
                 if (params.maxpid>0 and params.minpid>0) {
                    resultMax = dbGate.select(MaxValue, kvQueries::selectData(id->stationID(),params.maxpid,YTime,YTime));
                    resultMin = dbGate.select(MinValue, kvQueries::selectData(id->stationID(),params.minpid,YTime,YTime)); 
                    if (MaxValue.size()==1 && MinValue.size()==1 && MaxValue.begin()->original() > -99.9 && MinValue.begin()->original() > -99.9){
                       if (LinInterpolated > MaxValue.begin()->original()) NewCorrected=MaxValue.begin()->original(); 
                       if (LinInterpolated < MinValue.begin()->original()) NewCorrected=MinValue.begin()->original(); 
                    }
		    if (Tseries[1].corrected() >= MinValue.begin()->original() && Tseries[1].corrected() <= MaxValue.begin()->original()) {
                       //NB if a corrected value exists and it is already between the min and max then do not overwrite
                       NewCorrected=Tseries[1].corrected(); 
                    }
                 }
                 // If NewCorrected has not been set then use the LinInerpolated Result
                 if (NewCorrected == -99999.0) NewCorrected=LinInterpolated;
                 try{
                     if (Tseries[1].corrected() != NewCorrected && NewCorrected != -99999.0 && CheckFlags.true_nibble(id->controlinfo(),params.Wflag,15,params.Wbool) ) {  
                        fixflags=Tseries[1].controlinfo();
                        CheckFlags.setter(fixflags,params.Sflag);
                        CheckFlags.conditional_setter(fixflags,params.chflag);
                        kvData d;                                                   
                        // Round the value to the correct precision before writing back
                        NewCorrected=round<float,1>(NewCorrected);
                        d.set(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].original(),Tseries[1].paramID(),Tseries[1].tbtime(),
                              Tseries[1].typeID(),Tseries[1].sensor(), Tseries[1].level(),NewCorrected,fixflags,Tseries[1].useinfo(),
                              Tseries[1].cfailed()+" QC2d-2 "+params.CFAILED_STRING );
                        kvUseInfo ui = d.useinfo();
                        ui.setUseFlags( d.controlinfo() );
                        d.useinfo( ui );   
                        LOGINFO("ProcessUnitT Writing Data "+StrmConvert(d.corrected())+" " +StrmConvert(Tseries[1].stationID())+" " +StrmConvert(Tseries[1].obstime().year())+"-" +StrmConvert(Tseries[1].obstime().month())+"-" +StrmConvert(Tseries[1].obstime().day())+" " +StrmConvert(Tseries[1].obstime().hour())+":" +StrmConvert(Tseries[1].obstime().min())+":" +StrmConvert(Tseries[1].obstime().sec()) );
                        dbGate.insert( d, "data", true); 
                        kvalobs::kvStationInfo::kvStationInfo DataToWrite(id->stationID(),id->obstime(),id->paramID());
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
