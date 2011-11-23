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
#include <milog/milog.h>
#include <kvalobs/kvPath.h>
#include <signal.h>
#include "foreach.h"

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
    , shutdown_(false)
    , orbIsDown(false)
{
    setSigHandlers();

    const std::string driver(kvPath("pkglibdir")+"/db/"+dbDriver);
    if( dbMgr.loadDriver(driver, dbDriverId)){
        LOGINFO("Database driver '" << dbDriverId<< "' loaded.");
    } else {
        LOGERROR("Database driver '" << driver << "' not loaded. Error " << dbMgr.getErr());
        exit(1);
    }
}

Qc2App::~Qc2App()
{
}

dnmi::db::Connection* Qc2App::getNewDbConnection()
{
    dnmi::db::Connection *con = dbMgr.connect(dbDriverId, dbConnect);
    if( !con ) {
        LOGERROR("Can't create a database connection  ("
                 << dbDriverId << ")" << std::endl
                 << "Connect string: <" << dbConnect << ">!");
        return 0;
    }

    LOGINFO("New database connection (" << dbDriverId << ") created!");
    return con;
}

void Qc2App::releaseDbConnection(dnmi::db::Connection *con)
{
    dbMgr.releaseConnection(con);
    LOGINFO("Database connection (" << dbDriverId << ") released");
}

bool Qc2App::sendDataToKvService(const kvalobs::kvStationInfoList &info, bool &busy)
{
    if(info.empty()){
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

bool Qc2App::shutdown()
{
    if(shutdown_ || sigTerm) {
        boost::mutex::scoped_lock l(mutex);
        if(!orbIsDown){
            LOGDEBUG("shutdown CORBA!\n");
            orbIsDown=true;
            getOrb()->shutdown(false);
        }
    }
    return shutdown_ || sigTerm;
}

// FIXME this is a global variable also used in Qc2Main.cc : main
pthread_t qc2thread_pid = 0;

namespace {

void setSigHandlers()
{
    // see http://www.cs.utah.edu/dept/old/texinfo/glibc-manual-0.02/library_21.html
    // TODO keep ignoring signals if they were ignored

    //  sigset_t     oldmask;
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

void sig_term(int i)
{
    //CERR("sig_term("<<i<<"): called!\n");
    if( sigTerm == 0 && qc2thread_pid != 0 )
        pthread_kill(qc2thread_pid, i);
    sigTerm=1;
}

} // anonymous namespace
