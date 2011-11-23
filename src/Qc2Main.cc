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

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include "Qc2App.h"
#include <milog/milog.h>
#include "InitLogger.h"
#include <miconfparser/miconfparser.h>
#include <fileutil/pidfileutil.h>
#include <kvalobs/kvPath.h>

#include <string>

const char* options[][ 2 ] =
  {
    {"InitRef",
     "NameService=corbaname::monsoon.oslo.dnmi.no"
    },
    {0, 0}
  };

namespace {

////////////////////////////////////////////////////////////////////////

bool check_rundir()
{
    boost::filesystem::path rundir( kvPath("localstatedir") + "/run" );
    if( !boost::filesystem::exists(rundir) ) {
        try {
            boost::filesystem::create_directories(rundir);
        } catch( boost::filesystem::filesystem_error& e ) {
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
                      << pidfile.native_file_string() << " remove the file if it exists and"
                      << std::endl << "kvqc2d is not running. "
                      << "If it is running and there is problems. Kill kvqc2d and" << std::endl
                      << "restart it." << std::endl << std::endl );
            return false;
        } else {
            LOGFATAL( "Is kvqc2d already running?" << std::endl
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

} // anonymous namespace

// ########################################################################

int main( int argc, char** argv )
{
    milog::LogContext logContext("kvqc2d ...");
    
    InitLogger( argc, argv, "kvqc2d" );
    LOGINFO( "kvqc2d: starting ...." );
    
    if( !check_rundir() )
        return 1;

    const std::string dbdriver = find_dbdriver();
    const std::string constr( KvApp::createConnectString() );

    Qc2App app( argc, argv, dbdriver, constr, options );
  
    if( !app.isOk() ) {
        LOGFATAL( "Initialization problem" );
        return 1;
    }

    return app.run();
}
