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

#include "KvalobsElemExtract.h"
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

void KvalobsDB::selectData(kvDataList_t& d, const std::string& where) throw (DBException)
{
    d.clear();
    std::auto_ptr<KvalobsDbExtract> extract(makeElementExtract<kvalobs::kvData>(std::back_inserter(d)));
    mDbGate.select(extract.get(), kvalobs::kvData().selectAllQuery() + " " + where);
}

void KvalobsDB::selectStations(kvStationList_t& s) throw (DBException)
{
    s.clear();
    std::auto_ptr<KvalobsDbExtract> extract(makeElementExtract<kvalobs::kvStation>(std::back_inserter(s)));
    mDbGate.select(extract.get(), kvalobs::kvStation().selectAllQuery());
}

void KvalobsDB::selectStationparams(kvStationParamList_t& s, int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException)
{
    s.clear();
    const std::list<int> station(1, stationID);
    const std::string where = kvQueries::selectStationParam(station, time, qcx );
    std::auto_ptr<KvalobsDbExtract> extract(makeElementExtract<kvalobs::kvStationParam>(std::back_inserter(s)));
    mDbGate.select(extract.get(), kvalobs::kvStationParam().selectAllQuery());
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
        LOGERROR("Problem while storing data, SQL='" + sql.str() + "'; exception=" + ex.what() + "; trying rollback");
        try {
            mDbGate.exec("ROLLBACK;");
        } catch( dnmi::db::SQLException& rex ) {
            LOGERROR("Rollback failed after problem wit SQL='" + sql.str() + "'; exception=" + ex.what());
        }
        connect();
    }
}

// ------------------------------------------------------------------------

namespace {

struct ExtractReferenceValue : public KvalobsDbExtract {
    ExtractReferenceValue(DBInterface::reference_value_map_t& rvm, float missingValue)
        : mRVM(rvm), mMissingValue(missingValue) { }

    void extractFromRow(const dnmi::db::DRow& row);

private:
    DBInterface::reference_value_map_t& mRVM;
    float mMissingValue;
};

void ExtractReferenceValue::extractFromRow(const dnmi::db::DRow& row)
{
    dnmi::db::CIDRow col = row.begin();
    const int stationid   = std::atoi((*col++).c_str());
    const int day_of_year = std::atoi((*col++).c_str());
    const float value     = std::atof((*col++).c_str());
    if( mRVM.find(stationid) == mRVM.end() )
        mRVM[stationid] = DBInterface::reference_values_t(365, mMissingValue);
    if( day_of_year >= 1 && day_of_year <= 365 )
        mRVM[stationid][day_of_year-1] = value;
}

} // anonymous namespace

DBInterface::reference_value_map_t KvalobsDB::selectStatisticalReferenceValues(int paramid, const std::string& key, float missingValue)
{
    DBInterface::reference_value_map_t rvm;
    std::auto_ptr<KvalobsDbExtract> extract(new ExtractReferenceValue(rvm, missingValue));
    std::ostringstream sql;
    sql << "SELECT stationid, day_of_year, value FROM statistical_reference_values"
        << " WHERE paramid = " << paramid << " AND key = '" << key << "'";
    mDbGate.select(extract.get(), sql.str());
    return rvm;
}

// ------------------------------------------------------------------------

namespace {

struct ExtractNeighborData : public KvalobsDbExtract {
    ExtractNeighborData(CorrelatedNeighbors::neighbors_t& neighbors)
        : mNeighbors(neighbors) { }

    void extractFromRow(const dnmi::db::DRow& row);

private:
    CorrelatedNeighbors::neighbors_t& mNeighbors;
};

void ExtractNeighborData::extractFromRow(const dnmi::db::DRow& row)
{
    dnmi::db::CIDRow col = row.begin();
    const int neighborid = std::atoi((*col++).c_str());
    const float offset   = std::atof((*col++).c_str());
    const float slope    = std::atof((*col++).c_str());
    const float sigma    = std::atof((*col++).c_str());

    mNeighbors.push_back(CorrelatedNeighbors::NeighborData(neighborid, offset, slope, sigma));
}

} // anonymous namespace

CorrelatedNeighbors::neighbors_t KvalobsDB::selectNeighborData(int stationid, int paramid)
{
    CorrelatedNeighbors::neighbors_t neighbors;
    std::auto_ptr<KvalobsDbExtract> extract(new ExtractNeighborData(neighbors));
    std::ostringstream sql;
    sql << "SELECT neighborid, offset, slope, sigma FROM interpolation_best_neighbors"
        << " WHERE stationid = " << stationid << " AND paramid = " << paramid;
    mDbGate.select(extract.get(), sql.str());
    return neighbors;
}

// ------------------------------------------------------------------------

void KvalobsDB::selectModelData(kvModelDataList_t& modelData, int stationid, int paramid, int level, const TimeRange& time)
{
    modelData.clear();
    std::auto_ptr<KvalobsDbExtract> extract(makeElementExtract<kvalobs::kvModelData>(std::back_inserter(modelData)));
    std::ostringstream sql;
    sql << kvalobs::kvModelData().selectAllQuery()
        << " WHERE stationid = " << stationid << " AND paramid = " << paramid << " AND level = " << level
        << " AND obstime BETWEEN '" << time.t0.isoTime() << "' AND '" << time.t1.isoTime() << "'";
    mDbGate.select(extract.get(), sql.str());
}

// ------------------------------------------------------------------------

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
        mDbGate.setConnection( 0 );
        mApp.releaseDbConnection( c );
    }
}
