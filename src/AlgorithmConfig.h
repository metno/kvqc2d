// -*- c++ -*-

#ifndef __ReadProgramOptions_h__
#define __ReadProgramOptions_h__

#include "ConfigParser.h"
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

public:
    void setConfigPath(const boost::filesystem::path& path);

    template<typename T>
    T getParameter(const std::string& name, const T& dflt) const;

    template<typename T>
    T getParameter(const std::string& name) const;

    template<typename T>
    std::vector<T> getMultiParameter(const std::string& name) const;

    void getFlagSetCU(FlagSetCU& fcu, const std::string& name) const;
    void getFlagChange(FlagChange& fc, const std::string& name) const;

    ErrorList check() const
        { return c.checkUnrequested(); }

private:
    void getFlagSet(FlagPatterns& f, const std::string& name, FlagPattern::FlagType type) const;

public:
    static const std::string CFG_EXT;

private:
    boost::filesystem::path mConfigPath;
    const vector_uchar Vfull; // TODO could also be static

    ConfigParser c;
};

template<typename T>
T AlgorithmConfig::getParameter(const std::string& name, const T& dflt) const
{
    if( !c.has(name) )
        return dflt;
    const ConfigParser::Item& item = c.get(name);
    if( item.count() != 1 )
        throw ConfigException("setting '" + name + "' has != 1 value");
    return item.convert<T>(0);
}

template<typename T>
T AlgorithmConfig::getParameter(const std::string& name) const
{
    if( !c.has(name) )
        throw ConfigException("no such setting: '" + name + "'");
    const ConfigParser::Item& item = c.get(name);
    if( item.count() != 1 )
        throw ConfigException("setting '" + name + "' has != 1 value");
    return item.convert<T>(0);
}

template<typename T>
std::vector<T> AlgorithmConfig::getMultiParameter(const std::string& name) const
{
    if( !c.has(name) )
        throw ConfigException("no such setting: '" + name + "'");
    return c.get(name).convert<T>();
}

#endif