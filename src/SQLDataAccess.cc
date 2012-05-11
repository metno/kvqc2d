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

#include "KvalobsDB.h"

#include "FlagPatterns.h"
#include "foreach.h"

#include <kvalobs/kvQueries.h>
#include <kvalobs/kvStation.h>
#include <sstream>

#define NDEBUG
#include "debug.h"

namespace {

template<class CC>
void formatIDList(std::ostream& sql, const CC& ids, const std::string& column)
{
    sql << ' ';
    if( ids.empty() ) {
        sql << "0 = 1";
    } else if( ids.size() == 1 ) {
        if( ids.front() == DBInterface::INVALID_ID )
            sql << "0 = 0";
        else
            sql << column << " = " << ids.front();
    } else {
        sql << column << " IN ";
        char sep = '(';
        foreach(int ii, ids) {
            sql << sep << ii;
            sep = ',';
        }
        sql << ')';
    }
}
} // anonymous namespace

// ------------------------------------------------------------------------

DBInterface::StationList SQLDataAccess::findNorwegianFixedStations() throw (DBException)
{
    return extractStations(kvalobs::kvStation().selectAllQuery()
                           + " WHERE stationid BETWEEN 60 AND 99999 AND maxspeed = 0");
}

// ------------------------------------------------------------------------

DBInterface::StationIDList SQLDataAccess::findNorwegianFixedStationIDs() throw (DBException)
{
    return extractStationIDs("SELECT stationid FROM station"
                             " WHERE stationid BETWEEN 60 AND 99999 AND maxspeed = 0");
}

// ------------------------------------------------------------------------

DBInterface::StationParamList SQLDataAccess::findStationParams(int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException)
{
    const std::list<int> station(1, stationID);
    return extractStationParams(kvalobs::kvStationParam().selectAllQuery()
                                + kvQueries::selectStationParam(station, time, qcx));
}

DBInterface::StationParamList SQLDataAccess::findStationParams(const StationIDList& stationIDs, const std::vector<int>& pids, const std::string& qcxPrefix) throw (DBException)
{
    std::ostringstream sql;
    sql << kvalobs::kvStationParam().selectAllQuery() << " WHERE ";
    formatStationIDList(sql, stationIDs);
    sql << " AND ";
    formatIDList(sql, pids, "paramid");
    sql << " AND qcx LIKE '" << qcxPrefix << "%'";
    return extractStationParams(sql.str());
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(const StationIDList& stationIDs, int paramID, const TimeRange& time, const FlagSetCU& flags) throw (DBException)
{
    const std::vector<int> pids(1, paramID), tids(1, DBInterface::INVALID_ID);
    return findData(stationIDs, pids, tids, INVALID_ID, INVALID_ID, time, flags);
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(int stationID, int paramID, const TimeRange& time) throw (DBException)
{
    const StationIDList sids(1, stationID);
    return findDataOrderObstime(sids, paramID, time, FlagSetCU());
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(int stationID, int paramID, int typeID, const TimeRange& time) throw (DBException)
{
    return findDataMaybeTSLOrderObstime(stationID, paramID, typeID, INVALID_ID, INVALID_ID, time, FlagSetCU());
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& time) throw (DBException)
{
    return findDataMaybeTSLOrderObstime(stationID, paramID, typeID, sensor, level, time, FlagSetCU());
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataMaybeTSLOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& time, const FlagSetCU& flags) throw (DBException)
{
    const StationIDList sids(1, stationID);
    const std::vector<int> pids(1, paramID), tids(1, typeID);
    return findData(sids, pids, tids, sensor, level, time, flags, false);
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(const StationIDList& stationIDs, int paramID, const std::vector<int>& tids, const TimeRange& time) throw (DBException)
{
    const std::vector<int> pids(1, paramID);
    return findData(stationIDs, pids, tids, INVALID_ID, INVALID_ID, time, FlagSetCU(), false);
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(int stationID, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags) throw (DBException)
{
    const StationIDList sids(1, stationID);
    return findData(sids, pids, tids, sensor, level, time, flags, false);
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderObstime(const StationIDList& stationIDs, int paramID, int typeID, const TimeRange& time, const FlagSetCU& flags) throw (DBException)
{
    const std::vector<int> pids(1, paramID), tids(1, typeID);
    return findData(stationIDs, pids, tids, INVALID_ID, INVALID_ID, time, flags, false);
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataOrderStationObstime(const StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, const TimeRange& time, const FlagSetCU& flags) throw (DBException)
{
    return findData(stationIDs, pids, tids, INVALID_ID, INVALID_ID, time, flags, true);
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findData(const StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags, bool orderByStation) throw (DBException)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery() + " WHERE ";
    formatStationIDList(sql, stationIDs);
    sql << " AND ";
    formatIDList(sql, pids, "paramid");
    sql << " AND ";
    formatIDList(sql, tids, "typeid");
    sql << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " AND " << flags.sql();
    if( sensor != INVALID_ID )
        sql << " AND sensor = '" << sensor << '\'';
    if( level != INVALID_ID )
        sql << " AND level = " << level;
    sql << " ORDER BY ";
    if( orderByStation )
        sql << "stationid, ";
    sql << "obstime";
    return extractData(sql.str());
}

// ------------------------------------------------------------------------

DBInterface::DataList SQLDataAccess::findDataAggregations(const StationIDList& stationIDs, const std::vector<int>& pids, const TimeRange& time, const FlagSetCU& flags) throw (DBException)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery() + " WHERE ";
    formatStationIDList(sql, stationIDs);
    sql << " AND ";
    formatIDList(sql, pids, "paramid");
    sql << " AND typeid < 0"
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " AND " << flags.sql()
        << " ORDER BY stationid, obstime";
    return extractData(sql.str());
}

// ------------------------------------------------------------------------

DBInterface::reference_value_map_t SQLDataAccess::findStatisticalReferenceValues(int paramID, const std::string& key, float missingValue) throw (DBException)
{
    std::ostringstream sql;
    sql << "SELECT stationid, day_of_year, value FROM statistical_reference_values"
        << " WHERE paramid = " << paramID << " AND key = '" << key << "'";
    return extractStatisticalReferenceValues(sql.str(), missingValue);
}

// ------------------------------------------------------------------------

NeighborDataVector SQLDataAccess::findNeighborData(int stationid, int paramid, float maxsigma) throw (DBException)
{
    std::ostringstream sql;
    sql << "SELECT neighborid, fit_offset, fit_slope, fit_sigma FROM interpolation_best_neighbors"
        << " WHERE stationid = " << stationid
        << " AND paramid = " << paramid
        << " AND interpolation_id = 0"
        << " AND fit_sigma < " << maxsigma;
    return extractNeighborData(sql.str());
}

// ------------------------------------------------------------------------

DBInterface::ModelDataList SQLDataAccess::findModelData(int stationID, int paramID, int level, const TimeRange& time) throw (DBException)
{
    std::ostringstream sql;
    sql << kvalobs::kvModelData().selectAllQuery()
        << " WHERE stationid = " << stationID
        << " AND paramid = " << paramID
        << " AND level = " << level
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'";
    return extractModelData(sql.str());
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

// ------------------------------------------------------------------------

void SQLDataAccess::formatStationIDList(std::ostream& sql, const StationIDList& stationIDs)
{
    if( stationIDs.size() == 1 and stationIDs.front() == ALL_STATIONS ) {
        // no constraint on stationid by default
        sql << " 0=0 ";
    } else {
        formatIDList(sql, stationIDs, "stationid");
    }
}
