/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  Copyright (C) 2007-2011 met.no

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

#include <kvalobs/kvPath.h>
#include <milog/milog.h>
#include <puTools/miTime.h>

#include <boost/bind.hpp>
#include <signal.h>
#include <stdexcept>

#define NDEBUG 1
#include "debug.h"

//#define CORBA_IN_BACKGROUND 1

namespace{
    volatile sig_atomic_t sigTerm=0;
    void     sig_term(int);
    void     setSigHandlers();
};

Qc2App::Qc2App(int argc, char **argv,
               const std::string &dbDriver,
               const std::string &connect_,
               const char *opt[][2])
    : KvApp(argc, argv, opt)
    , dbConnect(connect_)
    , mShouldShutdown(false)
    , mCORBAThread(0)
{
    setSigHandlers();
    createPidFile("kvqc2d");
    initializeDB(dbDriver);
    initializeCORBA();
}

Qc2App::~Qc2App()
{
    shutdownCORBA();
    deletePidFile();
}

void Qc2App::initializeDB(const std::string& dbDriver)
{
    const std::string driver(kvPath("pkglibdir")+"/db/"+dbDriver);
    if( !dbMgr.loadDriver(driver, dbDriverId) ) {
        std::ostringstream msg;
        msg << "Database driver '" << driver << "' not loaded. Error " << dbMgr.getErr();
        throw std::runtime_error(msg.str());
    }
}

dnmi::db::Connection* Qc2App::getNewDbConnection()
{
    return dbMgr.connect(dbDriverId, dbConnect);
}

void Qc2App::releaseDbConnection(dnmi::db::Connection *con)
{
    dbMgr.releaseConnection(con);
}

void Qc2App::initializeCORBA()
{
    // part of initialization is in Qc2App/KvApp/CorbaApp constructor
}

void Qc2App::runCORBA()
{
    try {
        getPoaMgr()->activate();
        getOrb()->run();
    } catch ( CORBA::SystemException& ) {
        LOGFATAL( "CORBA::SystemException." );
        startShutdown();
    } catch ( CORBA::Exception& ) {
        LOGFATAL( "CORBA::Exception." );
        startShutdown();
    } catch ( omniORB::fatalException& fe ) {
        LOGFATAL( "omniORB::fatalException: file='" << fe.file() << "', line=" << fe.line() << ", msg='" << fe.errmsg() << "'");
        startShutdown();
    } catch ( ... ) {
        LOGFATAL( "Unknown exception." );
        startShutdown();
    }
}

void Qc2App::shutdownCORBA()
{
    static boost::mutex shutdownMutex;
    boost::mutex::scoped_lock lock(shutdownMutex);
#ifdef CORBA_IN_BACKGROUND
    if( mCORBAThread ) {
#endif /* CORBA_IN_BACKGROUND */
        try {
#ifdef CORBA_IN_BACKGROUND
            getOrb()->shutdown(false);
#endif /* CORBA_IN_BACKGROUND */
            getOrb()->destroy();
        } catch ( CORBA::SystemException& ) {
            LOGFATAL( "CORBA::SystemException during shutdown." );
        } catch ( CORBA::Exception& ce ) {
            LOGFATAL( "CORBA::Exception during shutdown." );
        } catch ( ... ) {
            LOGFATAL( "Unknown exception during shutdown." );
        }
#ifdef CORBA_IN_BACKGROUND
        mCORBAThread->join();
        delete mCORBAThread;
        mCORBAThread = 0;
    }
#endif /* CORBA_IN_BACKGROUND */
}

void Qc2App::run()
{
#ifdef CORBA_IN_BACKGROUND
    mCORBAThread = new boost::thread( boost::bind(&Qc2App::runCORBA, this) );
    runAlgorithms();
#else /* !CORBA_IN_BACKGROUND */
    mCORBAThread = new boost::thread( boost::bind(&Qc2App::runAlgorithms, this) );
    runCORBA();

    mCORBAThread->join();
    delete mCORBAThread;
    mCORBAThread = 0;
#endif /* !CORBA_IN_BACKGROUND */
}

void Qc2App::runAlgorithms()
{
    AlgorithmRunner runner;
    runner.runAlgorithms(*this);
#ifndef CORBA_IN_BACKGROUND
    getOrb()->shutdown(false);
#endif /* CORBA_IN_BACKGROUND */
}

bool Qc2App::sendDataToKvService(const kvalobs::kvStationInfoList &info, bool &busy)
{
    if( info.empty() ) {
        LOGDEBUG("No data to send to kvServiced.");
        return true;
    }

    CKvalObs::StationInfoList stInfoList;
    stInfoList.length(info.size());
    CORBA::Long k=0;
    foreach(const kvalobs::kvStationInfo& si, info) {
        stInfoList[k].stationId = si.stationID();
        stInfoList[k].obstime   = CORBA::string_dup(si.obstime().isoTime().c_str());
        stInfoList[k].typeId_   = si.typeID();
        k += 1;
    }

    try {
        bool forceNS = false, usedNS = false;
        busy = false;
        for(int i=0; i<2; i++) {
            CKvalObs::CService::DataReadyInput_ptr service = lookUpKvService(forceNS, usedNS);
            try {
                CORBA::Boolean serviceBussy;
                if(!service->dataReady(stInfoList, refServiceCheckedInput, serviceBussy)) {
                    if(serviceBussy)
                        busy=true;
                }
                return true;
            } catch(CORBA::TRANSIENT &ex) {
                LOGWARN("(sendDataToKvService) Exception CORBA::TRANSIENT!" << std::endl
                        << "The reason can be a network problem!");
            } catch(CORBA::COMM_FAILURE &ex) {
                LOGERROR("Can't send data to kvService! Is kvService running!");
            } catch(...) {
                LOGERROR("Can't send data to kvService. Reason unknown!\n");
                return false;
            }

            if(usedNS){
                LOGERROR("Can't send data to kvService, is kvService running!\n");
                return false;
            }
            forceNS=true;

        }
    } catch(LookUpException &ex) {
        LOGERROR("Can't contact CORBA nameservice. Is the CORBA nameservice running."
                 << std::endl << "(" << ex.what() << ")" << std::endl);
        return false;
    } catch(...) {
        LOGERROR("Cant send data to kvService, hmmm, very strange, a unkown exception!\n");
        return false;
    }

    //Shall never happen!
    return false;
}

/*
 *lookUpManager will either return the refMgr or look up kvManagerInput'
 *in the CORBA nameservice.
 */
CKvalObs::CService::DataReadyInput_ptr Qc2App::lookUpKvService(bool forceNS, bool &usedNS)
{
    usedNS=false;

    while( true ) {
        if( forceNS ) {
            usedNS=true;
            
            CORBA::Object_var obj = getRefInNS("kvServiceDataReady");
            if(CORBA::is_nil(obj))
                throw LookUpException("EXCEPTION: Can't obtain a reference for 'kvServiceDataReady'\n           from the CORBA nameserver!");

            CKvalObs::CService::DataReadyInput_ptr ptr = CKvalObs::CService::DataReadyInput::_narrow(obj);

            if(CORBA::is_nil(ptr))
                throw LookUpException("EXCEPTION: Can't narrow reference for 'kvServiceDataReady'!");

            refKvServiceDataReady=ptr;
            break;
        }

        if(CORBA::is_nil(refKvServiceDataReady))
            forceNS=true;
        else
            break;
    }
    return refKvServiceDataReady;
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
