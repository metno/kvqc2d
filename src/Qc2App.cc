/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2007-2016 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
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

#include "Qc2App.h"

#include "AlgorithmRunner.h"
#include "foreach.h"
#include "helpers/timeutil.h"

#include <kvalobs/kvPath.h>
#include <kvdb/kvdb.h>
#include <kvdb/dbdrivermgr.h>
#include <kvsubscribe/KvDataSerializeCommand.h>
#include <kvsubscribe/queue.h>
#include <milog/milog.h>

#include <boost/bind.hpp>
#include <signal.h>
#include <stdexcept>

#define NDEBUG 1
#include "debug.h"

//#define CORBA_IN_BACKGROUND 1

namespace {

volatile sig_atomic_t sigTerm=0;
void sig_term(int);
void setSigHandlers();

std::string getValue(const std::string& key, std::shared_ptr<miutil::conf::ConfSection> conf)
{
    // FIXME this is the same as CurrentKvApp.cc getValue
    auto val = conf->getValue(key);
    if (val.empty())
        throw std::runtime_error("missing <" + key + "> in config file");
    if (val.size() > 1)
        throw std::runtime_error("Too many entries for <" + key + "> in config file");
    return val.front().valAsString();
}

} // namespace


Qc2App::Qc2App(int argc, char **argv, std::shared_ptr<miutil::conf::ConfSection> conf)
    : confSection(conf)
    , app(kvservice::KvApp::create("kvqc2", argc, argv, confSection))
    , mShouldShutdown(false)
{
    setSigHandlers();
    initializeKAFKA();

    for (int i=1; i<argc; ++i) {
        const std::string argi(argv[i]);
        if (argi == "--run-config") {
            i += 1;
            if (i < argc) {
                algorithmFiles_.push_back(argv[i]);
            } else {
                LOGERROR("Missing argument to '" << argi << "', ignored");
            }
        }
    }
}

Qc2App::~Qc2App()
{
    shutdownKAFKA();
}

dnmi::db::Connection* Qc2App::getNewDbConnection()
{
    // FIXME this is almost the same as CurrentKvApp.cc createConnection
    std::string dbdriver;
    if (confSection) {
        dbdriver = getValue("database.dbdriver", confSection);
        std::cout <<  "FROM CONFIGURATION FILE: " << dbdriver << std::endl;
    }

    if (dbdriver.empty()) {
        // use postgresql as a last guess.
        dbdriver = "pgdriver.so";
    }

    std::string driverId;
    if (!dnmi::db::DriverManager::loadDriver(dbdriver, driverId)) {
        std::ostringstream msg;
        msg << "Unable to load database driver '" << dbdriver << "'.";
        throw std::runtime_error(msg.str());
    }

    // FIXME this is almost the same as CurrentKvApp.cc createConnection
    std::string connectString = getValue("database.dbconnect", confSection);
    return dnmi::db::DriverManager::connect(driverId, connectString);
}

void Qc2App::releaseDbConnection(dnmi::db::Connection *con)
{
    // FIXME this is the same as CurrentKvApp.cc releaseConnection
    dnmi::db::DriverManager::releaseConnection(con);
}

void Qc2App::initializeKAFKA()
{
    mProducerThread.reset(new kvalobs::service::KafkaProducerThread);
}

void Qc2App::runKAFKA()
{
    const std::string brokers = getValue("kafka.brokers", confSection);
    const std::string domain  = getValue("kafka.domain",  confSection);
    mProducerThread->start(brokers, kvalobs::subscribe::queue::checked(domain));
}

void Qc2App::shutdownKAFKA()
{
    mProducerThread->shutdown();
    mProducerThread->join(std::chrono::seconds(15));
    mProducerThread.reset(0);
}

void Qc2App::run()
{
    runKAFKA();
    runAlgorithms();
}

void Qc2App::runAlgorithms()
{
    AlgorithmRunner runner(*this);
    if (algorithmFiles_.empty()) {
        runner.runAlgorithms();
    } else {
        for (std::vector<std::string>::const_iterator it = algorithmFiles_.begin(); it != algorithmFiles_.end(); ++it) {
            runner.runOneAlgorithm(*it);
        }
    }
}

bool Qc2App::sendDataToKvService(const std::list<kvalobs::kvData> &data, bool& busy)
{
    if (data.empty()) {
        LOGDEBUG("No data to send to kvServiced.");
        return true;
    }

    using kvalobs::service::KvDataSerializeCommand;
    KvDataSerializeCommand* dataCmd(new KvDataSerializeCommand(data, "kvqc2"));
    mProducerThread->send(dataCmd);
    LOGINFO("sent data to kafka, hope that the message arrives ...");
    busy = false;
    return true;
}

bool Qc2App::isShuttingDown()
{
    return mShouldShutdown || sigTerm;
}

namespace {

void setSigHandlers()
{
    // see http://www.cs.utah.edu/dept/old/texinfo/glibc-manual-0.02/library_21.html
    // TODO keep ignoring signals if they were ignored

    struct sigaction act, oldact;

    act.sa_handler=sig_term;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;

    if(sigaction(SIGTERM, &act, &oldact)<0) {
        LOGFATAL("ERROR: Can't install signal handler for SIGTERM\n");
        exit(1);
    }

    act.sa_handler=sig_term;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;

    if(sigaction(SIGINT, &act, &oldact)<0) {
        LOGFATAL("ERROR: Can't install signal handler for SIGINT\n");
        exit(1);
    }
}

void sig_term(int)
{
    sigTerm = 1;
}

} // anonymous namespace
