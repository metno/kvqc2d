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

#include "KvalobsDB.h"

#include "DataAccessSQLQueries.h"
#include "foreach.h"

// ------------------------------------------------------------------------

DBInterface::StationList SQLDataAccess::findNorwegianFixedStations() throw (DBException)
{
    return extractStations(DataAccessSQLQueries::queryForNorwegianFixedStations());
}

// ------------------------------------------------------------------------

DBInterface::StationIDList SQLDataAccess::findNorwegianFixedStationIDs() throw (DBException)
{
    return extractStationIDs(DataAccessSQLQueries::queryForNorwegianFixedStationIDs());
}

// ------------------------------------------------------------------------

DBInterface::StationParamList SQLDataAccess::findStationParams(int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException)
{
    return extractStationParams(DataAccessSQLQueries::queryForStationParams(stationID, time, qcx));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderNone(const StationIDList& stationIDs, int pid, const TimeRange& time, const FlagSetCU& flags) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationIDs, pid, time, flags));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(int stationID, int paramID, const TimeRange& time) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationID, paramID, time));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(int stationID, int paramID, int typeID, const TimeRange& time) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationID, paramID, typeID, time));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationID, paramID, typeID, sensor, level, t));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataMaybeTSLOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t, const FlagSetCU& flags) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationID, paramID, typeID, sensor, level, t, flags));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(const StationIDList& stationIDs, int paramID, const std::vector<int>& tids, const TimeRange& time) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationIDs, paramID, tids, time));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(int stationID, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationID, pids, tids, sensor, level, time, flags));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(const StationIDList& stationIDs, int paramID, int typeID, const TimeRange& t, const FlagSetCU& flags) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationIDs, paramID, typeID, t, flags));
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderStationObstime(const StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, const TimeRange& t, const FlagSetCU& flags) throw (DBException)
{
    return extractData(DataAccessSQLQueries::queryForData(stationIDs, pids, tids, t, flags));
}

// ------------------------------------------------------------------------

DBInterface::reference_value_map_t SQLDataAccess::findStatisticalReferenceValues(int paramid, const std::string& key, float missingValue) throw (DBException)
{
    return extractStatisticalReferenceValues(DataAccessSQLQueries::queryForStatisticalReferenceValues(paramid, key), missingValue);
}

// ------------------------------------------------------------------------

CorrelatedNeighbors::neighbors_t SQLDataAccess::findNeighborData(int stationid, int paramid) throw (DBException)
{
    return extractNeighborData(DataAccessSQLQueries::queryForNeighborData(stationid, paramid));
}

// ------------------------------------------------------------------------

DBInterface::ModelDataList SQLDataAccess::findModelData(int stationid, int paramid, int level, const TimeRange& time) throw (DBException)
{
    return extractModelData(DataAccessSQLQueries::queryForModelData(stationid, paramid, level, time));
}

// ------------------------------------------------------------------------

void SQLDataAccess::storeData(const DataList& toUpdate, const DataList& toInsert) throw (DBException)
{
    if( toUpdate.empty() && toInsert.empty() )
        return;
    std::ostringstream sql;
    if( (toUpdate.size() + toInsert.size()) > 1 )
        sql << "BEGIN; ";
    foreach(const kvalobs::kvData& i, toInsert)
        sql << "INSERT INTO " << i.tableName() << " VALUES" << i.toSend() << "; ";
    foreach(const kvalobs::kvData& u, toUpdate)
        sql << "UPDATE " << u.tableName() << " " << u.toUpdate() << "; ";
    if( (toUpdate.size() + toInsert.size()) > 1 )
        sql << "COMMIT; " << std::endl;
    execSQLUpdate(sql.str());
}
