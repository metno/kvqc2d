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

#ifndef ALGORITHMTESTBASE_H_
#define ALGORITHMTESTBASE_H_

#include <gtest/gtest.h>
#include "DBInterface.h"
#include "Qc2Algorithm.h"
#include <sqlite3.h>
#include <vector>

class SqliteTestDB: public DBInterface {
public:
    SqliteTestDB();
    ~SqliteTestDB();

    virtual void selectData(kvDataList_t&, const miutil::miString& where)  throw (DBException);
    virtual void selectStationparams(kvStationParamList_t&, int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException);
    virtual void selectStations(kvStationList_t&) throw (DBException);
    virtual void storeData(const kvDataList_t& toUpdate, const kvDataList_t& toInsert) throw (DBException);
    void exec(const std::string& statement) throw (DBException);
private:
    sqlite3 *db;
};

// #######################################################################

class TestBroadcaster: public Broadcaster {
public:
    typedef std::vector<kvalobs::kvData> updates_t;

    TestBroadcaster()
        { }

    virtual void queueChanged(const kvalobs::kvData& d)
        { mUpdates.push_back(d); }

    virtual void sendChanges()
        { }

    int count() const
        { return mUpdates.size(); }

    void clear()
        { mUpdates.clear(); }

    const updates_t& updates() const
        { return mUpdates; }

private:
    updates_t mUpdates;
};

// #######################################################################

class AlgorithmTestBase: public ::testing::Test {
public:
    void SetUp();
    void TearDown();
protected:
    SqliteTestDB* db;
    TestBroadcaster* bc;
};

#endif /* ALGORITHMTESTBASE_H_ */
