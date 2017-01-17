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

#include <kvalobs/kvStationInfo.h>
#include <kvcpp/KvApp.h>
#include <kvskel/kvService.hh>
#include <kvsubscribe/KafkaProducerThread.h>
#include <kvdb/dbdrivermgr.h>

#include <boost/thread.hpp>

#include <string>
#include <vector>

class Qc2App
{
public:
    Qc2App(int argc, char **argv, std::shared_ptr<miutil::conf::ConfSection> conf);

    virtual ~Qc2App();

    const std::vector<std::string>& algorithmFiles() const
        { return algorithmFiles_; }

    void run();

    bool sendDataToKvService(const std::list<kvalobs::kvData>& data, bool &busy);

    void startShutdown()
        { mShouldShutdown = true; }

    bool isShuttingDown();

    /**
     * Creates a new connection to the database. The caller must
     * call releaseDbConnection after use.
     */
    dnmi::db::Connection *getNewDbConnection();
    void releaseDbConnection(dnmi::db::Connection *con);

private:
    void initializeKAFKA();
    void runKAFKA();
    void shutdownKAFKA();
    void runAlgorithms();

    Qc2App(); // no implementation
    Qc2App& operator=(const Qc2App& other); // no implementation

private:
    std::shared_ptr<miutil::conf::ConfSection> confSection;
    std::unique_ptr<kvservice::KvApp> app;
    bool mShouldShutdown;

    std::unique_ptr<kvalobs::service::KafkaProducerThread> mProducerThread;

    std::vector<std::string> algorithmFiles_;
};

#endif
