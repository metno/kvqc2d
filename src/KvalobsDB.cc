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

#include "foreach.h"
#include "Qc2App.h"

#include <kvalobs/kvQueries.h>
#include <milog/milog.h>

KvalobsDB::KvalobsDB(Qc2App& app)
    : mApp( app )
{
    connect();
}

KvalobsDB::~KvalobsDB()
{
    disconnect();
}

template<class E, class I> KvalobsElemExtract<E, I>* kee(const E&, I i) { return new KvalobsElemExtract<E, I>(i); }

void KvalobsDB::selectData(kvDataList_t& d, const std::string& where) throw (DBException)
{
    std::auto_ptr<KvalobsDbExtract> extract(kee(kvalobs::kvData(), std::back_inserter(d)));
    mDbGate.select(extract.get(), where);
}

void KvalobsDB::selectStations(kvStationList_t& s) throw (DBException)
{
    std::auto_ptr<KvalobsDbExtract> extract(kee(kvalobs::kvStation(), std::back_inserter(s)));
    mDbGate.select(extract.get(), "");
}

void KvalobsDB::selectStationparams(kvStationParamList_t& s, int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException)
{
    const std::list<int> station(1, stationID);
    const std::string where = kvQueries::selectStationParam(station, time, qcx );
    std::auto_ptr<KvalobsDbExtract> extract(kee(kvalobs::kvStationParam(), std::back_inserter(s)));
    mDbGate.select(extract.get(), "");
}

void KvalobsDB::storeData(const kvDataList_t& toUpdate, const kvDataList_t& toInsert) throw (DBException)
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
    try {
        mDbGate.exec(sql.str());
    } catch( dnmi::db::SQLException& ex ) {
        try {
            mDbGate.exec("ROLLBACK;");
        } catch( dnmi::db::SQLException& rex ) {
        }
        connect();
    }
}

DBInterface::reference_value_map_t KvalobsDB::selectStatisticalReferenceValues(int paramid, const std::string& key, float missingValue)
{
    return DBInterface::reference_value_map_t();
}

void KvalobsDB::connect()
{
    if( mDbGate.getConnection() != 0 )
        disconnect();
    while( !mApp.isShuttingDown() ) {
        dnmi::db::Connection* c = mApp.getNewDbConnection();
        if( c ) {
            mDbGate.setConnection( c );
            break;
        }
        LOGINFO( "Cannot connect to database now, retry in 5 seconds." );
        sleep( 5 );
    }
}

void KvalobsDB::disconnect()
{
    dnmi::db::Connection* c = mDbGate.getConnection();
    if( c != 0 ) {
        mApp.releaseDbConnection( c );
        mDbGate.setConnection( 0 );
    }
}
