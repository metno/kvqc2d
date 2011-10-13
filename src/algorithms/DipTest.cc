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

#include "DipTestAlgorithm.h"

#include "AkimaSpline.h"
#include "AlgorithmHelpers.h"
#include "GetStationParam.h"
#include "Helpers.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "ParseParValFile.h"
#include "ProcessControl.h"
#include "ReadProgramOptions.h"
#include "scone.h"
#include "tround.h"

#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>
#include <puTools/miTime.h>
#include "foreach.h"

float DipTestAlgorithm::getDeltaCheck(int stationID, const miutil::miTime& time, const std::string& qcx, bool max)
{
    DBInterface::kvStationParamList_t splist;
    if( !database()->queryStationparams(splist, stationID, time, qcx ) ) {
        LOGERROR("Failed to select station_params for qcx='" << qcx << "'");
        return -1e6;
    }

    return GetStationParam(splist).ValueOf(max ? "max" : "min").toFloat();
}

void DipTestAlgorithm::run(const ReadProgramOptions& params)
{
    LOGINFO("Dip Test");
 
    ProcessControl CheckFlags;

    if( params.ParValFile == "NotSet")
        return;
    ParseParValFile ParValues(params.ParValFile);
    const std::map<int, float>  PidValMap = ParValues.ReturnMap();

    std::list<int> StationIds;
    fillStationIDList(StationIds);

    for (std::map<int, float>::const_iterator it=PidValMap.begin(); it!=PidValMap.end(); ++it) {
        std::cout << (*it).first << " " << (*it).second << std::endl;
        const float pid=(*it).first;
        const float delta=(*it).second;
        std::cout << pid << " " << delta << std::endl;
        std::cout << "------------------" << std::endl;

        const float DeltaCheck = getDeltaCheck(0, miutil::miTime::nowTime(), "QC1-3a-"+StrmConvert(pid), true); /// FIXME what time to use here?
        std::cout << "Delta automatically read from the database (under test!!!):  " << DeltaCheck << std::endl;

        for(miutil::miTime ProcessTime = params.UT1; ProcessTime >= params.UT0; ProcessTime.addHour(-1)) {
            miutil::miTime linearStart = ProcessTime, linearStop = ProcessTime;
            linearStart.addHour(-1);
            linearStop.addHour(1);
            miutil::miTime akimaStart = ProcessTime, akimaStop = ProcessTime;
            akimaStart.addHour(-3);
            akimaStop.addHour(2);

            /// Select all data for a given stationlist over three hours (T-1),(T),(T+1)
            //********* Change this to select the appropriate data
            std::list<kvalobs::kvData> Qc2Data;
            if( !database()->dataForStationsParamTimerange(Qc2Data, StationIds, pid, ProcessTime, ProcessTime) ) {
                LOGERROR("Failed to select data for DipTest at t=" << ProcessTime);
                continue;
            }
            if( Qc2Data.empty() )
                continue;

            foreach( const kvalobs::kvData& d, Qc2Data) {
                std::list<kvalobs::kvData> Qc2SeriesData;
                if( !database()->dataForStationParamTimerange(Qc2SeriesData, d.stationID(), pid, linearStart, linearStop) ) {
                    LOGERROR("Could not select time neighbors for DipTest " << linearStart << ".." << linearStop);
                    continue;
                }

                const std::vector<kvalobs::kvData> Tseries(Qc2SeriesData.begin(), Qc2SeriesData.end());

                // Check that we have three valid data points and no mixing of typeids
                if( Tseries.size()!= 3 || Helpers::checkContinuousHourAndSameTypeID(Tseries)
                        || Tseries[0].controlinfo().flag(3) != 1
                        || Tseries[1].controlinfo().flag(3) != 2
                        || Tseries[2].controlinfo().flag(3) != 2 )
                    continue;

                //            x
                //
                //      x           x     -> time
                //
                //     A(0)  A(1)  A(2)
                //
                const float ABS20 = fabs( Tseries[2].original()-Tseries[0].original() );
                const float ABS10 = fabs( Tseries[1].original()-Tseries[0].original() );
                const float ABS21 = fabs( Tseries[2].original()-Tseries[1].original() );

                if( ABS20 < ABS10 && ABS20 < delta ) {
                    const float LinInterpolated = round<float,1>( 0.5*(Tseries[0].original()+Tseries[2].original()) );
                    float AkimaInterpolated = LinInterpolated;
                    bool AkimaPresent = false;

                    // Now see if we can also do Akima
                    // Just need some extra points

                    // Calculate Akima Block
                    if( database()->dataForStationParamTimerange(Qc2SeriesData, d.stationID(),pid, akimaStart, akimaStop) ) {
                        const std::vector<kvalobs::kvData> Aseries(Qc2SeriesData.begin(), Qc2SeriesData.end());
                        // A(0)
                        // A(1)
                        // A(2) = T(0)
                        // A(3) = T(1)
                        // A(4) = T(2)
                        // A(5)
                        std::cout << "ASeries Size: " << Aseries.size() << std::endl;
                        if( Aseries.size()==6                                         &&
                            Aseries[3].obstime()==Tseries[1].obstime()           	  &&
                            Aseries[3].stationID()==Tseries[1].stationID()            &&
                            Aseries[3].paramID()==Tseries[1].paramID()           	  &&
                            Helpers::checkContinuousHourAndSameTypeID(Aseries)        &&
                            CheckFlags.condition(Aseries[0].useinfo(),params.Uflag)   &&
                            CheckFlags.condition(Aseries[1].useinfo(),params.Uflag)   &&
                            CheckFlags.condition(Aseries[5].useinfo(),params.Uflag)   &&
                            Aseries[0].original() > params.missing                    &&
                            Aseries[1].original() > params.missing                    &&
                            Aseries[2].original() > params.missing                    &&
                            Aseries[4].original() > params.missing                    &&
                            Aseries[5].original() > params.missing                    &&
                            Aseries[3].original() > params.missing )
                        {
                            std::vector<double> xt,yt;
                            for( int i=0;i<6;i++ ) {
                                if( i != 3 ) {
                                    xt.push_back(i*1.0);
                                    yt.push_back( Aseries[i].original() );
                                }
                            }
                            AkimaSpline AkimaX(xt,yt);
                            AkimaInterpolated=round<float,1>( AkimaX.AkimaPoint(3.0) );
                            AkimaPresent = true;
                        }
                    }

                    if( AkimaPresent ) {
                        // FIXME Looking into providing minimum check
                        // FIXME what time to use here?
                        const float MinimumCheck = getDeltaCheck(d.stationID(), miutil::miTime::nowTime(), "QC1-1-"+StrmConvert(pid), false);
                        if( MinimumCheck == -1e6 ) {
                            LOGERROR("Failed to select data for MinimumCheck at " << ProcessTime << ". Assuming no akima interpolation.");
                            AkimaPresent = false;
                        } else if( AkimaInterpolated < MinimumCheck )
                            AkimaPresent = false;
                    }

                    if( CheckFlags.true_nibble(d.controlinfo(),params.Wflag,15,params.Wbool) ) {  // check for HQC action already

                        kvalobs::kvControlInfo fixflags1 = Tseries[1].controlinfo(); // later control this from the config file
                        fixflags1.set(3,9); // later control this from the config file

                        kvalobs::kvControlInfo fixflags2 = Tseries[1].controlinfo(); // later control this from the config file
                        fixflags2.set(3,4); // later control this from the config file

                        const float Interpolated = AkimaPresent ? AkimaInterpolated : LinInterpolated;

                        kvalobs::kvData dwrite1(Tseries[1]);
                        dwrite1.corrected(Interpolated);
                        dwrite1.controlinfo(fixflags1);
                        Helpers::updateCfailed(dwrite1, AkimaPresent ? "QC2d-1-A" : "QC2d-1-L", params.CFAILED_STRING);
                        Helpers::updateUseInfo(dwrite1);

                        kvalobs::kvData dwrite2(Tseries[2]);
                        dwrite2.controlinfo(fixflags2);
                        Helpers::updateCfailed(dwrite2, "QC2d-1", params.CFAILED_STRING);
                        Helpers::updateUseInfo(dwrite2);

                        std::list<kvalobs::kvData> toWrite;
                        toWrite.push_back(dwrite1);
                        toWrite.push_back(dwrite2);
                        database()->insert( toWrite, true, "data" );

                        broadcaster()->queueChanged(Tseries[1]);
                        broadcaster()->queueChanged(Tseries[2]);
                        broadcaster()->sendChanges();

                        LOGINFO("DipTest: "+Helpers::kvqc2logstring(dwrite1) );
                        LOGINFO("DipTest: "+Helpers::kvqc2logstring(dwrite2) );
                    }
                }
            } // Loopthrough all stations at the given time
        } // TimeLoop
    } // parameter loop
}
