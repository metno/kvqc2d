/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2007-2012 met.no

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

#include "AlgorithmConfig.h"
#include "AlgorithmDispatcher.h"
#include "foreach.h"
#include "KvalobsDB.h"
#include "KvServicedBroadcaster.h"
#include "LogfileNotifier.h"
#include "Qc2App.h"

#include <milog/milog.h>

#include <map>

#define NDEBUG 1
#include "debug.h"

AlgorithmRunner::AlgorithmRunner(Qc2App& app_)
    : app(app_)
    , database(new KvalobsDB(app))
    , broadcaster(new KvServicedBroadcaster(app))
    , notifier(new LogfileNotifier)
{
    dispatcher.setDatabase(database.get());
    dispatcher.setBroadcaster(broadcaster.get());
    dispatcher.setNotifier(notifier.get());

}

AlgorithmRunner::~AlgorithmRunner()
{
}

void AlgorithmRunner::runAlgorithms()
{
    kvtime::time lastEnd = kvtime::now();
    kvtime::addSeconds(lastEnd, -kvtime::second(lastEnd));
    kvtime::addMinutes(lastEnd, -1);

    while( !app.isShuttingDown() ) {
        AlgorithmConfig params;
        std::vector<std::string> config_files;
        params.SelectConfigFiles(config_files);

        kvtime::time now = kvtime::now();
        kvtime::addSeconds(now, -kvtime::second(now)); // set seconds to 0
        if( kvtime::minute(now) == 0 )
            LOGINFO("kvqc2d is running :-)");

        // XXX if an algorithm is scheduled hourly and the previous algorithm is taking 2 hours, it will be run only once

        // sort algorithms to be run by scheduled time
        typedef std::multimap<kvtime::time, std::string> queue_t;
        queue_t queue;
        foreach(const std::string& cf, config_files) {
            try {
                params.Parse( cf );
                const int hour = params.RunAtHour < 0 ? kvtime::hour(now) : params.RunAtHour;
                const kvtime::time runAt = kvtime::maketime(kvtime::year(now), kvtime::month(now), kvtime::day(now),
                        hour, params.RunAtMinute, 0);
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
                        << std::setw(2) << std::setfill('0') << kvtime::hour(tc.first) << ':'
                        << std::setw(2) << std::setfill('0') << kvtime::minute(tc.first) << " is delayed");
            runAlgorithmFromConfig(params);
        }
        lastEnd = now;
        if( app.isShuttingDown() )
            break;
        // check config files every minute
        for( int i=0; i<60 && !app.isShuttingDown(); ++i )
            sleep(1);
    }
}

void AlgorithmRunner::runOneAlgorithm(const std::string& config)
{
    AlgorithmConfig params;
    params.Parse(config);
    runAlgorithmFromConfig(params);
}

void AlgorithmRunner::runAlgorithmFromConfig(const AlgorithmConfig& params)
{
    try {
        dispatcher.select(params);
    } catch (dnmi::db::SQLException& ex) {
        LOGERROR("SQL Exception: " << ex.what());
    } catch (std::exception& ex) {
        LOGERROR("Exception: " << ex.what());
    } catch ( ... ) {
        LOGERROR("Unknown exception: ...");
    }
}
