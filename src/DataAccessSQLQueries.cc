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

#include "DataAccessSQLQueries.h"

#include "FlagPatterns.h"
#include "foreach.h"

#include <kvalobs/kvQueries.h>
#include <kvalobs/kvStation.h>
#include <sstream>

namespace DataAccessSQLQueries {

std::string queryForNorwegianFixedStations() throw (DBException)
{
    return kvalobs::kvStation().selectAllQuery()
        + " WHERE stationid BETWEEN 60 AND 99999 AND maxspeed = 0";
}

// ----------------------------------------

std::string queryForNorwegianFixedStationIDs() throw (DBException)
{
    return "SELECT stationid FROM station"
        " WHERE stationid BETWEEN 60 AND 99999 AND maxspeed = 0";
}

// ------------------------------------------------------------------------

std::string queryForStationParams(int stationID, const miutil::miTime& time, const std::string& qcx)
{
    const std::list<int> station(1, stationID);
    return kvalobs::kvStationParam().selectAllQuery()
        + kvQueries::selectStationParam(station, time, qcx);
}

// ----------------------------------------

namespace {
template<class CC>
void formatIDList(std::ostream& sql, const CC& ids, const std::string& column)
{
    sql << ' ';
    if( ids.empty() ) {
        sql << "0 = 1";
    } else if( ids.size() == 1 ) {
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

std::string queryForData(const DBInterface::StationIDList& stationIDs, int paramID, const TimeRange& time, const FlagSetCU& flags)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery() + " WHERE ";
    formatIDList(sql, stationIDs, "stationid");
    sql << " AND paramid = " << paramID
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " AND " << flags.sql();
    return sql.str();
}

// ------------------------------------------------------------------------

std::string queryForData(int stationID, int paramID, const TimeRange& time)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery()
        << " WHERE stationid = " << stationID
        << " AND paramid = " << paramID
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " ORDER BY obstime";
    return sql.str();
}

// ------------------------------------------------------------------------

std::string queryForData(int stationID, int paramID, int typeID, const TimeRange& time)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery()
        << " WHERE stationid = " << stationID
        << " AND paramid = " << paramID
        << " AND typeid = " << typeID
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " ORDER BY obstime";
    return sql.str();
}

// ------------------------------------------------------------------------

std::string queryForData(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& time)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery()
        << " WHERE stationid = " << stationID
        << " AND paramid = " << paramID
        << " AND typeid = " << typeID
        << " AND sensor = '" << sensor << '\''
        << " AND level = " << level
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " ORDER BY obstime";
    return sql.str();
}

// ------------------------------------------------------------------------

std::string queryForData(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& time, const FlagSetCU& flags)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery()
        << " WHERE stationid = " << stationID
        << " AND paramid = " << paramID;
    if( typeID > 0 )
        sql << " AND typeid = " << typeID;
    if( sensor >= 0 )
        sql << " AND sensor = '" << sensor << '\'';
    if( level >= 0 )
        sql << " AND level = " << level;
    sql << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " AND " << flags.sql()
        << " ORDER BY obstime";
    return sql.str();
}

// ------------------------------------------------------------------------

std::string queryForData(const DBInterface::StationIDList& stationIDs, int paramID, const std::vector<int>& tids, const TimeRange& time)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery() << " WHERE ";
    formatIDList(sql, stationIDs, "stationid");
    sql << " AND paramid = " << paramID << " AND ";
    formatIDList(sql, tids, "typeid");
    sql << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " ORDER BY obstime";
    return sql.str();
}

// ------------------------------------------------------------------------

std::string queryForData(int stationID, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery()
        << " WHERE stationid = " << stationID << " AND ";
    formatIDList(sql, pids, "paramid");
    sql << " AND ";
    formatIDList(sql, tids, "typeid");
    sql << " AND sensor = '" << sensor << '\''
        << " AND level = " << level
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " AND " << flags.sql()
        << " ORDER BY obstime";
    return sql.str();
}

// ------------------------------------------------------------------------

std::string queryForData(const DBInterface::StationIDList& stationIDs, int paramID, int typeID, const TimeRange& time, const FlagSetCU& flags)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery() + " WHERE ";
    formatIDList(sql, stationIDs, "stationid");
    sql << " AND paramid = " << paramID
        << " AND typeid = " << typeID
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " AND " << flags.sql()
        << " ORDER BY obstime";
    return sql.str();
}

// ------------------------------------------------------------------------

std::string queryForData(const DBInterface::StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, const TimeRange& time, const FlagSetCU& flags)
{
    std::ostringstream sql;
    sql << kvalobs::kvData().selectAllQuery() + " WHERE ";
    formatIDList(sql, stationIDs, "stationid");
    sql << " AND ";
    formatIDList(sql, pids, "paramid");
    sql << " AND ";
    formatIDList(sql, tids, "typeid");
    sql << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'"
        << " AND " << flags.sql()
        << " ORDER BY stationid, obstime";
    return sql.str();
}

// ----------------------------------------

std::string queryForStatisticalReferenceValues(int paramID, const std::string& key)
{
    std::ostringstream sql;
    sql << "SELECT stationid, day_of_year, value FROM statistical_reference_values"
        << " WHERE paramid = " << paramID << " AND key = '" << key << "'";
    return sql.str();
}

// ----------------------------------------

std::string queryForNeighborData(int stationid, int paramid)
{
    std::ostringstream sql;
    sql << "SELECT neighborid, offset, slope, sigma FROM interpolation_best_neighbors"
        << " WHERE stationid = " << stationid << " AND paramid = " << paramid;
    return sql.str();
}

// ----------------------------------------

std::string queryForModelData(int stationID, int paramID, int level, const TimeRange& time)
{
    std::ostringstream sql;
    sql << kvalobs::kvModelData().selectAllQuery()
        << " WHERE stationid = " << stationID
        << " AND paramid = " << paramID
        << " AND level = " << level
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'";
    return sql.str();
}

// ------------------------------------------------------------------------


} // namespace DataAccessSQLQueries
