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

#include "GapInterpolationAlgorithm.h"

#include "AkimaSpline.h"
#include "CheckedDataCommandBase.h"
#include "CheckedDataHelper.h"
#include "BasicStatistics.h"
#include "Helpers.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "Qc2D.h"
#include "ProcessControl.h"
#include "ProcessImpl.h"
#include "ReadProgramOptions.h"
#include "scone.h"
#include "tround.h"

#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>
#include <math.h>
#include <memory>
#include <stdexcept>
#include <sstream>

//GNU Statistical library
//#include <gsl/gsl_errno.h>
//#include <gsl/gsl_spline.h>

using namespace kvalobs;
using namespace std;
using namespace miutil;

void GapInterpolationAlgorithm::run( const ReadProgramOptions& params )
{
    int pid=params.pid;
    int tid=params.tid;
    miutil::miTime stime=params.UT0;
    miutil::miTime etime=params.UT1;
    miutil::miDate PDate;
    miutil::miString ladle;
    //int ngap=parama.ngaps;
    int ngap=params.Ngap;
    std::cout << " ############### " << ngap << std::endl;
    double JulDec;
    long StartDay;
    double HourDec;
    double lowHour;
    double highHour;

    ProcessControl CheckFlags;
    kvalobs::kvControlInfo fixflags;
    kvalobs::kvData dwrite;                                                   
    miutil::miString new_cfailed;
    float NewCorrected;

    PDate.setDate(stime.year(),stime.month(),stime.day());
    StartDay=PDate.julianDay();

    std::vector<double> xt,yt;           // For holding only good data
    std::vector<double> xat,yat;         // For holiding all the data series (missing values as well as good)

    std::list<kvalobs::kvData> Qc2SeriesData;
    std::list<kvalobs::kvData> ReturnData;
    bool result;

    kvalobs::kvStationInfoList  stList;
    CheckedDataHelper checkedDataHelper(dispatcher()->getApp());

    kvalobs::kvDbGate dbGate( &dispatcher()->getConnection() );



    std::vector<kvalobs::kvData> Tseries;

    std::list<kvalobs::kvStation> StationList;
    std::list<int> StationIds;
    fillStationLists(StationList, StationIds);

    /// LOOP THROUGH STATIONS
    for (std::list<kvalobs::kvStation>::const_iterator sit=StationList.begin(); sit!=StationList.end(); ++ sit) {
        //std::cout << sit->stationID() << std::endl;
        try {
            ladle="WHERE STATIONID="+StrmConvert(sit->stationID())+" AND PARAMID="+StrmConvert(pid)+" AND TYPEID="+StrmConvert(tid)+" AND level=0 AND sensor=\'0\' AND obstime BETWEEN \'"+stime.isoTime()+"\' AND \'"+etime.isoTime()+"\'";
            //std::cout << ladle << std::endl;
            result = dbGate.select(Qc2SeriesData, ladle);
            //result = dbGate.select(Qc2SeriesData, kvQueries::selectData(sit->stationID(),pid,tid,stime,etime));
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
                        NewCorrected=round<float,1>(AkimaX.AkimaPoint(HourDec));
                        // Push the data back     
                        fixflags=id->controlinfo();
                        CheckFlags.setter(fixflags,params.Sflag);
                        CheckFlags.conditional_setter(fixflags,params.chflag);
                        new_cfailed=id->cfailed();
                        if (new_cfailed.length() > 0) new_cfailed += ",";
                        //new_cfailed += "QC2d-2";
                        new_cfailed += "QC2d-2-A";
                        if (params.CFAILED_STRING.length() > 0) new_cfailed += ","+params.CFAILED_STRING;

                        dwrite.clean();
                        dwrite.set(id->stationID(),id->obstime(),id->original(),id->paramID(),id->tbtime(),
                                   id->typeID(),id->sensor(), id->level(),NewCorrected,fixflags,id->useinfo(),
                                   new_cfailed );
                        kvUseInfo ui = dwrite.useinfo();
                        ui.setUseFlags( dwrite.controlinfo() );
                        dwrite.useinfo( ui );   
                        LOGINFO("Long Akima: "+Helpers::kvqc2logstring(dwrite) );
                        dbGate.insert( dwrite, "data", true); 
                        kvalobs::kvStationInfo::kvStationInfo DataToWrite(id->stationID(),id->obstime(),id->typeID());
                        stList.push_back(DataToWrite);
                    }

                } else {
                    //std::cout << id->obstime() << " " << id->original() << " " << id->corrected() << " --------- " << std::endl;
                }
            }
        } // end of IF enough points for AKima
    }  
}
