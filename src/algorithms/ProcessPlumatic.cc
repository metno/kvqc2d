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

#include "ProcessPlumatic.h"

#include "AlgorithmHelpers.h"
#include "Helpers.h"
#include "Plumatic.h"
#include "ReadProgramOptions.h"
#include "scone.h"

#include <milog/milog.h>
#include <puTools/miTime.h>
#include "foreach.h"

void PlumaticAlgorithm::run(const ReadProgramOptions& params)
{
    const int pid = params.getParameter<int>("ParamId");

    LOGINFO("Plumatic Control");

    std::list<kvalobs::kvStation> StationList;
    std::list<int> StationIds;
    fillStationLists(StationList, StationIds);

    foreach(const kvalobs::kvStation& station, StationList) {
        const miutil::miString ladle = "WHERE STATIONID="+StrmConvert(station.stationID())
            +" AND PARAMID="+StrmConvert(pid)
            +" AND obstime BETWEEN \'"+params.UT0.isoTime()+"\' AND \'"+params.UT1.isoTime()+"\'";
        std::list<kvalobs::kvData> PluviData;
        database()->selectData(PluviData, ladle);
        if( PluviData.empty() )
            continue;

        Plumatic PL(PluviData, params);
        std::cout << " --------------- " << std::endl;
        std::list<miutil::miTime> TimeList;
        PL.aggregate_window(params, TimeList);
        foreach( const kvalobs::kvData& data, PluviData ) {
            const std::list<miutil::miTime>::iterator iTime = std::find(TimeList.begin(), TimeList.end(), data.obstime());
            if( iTime != TimeList.end() ) {
                FlagChange fc;
                params.getFlagChange(fc, "plumatic_flagchange");

                kvalobs::kvData dwrite(data);
                dwrite.controlinfo(fc.apply(dwrite.controlinfo()));
                Helpers::updateCfailed(dwrite, "PLX", params.CFAILED_STRING);
                Helpers::updateUseInfo(dwrite);

                updateData(dwrite);
            }
        }
    }
}
