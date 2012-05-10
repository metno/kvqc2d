/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011 met.no

  Contact information:
  Norwegian Meteorological Institute
  Postboks 43 Blindern
  N-0313 OSLO
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

#include "AggregatorLimits.h"
#include "DataUpdate.h"
#include "GetStationParam.h"
#include "foreach.h"

#define NDEBUG 1
#include "debug.h"

AggregatorLimits::AggregatorLimits()
    : Qc2Algorithm("AggregatorLimits")
{
}

void AggregatorLimits::configure(const AlgorithmConfig& config)
{
    mParameters = config.getMultiParameter<int>("ParamID");
    mMinQCX = config.getParameter<std::string>("minQCX", "min");
    mMaxQCX = config.getParameter<std::string>("maxQCX", "max");
    config.getFlagSetCU(mFlags, "aggregation", "fr=)6(", "");
    config.getFlagChange(mFlagChangeMin, "aggregation_flagchange_min", "fr=6");
    config.getFlagChange(mFlagChangeMax, "aggregation_flagchange_max", "fr=6");

    Qc2Algorithm::configure(config);
}

void AggregatorLimits::run()
{
    const miutil::miTime now = miutil::miTime::nowTime();
    const DBInterface::StationIDList allStations(1, DBInterface::ALL_STATIONS);

    DBInterface::DataList updates;
    foreach(const int paramid, mParameters) {
        std::ostringstream qcx;
        qcx << "QC1-1-" << paramid;
        const DBInterface::StationParamList spl = database()->findStationParams(0, now, qcx.str());

        GetStationParam gsp(spl);
        const float min = std::atof(gsp.ValueOf(mMinQCX).c_str()), max = std::atof(gsp.ValueOf(mMaxQCX).c_str());

        const std::vector<int> pids(1, paramid);
        const DBInterface::DataList outOfRange = database()->findAggregationOutsideRange(allStations, pids, TimeRange(UT0, UT1), mFlags, min, max);
        DBG(DBG1(paramid) << DBG1(mMinQCX) << DBG1(min) << DBG1(max) DBG1(outOfRange.size()));

        foreach(const kvalobs::kvData& data, outOfRange) {
            DataUpdate du(data);
            DBGV(data);
            if( du.original() < min ) {
                du.controlinfo(mFlagChangeMin.apply(du.controlinfo()));
                du.cfailed("QC2-agglim-min");
            } else if( du.original() > max ) {
                du.controlinfo(mFlagChangeMax.apply(du.controlinfo()));
                du.cfailed("QC2-agglim-max");
            } else {
                DBGL;
                continue;
            }

            updates.push_back(du.data());
        }
    }
    const DBInterface::DataList inserts; // empty
    storeData(updates, inserts);
}
