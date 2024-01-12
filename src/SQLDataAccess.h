/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

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

#ifndef SQLDataAccess_h
#define SQLDataAccess_h

#include "DBInterface.h"
#include <iosfwd>

class SQLDataAccess : public DBInterface {
public:

    StationList findFixedStations() override;
    StationIDList findFixedStationIDs() override;

    StationParamList findStationParams(int stationID, const kvtime::time& time, const std::string& qcx) override;
    StationParamList findStationParams(const StationIDList& stationIDs, const std::vector<int>& pids, const std::string& qcxPrefix) override;

    DataList findDataOrderObstime(const StationIDList& stationIDs, int pid, const TimeRange& time, const FlagSetCU& flags) override;
    DataList findDataOrderObstime(int stationID, int paramID, const TimeRange& time) override;
    DataList findDataOrderObstime(int stationID, int paramID, int typeID, const TimeRange& time) override;
    DataList findDataOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t) override;
    DataList findDataMaybeTSLOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t, const FlagSetCU& flags) override;
    DataList findDataOrderObstime(int stationID, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags) override;
    DataList findDataOrderObstime(const StationIDList& stationIDs, int paramID, int typeID, const TimeRange& t, const FlagSetCU& flags) override;
    DataList findDataOrderStationObstime(const StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, const TimeRange& t, const FlagSetCU& flags) override;
    DataList findDataAggregations(const StationIDList& stationIDs, const std::vector<int>& pids, const TimeRange& t, const FlagSetCU& flags) override;

    reference_value_map_t findStatisticalReferenceValues(int paramid, const std::string& key, float missingValue) override;
    NeighborDataVector findNeighborData(int stationid, int paramid, float maxsigma) override;

    ModelDataList findModelData(int stationID, int paramID, int level, const TimeRange& time) override;

    void storeData(const DataList& toUpdate, const DataList& toInsert) override;

protected:
    virtual StationList extractStations(const std::string& sql) = 0;
    virtual StationIDList extractStationIDs(const std::string& sql) = 0;
    virtual StationParamList extractStationParams(const std::string& sql) = 0;
    virtual DataList extractData(const std::string& sql) = 0;
    virtual reference_value_map_t extractStatisticalReferenceValues(const std::string& sql, float missingValue) = 0;
    virtual NeighborDataVector extractNeighborData(const std::string& sql) = 0;
    virtual ModelDataList extractModelData(const std::string& sql) = 0;
    virtual void execSQLUpdate(const std::string& sql) = 0;

    void formatStationIDList(std::ostream& sql, const StationIDList& stationIDs);

private:
    DataList findData(const StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags, bool orderByStation=false);
};

#endif /* SQLDataAccess_h */
