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

class SqliteTestDB : public DBInterface {
public:
    SqliteTestDB();
    ~SqliteTestDB();

    virtual void selectData(kvDataList_t&, const miutil::miString& where)  throw (DBException);
    virtual void selectStationparams(kvStationParamList_t&, int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException);
    virtual void selectStations(kvStationList_t&) throw (DBException);
    virtual void storeData(const kvDataList_t& toUpdate, const kvDataList_t& toInsert) throw (DBException);

    // test helpers
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

    const kvalobs::kvData& update(int i) const
        { return mUpdates[i]; }

private:
    updates_t mUpdates;
};

// #######################################################################

class DataList : public std::list<kvalobs::kvData> {
public:
    DataList(int stationid, int paramid, int tid)
        : mStationId(stationid), mParamId(paramid), mTypeId(tid) { }

    DataList& setStation(int sid)
        { mStationId = sid; return *this; }
    DataList& setParam(int pid)
        { mParamId = pid; return *this; }
    DataList& setType(int tid)
        { mTypeId = tid; return *this; }

    DataList& add(int stationid, const miutil::miTime& obstime, float original, int paramid, int type, float corrected, const std::string& controlinfo, const std::string& cfailed);

    DataList& add(int stationid, const miutil::miTime& obstime, float original, int paramid, int type, const std::string& controlinfo, const std::string& cfailed)
        { return add(stationid, obstime, original, paramid, type, original, controlinfo, cfailed); }

    DataList& add(const miutil::miTime& obstime, float original, float corrected, const std::string& controlinfo, const std::string& cfailed)
        { return add(mStationId, obstime, original, mParamId, mTypeId, corrected, controlinfo, cfailed); }

    DataList& add(const miutil::miTime& obstime, float original, const std::string& controlinfo, const std::string& cfailed)
        { return add(obstime, original, original, controlinfo, cfailed); }

    DataList& add(int stationid, const miutil::miTime& obstime, float original, const std::string& controlinfo, const std::string& cfailed)
        { return add(stationid, obstime, original, mParamId, mTypeId, original, controlinfo, cfailed); }

    void insert(SqliteTestDB* db);
    void update(SqliteTestDB* db);

private:
    int mStationId, mParamId, mTypeId;
};

// ########################################################################

::testing::AssertionResult AssertObstime(const char* e_expr, const char* /*a_expr*/,
                                         const miutil::miTime& e, const kvalobs::kvData& a);

#define EXPECT_OBSTIME(e, a) EXPECT_PRED_FORMAT2(AssertObstime, e, a)
#define ASSERT_OBSTIME(e, a) ASSERT_PRED_FORMAT2(AssertObstime, e, a)

#define EXPECT_CFAILED(e, a) EXPECT_PRED_FORMAT2(::testing::IsSubstring, e, a.cfailed())
#define ASSERT_CFAILED(e, a) EXPECT_PRED_FORMAT2(::testing::IsSubstring, e, a.cfailed())

::testing::AssertionResult AssertObsControlCfailed(const char* eo_expr, const char* eci_expr, const char* ecf_expr, const char* a_expr,
                                                   const miutil::miTime& eo, const std::string& eci, const std::string& ecf, const kvalobs::kvData& a);

#define EXPECT_OBS_CONTROL_CFAILED(eo, eci, ecf, a) EXPECT_PRED_FORMAT4(AssertObsControlCfailed, eo, eci, ecf, a)
#define ASSERT_OBS_CONTROL_CFAILED(eo, eci, ecf, a) ASSERT_PRED_FORMAT4(AssertObsControlCfailed, eo, eci, ecf, a)

::testing::AssertionResult AssertStationObsControlCorrected(const char* es_expr, const char* eo_expr, const char* eci_expr, const char* eco_expr, const char* a_expr,
                                                            int es, const miutil::miTime& eo, const std::string& eci, float eco, const kvalobs::kvData& a);

#define EXPECT_STATION_OBS_CONTROL_CORR(es, eo, eci, eco, a) EXPECT_PRED_FORMAT5(AssertStationObsControlCorrected, es, eo, eci, eco, a)
#define ASSERT_STATION_OBS_CONTROL_CORR(es, eo, eci, eco, a) ASSERT_PRED_FORMAT5(AssertStationObsControlCorrected, es, eo, eci, eco, a)

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
