// -*- c++ -*-

#ifndef __ReadProgramOptions_h__
#define __ReadProgramOptions_h__

#include "ConfigParser.h"
#include "FlagSet.h"
#include <puTools/miTime.h>

#include <boost/filesystem/path.hpp>
#include <istream>
#include <map>
#include <string>
#include <vector>

///Selects and reads the configuration files driving each of the Qc2 algorithms.

class ReadProgramOptions{

public:
    ReadProgramOptions();
    ~ReadProgramOptions(){};

    typedef std::vector<unsigned char> vector_uchar;

    miutil::miTime UT0;
    miutil::miTime UT1;

    int AlgoCode;
    int InterpCode;

    bool Wbool;

    std::string Algorithm;                  
    std::string NeighbourFilename;
    std::string ParValFile;
    std::string CFAILED_STRING;

    int RunAtMinute;
    int RunAtHour;
    int pid;
    int tid;
    int nibble_index;
    std::vector<int> tids;
    float missing;
    float rejected;
    float InterpolationLimit;
    int Ngap;
    std::map<int, vector_uchar> Iflag;    //Requirements for interpolation
    std::map<int, vector_uchar> Aflag;    //Requirements for analysis
    std::map<int, vector_uchar> Notflag;  //Restrictions for analysis
    std::map<int, vector_uchar> Wflag;    //Requirements for write
    std::map<int, vector_uchar> Uflag;    //Requirements for User flag 
    std::map<int, vector_uchar> NotUflag; //Requirements for not User flag 
    std::map<int, unsigned char> Sflag;                  //Flags to set
    std::map<int, std::vector<std::string> > chflag;

    int Parse(const std::string& filename);
    int Parse(std::istream& input);
    bool SelectConfigFiles(std::vector<std::string>& config_files);
    int clear();

public:
    void setConfigPath(const boost::filesystem::path& path);

    bool getFlagSet(FlagSet& flags, const std::string& name) const;

private:
    boost::filesystem::path mConfigPath;
    const vector_uchar Vfull; // TODO could also be static

    // FIXME what are these used for?
    int StepD;
    int StepH;
    bool zbool,Rbool,Ibool,Abool,Notbool,Ubool,NotUbool;
    std::string InFlagFilename;
    std::string OutFlagFilename;
    int maxpid,minpid;
    float MinimumValue;
    float delta;
    std::string ControlInfoString;
    std::vector<int> ControlInfoVector;
    std::map<int, vector_uchar> zflag;
    std::map<int, vector_uchar> Rflag;    //Requirements for reading
    vector_uchar Vfqclevel,Vfr,Vfcc,Vfs,Vfnum,Vfpos,Vfmis,Vftime,Vfw,Vfstat,Vfcp,Vfclim,Vfd,Vfpre,Vfcombi,Vfhqc;

    ConfigParser c;
};

/** @} */
#endif
