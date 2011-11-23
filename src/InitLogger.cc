/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id: InitLogger.cc,v 1.1.2.3 2007/09/27 09:02:20 paule Exp $                                                       

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
	if( !logFile->open( filename.native_file_string()) ) {
	    std::cerr << "FATAL: Can't initialize the Logging system.\n";
	    std::cerr << "------ Cant open the Logfile <" << filename.native_file_string() << ">\n";
	    delete logFile;
	    exit(1);
	}
	
	logConsole = new milog::StdErrStream();
    
	if( !milog::LogManager::createLogger(logname, logConsole) ) {
	    std::cerr << "FATAL: Can't initialize the Logging system.\n";
	    std::cerr << "------ Cant create logger\n";
	    exit(1);
	}
	
	if( !milog::LogManager::addStream(logname, logFile) ) {
	    std::cerr << "FATAL: Can't initialize the Logging system.\n";
	    std::cerr << "------ Cant add filelogging to the Logging system\n";
	    exit(1);
	}
	
	logConsole->loglevel(traceLevel);
	logFile   ->loglevel(logLevel);
	
	milog::LogManager::setDefaultLogger(logname);
    } catch(...) {
	std::cerr << "FATAL: Can't initialize the Logging system.\n";
	std::cerr << "------ OUT OF MEMMORY!!!\n";
	exit(1);
    }

    std::cerr << "Logging to file <" << filename.native_file_string() << ">!\n";
}
