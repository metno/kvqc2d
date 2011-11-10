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

#include "ReadProgramOptions.h"

#include "Helpers.h"

#include <kvalobs/kvPath.h>
#include <milog/milog.h>

#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/version.hpp>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = boost::filesystem;

namespace {

void extractTime(const ConfigParser& c, const std::string& prefix, miutil::miTime& time)
{
    const int Year   = c.get(prefix+"_YYYY").convert<int>(0, time.year());
    const int Month  = c.get(prefix+"_MM")  .convert<int>(0, time.month());
    const int Day    = c.get(prefix+"_DD")  .convert<int>(0, time.day());
    const int Hour   = c.get(prefix+"_hh")  .convert<int>(0, time.hour());
    const int Minute = c.get(prefix+"_mm")  .convert<int>(0, 0);
    const int Second = c.get(prefix+"_ss")  .convert<int>(0, 0);
    time = miutil::miTime(Year, Month, Day, Hour, Minute, Second);
}

//const char* flagnames[16] = { "fqclevel", "fr", "fcc", "fs", "fnum", "fpos", "fmis", "ftime",
//                              "fw", "fstat", "fcp", "fclim", "fd", "fpre", "fcombi", "fhqc"
//};

} // anonymous namespace

const std::string ReadProgramOptions::CFG_EXT = ".cfg2";

ReadProgramOptions::ReadProgramOptions()
{
    fs::initial_path();
    setConfigPath( fs::path(kvPath("sysconfdir")) / "Qc2Config" );
}

void ReadProgramOptions::setConfigPath(const fs::path& path)
{
    // this will throw an exception in some unusual cases on Windows systems
    mConfigPath = fs::complete( path );
}

///Scans $KVALOBS/Qc2Config and searched for configuration files "*.cfg".

bool ReadProgramOptions::SelectConfigFiles(std::vector<std::string>& config_files)
{
    // TODO this has little to do with qc2 configuration files, move it elsewhere
    config_files.clear();
    if( !fs::exists( mConfigPath ) || !fs::is_directory( mConfigPath )) {
#if BOOST_VERSION <= 103500
        LOGWARN("Not a directory: '" << mConfigPath.native_file_string() << "'");
#elif !defined(BOOST_VERSION)
#error "BOOST_VERSION not defined"
#else
        LOGWARN("Not a directory: '" << mConfigPath.file_string() << "'");
#endif
        return false;
    }

    LOGINFO("Scanning for files in '" << mConfigPath.native_file_string() << "'");
    try {
        const fs::directory_iterator end;
        for( fs::directory_iterator dit( mConfigPath ); dit != end; ++dit ) {
            if( !fs::exists(
#if BOOST_VERSION <= 103500
                    *dit
#else
                    dit->path()
#endif
                    ) )
                continue;
            if( fs::is_directory(*dit) )
                continue;
            if( Helpers::string_endswith(
#if BOOST_VERSION <= 103500
                    dit->leaf(),
#else
                    dit->path().filename(),
#endif
                    CFG_EXT) )
            {
#if BOOST_VERSION <= 103500
                const std::string& n = dit->native_file_string();
#else
                const std::string& n = dit->path().file_string();
#endif
                config_files.push_back(n);
                LOGINFO("Found configuration file '" << n << "'");
            }
        }

        // ordering of files listed from directory_iterator is not defined
        std::sort(config_files.begin(), config_files.end());
    } catch( fs::filesystem_error& e ) {
        LOGERROR("Error scanning configuration files for Qc2:" << e.what());
        return false;
    }
    return true;
}

void ReadProgramOptions::Parse(const std::string& filename)
{
    std::ifstream input(filename.c_str());
    Parse(input);
}

void ReadProgramOptions::Parse(std::istream& input)
{
    if( !c.load(input) ) {
        std::ostringstream errors;
        for(int i=0; i<c.errors().size(); ++i)
            errors << c.errors().get(i) << std::endl;
        throw ConfigException("Problems parsing kvqc2d algorithm configuration:\n" + errors.str() + "Giving up!");
    }

    const miutil::miTime now = miutil::miTime::nowTime();

    // see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:user:config_summary (bottom) for some hints
    // also https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:user:configuration

    RunAtMinute = c.get("RunAtMinute").convert<int>(0, 0); // Minute at which to run the algorithm
    RunAtHour   = c.get("RunAtHour")  .convert<int>(0, 2); // Hour at which to run the algorithm

    UT0 = now;
    UT1 = now;
    if( c.has("Last_NDays") ) {
        // Ho Ho Ho retain the option to run into the future
        UT0.addDay( -c.get("Last_NDays").convert<int>(0) );
    } else {
        extractTime(c, "Start", UT0);
        extractTime(c, "End",   UT1);
    }

    Algorithm          = c.get("Algorithm")            .value(0, "NotSet"); // Algorithm Name
    CFAILED_STRING     = c.get("CfailedString")        .value(0, ""); // Value to add to CFAILED if the algorithm runs and writes data back to the database

    missing            = c.get("MissingValue")         .convert<float>(0, -32767.0); // Original Missing Data Value
    rejected           = c.get("RejectedValue")        .convert<float>(0, -32766.0); // Original Rejected Data Value
}

void ReadProgramOptions::getFlagSet(FlagSet& flags, const std::string& name) const
{
    flags.reset();
    if( !c.has(name) )
        throw ConfigException("no such flag spec: '" + name + "'");
    const ConfigParser::Item& item = c.get(name);
    for(int i=0; i<item.count(); ++i) {
        if( !flags.parse(item.value(i)) )
            throw ConfigException("error parsing flag '" + name + "' from '" + item.value(i) + "'");
    }
}

void ReadProgramOptions::getFlagSetCU(FlagSetCU& fcu, const std::string& name) const
{
    const bool hasC = c.has(name + "_cflags"), hasU = c.has(name + "_uflags");
    if( !hasC && !hasU )
        throw ConfigException("no setting for '" + name + "'");
    if( hasC )
        getFlagSet(fcu.controlflags(), name + "_cflags");
    if( hasU )
        getFlagSet(fcu.useflags(),     name + "_uflags");
}

void ReadProgramOptions::getFlagChange(FlagChange& fc, const std::string& name) const
{
    fc.reset();
    if( !c.has(name) )
        throw ConfigException("no such flag change: '" + name + "'");
    const ConfigParser::Item& item = c.get(name);
    for(int i=0; i<item.count(); ++i) {
        if( !fc.parse(item.value(i)) )
            throw ConfigException("error parsing flag update '" + name + "' from '" + item.value(i) + "'");
    }
}
