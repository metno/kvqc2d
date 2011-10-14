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

class DebugDB : public DBInterface {
public:
    DebugDB();
    ~DebugDB();

    virtual bool selectData(kvDataList_t&, const miutil::miString& where);
    virtual bool selectStationparams(kvStationParamList_t&, int stationID, const miutil::miTime& time, const std::string& qcx);
    virtual bool selectStations(kvStationList_t&);
    virtual bool insertData(const kvDataList_t&, bool replace);
    bool exec(const std::string& statement);
private:
    sqlite3 *db;
};

// #######################################################################

class DebugBroadcaster : public Broadcaster {
public:
    virtual void queueChanged(const kvalobs::kvData&) { }
    virtual void sendChanges() { }
};

// #######################################################################

class AlgorithmTestBase : public ::testing::Test {
public:
    void SetUp();
    void TearDown();
protected:
    DebugDB* db;
    DebugBroadcaster* bc;
};

#endif /* ALGORITHMTESTBASE_H_ */
