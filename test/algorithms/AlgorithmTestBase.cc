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

#include "AlgorithmTestBase.h"

#include "Helpers.h"
#include <kvalobs/kvQueries.h>
#include "foreach.h"
#include <cstdlib>
#include <stdexcept>

// ------------------------------------------------------------------------

int MemoryNotifier::find(const std::string& needle, int start) const
{
    while(start < size()) {
        if( mMessages[start].text.find(needle) != std::string::npos )
            return start;
        start += 1;
    }
    return -1;
}

// ------------------------------------------------------------------------

int MemoryNotifier::count(Message::Level level) const
{
    int c = 0;
    foreach(const Record& r, mMessages) {
        if( r.level == level )
            c += 1;
    }
    return c;
}

// ------------------------------------------------------------------------

int MemoryNotifier::next(Message::Level lvl, int idx) const
{
    for(; idx>=0 && idx<size(); ++idx)
        if( mMessages[idx].level == lvl )
            return idx;
    return -1;
}

// ------------------------------------------------------------------------

void MemoryNotifier::dump(std::ostream& out)
{
    const char* levels[] = { "DEBUG", "INFO", "WARNING", "ERROR", "FATAL" };
    for(int i=0; i<size(); ++i)
        out << std::setw(3) << i << ' ' << std::setw(7) << levels[mMessages[i].level] << " \'" << mMessages[i].text << "\'\n";
}

// ########################################################################

SqliteTestDB::SqliteTestDB()
{
    if( sqlite3_open("", &db) )
        throw std::runtime_error("could not create db");

    exec("CREATE TABLE data ("
        "stationid   INTEGER NOT NULL, "
        "obstime     TIMESTAMP NOT NULL, "
        "original    FLOAT NOT NULL, "
        "paramid     INTEGER NOT NULL, "
        "tbtime      TIMESTAMP NOT NULL, "
        "typeid      INTEGER NOT NULL, "
        "sensor      CHAR(1) DEFAULT '0', "
        "level       INTEGER DEFAULT 0, "
        "corrected   FLOAT NOT NULL, "
        "controlinfo CHAR(16) DEFAULT '0000000000000000', "
        "useinfo     CHAR(16) DEFAULT '0000000000000000', "
        "cfailed     TEXT DEFAULT NULL);");

    exec("CREATE TABLE station_param ("
        "stationid INTEGER NOT NULL, "
        "paramid   INTEGER NOT NULL, "
        "level     INTEGER DEFAULT 0, "
        "sensor    CHAR(1) DEFAULT '0', "
        "fromday   INTEGER NOT NULL, "
        "today     INTEGER NOT NULL, "
        "hour      INTEGER DEFAULT -1, "
        "qcx       TEXT NOT NULL, "
        "metadata  TEXT DEFAULT NULL, "
        "desc_metadata TEXT DEFAULT NULL, "
        "fromtime TIMESTAMP NOT NULL);");

    exec("CREATE TABLE station ("
        "stationid INTEGER NOT NULL, "
        "lat FLOAT DEFAULT NULL, "
        "lon FLOAT DEFAULT NULL, "
        "height FLOAT DEFAULT NULL, "
        "maxspeed FLOAT DEFAULT NULL, "
        "name       TEXT DEFAULT NULL, "
        "wmonr      INTEGER DEFAULT NULL, "
        "nationalnr INTEGER DEFAULT NULL, "
        "ICAOid     CHAR(4) DEFAULT NULL, "
        "call_sign  CHAR(7) DEFAULT NULL, "
        "stationstr TEXT DEFAULT NULL, "
        "environmentid  INTEGER DEFAULT NULL, "
        "static    BOOLEAN DEFAULT FALSE, "
        "fromtime TIMESTAMP NOT NULL);");

    exec("CREATE TABLE statistical_reference_values ("
        "stationid   INTEGER NOT NULL, "
        "paramid     INTEGER NOT NULL, "
        "day_of_year INTEGER NOT NULL, "
        "key         TEXT NOT NULL, "
        "value       FLOAT NOT NULL);");
}

// ------------------------------------------------------------------------

SqliteTestDB::~SqliteTestDB()
{
    sqlite3_close(db);
}

// ------------------------------------------------------------------------

void SqliteTestDB::selectData(kvDataList_t& d, const miutil::miString& where) throw (DBException)
{
    d.clear();
    const miutil::miString& sql = "SELECT * FROM data " + where + ";";
    sqlite3_stmt *stmt;
#if SQLITE_VERSION_NUMBER >= 3003009 // see http://www.sqlite.org/oldnews.html
    if( sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK )
        throw DBException("preparing '" + sql + "'");
#else
    if( sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK )
        throw DBException("preparing '" + sql + "'");
#endif
    int step;
    while( (step = sqlite3_step(stmt)) == SQLITE_ROW ) {
        int col = 0;
        const int stationid = sqlite3_column_int(stmt, col++);
        const miutil::miTime obstime((const char*)(sqlite3_column_text(stmt, col++)));
        const float original = sqlite3_column_double(stmt, col++);
        const int paramid = sqlite3_column_int(stmt, col++);
        const miutil::miTime tbtime((const char*)(sqlite3_column_text(stmt, col++)));
        const int type_id = sqlite3_column_int(stmt, col++);
        const int sensor = sqlite3_column_int(stmt, col++);
        const int level = sqlite3_column_int(stmt, col++);
        const float corrected = sqlite3_column_double(stmt, col++);
        const kvalobs::kvControlInfo controlinfo(sqlite3_column_text(stmt, col++));
        const kvalobs::kvUseInfo useinfo(sqlite3_column_text(stmt, col++));
        const miutil::miString cfailed = (const char*)sqlite3_column_text(stmt, col++);

        kvalobs::kvData data(stationid, obstime, original, paramid, tbtime, type_id, sensor, level, corrected, controlinfo, useinfo, cfailed);
        d.push_back(data);
    }
    sqlite3_finalize(stmt);
    if( step != SQLITE_DONE)
        throw DBException(sql);
}

// ------------------------------------------------------------------------

void SqliteTestDB::selectStationparams(kvStationParamList_t& d, int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException)
{
    d.clear();
    const std::list<int> station(1, stationID);
    const std::string sql = kvalobs::kvStationParam().selectAllQuery() + kvQueries::selectStationParam(station, time, qcx ) + ";";
    sqlite3_stmt *stmt;
#if SQLITE_VERSION_NUMBER >= 3003009 // see http://www.sqlite.org/oldnews.html
    if( sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK )
        throw DBException("preparing '" + sql + "'");
#else
    if( sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK )
        throw DBException("preparing '" + sql + "'");
#endif
    int step;
    while( (step = sqlite3_step(stmt)) == SQLITE_ROW ) {
        int col = 0;
        const int stationid = sqlite3_column_int(stmt, col++);
        const int paramid = sqlite3_column_int(stmt, col++);
        const int level = sqlite3_column_int(stmt, col++);
        const int sensor = sqlite3_column_int(stmt, col++);
        const int fromday = sqlite3_column_int(stmt, col++);
        const int today = sqlite3_column_int(stmt, col++);
        const int hour = sqlite3_column_int(stmt, col++);
        const miutil::miString qcx = (const char*)sqlite3_column_text(stmt, col++);
        const miutil::miString metadata = (const char*)sqlite3_column_text(stmt, col++);
        const miutil::miString desc_metadata = (const char*)sqlite3_column_text(stmt, col++);
        const miutil::miTime fromtime((const char*)sqlite3_column_text(stmt, col++));

        kvalobs::kvStationParam sp(stationid, paramid, level, sensor, fromday, today, hour, qcx, metadata, desc_metadata, fromtime);
        d.push_back(sp);
    }
    sqlite3_finalize(stmt);
    if(step != SQLITE_DONE)
        throw DBException(sql);
}

// ------------------------------------------------------------------------

void SqliteTestDB::selectStations(kvStationList_t& stations) throw (DBException)
{
    stations.clear();
    const std::string sql = kvalobs::kvStation().selectAllQuery();
    sqlite3_stmt *stmt;
#if SQLITE_VERSION_NUMBER >= 3003009 // see http://www.sqlite.org/oldnews.html
    if( sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK )
        throw DBException("preparing '" + sql + "'");
#else
    if( sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK )
        throw DBException("preparing '" + sql + "'");
#endif
    int step;
    while( (step = sqlite3_step(stmt)) == SQLITE_ROW ) {
        int col = 0;

        const int stationid = sqlite3_column_int(stmt, col++);
        const float lat = sqlite3_column_double(stmt, col++);
        const float lon = sqlite3_column_double(stmt, col++);
        const float height = sqlite3_column_double(stmt, col++);
        const float maxspeed = sqlite3_column_double(stmt, col++);
        const miutil::miString name = (const char*)sqlite3_column_text(stmt, col++);
        const int wmonr = sqlite3_column_int(stmt, col++);
        const int nationalnr = sqlite3_column_int(stmt, col++);
        miutil::miString ICAOid = (const char*)sqlite3_column_text(stmt, col++);
        if( ICAOid.length() != 4 )
            ICAOid = "";
        const miutil::miString call_sign = (const char*)sqlite3_column_text(stmt, col++);
        const miutil::miString stationstr = (const char*)sqlite3_column_text(stmt, col++);
        const int environmentid = sqlite3_column_int(stmt, col++);
        const bool is_static = miutil::miString((const char*)sqlite3_column_text(stmt, col++)) == "t";
        const miutil::miTime fromtime = (const char*)sqlite3_column_text(stmt, col++);

        kvalobs::kvStation station(stationid, lat, lon, height, maxspeed, name, wmonr, nationalnr, ICAOid, call_sign, stationstr, environmentid, is_static, fromtime);
        stations.push_back(station);
    }
    sqlite3_finalize(stmt);
    if(step != SQLITE_DONE)
        throw DBException(sql);
}

// ------------------------------------------------------------------------

void SqliteTestDB::storeData(const kvDataList_t& toUpdate, const kvDataList_t& toInsert) throw (DBException)
{
    if( toUpdate.empty() && toInsert.empty() )
        return;
    std::ostringstream sql;
    if( (toUpdate.size() + toInsert.size()) > 1 )
        sql << "BEGIN; " << std::endl;
    foreach(const kvalobs::kvData& i, toInsert)
        sql << "INSERT INTO " << i.tableName() << " VALUES" << i.toSend() << "; " << std::endl;
    foreach(const kvalobs::kvData& u, toUpdate)
        sql << "UPDATE " << u.tableName() << " " << u.toUpdate() << "; " << std::endl;
    if( (toUpdate.size() + toInsert.size()) > 1 )
        sql << "COMMIT; " << std::endl << std::endl;

#if 0
    std::cout << "------------------------------------------------------------------------" << std::endl;
    std::cout << __PRETTY_FUNCTION__ << " sql='" << sql.str() << "'" << std::endl;
    std::cout << "------------------------------------------------------------------------" << std::endl;
#endif

    exec(sql.str());
}

// ------------------------------------------------------------------------

void SqliteTestDB::selectStatisticalReferenceValue(int stationid, int paramid, int dayOfYear, const std::string& key, bool& valid, float& value)
{
    std::ostringstream sqlS;
    sqlS << "SELECT value FROM statistical_reference_values WHERE stationid = " << stationid
         << " AND paramid = " << paramid << " AND day_of_year = " << dayOfYear
         << " AND key = '" << key << "'";
    const std::string sql = sqlS.str();

    sqlite3_stmt *stmt;
#if SQLITE_VERSION_NUMBER >= 3003009 // see http://www.sqlite.org/oldnews.html
    if( sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK )
        throw DBException("preparing '" + sql + "'");
#else
    if( sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK )
        throw DBException("preparing '" + sql + "'");
#endif
    int step = sqlite3_step(stmt);
    if( step == SQLITE_ROW ) {
        valid = true;
        value = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if(step != SQLITE_DONE)
        throw DBException(sql);
}

// ------------------------------------------------------------------------

void SqliteTestDB::exec(const std::string& statement) throw (DBException)
{
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, statement.c_str(), 0, 0, &zErrMsg);
    if( rc != SQLITE_OK ) {
        const std::string what = zErrMsg;
        sqlite3_free(zErrMsg);
        throw DBException(what);
    }
}

// #######################################################################

DataList& DataList::add(int stationid, const miutil::miTime& obstime, float original, int paramid,
                      int type, float corrected, const std::string& controlinfo, const std::string& cfailed)
{
    kvalobs::kvControlInfo ci(controlinfo);
    kvalobs::kvUseInfo ui;
    ui.setUseFlags(ci);
    push_back(kvalobs::kvData(stationid, obstime, original, paramid, obstime, type, 0, 0, corrected, ci, ui, cfailed));
    return *this;
}

// ------------------------------------------------------------------------

void DataList::insert(SqliteTestDB* db)
{
    std::list<kvalobs::kvData> toUpdate;
    db->storeData(toUpdate, *this);
    clear();
}

// ------------------------------------------------------------------------

void DataList::update(SqliteTestDB* db)
{
    // cannot use storeData because it does not modify the original value
    std::ostringstream sql;
    sql << "BEGIN;";
    foreach(const kvalobs::kvData& d, *this) {
        sql << "UPDATE data SET original=" << d.original() << ", corrected=" << d.corrected()
            << ", controlinfo='" << d.controlinfo().flagstring() << "', useinfo='" << d.useinfo().flagstring() 
            << "', cfailed='" << d.cfailed() << "' " << d.uniqueKey() << ';';
    }
    sql << "COMMIT;";
    db->exec(sql.str());
    clear();
}

// ########################################################################

::testing::AssertionResult AssertObstime(const char* e_expr, const char* /*a_expr*/,
                                         const miutil::miTime& e, const kvalobs::kvData& a)
{
    if( e == a.obstime() )
        return ::testing::AssertionSuccess();
 
    ::testing::Message msg;
    msg << e_expr << " != " << e_expr << " (" << e.isoTime() << " and " << a.obstime().isoTime() << ")";
    return ::testing::AssertionFailure(msg);
}

::testing::AssertionResult AssertObsControlCfailed(const char* eo_expr, const char* eci_expr, const char* ecf_expr, const char* /*a_expr*/,
// ------------------------------------------------------------------------

                                                   const miutil::miTime& eo, const std::string& eci, const std::string& ecf, const kvalobs::kvData& a)
{
    bool failed = false;
    ::testing::Message msg;
    if( eo != a.obstime() ) {
        msg << "(obstime " << eo_expr << " != " << a.obstime().isoTime() << ")";
        failed = true;
    }
    if( eci != a.controlinfo().flagstring() ) {
        if( failed )
            msg << "; ";
        msg << "(controlinfo " << eci_expr << " != " << a.controlinfo().flagstring() << ")";
        failed = true;
    }
    if( a.cfailed().find(ecf) == std::string::npos ) {
        if( failed )
            msg << "; ";
        msg << "(cfailed " << ecf_expr << " not in " << a.cfailed() << ")";
        failed = true;
    }
    return failed ? ::testing::AssertionFailure(msg) : ::testing::AssertionSuccess();
}

// ------------------------------------------------------------------------

::testing::AssertionResult AssertStationObsControlCorrected(const char* es_expr, const char* eo_expr, const char* eci_expr, const char* eco_expr, const char* /*a_expr*/,
                                                            int es, const miutil::miTime& eo, const std::string& eci, float eco, const kvalobs::kvData& a)
{
    bool failed = false;
    ::testing::Message msg;
    if( es != a.stationID() ) {
        msg << "(station " << es_expr << " != " << a.stationID() << ")";
        failed = true;
    }
    if( eo != a.obstime() ) {
        if( failed )
            msg << "; ";
        msg << "(obstime " << eo_expr << " != " << a.obstime().isoTime() << ")";
        failed = true;
    }
    if( eci != a.controlinfo().flagstring() ) {
        if( failed )
            msg << "; ";
        msg << "(controlinfo " << eci_expr << " != " << a.controlinfo().flagstring() << ")";
        failed = true;
    }
    if( !Helpers::equal(a.corrected(), eco) ) {
        if( failed )
            msg << "; ";
        msg << "(corrected " << eco_expr << " != " << a.corrected() << ")";
        failed = true;
    }
    return failed ? ::testing::AssertionFailure(msg) : ::testing::AssertionSuccess();
}

// ########################################################################

AlgorithmTestBase::AlgorithmTestBase()
    : algo(0)
{
}

//------------------------------------------------------------------------

void AlgorithmTestBase::SetUp()
{
    logs = new MemoryNotifier();
    db = new SqliteTestDB();
    bc = new TestBroadcaster();
    if( algo != 0 ) {
        algo->setNotifier(logs);
        algo->setDatabase(db);
        algo->setBroadcaster(bc);
    }
}

// ------------------------------------------------------------------------

void AlgorithmTestBase::TearDown()
{
    delete algo;
    delete bc;
    delete db;
    delete logs;
}
