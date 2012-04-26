/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

  Contact information:
  Norwegian Meteorological Institute
  Postboks 43 Blindern
  N-0313 OSLO
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

#ifndef __ReadProgramOptions_h__
#define __ReadProgramOptions_h__

#include "helpers/ConfigParser.h"
#include "FlagChange.h"
#include "FlagPatterns.h"
#include <puTools/miTime.h>

#include <boost/filesystem/path.hpp>
#include <exception>
#include <istream>
#include <map>
#include <string>
#include <vector>


class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& what)
        : std::runtime_error(what) { }
};

///Selects and reads the configuration files driving each of the Qc2 algorithms.

class AlgorithmConfig{

public:
    AlgorithmConfig();
    ~AlgorithmConfig(){};

    typedef std::vector<unsigned char> vector_uchar;

    miutil::miTime UT0;
    miutil::miTime UT1;

    std::string Algorithm;
    std::string CFAILED_STRING;

    int RunAtMinute;
    int RunAtHour;

    float missing;
    float rejected;

    void Parse(const std::string& filename);
    void Parse(std::istream& input);
    bool SelectConfigFiles(std::vector<std::string>& config_files);
    const std::string& filename() const
        { return mFilename; }

public:
    void setConfigPath(const boost::filesystem::path& path);

    template<typename T>
    T getParameter(const std::string& name, const T& dflt) const;

    template<typename T>
    T getParameter(const std::string& name) const;

    template<typename T>
    std::vector<T> getMultiParameter(const std::string& name) const;

    void getFlagSetCU(FlagSetCU& fcu, const std::string& name, const std::string& dfltC="", const std::string& dfltU="") const;
    void getFlagChange(FlagChange& fc, const std::string& name, const std::string& dflt="") const;

    ErrorList check() const
        { return c.checkUnrequested(); }

private:
    void getFlagSet(FlagPatterns& f, const std::string& name, FlagPattern::FlagType type) const;
    void ParseStream(std::istream& input);
    void ParseStreamThrow(std::istream& input);

public:
    static const std::string CFG_EXT;

private:
    boost::filesystem::path mConfigPath;
    const vector_uchar Vfull; // TODO could also be static

    ConfigParser c;
    std::string mFilename;
};

template<typename T>
T AlgorithmConfig::getParameter(const std::string& name, const T& dflt) const
{
    if( !c.has(name) )
        return dflt;
    const ConfigParser::Item& item = c.get(name);
    if( item.count() != 1 )
        throw ConfigException("setting '" + name + "' has != 1 value");
    try {
        return item.convert<T>(0);
    } catch( ConvertException& e ) {
        throw ConfigException("problem converting value for '" + name + "': " + e.what());
    }
}

template<typename T>
T AlgorithmConfig::getParameter(const std::string& name) const
{
    if( !c.has(name) )
        throw ConfigException("no such setting: '" + name + "'");
    const ConfigParser::Item& item = c.get(name);
    if( item.count() != 1 )
        throw ConfigException("setting '" + name + "' has != 1 value");
    try {
        return item.convert<T>(0);
    } catch( ConvertException& e ) {
        throw ConfigException("problem converting value for '" + name + "': " + e.what());
    }
}

template<typename T>
std::vector<T> AlgorithmConfig::getMultiParameter(const std::string& name) const
{
    if( !c.has(name) )
        throw ConfigException("no such setting: '" + name + "'");
    try {
        return c.get(name).convert<T>();
    } catch( ConvertException& e ) {
        throw ConfigException("problem converting value for '" + name + "': " + e.what());
    }
}

#endif
