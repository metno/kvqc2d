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
SingleLinear_v32( ReadProgramOptions params )
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

   std::map<int, std::vector<std::string> > setmissing_chflag;
   std::vector<std::string> setmissing_fmis;
   //char *s1="1->3";
   //char *s2="4->2";
   setmissing_fmis.push_back("1->3");
   setmissing_fmis.push_back("4->2");
   setmissing_chflag[6]=setmissing_fmis;
 
   GetStationList(StationList);  /// StationList is all the possible stations ... Check
   for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
      StationIds.push_back( sit->stationID() );
   } 

//
//
//  while (ProcessTime >= stime)
//  {
//  Find Missing
//    >>Loop Results
//       
//        Get Neigbours
//
//        If Good{
//        if ftime=0 Interpolate and update
//        if ftime=1 Check and update if differet 
//        }
//        Else Bad{
//        if ftime=0 Do Nothing  (NB this line generates no code)
//        if ftime=1 Revert to missing ... missing update
//        }
//        
//       If update set{write back to the db}
//
//    ##Loop Results
//  Next Time
//  }
//
// NB code like this first until logic checked ... if (id->controlinfo().flag(13)==9) 
//
// and then add in the config file parameters ... 



   miutil::miString ladle;

   while (ProcessTime >= stime) 
   {
      XTime=ProcessTime;
      XTime.addHour(-1);
      YTime=ProcessTime;
      YTime.addHour(1);
      Tseries.clear();
      try {
        //result = dbGate.select(Qc2Data, kvQueries::selectMissingData(params.missing,pid,ProcessTime));
        //result = dbGate.select(Qc2Data, "WHERE (substr(controlinfo,7,1)='1' OR substr(controlinfo,7,1)='2' OR substr(controlinfo,7,1)='3' OR substr(controlinfo,7,1)='4') AND PARAMID=211 AND obstime=ProcessTime.isoTime()");
        ladle="WHERE (substr(controlinfo,7,1)='1' OR substr(controlinfo,7,1)='2' OR substr(controlinfo,7,1)='3' OR substr(controlinfo,7,1)='4') AND PARAMID=211 AND obstime=\'"+ProcessTime.isoTime()+"\'";
        //std::cout << ladle << std::endl;
        result = dbGate.select(Qc2Data, ladle);
      }
      catch ( dnmi::db::SQLException & ex ) {
        IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
      }
      catch ( ... ) {
        IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
      }
      if(!Qc2Data.empty()) { 
         std::cout<<"Not Emplty" <<std::endl;
         for (std::list<kvalobs::kvData>::const_iterator id = Qc2Data.begin(); id != Qc2Data.end(); ++id) {
            Tseries.clear();  
            result = dbGate.select(Qc2SeriesData, kvQueries::selectData(id->stationID(),pid,XTime,YTime));

            for (std::list<kvalobs::kvData>::const_iterator is = Qc2SeriesData.begin(); is != Qc2SeriesData.end(); ++is) {
                     Tseries.push_back(*is);
            }
            
            NewCorrected=-99999.0;
            if (Tseries.size()==3                                                            &&
                Tseries[1].obstime().hour() == (Tseries[0].obstime().hour() + 1) % 24        &&
                Tseries[1].obstime().hour() == (24 + (Tseries[2].obstime().hour() - 1)) % 24 &&      
                Tseries[1].typeID() == Tseries[0].typeID()                                   &&
                Tseries[1].typeID() == Tseries[2].typeID()                                   &&
                !CheckFlags.condition(Tseries[0].controlinfo(),params.Notflag)               &&
                !CheckFlags.condition(Tseries[2].controlinfo(),params.Notflag)               &&
                CheckFlags.condition(Tseries[0].controlinfo(),params.Aflag)                  &&
                CheckFlags.condition(Tseries[2].controlinfo(),params.Aflag)                  &&
                !CheckFlags.condition(Tseries[0].useinfo(),params.NotUflag)                  &&
                !CheckFlags.condition(Tseries[2].useinfo(),params.NotUflag)                  &&
                CheckFlags.condition(Tseries[0].useinfo(),params.Uflag)                      &&
                CheckFlags.condition(Tseries[2].useinfo(),params.Uflag)                      &&
                Tseries[0].original() > params.missing                                       && 
                (Tseries[1].original()== params.missing || Tseries[1].corrected() == params.missing) && 
                Tseries[2].original() > params.missing  )     
                {
                   if (Tseries[1].controlinfo().flag(7) == 0){  
                   std::cout << " ftime=0 " << std::endl;
                      LinInterpolated=0.5*(Tseries[0].original()+Tseries[2].original());
                      NewCorrected=LinInterpolated;
                      if (params.maxpid>0 and params.minpid>0) {
                         resultMax = dbGate.select(MaxValue, kvQueries::selectData(id->stationID(),params.maxpid,YTime,YTime));
                         resultMin = dbGate.select(MinValue, kvQueries::selectData(id->stationID(),params.minpid,YTime,YTime)); 
                         if (MaxValue.size()==1 && MinValue.size()==1 && MaxValue.begin()->original() > -99.9 && MinValue.begin()->original() > -99.9){
                            if (LinInterpolated > MaxValue.begin()->original()) NewCorrected=MaxValue.begin()->original(); 
                            if (LinInterpolated < MinValue.begin()->original()) NewCorrected=MinValue.begin()->original(); 
                         }
		         if (Tseries[1].corrected() >= MinValue.begin()->original() && Tseries[1].corrected() <= MaxValue.begin()->original()) {
                         //NB if a corrected value exists and it is already between the min and max then do not overwrite
                            NewCorrected=-99999.0;  // -99999.0 is the flag to not write
                         }
                      }
                   }
                   if (Tseries[1].controlinfo().flag(7) == 1){	
                      std::cout << " ftime=1 " << std::endl;
                      LinInterpolated=0.5*(Tseries[0].original()+Tseries[2].original());
                      NewCorrected=LinInterpolated;
                      if (params.maxpid>0 and params.minpid>0) {
                         resultMax = dbGate.select(MaxValue, kvQueries::selectData(id->stationID(),params.maxpid,YTime,YTime));
                         resultMin = dbGate.select(MinValue, kvQueries::selectData(id->stationID(),params.minpid,YTime,YTime)); 
                         if (MaxValue.size()==1 && MinValue.size()==1 && MaxValue.begin()->original() > -99.9 && MinValue.begin()->original() > -99.9){
                            if (LinInterpolated > MaxValue.begin()->original()) NewCorrected=MaxValue.begin()->original(); 
                            if (LinInterpolated < MinValue.begin()->original()) NewCorrected=MinValue.begin()->original(); 
                         }
		         if (Tseries[1].corrected() >= MinValue.begin()->original() && Tseries[1].corrected() <= MaxValue.begin()->original()) {
                            //NB if a corrected value exists and it is already between the min and max then do not overwrite
                            NewCorrected=-99999.0; 
                         }
                         if (NewCorrected==Tseries[1].corrected()) {
                            NewCorrected=-99999.0;
                         }
                      }
                   }
                }
                else {
                   //if ftime=0{ } ... for this option we do nothing
                   //and for this option we reset to missing value ...
                   if (Tseries[1].controlinfo().flag(7) == 1){	
                      NewCorrected=params.missing;
                   }                	
                }              
           
                 // If NewCorrected has not been set then use the LinInerpolated Result
                 //if (NewCorrected == -99999.0) NewCorrected=LinInterpolated;
                 try{
                     if (NewCorrected != -99999.0 && CheckFlags.true_nibble(id->controlinfo(),params.Wflag,15,params.Wbool) ) {  
                        fixflags=Tseries[1].controlinfo();
                        CheckFlags.setter(fixflags,params.Sflag);
                        CheckFlags.conditional_setter(fixflags,params.chflag);
                        if (NewCorrected == params.missing){
                           CheckFlags.conditional_setter(fixflags,setmissing_chflag);
                        }
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
                  
         } // Loopthrough all stations at the given time
      } // There is Qc2Data Loop
   ProcessTime.addHour(-1);
   } // TimeLoop
   return 0;
}
