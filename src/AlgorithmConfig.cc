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

#include "AlgorithmConfig.h"

#include "helpers/Helpers.h"

#include <kvalobs/kvPath.h>
#include <milog/milog.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <fstream>
#include <sstream>
#include <vector>

#define NDEBUG
#include "debug.h"

namespace fs = boost::filesystem;

namespace {

void extractTime(const ConfigParser& c, const std::string& prefix, kvtime::time& time)
{
    const int Year   = c.get(prefix+"_YYYY").convert<int>(0, kvtime::year (time));
    const int Month  = c.get(prefix+"_MM")  .convert<int>(0, kvtime::month(time));
    const int Day    = c.get(prefix+"_DD")  .convert<int>(0, kvtime::day  (time));
    const int Hour   = c.get(prefix+"_hh")  .convert<int>(0, kvtime::hour (time));
    const int Minute = c.get(prefix+"_mm")  .convert<int>(0, 0);
    const int Second = c.get(prefix+"_ss")  .convert<int>(0, 0);
    time = kvtime::maketime(Year, Month, Day, Hour, Minute, Second);
}

void extractHHMMSS(const ConfigParser& c, const std::string& prefix, kvtime::time& time)
{
    if( c.has(prefix + "_hh") )
        kvtime::addHours(time, -kvtime::hour(time) + c.get(prefix + "_hh").convert<int>(0));
    if( c.has(prefix + "_mm") )
        kvtime::addMinutes(time, -kvtime::minute(time)   + c.get(prefix + "_mm").convert<int>(0));
    if( c.has(prefix + "_ss") )
        kvtime::addSeconds(time, -kvtime::second(time)   + c.get(prefix + "_ss").convert<int>(0));
}

#if BOOST_FILESYSTEM_VERSION >= 3
    inline std::string to_native_file(const boost::filesystem::path& path) {
        return path.native();
    }
    inline std::string to_native_dir(const boost::filesystem::path& path) {
        return path.native();
    }
#else
    inline std::string to_native_file(const boost::filesystem::path& path) {
        return path.native_file_string();
    }
    inline std::string to_native_dir(const boost::filesystem::path& path) {
        return path.native_directory_string();
    }
#endif

} // anonymous namespace

const char AlgorithmConfig::CFG_EXT[] = ".cfg2";

AlgorithmConfig::AlgorithmConfig()
{
    fs::initial_path();
    setConfigPath( fs::path(kvPath("sysconfdir")) / "Qc2Config" );
}

void AlgorithmConfig::setConfigPath(const fs::path& path)
{
    // this will throw an exception in some unusual cases on Windows systems
#if BOOST_FILESYSTEM_VERSION >= 3
    mConfigPath = boost::filesystem3::complete( path );
#else
    mConfigPath = fs::complete( path );
#endif
}

///Scans $KVALOBS/Qc2Config and searched for configuration files "*.cfg".

bool AlgorithmConfig::SelectConfigFiles(std::vector<std::string>& config_files)
{
    // TODO this has little to do with qc2 configuration files, move it elsewhere
    config_files.clear();
    if( !fs::exists( mConfigPath ) || !fs::is_directory( mConfigPath )) {
        LOGWARN("Not a directory: '" << to_native_file(mConfigPath) << "'");
        return false;
    }

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
#if BOOST_VERSION <= 103500
            const std::string name = dit->leaf();
#else
            const std::string name = to_native_file(dit->path().filename());
#endif
            if( boost::algorithm::ends_with(name, CFG_EXT) ) {
                const std::string& n = to_native_file(dit->path());
                config_files.push_back(n);
                //LOGINFO("Found configuration file '" << n << "'");
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

void AlgorithmConfig::Parse(const std::string& filename)
{
    mFilename = filename;
    std::ifstream input(filename.c_str());
    ParseStream(input);
}

void AlgorithmConfig::Parse(std::istream& input)
{
    mFilename = "//stream//";
    ParseStream(input);
}

void AlgorithmConfig::ParseStream(std::istream& input)
{
    try {
        ParseStreamThrow(input);
    } catch( ConvertException& e ) {
        throw ConfigException(std::string("problem with parsing configuration: ") + e.what());
    }
}

void AlgorithmConfig::ParseStreamThrow(std::istream& input)
{
    if( !c.load(input) )
        throw ConfigException("Problems parsing kvqc2d algorithm configuration: " + c.errors().format("; ") + " -- giving up!");

    kvtime::time now = kvtime::now();
    kvtime::addSeconds(now, -kvtime::second(now));
    kvtime::addMinutes(now, -kvtime::minute(now));

    // see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:user:config_summary (bottom) for some hints
    // also https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:user:configuration

    RunAtMinute = c.get("RunAtMinute").convert<int>(0, 0); // Minute at which to run the algorithm
    RunAtHour   = c.get("RunAtHour")  .convert<int>(0, 2); // Hour at which to run the algorithm

    UT1 = UT0 = now;

    if( c.has("Last_NDays") ) {
        extractHHMMSS(c, "Start", UT0);
        UT1 = UT0;
        extractHHMMSS(c, "End", UT1);

        kvtime::addDays(UT0, -c.get("Last_NDays").convert<int>(0) );
    } else {
        extractTime(c, "Start", UT0);
        extractTime(c, "End",   UT1);
    }

    Algorithm          = c.get("Algorithm")    .convert<std::string>(0, "NotSet"); // Algorithm Name
    CFAILED_STRING     = c.get("CfailedString").convert<std::string>(0, ""); // Value to add to CFAILED if the algorithm runs and writes data back to the database

    missing            = c.get("MissingValue") .convert<float>(0, -32767.0); // Original Missing Data Value
    rejected           = c.get("RejectedValue").convert<float>(0, -32766.0); // Original Rejected Data Value
}

bool AlgorithmConfig::hasParameter(const std::string& name) const
{
    return c.has(name);
}

void AlgorithmConfig::getFlagSet(FlagPatterns& flags, const std::string& name, FlagPattern::FlagType type) const
{
    flags.reset();
    if( !c.has(name) )
        throw ConfigException("no such flag spec: '" + name + "'");
    const ConfigParser::Item& item = c.get(name);
    for(int i=0; i<item.count(); ++i) {
        const std::string value = item.convert<std::string>(i);
        if( !flags.parse(value, type) )
            throw ConfigException("error parsing flag '" + name + "' from '" + value + "'");
    }
}

void AlgorithmConfig::getFlagSetCU(FlagSetCU& fcu, const std::string& name, const std::string& dfltC, const std::string& dfltU) const
{
    const bool hasC = c.has(name + "_cflags"), hasU = c.has(name + "_uflags");
    if( !hasC && dfltC.empty() && !hasU && dfltU.empty() )
        throw ConfigException("no setting for '" + name + "'");

    if( hasC )
        getFlagSet(fcu.controlflags(), name + "_cflags", FlagPattern::CONTROLINFO);
    else if( !dfltC.empty() && !hasU ) {
        fcu.controlflags().reset();
        if( !fcu.controlflags().parse(dfltC, FlagPattern::CONTROLINFO) )
            throw ConfigException("error parsing default flags '" + dfltC + "' for '" + name + "_cflags'");
    }

    if( hasU )
        getFlagSet(fcu.useflags(),     name + "_uflags", FlagPattern::USEINFO);
    else if( !dfltU.empty() && !hasC ) {
        fcu.useflags().reset();
        if( !fcu.useflags().parse(dfltU, FlagPattern::USEINFO) )
            throw ConfigException("error parsing default flags '" + dfltU + "' for '" + name + "_uflags'");
    }
}

void AlgorithmConfig::getFlagChange(FlagChange& fc, const std::string& name, const std::string& dflt) const
{
    fc.reset();
    const bool has = c.has(name);
    if( !has && dflt.empty() )
        throw ConfigException("no such flag change: '" + name + "'");
    if( has ) {
        const ConfigParser::Item& item = c.get(name);
        for(int i=0; i<item.count(); ++i) {
            const std::string value = item.convert<std::string>(i);
            if( !fc.parse(value) )
                throw ConfigException("error parsing flag update '" + name + "' from '" + value + "'");
        }
    } else {
        if( !fc.parse(dflt) )
            throw ConfigException("error parsing default flag update '" + dflt + "' for '" + name + "'");
    }
}
