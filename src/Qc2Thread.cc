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

#include "Qc2Thread.h"

#include "AlgorithmDispatcher.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "AlgorithmConfig.h"

#include <milog/milog.h>
#include <puTools/miTime.h>

#include <boost/version.hpp>
#include <map>
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
        // FIXME just continue if no database connection? will cause sefault when constructing AlgorithmDispatcher (*con)
    }

    LOGINFO( "%%%%%%%%%%%%%%%%%%%%%%%%" );

    AlgorithmDispatcher dispatcher( app, *con);

    miutil::miTime lastEnd = miutil::miTime::nowTime();
    lastEnd.addSec(-lastEnd.sec());
    lastEnd.addMin(-1);

    while( !app.shutdown() ) {
        AlgorithmConfig params;
        std::vector<std::string> config_files;
        params.SelectConfigFiles(config_files);

        miutil::miTime now = miutil::miTime::nowTime();
        now.addSec(-now.sec()); // set seconds to 0
        LOGINFO("now = " << now);

        // XXX if an algorithm is scheduled hourly and the previous algorithm is taking 2 hours, it will be run only once

        // sort algorithms to be run by scheduled time
        typedef std::multimap<miutil::miTime, std::string> queue_t;
        queue_t queue;
        foreach(const std::string& cf, config_files) {
            try {
                params.Parse( cf );
                const int hour = params.RunAtHour < 0 ? now.hour() : params.RunAtHour;
                const miutil::miTime runAt(now.year(), now.month(), now.day(),
                                           hour, params.RunAtMinute, 0);
                LOGINFO("Algorithm='" << params.Algorithm << "' runAt = " << runAt);
                if( runAt > lastEnd && runAt <= now )
                    queue.insert(queue_t::value_type(runAt, cf));
            } catch(ConfigException& ce) {
                LOGERROR("Configuration parser exception: " << ce.what() << " while reading '" << cf << "'");
            } catch ( ... ) {
                LOGERROR("Unknown exception while reading '" << cf << "'");
            }
        }

        // run queued algorithms
        foreach(queue_t::value_type tc, queue) {
            params.Parse( tc.second );
            if( tc.first < now )
                LOGINFO("Algorithm " << params.Algorithm << " scheduled for "
                        << std::setw(2) << std::setfill('0') << tc.first.hour() << ':'
                        << std::setw(2) << std::setfill('0') << tc.first.min() << " is delayed");
            try {
                dispatcher.select(params);
            } catch ( dnmi::db::SQLException & ex ) {
                LOGERROR("Exception: " << ex.what());
            } catch ( ... ) {
                LOGERROR("Unknown exception: ...");
            }
        }
        lastEnd = now;
        if( app.shutdown() )
            break;
#if BOOST_VERSION >= 103500
        sleep(59);   //check config files every minute 
#elif !defined(BOOST_VERSION)
#error "BOOST_VERSION not defined"
#else
        for(int i=0; i<59 && !app.shutdown(); ++i)
            sleep(1);
#endif
    }
    LOGINFO( "Qc2Work: Thread terminating!" );
}
