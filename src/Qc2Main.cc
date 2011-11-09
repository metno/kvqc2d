/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id$                                                       

  Copyright (C) 2007 met.no

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

#include <boost/thread/thread.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/version.hpp>
#include "Qc2App.h"
#include <milog/milog.h>
#include "InitLogger.h"
#include <ostream>
#include "StopWatch.h"
#include <miconfparser/miconfparser.h>
#include <fileutil/pidfileutil.h>
#include "Qc2Thread.h"
#include <puTools/miTime.h>
#include <kvalobs/kvPath.h>

#include "CheckedDataCommandBase.h"
#include "CheckedDataHelper.h"


//For test
const char* options[][ 2 ] =
  {
    {"InitRef",
     "NameService=corbaname::monsoon.oslo.dnmi.no"
    },
    {0, 0}
  };

///The kvalobs Qc2 main program.

// FIXME this is a global variable used in Qc2App.cc : sig_term signal handler
extern pthread_t qc2thread_pid;

namespace {

////////////////////////////////////////////////////////////////////////

bool check_rundir()
{
    boost::filesystem::path rundir( kvPath("localstatedir") + "/run" );
    std::cout << kvPath("localstatedir")  << std::endl;
    if( !boost::filesystem::exists(rundir) ) {
        try {
            boost::filesystem::create_directories(rundir);
        } catch( boost::filesystem::filesystem_error & e ) {
            LOGFATAL( e.what() );
            return false;
        }
    } else if( ! boost::filesystem::is_directory(rundir) ) {
        LOGFATAL( rundir.native_file_string() << " exists but is not a directory" );
        return false;
    }

    boost::filesystem::path pidfile( dnmi::file::createPidFileName( rundir.native_file_string(), "kvqc2d" ) );
    bool error = false;
    if ( dnmi::file::isRunningPidFile( pidfile.native_file_string(), error ) ) {
        if ( error ) {
            LOGFATAL( "An error occured while reading the pidfile:" << std::endl
                      << pidfile.native_file_string() << " remove the file if it exist and"
                      << std::endl << "kvqc2d is not running. "
                      << "If it is running and there is problems. Kill kvqc2d and" << std::endl
                      << "restart it." << std::endl << std::endl );
            return false;
        } else {
            LOGFATAL( "Is kvqc2d allready running?" << std::endl
                      << "If not remove the pidfile: " << pidfile.native_file_string() );
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////

std::string find_dbdriver()
{
    std::string dbdriver = "pgdriver.so";
    miutil::conf::ConfSection *conf = KvApp::getConfiguration();
    if( conf ) {
        miutil::conf::ValElementList val = conf->getValue( "database.dbdriver" );
        if( val.size() == 1 )
            dbdriver = val[ 0 ].valAsString();
        std::cout <<  "FROM CONFIGURATION FILE: " << dbdriver << std::endl;
    }
    
    if ( dbdriver.empty() ) {
        // use postgresql as a last guess.
        dbdriver = "pgdriver.so";
    }

    return dbdriver;
}

////////////////////////////////////////////////////////////////////////

struct PidCreateDelete {
    Qc2App& app;
    PidCreateDelete(Qc2App& a, const std::string& pidfilename)
        : app(a)
        { app.createPidFile(pidfilename); }
    ~PidCreateDelete()
        { app.deletePidFile(); }
};

} // anonymous namespace

// ########################################################################

int main( int argc, char** argv )
{
    stopwatch SW;
    
    milog::LogContext logContext("kvqc2d ...");
    
    string htmlpath;
    string logpath_(htmlpath);
    InitLogger( argc, argv, "kvqc2d", htmlpath );
    LOGINFO( "kvqc2d: starting ...." );
    
    if( !check_rundir() )
        return 1;

    const std::string dbdriver = find_dbdriver();
    const std::string constr( KvApp::createConnectString() );

    Qc2App app( argc, argv, dbdriver, constr, options );
  
    if( !app.isOk() ) {
        LOGFATAL( "FATAL: can't  initialize " << argv[ 0 ] << "!\n" );
        return 1;
    }

    CORBA::ORB_ptr orb = app.getOrb();
    PortableServer::POA_ptr poa = app.getPoa();
    PortableServer::POAManager_var pman = app.getPoaMgr();   ///NEW
    pman->activate();  ///NEW

 
    PidCreateDelete pid(app, "kvqc2d");
    sleep(1);

    Qc2Work Qc2Work( app, htmlpath );    //commented out while I test program options !!!!
    boost::thread Qc2Thread( Qc2Work );
#if BOOST_VERSION >= 103500
    qc2thread_pid = Qc2Thread.native_handle(); // FIXME
#elif !defined(BOOST_VERSION)
#error "BOOST_VERSION not defined"
#endif

    int exitcode = 0;
    try {
        // This is where all the *InputImpl(app) and AdminImpl( App ) can be reinstalled if ti is needed
        //app.createPidFile( "Qc2" );
        orb->run();
        orb->destroy();
    } catch ( CORBA::SystemException& ) {
        LOGFATAL( "Caught CORBA::SystemException." );
        exitcode = 1;
    } catch ( CORBA::Exception& ) {
        LOGFATAL( "Caught CORBA::Exception." );
        exitcode = 1;
    } catch ( omniORB::fatalException & fe ) {
        LOGFATAL( "Caught omniORB::fatalException:" << std::endl
                  << "  file: " << fe.file() << std::endl
                  << "  line: " << fe.line() << std::endl
                  << "  mesg: " << fe.errmsg() );
        exitcode = 1;
    } catch ( ... ) {
        LOGFATAL( "Caught unknown exception." );
        exitcode = 1;
    }

    if( exitcode == 0 )
        CERR( "kvqc2d: exit ....\n" );

    // TODO shouldn't there be some kind of Qc2Thread.join() ?

    return exitcode;
}
