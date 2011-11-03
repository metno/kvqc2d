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

#include "Qc2Thread.h"

#include "ProcessImpl.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "ReadProgramOptions.h"

#include <milog/milog.h>
#include <puTools/miTime.h>

#include "foreach.h"

Qc2Work::Qc2Work( Qc2App &app_, const std::string& logpath )
    : app( app_ )
    , logpath_( logpath )
{
}

void Qc2Work::operator() ()
{
    LOGINFO( "Qc2Work: starting work thread!\n" );

    // Establish The Connection
    ConnectionHandler connectionHandler( app );
    dnmi::db::Connection * con = connectionHandler.getConnection();

    if( !con ) {
        LOGERROR( "Could not get connection to database" );
        // FIXME just continue if no database connection? will cause sefault when constructing ProcessImpl (*con)
    }

    IDLOGERROR( "html","%%%%%%%%%%%%%%%%%%%%%%%%" );
    LOGINFO( "%%%%%%%%%%%%%%%%%%%%%%%%" );

    ProcessImpl Processor( app, *con);

    while( !app.shutdown() ) {
        ReadProgramOptions params;
        std::vector<std::string> config_files;
        params.SelectConfigFiles(config_files);
        foreach(const std::string& cf, config_files) {
            params.Parse( cf );
            // FIXME this will not necessarily run all algorithms -- 16:20 start for 20min, 16:25 start for other => other not run
            const miutil::miTime now = miutil::miTime::nowTime();
            if ( now.min() == params.RunAtMinute && now.hour() == params.RunAtHour ) {
                try {
                    Processor.select(params);
                } catch ( dnmi::db::SQLException & ex ) {
                    IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
                } catch ( ... ) {
                    IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
                }
            }
        }
        if( app.shutdown() )
            break;
        sleep(59);   //check config files every minute
    }                //end of app while loop
                     //59 seconds is set to avoid the thread getting trapped on a minute boundary
    LOGINFO( "Qc2Work: Thread terminating!" );
}
