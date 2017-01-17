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

#include "InitLogger.h"
#include "Qc2App.h"

#include <fileutil/pidfileutil.h>
#include <kvalobs/kvPath.h>
#include <miconfparser/miconfparser.h>
#include <milog/milog.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <string>

#include "version.h"

namespace {

class PidFiler {
public:
    PidFiler(const std::string& name);
    ~PidFiler();

private:
    std::string pidfile;

    PidFiler();
    PidFiler& operator=(const PidFiler&);
};

PidFiler::PidFiler(const std::string& name)
{
    const std::string rundir = kvPath("rundir");
    const boost::filesystem::path runpath(rundir);
    if (!boost::filesystem::exists(runpath)) {
        boost::filesystem::create_directories(runpath);
    } else if (!boost::filesystem::is_directory(runpath)) {
        throw std::runtime_error(rundir + " exists but is not a directory");
    }

    pidfile = dnmi::file::createPidFileName(rundir, name);
    bool error = false;
    if (dnmi::file::isRunningPidFile(pidfile, error)) {
        if (error) {
            throw std::runtime_error
                ("An error occured while reading the pid file '" + pidfile + "'. "
                 "Remove the file if it exists and " + name + " is not running. "
                 "Otherwise, you might want to stop and restart it.");
        } else {
            throw std::runtime_error
                ("Is " + name + " already running? "
                 "If not remove the pid file '" + pidfile + "' and try again.");
        }
    }
    if (!dnmi::file::createPidFile(pidfile)) {
        throw std::runtime_error("Could not write pid file '" + pidfile + "'.");
    }
}

PidFiler::~PidFiler()
{
    dnmi::file::deletePidFile(pidfile);
}

const char APP[] = "kvqc2d";

} // anonymous namespace

// ########################################################################

int main(int argc, char** argv)
{
    InitLogger(argc, argv, APP);
    milog::LogContext logContext(APP);
    LOGINFO("This is " << APP << " version " KVQC2D_VERSION_FULL);

    try {
        PidFiler pid(APP);

        boost::filesystem::path confPath = boost::filesystem::path(kvPath("sysconfdir")) / "kvalobs.conf";
        std::shared_ptr<miutil::conf::ConfSection> confSec(miutil::conf::ConfParser::parse(confPath.native()));
        if (!confSec) {
            LOGFATAL("Could not read config '" << confPath << "'");
            return 1;
        }

        Qc2App app(argc, argv, confSec);
        app.run();
    } catch (std::exception& e) {
        LOGFATAL("Exception: " << e.what());
        return 1;
    } catch (...) {
        LOGFATAL("Unknown exception");
        return 1;
    }
    return 0;
}
