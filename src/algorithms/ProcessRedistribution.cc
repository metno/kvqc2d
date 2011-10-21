/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011 met.no

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

#include "ProcessRedistribution.h"

#include "AlgorithmHelpers.h"
#include "Helpers.h"
#include "ProcessControl.h"
#include "ProcessImpl.h"
#include "Qc2D.h"
#include "ReadProgramOptions.h"

#include <milog/milog.h>
#include <puTools/miTime.h>
#include "foreach.h"

void RedistributionAlgorithm::run(const ReadProgramOptions& params)
{
    LOGINFO("Redistribute Accumulations");

    std::list<kvalobs::kvStation> StationList;
    std::list<int> StationIds;
    fillStationLists(StationList, StationIds);

    for(miutil::miTime ProcessTime = params.UT0; ProcessTime <= params.UT1; ProcessTime.addDay(1)) {

        std::list<kvalobs::kvData> Qc2Data;
        if( !database()->dataForStationsParamTimerange(Qc2Data, StationIds, params.pid, ProcessTime, ProcessTime) ) {
            LOGERROR("Problem with query in ProcessRedistribution");
            continue;
        }
        if( Qc2Data.empty() ) {
            continue;
        }

        Qc2D GSW(Qc2Data, StationList, params, "Generate Missing Rows");
        GSW.Qc2_interp();
        std::list<kvalobs::kvData> ReturnData;
        GSW.distributor(ReturnData, 0);
        //GSW.write_cdf(StationList);

        //std::cout << "Not Empty" << std::endl;
        foreach(const kvalobs::kvData& d, ReturnData) {
            //LOGINFO("---------------->: "+ kvqc2logstring(*id) );
            miutil::miTime PreviousCheck = d.obstime();
            PreviousCheck.addDay(-1); // FIXME this should be Step_DD from the config file
            std::list<kvalobs::kvData> CheckData;
            if( !database()->dataForStationParamTimerange(CheckData, d.stationID(), params.pid, PreviousCheck, PreviousCheck) ) {
                LOGERROR("Problem with station data query in ProcessRedistribution");
                continue;
            }
            int ignore_station = 0;
            foreach(const kvalobs::kvData& c, CheckData) {
                if (c.corrected()==params.missing) {
                    ignore_station = c.stationID();
                    LOGWARN("Incomplete redistribution (skipped): "+ Helpers::kvqc2logstring(c) );
                }
            }
            if( checkFlags().true_nibble(d.controlinfo(), params.Wflag, params.nibble_index, params.Wbool)
                && d.stationID() != ignore_station )
            {
                LOGINFO("Redistribution: "+Helpers::kvqc2logstring(d) );

                kvalobs::kvData dwrite(d);
                Helpers::updateUseInfo(dwrite);
                if( !database()->insertData(dwrite, true) ) {
                    LOGERROR("Could not write to database");
                    continue;
                }

                broadcaster()->queueChanged(d);
                broadcaster()->sendChanges();
            }
        }
        ReturnData.clear();

    }
    std::list<kvalobs::kvData> Qc2Data;
    std::list<kvalobs::kvData> ReturnData;
    Qc2D GSW(Qc2Data, StationList, params);
    GSW.distributor(ReturnData, 1); /// solution for memory cleanup ... maybe needs to be improved.
}
