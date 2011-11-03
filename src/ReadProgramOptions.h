// -*- c++ -*-

#ifndef __ReadProgramOptions_h__
#define __ReadProgramOptions_h__

#include "ConfigParser.h"
#include "FlagChange.h"
#include "FlagSet.h"
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

class ReadProgramOptions{

public:
    ReadProgramOptions();
    ~ReadProgramOptions(){};

    typedef std::vector<unsigned char> vector_uchar;

    miutil::miTime UT0;
    miutil::miTime UT1;

    std::string Algorithm;
    std::string CFAILED_STRING;

    int RunAtMinute;
    int RunAtHour;
    int pid;
    int tid;
    std::vector<int> tids;
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

    void getFlagSet(FlagSet& f, const std::string& name) const;
    void getFlagSetCU(FlagSetCU& fcu, const std::string& name) const;
    void getFlagChange(FlagChange& fc, const std::string& name) const;

private:
    boost::filesystem::path mConfigPath;
    const vector_uchar Vfull; // TODO could also be static

    ConfigParser c;
};

template<typename T>
T ReadProgramOptions::getParameter(const std::string& name, const T& dflt) const
{
    if( !c.has(name) )
        return dflt;
    const ConfigParser::Item& item =c.get(name);
    if( item.count() != 1 )
        throw ConfigException("setting '" + name + "' has != 1 value");
    return item.convert<T>(0);
}

template<typename T>
T ReadProgramOptions::getParameter(const std::string& name) const
{
    if( !c.has(name) )
        throw ConfigException("no such setting: '" + name + "'");
    const ConfigParser::Item& item =c.get(name);
    if( item.count() != 1 )
        throw ConfigException("setting '" + name + "' has != 1 value");
    return item.convert<T>(0);
}

#endif
