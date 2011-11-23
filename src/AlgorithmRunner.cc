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

#include "AlgorithmRunner.h"

#include "AlgorithmDispatcher.h"
#include "Qc2App.h"
#include "AlgorithmConfig.h"
#include "KvalobsDB.h"
#include "KvServicedBroadcaster.h"

#include <milog/milog.h>
#include <puTools/miTime.h>

#include <map>
#include "foreach.h"

void AlgorithmRunner::runAlgorithms(Qc2App& app)
{
    std::auto_ptr<DBInterface> database(new KvalobsDB(app));
    std::auto_ptr<Broadcaster> broadcaster(new KvServicedBroadcaster(app));


    AlgorithmDispatcher dispatcher;
    dispatcher.setDatabase(database.get());
    dispatcher.setBroadcaster(broadcaster.get());

    miutil::miTime lastEnd = miutil::miTime::nowTime();
    lastEnd.addSec(-lastEnd.sec());
    lastEnd.addMin(-1);

    while( !app.isShuttingDown() ) {
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
        if( app.isShuttingDown() )
            break;
        sleep(60); // check config files every minute 
    }
}
