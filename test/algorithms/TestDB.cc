
#include "TestDB.h"

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

    exec("CREATE TABLE interpolation_best_neighbors ("
         "stationid   INTEGER NOT NULL, "
         "neighborid  INTEGER NOT NULL, "
         "paramid     INTEGER NOT NULL, "
         "fit_offset  FLOAT NOT NULL, "
         "fit_slope   FLOAT NOT NULL, "
         "fit_sigma   FLOAT NOT NULL);");

    exec("CREATE TABLE model_data ("
         "stationid INTEGER   NOT NULL, "
         "obstime   TIMESTAMP NOT NULL, "
         "paramid   INTEGER   NOT NULL, "
         "level     INTEGER   NOT NULL, "
         "modelid   INTEGER   NOT NULL, "
         "original  DOUBLE);");
}

// ------------------------------------------------------------------------

SqliteTestDB::~SqliteTestDB()
{
    sqlite3_close(db);
}

// ------------------------------------------------------------------------

sqlite3_stmt* SqliteTestDB::prepare_statement(const std::string& sql)
{
    int status;
    sqlite3_stmt *stmt;
#if SQLITE_VERSION_NUMBER >= 3003009 // see http://www.sqlite.org/oldnews.html
    status = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, 0);
#else
    status = sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, 0);
#endif
    if( status != SQLITE_OK ) {
        std::ostringstream msg;
        msg << "Error preparing SQL statement '" << sql
            << "'; message from sqlite3 is:" << sqlite3_errmsg(db);
        throw DBException(msg.str());
    }
    return stmt;
}

// ------------------------------------------------------------------------

void SqliteTestDB::finalize_statement(sqlite3_stmt* stmt, int lastStep)
{
    sqlite3_finalize(stmt);
    if(lastStep != SQLITE_DONE) {
        std::ostringstream msg;
        msg << "Statement stepping not finished with DONE; error=" << sqlite3_errmsg(db);
        throw DBException(msg.str());
    }
}

// ------------------------------------------------------------------------

static std::string sqlite3_column_string(sqlite3_stmt *stmt, int col)
{
    const char* c = (const char*)sqlite3_column_text(stmt, col++);
    if( !c )
        return "";
    else
        return c;
}

// ------------------------------------------------------------------------

DBInterface::StationList SqliteTestDB::extractStations(const std::string& sql) throw (DBException)
{
    StationList stations;
    sqlite3_stmt *stmt = prepare_statement(sql);
    int step;
    while( (step = sqlite3_step(stmt)) == SQLITE_ROW ) {
        int col = 0;

        const int stationid = sqlite3_column_int(stmt, col++);
        const float lat = sqlite3_column_double(stmt, col++);
        const float lon = sqlite3_column_double(stmt, col++);
        const float height = sqlite3_column_double(stmt, col++);
        const float maxspeed = sqlite3_column_double(stmt, col++);
        const std::string name = sqlite3_column_string(stmt, col++);
        const int wmonr = sqlite3_column_int(stmt, col++);
        const int nationalnr = sqlite3_column_int(stmt, col++);
        std::string ICAOid = sqlite3_column_string(stmt, col++);
        if( ICAOid.length() != 4 )
            ICAOid = "";
        const std::string call_sign = sqlite3_column_string(stmt, col++);
        const std::string stationstr = sqlite3_column_string(stmt, col++);
        const int environmentid = sqlite3_column_int(stmt, col++);
        const bool is_static = sqlite3_column_string(stmt, col++) == "t";
        const miutil::miTime fromtime = (const char*)sqlite3_column_text(stmt, col++);

        kvalobs::kvStation station(stationid, lat, lon, height, maxspeed, name, wmonr, nationalnr, ICAOid, call_sign, stationstr, environmentid, is_static, fromtime);
        stations.push_back(station);
    }
    finalize_statement(stmt, step);
    return stations;
}

// ------------------------------------------------------------------------

DBInterface::StationIDList SqliteTestDB::extractStationIDs(const std::string& sql) throw (DBException)
{
    StationIDList stations;
    sqlite3_stmt *stmt = prepare_statement(sql);
    int step;
    while( (step = sqlite3_step(stmt)) == SQLITE_ROW ) {
        const int stationid = sqlite3_column_int(stmt, 0);
        stations.push_back(stationid);
    }
    finalize_statement(stmt, step);
    return stations;
}

// ------------------------------------------------------------------------

DBInterface::StationParamList SqliteTestDB::extractStationParams(const std::string& sql) throw (DBException)
{
    StationParamList d;
    sqlite3_stmt *stmt = prepare_statement(sql);
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
        const std::string qcx = sqlite3_column_string(stmt, col++);
        const std::string metadata = sqlite3_column_string(stmt, col++);
        const std::string desc_metadata = sqlite3_column_string(stmt, col++);
        const miutil::miTime fromtime((const char*)sqlite3_column_text(stmt, col++));

        kvalobs::kvStationParam sp(stationid, paramid, level, sensor, fromday, today, hour, qcx, metadata, desc_metadata, fromtime);
        d.push_back(sp);
    }
    finalize_statement(stmt, step);
    return d;
}

// ------------------------------------------------------------------------

DBInterface::DataList SqliteTestDB::extractData(const std::string& sql) throw (DBException)
{
    DataList d;
    sqlite3_stmt *stmt = prepare_statement(sql);
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
        const std::string cfailed = sqlite3_column_string(stmt, col++);

        kvalobs::kvData data(stationid, obstime, original, paramid, tbtime, type_id, sensor, level, corrected, controlinfo, useinfo, cfailed);
        d.push_back(data);
    }
    finalize_statement(stmt, step);
    return d;
}

// ------------------------------------------------------------------------

DBInterface::reference_value_map_t SqliteTestDB::extractStatisticalReferenceValues(const std::string& sql, float missingValue) throw (DBException)
{
    reference_value_map_t rvm;
    sqlite3_stmt *stmt = prepare_statement(sql);
    int step;
    while( (step = sqlite3_step(stmt)) == SQLITE_ROW ) {
        int col = 0;
        const int stationid   = sqlite3_column_int(stmt, col++);
        const int day_of_year = sqlite3_column_int(stmt, col++);
        const float value     = sqlite3_column_double(stmt, col++);
        if( rvm.find(stationid) == rvm.end() )
            rvm[stationid] = reference_values_t(365, missingValue);
        if( day_of_year >= 1 && day_of_year <= 365 )
            rvm[stationid][day_of_year-1] = value;
    }
    finalize_statement(stmt, step);
    return rvm;
}

// ------------------------------------------------------------------------

NeighborDataVector SqliteTestDB::extractNeighborData(const std::string& sql) throw (DBException)
{
    NeighborDataVector neighbors;
    sqlite3_stmt *stmt = prepare_statement(sql);
    int step;
    while( (step = sqlite3_step(stmt)) == SQLITE_ROW ) {
        int col = 0;
        const int neighborid = sqlite3_column_int(stmt, col++);
        const float offset   = sqlite3_column_double(stmt, col++);
        const float slope    = sqlite3_column_double(stmt, col++);
        const float sigma    = sqlite3_column_double(stmt, col++);
        neighbors.push_back(NeighborData(neighborid, offset, slope, sigma));
    }
    finalize_statement(stmt, step);
    return neighbors;
}

// ------------------------------------------------------------------------

DBInterface::ModelDataList SqliteTestDB::extractModelData(const std::string& sql) throw (DBException)
{
    ModelDataList modelData;
    sqlite3_stmt *stmt = prepare_statement(sql);
    int step;
    while( (step = sqlite3_step(stmt)) == SQLITE_ROW ) {
        int col = 0;
        const int stationid = sqlite3_column_int(stmt, col++);
	const miutil::miTime obstime((const char*)(sqlite3_column_text(stmt, col++)));
        const int paramid = sqlite3_column_int(stmt, col++);
        const int level = sqlite3_column_int(stmt, col++);
        const int modelid = sqlite3_column_int(stmt, col++);
        const float original = sqlite3_column_double(stmt, col++);
        modelData.push_back(kvalobs::kvModelData(stationid, obstime, paramid, level, modelid, original));
    }
    finalize_statement(stmt, step);
    return modelData;
}

// ------------------------------------------------------------------------

void SqliteTestDB::execSQLUpdate(const std::string& sql) throw (DBException)
{
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
    if( rc != SQLITE_OK ) {
        const std::string what = zErrMsg;
        sqlite3_free(zErrMsg);
        throw DBException(what);
    }
}
