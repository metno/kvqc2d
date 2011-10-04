// -*- c++ -*-

#ifndef __ReadProgramOptions_h__
#define __ReadProgramOptions_h__

#include <puTools/miTime.h>

#include <vector>
#include <map>
#include <string>

///Selects and reads the configuration files driving each of the Qc2 algorithms.

class ReadProgramOptions{

public:
    ReadProgramOptions();
    ~ReadProgramOptions(){};

    typedef std::vector<unsigned char> vector_uchar;

    //std::vector<miutil::miTime> UT0;
    //std::vector<miutil::miTime> UT1;
    miutil::miTime UT0;
    miutil::miTime UT1;

    int StepD;
    int StepH;
    int AlgoCode;
    int InterpCode;
    int LastN;
    std::string ControlInfoString;
    std::vector<int> ControlInfoVector;

    bool zbool,Rbool,Ibool,Abool,Notbool,Wbool,Ubool,NotUbool;

    std::string Algorithm;                  
    std::string NeighbourFilename;
    std::string ParValFile;
    std::string InFlagFilename;
    std::string OutFlagFilename;
    std::string CFAILED_STRING;

    int RunAtMinute;
    int RunAtHour;
    int pid;
    int maxpid,minpid;
    int tid;
    int nibble_index;
    std::vector<int> tids;
    std::map<int,float> parvals;
    float missing;
    float rejected;
    float MinimumValue;
    float InterpolationLimit;
    int Ngap;
    float delta;
    std::map<int, vector_uchar> zflag;
    std::map<int, vector_uchar> Rflag;    //Requirements for reading 
    std::map<int, vector_uchar> Iflag;    //Requirements for interpolation
    std::map<int, vector_uchar> Aflag;    //Requirements for analysis
    std::map<int, vector_uchar> Notflag;  //Restrictions for analysis
    std::map<int, vector_uchar> Wflag;    //Requirements for write
    std::map<int, vector_uchar> Uflag;    //Requirements for User flag 
    std::map<int, vector_uchar> NotUflag; //Requirements for not User flag 
    std::map<int, unsigned char> Sflag;                  //Flags to set
    std::map<int, std::vector<std::string> > chflag;

    vector_uchar Vfqclevel,Vfr,Vfcc,Vfs,Vfnum,Vfpos,Vfmis,Vftime,Vfw,Vfstat,Vfcp,Vfclim,Vfd,Vfpre,Vfcombi,Vfhqc;

    int Parse(const std::string& filename);
    int SelectConfigFiles(std::vector<std::string>& config_files);
    int clear();

    vector_uchar Vfull;

};

/** @} */
#endif
