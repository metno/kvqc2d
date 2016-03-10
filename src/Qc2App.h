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

#ifndef QC2APP_H
#define QC2APP_H

#include <kvalobs/kvapp.h>
#include <kvskel/kvService.hh>
#include <kvalobs/kvStationInfo.h>
#include <kvdb/dbdrivermgr.h>

#include <boost/thread.hpp>

#include <string>
#include <vector>

class Qc2App: public KvApp
{
public:
    Qc2App(int argc, char **argv,
           const std::string &driver,
           const std::string &connect_,
           const char *options[][2]=0);

    virtual ~Qc2App();

    const std::vector<std::string>& algorithmFiles() const
        { return algorithmFiles_; }

    void run();

    bool sendDataToKvService(const kvalobs::kvStationInfoList &info_, bool &busy);

    void startShutdown()
        { mShouldShutdown = true; }

    bool isShuttingDown();

    /**
     * Creates a new connection to the database. The caller must
     * call releaseDbConnection after use.
     */
    dnmi::db::Connection *getNewDbConnection();
    void                 releaseDbConnection(dnmi::db::Connection *con);

private:
    void initializeDB(const std::string& dbDriver);
    void initializeCORBA();
    void runCORBA();
    void shutdownCORBA();
    void runAlgorithms();

    CKvalObs::CService::DataReadyInput_ptr lookUpKvService(bool forceNS,
                                                           bool &usedNS);

    Qc2App(); //No implementation

private:
    dnmi::db::DriverManager dbMgr;
    std::string dbConnect;
    std::string dbDriverId;
    bool mShouldShutdown;

    CKvalObs::CManager::CheckedInput_var refServiceCheckedInput;
    CKvalObs::CService::DataReadyInput_var refKvServiceDataReady;

    boost::thread* mCORBAThread;

    std::vector<std::string> algorithmFiles_;
};

#endif
