#ifndef __ReadProgramOptions_h__
#define __ReadProgramOptions_h__

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>

#include <vector>
#include <map>
#include <puTools/miTime.h>

#include<string>

///Selects and reads the configuration files driving each of the Qc2 algorithms.

class ReadProgramOptions{

public:
ReadProgramOptions();
~ReadProgramOptions(){};
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

std::string NeighbourFilename;
std::string CFAILED_STRING;

int RunAtMinute;
int RunAtHour;
int pid;
int maxpid,minpid;
int tid;
int missing;
int MinimumValue;
float InterpolationLimit;
bool newfile;
std::map<int, std::vector<unsigned char> > zflag;
std::map<int, std::vector<unsigned char> > Rflag;    //Requirements for reading 
std::map<int, std::vector<unsigned char> > Iflag;    //Requirements for interpolation
std::map<int, std::vector<unsigned char> > Aflag;    //Requirements for analysis
std::map<int, std::vector<unsigned char> > Notflag;  //Restrictions for analysis
std::map<int, std::vector<unsigned char> > Wflag;    //Requirements for write
std::map<int, std::vector<unsigned char> > Uflag;    //Requirements for User flag 
std::map<int, std::vector<unsigned char> > NotUflag; //Requirements for not User flag 
std::map<int, unsigned char> Sflag;                  //Flags to set
std::map<int, std::vector<std::string> > chflag;

std::vector<unsigned char> Vfqclevel,Vfr,Vfcc,Vfs,Vfnum,Vfpos,Vfmis,Vftime,Vfw,Vfstat,Vfcp,Vfclim,Vfd,Vfpre,Vfcombi,Vfhqc;

int Parse(std::string filename);
int SelectConfigFiles(std::vector<std::string>& config_files);
int clear();

std::vector<unsigned char> Vfull;

};


/** @} */
#endif
