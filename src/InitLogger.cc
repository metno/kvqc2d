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

#include "SingleFileLogStream.h"

#include <kvalobs/kvPath.h>
#include <milog/milog.h>

#include <boost/filesystem/path.hpp>
#include <memory>
#include <iostream>

namespace fs = boost::filesystem;

namespace {

milog::LogLevel getLogLevel(const std::string& str)
{
    if (str == "FATAL" || str == "0") {
        return milog::FATAL;
    } else if (str == "ERROR" || str == "1") {
        return milog::ERROR;
    } else if (str == "WARN" || str == "2") {
        return milog::WARN;
    } else if (str == "DEBUG" || str == "4") {
        return milog::DEBUG;
    } else if (str == "INFO" || str == "3") {
        return milog::INFO;
    } else {
        return milog::NOTSET;
    }
}

std::string default_logfilename(const std::string &logname)
{
    const fs::path localstate(kvPath("logdir"));
    fs::path filename = localstate;
    filename /= logname + ".log";
#if BOOST_FILESYSTEM_VERSION >= 3
    return filename.native();
#else
    return filename.native_file_string();
#endif
}

void fail_missing_arg(const std::string& option)
{
    std::cerr << "FATAL: Missing option argument for '" << option << "'. Exit." << std::endl;
    exit(1);
}

} // anonymous namespace

void InitLogger(int argn, char **argv, const std::string &logname)
{
    milog::LogLevel levelConsole = milog::ERROR;
    milog::LogLevel levelFile    = milog::INFO;

    std::string logfilename = default_logfilename(logname);
    bool rotating_logfile = true;

    for (int i=1; i<argn; i++) {
        const std::string argi(argv[i]);
        if (argi == "--tracelevel") {
            i++;
            if (i >= argn)
                fail_missing_arg(argi);
            levelConsole = getLogLevel(argv[i]);
        } else if (argi == "--loglevel" && i+1 < argn) {
            i++;
            if (i >= argn)
                fail_missing_arg(argi);
            levelFile = getLogLevel(argv[i]);
        } else if (argi == "--logfile" && i+1 < argn) {
            i++;
            if (i >= argn)
                fail_missing_arg(argi);
            logfilename = argv[i];
            rotating_logfile = false;
        }
    }

    try {
        std::auto_ptr<milog::LogStream> logFile;
        if (rotating_logfile) {
            std::auto_ptr<milog::FLogStream> filelog(new milog::FLogStream(4, 1<<24));
            if (filelog->open(logfilename))
                logFile = filelog;
        } else {
            std::auto_ptr<SingleFileLogStream> filelog(new SingleFileLogStream(logfilename));
            if (filelog->is_open())
                logFile = filelog;
        }
        if (!logFile.get()) {
            std::cerr << "FATAL: Can't initialize the Logging system.\n";
            std::cerr << "------ Cant open the Logfile '" << logfilename << "'\n";
            exit(1);
        }

        std::auto_ptr<milog::LogStream> logConsole(new milog::StdErrStream());

        logConsole->loglevel(levelConsole);
        logFile->loglevel(levelFile);

        if (!milog::LogManager::createLogger(logname, logConsole.release()) ) {
            std::cerr << "FATAL: Cannot create console logger" << std::endl;
            exit(1);
        }

        if (!milog::LogManager::addStream(logname, logFile.release())) {
            std::cerr << "FATAL: Cannot add file-logging" << std::endl;
            exit(1);
        }

        milog::LogManager::setDefaultLogger(logname);
    } catch(...) {
        std::cerr << "FATAL: Can't initialize the Logging system." << std::endl;
        exit(1);
    }

    std::cerr << "Logging to file '" << logfilename << "'\n";
}
