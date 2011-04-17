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
SingleLinear_v33( ReadProgramOptions params )
{
   LOGINFO("Single Linear");
   int pid=params.pid;
   float LinInterpolated;
   float AkimaInterpolated;
   float NewCorrected;
   bool AkimaPresent=false;
   miutil::miTime stime=params.UT0;
   miutil::miTime etime=params.UT1;
                              //For later
                              //AkimaSpline AkimaX(xt,yt);
				              //AkimaInterpolated=round<float,1>( AkimaX.AkimaPoint(3.0) );
						      //AkimaPresent=true;
 
   std::list<kvalobs::kvStation> StationList;
   std::list<int> StationIds;
   std::list<kvalobs::kvData> Qc2Data;
   std::list<kvalobs::kvData> Qc2SeriesData;
   bool result;
 
   ProcessControl CheckFlags;
   kvalobs::kvControlInfo fixflags;
   kvalobs::kvData dwrite;                                                   

   kvalobs::kvStationInfoList  stList;
   CheckedDataHelper checkedDataHelper(app);

   kvalobs::kvDbGate dbGate( &con );

   miutil::miTime ProcessTime;
   miutil::miTime XTime;
   miutil::miTime YTime;
   ProcessTime = etime;

   std::vector<kvalobs::kvData> Tseries;

   std::map<int, std::vector<std::string> > setmissing_chflag;
   std::vector<std::string> setmissing_fmis;
   setmissing_fmis.push_back("1->3");
   setmissing_fmis.push_back("4->2");
   setmissing_chflag[6]=setmissing_fmis;
 
   GetStationList(StationList);  /// StationList is all the possible stations ... Check
   for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
      StationIds.push_back( sit->stationID() );
   } 

// PSEUDO-CODE
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
// NB code like this first until logic checked ...  
// and then add in the config file parameters ... 



   miutil::miString ladle;
   miutil::miString new_cfailed;

   while (ProcessTime >= stime) 
   {
      XTime=ProcessTime;
      XTime.addHour(-1);
      YTime=ProcessTime;
      YTime.addHour(1);
      try {
        //result = dbGate.select(Qc2Data, kvQueries::selectMissingData(params.missing,pid,ProcessTime));
        ladle="WHERE (substr(controlinfo,7,1)='1' OR substr(controlinfo,7,1)='2' OR substr(controlinfo,7,1)='3' OR substr(controlinfo,7,1)='4') AND PARAMID="+StrmConvert(pid)+" AND obstime=\'"+ProcessTime.isoTime()+"\'";
        result = dbGate.select(Qc2Data, ladle);
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
            
            if  (Tseries.size()==3 &&  // Check that we have three valid data points
                Tseries[1].obstime().hour() == (Tseries[0].obstime().hour() + 1) % 24        &&
                Tseries[1].obstime().hour() == (24 + (Tseries[2].obstime().hour() - 1)) % 24 &&      
                Tseries[1].typeID() == Tseries[0].typeID()                                   &&
                Tseries[1].typeID() == Tseries[2].typeID() ){

                NewCorrected=-99999.0; // By default no update will be made

                if (!CheckFlags.condition(Tseries[0].controlinfo(),params.Notflag)           && // check that the neighbours are good
                   !CheckFlags.condition(Tseries[2].controlinfo(),params.Notflag)            &&
                   CheckFlags.condition(Tseries[0].controlinfo(),params.Aflag)               &&
                   CheckFlags.condition(Tseries[2].controlinfo(),params.Aflag)               &&
                   !CheckFlags.condition(Tseries[0].useinfo(),params.NotUflag)               &&
                   !CheckFlags.condition(Tseries[2].useinfo(),params.NotUflag)               &&
                   CheckFlags.condition(Tseries[0].useinfo(),params.Uflag)                   &&
                   CheckFlags.condition(Tseries[2].useinfo(),params.Uflag)                   &&
                   Tseries[0].original() != params.missing                                   && 
                   Tseries[0].original() != params.rejected                                  && 
                   Tseries[2].original() != params.missing                                   && 
                   Tseries[2].original() != params.rejected )                                       
                   {
                      if (Tseries[1].controlinfo().flag(7) == 0){  
                         LinInterpolated=0.5*(Tseries[0].original()+Tseries[2].original());
                         NewCorrected=round<float,1>(LinInterpolated);
                      }
                      if (Tseries[1].controlinfo().flag(7) == 1){	
                         LinInterpolated=0.5*(Tseries[0].original()+Tseries[2].original());
                         NewCorrected=round<float,1>(LinInterpolated);
                         if (NewCorrected==Tseries[1].corrected()) {
                            NewCorrected=-99999.0;
                         }
                      }
                   }
                   else {
                      //NB for ftime=0 ... do nothing

                      //For ftime=1, reset to missing value ...
                      if (Tseries[1].controlinfo().flag(7) == 1){	
                         if (Tseries[1].controlinfo().flag(6)==1 || Tseries[1].controlinfo().flag(6)==3) NewCorrected=params.missing;
                         if (Tseries[1].controlinfo().flag(6)==2 || Tseries[1].controlinfo().flag(6)==4) NewCorrected=params.rejected;
                      }                	
                      if (NewCorrected==Tseries[1].corrected()) {
                            NewCorrected=-99999.0;
                      }
                   }              
              
                 try{
                     if (NewCorrected != -99999.0 && CheckFlags.true_nibble(id->controlinfo(),params.Wflag,15,params.Wbool) ) {  
                        fixflags=Tseries[1].controlinfo();
                        CheckFlags.setter(fixflags,params.Sflag);
                        CheckFlags.conditional_setter(fixflags,params.chflag);
                        if (NewCorrected == params.missing){
                           CheckFlags.conditional_setter(fixflags,setmissing_chflag);
                        }
                        if (NewCorrected == params.rejected){
                           CheckFlags.conditional_setter(fixflags,setmissing_chflag);
                        }
                        new_cfailed=Tseries[1].cfailed();
                        if (new_cfailed.length() > 0) new_cfailed += ",";
                        new_cfailed += "QC2d-2";
                        if (params.CFAILED_STRING.length() > 0) new_cfailed += ","+params.CFAILED_STRING;

                        dwrite.clean();
                        dwrite.set(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].original(),Tseries[1].paramID(),Tseries[1].tbtime(),
                              Tseries[1].typeID(),Tseries[1].sensor(), Tseries[1].level(),NewCorrected,fixflags,Tseries[1].useinfo(),
                              new_cfailed );
                        kvUseInfo ui = dwrite.useinfo();
                        ui.setUseFlags( dwrite.controlinfo() );
                        dwrite.useinfo( ui );   
                        LOGINFO("SingleLinear_v33: "+kvqc2logstring(dwrite) );
                        //LOGINFO("ProcessUnitT Writing Data "+StrmConvert(dwrite.corrected())+" " +StrmConvert(Tseries[1].stationID())+" " +StrmConvert(Tseries[1].obstime().year())+"-" +StrmConvert(Tseries[1].obstime().month())+"-" +StrmConvert(Tseries[1].obstime().day())+" " +StrmConvert(Tseries[1].obstime().hour())+":" +StrmConvert(Tseries[1].obstime().min())+":" +StrmConvert(Tseries[1].obstime().sec()) );
                        dbGate.insert( dwrite, "data", true); 
                        kvalobs::kvStationInfo::kvStationInfo DataToWrite(Tseries[1].stationID(),Tseries[1].obstime(),Tseries[1].typeID());
                        stList.push_back(DataToWrite);
                        NewCorrected=-99999.0;
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

            } // Three points to work with   
         } // Loopthrough all stations at the given time
      } // There is Qc2Data Loop
   ProcessTime.addHour(-1);
   } // TimeLoop
   return 0;
}
