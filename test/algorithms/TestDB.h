/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2012 met.no

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

#ifndef MEMORYTESTDB_H
#define MEMORYTESTDB_H

#include "SQLDataAccess.h"

#include <sqlite3.h>

class SqliteTestDB: public SQLDataAccess {
public:
    SqliteTestDB();
    ~SqliteTestDB();

public:
    virtual StationList extractStations(const std::string& sql) throw (DBException);
    virtual StationIDList extractStationIDs(const std::string& sql) throw (DBException);
    virtual StationParamList extractStationParams(const std::string& sql) throw (DBException);
    virtual DataList extractData(const std::string& sql) throw (DBException);
    virtual reference_value_map_t extractStatisticalReferenceValues(const std::string& sql, float missingValue) throw (DBException);
    virtual NeighborDataVector extractNeighborData(const std::string& sql) throw (DBException);
    virtual ModelDataList extractModelData(const std::string& sql) throw (DBException);
    virtual void execSQLUpdate(const std::string& sql) throw (DBException);

    virtual void formatStationIDList(std::ostream& sql, const StationIDList& stationIDs);

    // test helpers
    void exec(const std::string& statements) throw (DBException)
        { execSQLUpdate(statements); }

private:
    sqlite3_stmt* prepare_statement(const std::string& sql);
    void finalize_statement(sqlite3_stmt* stmt, int lastStep);

private:
    sqlite3 *db;
};

#endif /* MEMORYTESTDB_H */
