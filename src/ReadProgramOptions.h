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

bool zbool,Rbool,Ibool,Abool,Wbool;

std::string NeighbourFilename;
std::string CFAILED_STRING;

int RunAtMinute;
int RunAtHour;
int pid;
int tid;
int missing;
int MinimumValue;
float InterpolationLimit;
bool newfile;
std::map<int, unsigned char> zflag;
std::map<int, unsigned char> Rflag;
std::map<int, unsigned char> Iflag;
std::map<int, unsigned char> Aflag;
std::map<int, unsigned char> Wflag;
std::map<int, unsigned char> Sflag;

std::vector<std::string> ch_fqclevel,ch_fr,ch_fcc,ch_fs,ch_fnum,ch_fpos,ch_fmis,ch_ftime,ch_fw,ch_fstat,ch_fcp,ch_fclim,ch_fd,ch_fpre,ch_fcombi,ch_fhqc;
std::vector<unsigned char> Vfqclevel,Vfr,Vfcc,Vfs,Vfnum,Vfpos,Vfmis,Vftime,Vfw,Vfstat,Vfcp,Vfclim,Vfd,Vfpre,Vfcombi,Vfhqc;

int Parse(std::string filename);
int SelectConfigFiles(std::vector<std::string>& config_files);
int clear();
};


/** @} */
#endif
