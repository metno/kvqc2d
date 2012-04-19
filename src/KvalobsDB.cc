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

#include "KvalobsElemExtract.h"
#include "foreach.h"
#include "Qc2App.h"

#include <milog/milog.h>

#include <iterator>
#include <memory>

#define UNKNOWN_DBEXCEPTION DBException(std::string("non-std exception in ") + __PRETTY_FUNCTION__)

KvalobsDB::KvalobsDB(Qc2App& app)
    : mApp( app )
{
    connect();
}

// ------------------------------------------------------------------------

KvalobsDB::~KvalobsDB()
{
    try {
        disconnect();
    } catch(std::exception& e) {
        LOGERROR("Exception in ~KvalobsDB:" << e.what());
    } catch(...) {
        LOGERROR("Unknown exception in ~KvalobsDB");
    }
}

// ------------------------------------------------------------------------

DBInterface::StationList KvalobsDB::extractStations(const std::string& sql) throw (DBException)
{
    try {
        DBInterface::StationList s;
        std::auto_ptr<KvalobsDbExtract> extract(makeElementExtract<kvalobs::kvStation>(std::back_inserter(s)));
        mDbGate.select(extract.get(), sql);
        return s;
    } catch(std::exception& e) {
        throw DBException(e.what());
    } catch(...) {
        throw UNKNOWN_DBEXCEPTION;
    }
}

// ------------------------------------------------------------------------

namespace {

class ExtractStationIDs : public KvalobsDbExtract {
public:
    void extractFromRow(const dnmi::db::DRow& row)
        { mStationIds.push_back(std::atoi(row.begin()->c_str())); }

    DBInterface::StationIDList& ids()
        { return mStationIds; }

private:
    DBInterface::StationIDList mStationIds;
};

} // anonymous namespace

DBInterface::StationIDList KvalobsDB::extractStationIDs(const std::string& sql) throw (DBException)
{
    try {
        std::auto_ptr<ExtractStationIDs> extract(new ExtractStationIDs());
        mDbGate.select(extract.get(), sql);
        return extract->ids();
    } catch(std::exception& e) {
        throw DBException(e.what());
    } catch(...) {
        throw UNKNOWN_DBEXCEPTION;
    }
}

// ------------------------------------------------------------------------

DBInterface::StationParamList KvalobsDB::extractStationParams(const std::string& sql) throw (DBException)
{
    try {
        StationParamList s;
        std::auto_ptr<KvalobsDbExtract> extract(makeElementExtract<kvalobs::kvStationParam>(std::back_inserter(s)));
        mDbGate.select(extract.get(), sql);
        return s;
    } catch(std::exception& e) {
        throw DBException(e.what());
    } catch(...) {
        throw UNKNOWN_DBEXCEPTION;
    }
}

// ------------------------------------------------------------------------

DBInterface::DataList KvalobsDB::extractData(const std::string& sql) throw (DBException)
{
    try {
        DataList d;
        std::auto_ptr<KvalobsDbExtract> extract(makeElementExtract<kvalobs::kvData>(std::back_inserter(d)));
        mDbGate.select(extract.get(), sql);
        return d;
    } catch(std::exception& e) {
        throw DBException(e.what());
    } catch(...) {
        throw UNKNOWN_DBEXCEPTION;
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

DBInterface::reference_value_map_t KvalobsDB::extractStatisticalReferenceValues(const std::string& sql, float missingValue) throw (DBException)
{
    try {
        DBInterface::reference_value_map_t rvm;
        std::auto_ptr<KvalobsDbExtract> extract(new ExtractReferenceValue(rvm, missingValue));
        mDbGate.select(extract.get(), sql);
        return rvm;
    } catch(std::exception& e) {
        throw DBException(e.what());
    } catch(...) {
        throw UNKNOWN_DBEXCEPTION;
    }
}

// ------------------------------------------------------------------------

namespace {

struct ExtractNeighborData : public KvalobsDbExtract {
    ExtractNeighborData(NeighborDataVector& neighbors)
        : mNeighbors(neighbors) { }

    void extractFromRow(const dnmi::db::DRow& row);

private:
    NeighborDataVector& mNeighbors;
};

void ExtractNeighborData::extractFromRow(const dnmi::db::DRow& row)
{
    dnmi::db::CIDRow col = row.begin();
    const int neighborid = std::atoi((*col++).c_str());
    const float offset   = std::atof((*col++).c_str());
    const float slope    = std::atof((*col++).c_str());
    const float sigma    = std::atof((*col++).c_str());

    mNeighbors.push_back(NeighborData(neighborid, offset, slope, sigma));
}

} // anonymous namespace

NeighborDataVector KvalobsDB::extractNeighborData(const std::string& sql) throw (DBException)
{
    try {
        NeighborDataVector neighbors;
        std::auto_ptr<KvalobsDbExtract> extract(new ExtractNeighborData(neighbors));
        mDbGate.select(extract.get(), sql);
        return neighbors;
    } catch(std::exception& e) {
        throw DBException(e.what());
    } catch(...) {
        throw UNKNOWN_DBEXCEPTION;
    }
}

// ------------------------------------------------------------------------

DBInterface::ModelDataList KvalobsDB::extractModelData(const std::string& sql) throw (DBException)
{
    try {
        ModelDataList modelData;
        std::auto_ptr<KvalobsDbExtract> extract(makeElementExtract<kvalobs::kvModelData>(std::back_inserter(modelData)));
        mDbGate.select(extract.get(), sql);
        return modelData;
    } catch(std::exception& e) {
        throw DBException(e.what());
    } catch(...) {
        throw UNKNOWN_DBEXCEPTION;
    }
}

// ------------------------------------------------------------------------

void KvalobsDB::execSQLUpdate(const std::string& sql) throw (DBException)
{
    try {
        mDbGate.exec(sql);
    } catch( dnmi::db::SQLException& ex ) {
        LOGERROR("Problem in update, SQL='" + sql + "'; exception=" + ex.what() + "; trying rollback");
        try {
            mDbGate.exec("ROLLBACK;");
        } catch( dnmi::db::SQLException& rex ) {
            LOGERROR("Rollback failed after problem wit SQL='" + sql + "'; exception=" + ex.what());
        }
        connect();
        throw DBException(ex.what());
    } catch(std::exception& e) {
        throw DBException(e.what());
    } catch(...) {
        throw UNKNOWN_DBEXCEPTION;
    }
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

// ------------------------------------------------------------------------

void KvalobsDB::disconnect()
{
    dnmi::db::Connection* c = mDbGate.getConnection();
    if( c != 0 ) {
        mDbGate.setConnection( 0 );
        mApp.releaseDbConnection( c );
    }
}
