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

class SQLDataAccess : public DBInterface {
public:

    virtual StationList findNorwegianFixedStations() throw (DBException);
    virtual StationIDList findNorwegianFixedStationIDs() throw (DBException);

    virtual StationParamList findStationParams(int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException);

    virtual DataList findDataOrderObstime(const StationIDList& stationIDs, int pid, const TimeRange& time, const FlagSetCU& flags) throw (DBException);
    virtual DataList findDataOrderObstime(int stationID, int paramID, const TimeRange& time) throw (DBException);
    virtual DataList findDataOrderObstime(int stationID, int paramID, int typeID, const TimeRange& time) throw (DBException);
    virtual DataList findDataOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t) throw (DBException);
    virtual DataList findDataMaybeTSLOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t, const FlagSetCU& flags) throw (DBException);
    virtual DataList findDataOrderObstime(const StationIDList& stationIDs, int paramID, const std::vector<int>& tids, const TimeRange& time) throw (DBException);
    virtual DataList findDataOrderObstime(int stationID, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags) throw (DBException);
    virtual DataList findDataOrderObstime(const StationIDList& stationIDs, int paramID, int typeID, const TimeRange& t, const FlagSetCU& flags) throw (DBException);
    virtual DataList findDataOrderStationObstime(const StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, const TimeRange& t, const FlagSetCU& flags) throw (DBException);

    virtual reference_value_map_t findStatisticalReferenceValues(int paramid, const std::string& key, float missingValue) throw (DBException);
    virtual CorrelatedNeighbors::neighbors_t findNeighborData(int stationid, int paramid) throw (DBException);

    virtual ModelDataList findModelData(int stationID, int paramID, int level, const TimeRange& time) throw (DBException);

    virtual void storeData(const DataList& toUpdate, const DataList& toInsert) throw (DBException);

protected:
    virtual StationList extractStations(const std::string& sql) throw (DBException) = 0;
    virtual StationIDList extractStationIDs(const std::string& sql) throw (DBException) = 0;
    virtual StationParamList extractStationParams(const std::string& sql) throw (DBException) = 0;
    virtual DataList extractData(const std::string& sql) throw (DBException) = 0;
    virtual reference_value_map_t extractStatisticalReferenceValues(const std::string& sql, float missingValue) throw (DBException) = 0;
    virtual CorrelatedNeighbors::neighbors_t extractNeighborData(const std::string& sql) throw (DBException) = 0;
    virtual ModelDataList extractModelData(const std::string& sql) throw (DBException) = 0;
    virtual void execSQLUpdate(const std::string& sql) throw (DBException) = 0;

private:
    virtual DataList findData(const StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags, bool orderByStation=false) throw (DBException);
};

#endif /* SQLDataAccess_h */
