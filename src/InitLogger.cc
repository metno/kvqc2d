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

#include "InitLogger.h"
#include <kvalobs/kvPath.h>
#include <milog/milog.h>

#include <boost/filesystem/path.hpp>
#include <iostream>

namespace fs = boost::filesystem;

namespace{
    milog::LogLevel getLogLevel(const char *str) {
	if(strcmp("FATAL", str)==0){
	    return milog::FATAL;
	}else if(strcmp("ERROR", str)==0){
	    return milog::ERROR;
	}else if(strcmp("WARN", str)==0){
	    return milog::WARN;
	}else if(strcmp("DEBUG", str)==0){
	    return milog::DEBUG;
	}else if(strcmp("INFO", str)==0){
	    return milog::INFO;
	}else if(strcmp("0", str)==0){
	    return milog::FATAL;
	}else if(strcmp("1", str)==0){
	    return milog::ERROR;
	}else if(strcmp("2", str)==0){
	    return milog::WARN;
	}else if(strcmp("3", str)==0){
	    return milog::INFO;
	}else if(strcmp("4", str)==0){
	    return milog::DEBUG;
	}else{
	    return milog::NOTSET;
	}
    }

#if BOOST_FILESYSTEM_VERSION >= 3
    inline std::string to_native_file(const boost::filesystem::path& path) {
        return path.native();
    }
#else
    inline std::string to_native_file(const boost::filesystem::path& path) {
        return path.native_file_string();
    }
#endif
}

void InitLogger(int argn, char **argv, const std::string &logname)
{
    milog::LogLevel     traceLevel= milog::DEBUG;
    milog::LogLevel     logLevel  = milog::INFO;
    milog::FLogStream *logFile;
    milog::LogStream  *logConsole;
    
    const fs::path localstate(kvPath("logdir"));
    fs::path filename = localstate;
    filename /= logname + ".log";
    const std::string logfilename = to_native_file(filename);
    
    for(int i=0; i<argn; i++){
	if(strcmp("--tracelevel", argv[i])==0){
	    i++;
	    
	    if(i<argn){
	      traceLevel=getLogLevel(argv[i]);
	    }
	}else if(strcmp("--loglevel", argv[i])==0){
	    i++;
	    
	    if(i<argn){
		logLevel=getLogLevel(argv[i]);
	    }
	}
    }
    
    try {
	logFile = new milog::FLogStream(4, 1<<20);
	if( !logFile->open(logfilename) ) {
	    std::cerr << "FATAL: Can't initialize the Logging system.\n";
	    std::cerr << "------ Cant open the Logfile '" << logfilename << "'\n";
	    delete logFile;
	    exit(1);
	}
	
	logConsole = new milog::StdErrStream();
    
	if( !milog::LogManager::createLogger(logname, logConsole) ) {
	    std::cerr << "FATAL: Cannot create console logger" << std::endl;
	    exit(1);
	}
	
	if( !milog::LogManager::addStream(logname, logFile) ) {
	    std::cerr << "FATAL: Cannot add file-logging" << std::endl;
	    exit(1);
	}
	
	logConsole->loglevel(traceLevel);
	logFile   ->loglevel(logLevel);
	
	milog::LogManager::setDefaultLogger(logname);
    } catch(...) {
	std::cerr << "FATAL: Can't initialize the Logging system." << std::endl;
	exit(1);
    }

    std::cerr << "Logging to file '" << logfilename << "'\n";
}
