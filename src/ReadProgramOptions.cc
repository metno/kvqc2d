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

#include "ConfigParser.h"
#include "Helpers.h"

#include <kvalobs/kvPath.h>
#include <milog/milog.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

namespace fs = boost::filesystem;

namespace {
const char Vfull_values[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

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

const char* flagnames[16] = { "fqclevel", "fr", "fcc", "fs", "fnum", "fpos", "fmis", "ftime",
                              "fw", "fstat", "fcp", "fclim", "fd", "fpre", "fcombi", "fhqc"
};

template<class T>
void extractFlag(const ConfigParser& c, const std::string& prefix, 
                 std::map<int, std::vector<T> >& flag, bool& invert)
{
    for(int f=0; f<16; ++f)
        flag[f] = c.get(prefix+std::string("_")+flagnames[f]).convert<T>();

    invert = true;
    if( c.has(prefix+"bool") )
        invert = c.get(prefix+"bool").convert<bool>(0);
}

} // anonymous namespace

ReadProgramOptions::ReadProgramOptions()
    : Vfull(Vfull_values, Vfull_values + 16)
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

bool ReadProgramOptions::SelectConfigFiles(std::vector<string>& config_files)
{
    // TODO this has little to do with qc2 configuration files, move it elsewhere
    config_files.clear();
    if( !fs::exists( mConfigPath ) && !fs::is_directory( mConfigPath )) {
        LOGWARN("Not a directory: '" << mConfigPath.native_file_string() << "'");
        return false;
    }

    LOGINFO("Scanning for files in '" << mConfigPath.native_file_string() << "'");
    try {
        const fs::directory_iterator end;
        for( fs::directory_iterator dit( mConfigPath ); dit != end; ++dit ) {
            if( !fs::exists( dit->path()) )
                continue;
            if( fs::is_directory(*dit) )
                continue;
            if( Helpers::string_endswith(dit->path().filename(), ".cfg") ) {
                const std::string& n = dit->path().native_file_string();
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

int ReadProgramOptions::Parse(const std::string& filename)
{
    std::ifstream input(filename.c_str());
    return Parse(input);
}

int ReadProgramOptions::Parse(std::istream& input)
{
    ConfigParser c;
    if( !c.load(input) ) {
        std::ostringstream errors;
        for(int i=0; i<c.errors().size(); ++i)
            errors << c.errors().get(i) << std::endl;
        LOGWARN("Problems parsing kvqc2d algorithm configuration:" << std::endl
                << errors.str() 
                << "Continuing anyhow... good luck!");
    }

    const miutil::miTime now = miutil::miTime::nowTime();

    // see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:user:config_summary (bottom) for some hints

    RunAtMinute = c.get("RunAtMinute").convert<int>(0, 0); // Minute at which to run the algorithm
    RunAtHour   = c.get("RunAtHour")  .convert<int>(0, 2); // Hour at which to run the algorithm

    UT0 = now;
    UT1 = now;
    if( c.has("Last_NDays") ) {
        // Ho Ho Ho retain the option to run into the future 
        UT0.addDay( -c.get("Last_NDays").convert<int>(0) );
    } else {
        extractTime(c, "Start", UT0);
    }
    extractTime(c, "End", UT1); // TODO what if Last_NDays set, and also UT1? Move two lines up?

    // const int StepYear   = c.get("Step_YYYY").convert<int>(0, 0); // Step Year (to step through the data interval ...)
    // const int StepMonth  = c.get("Step_MM")  .convert<int>(0, 0); // Step Minute
    StepD   = c.get("Step_DD")  .convert<int>(0, 0); // Step Day
    StepH   = c.get("Step_hh")  .convert<int>(0, 0); // Step Hour
    // const int StepMinute = c.get("Step_mm")  .convert<int>(0, 0); // Step Minute
    // const int StepSecond = c.get("Step_ss")  .convert<int>(0, 0); // Step Second

    pid                = c.get("ParamId")              .convert<int>(0, 0); // Parameter ID
    maxpid             = c.get("MaxParamId")           .convert<int>(0, 0); // Parameter ID for a maximum value
    minpid             = c.get("MinParamId")           .convert<int>(0, 0); // Parameter ID for a minimum value

    tid                = c.get("TypeId")               .convert<int>(0, -1); // Type ID
    tids               = c.get("TypeIds")              .convert<int>(); // One of many Type IDs
    AlgoCode           = c.get("AlgoCode")             .convert<int>(0, -1); // Algorithm Code
    Algorithm          = c.get("Algorithm")            .value(0, "NotSet"); // Algorithm Name
    InterpCode         = c.get("InterpCode")           .convert<int>(0, -1); // Code to determine method of interpolation
    ControlInfoString  = c.get("ControlString")        .value(0, ""); // Control Info (not used)
    ControlInfoVector  = c.get("ControlVector")        .convert<int>(); // Control Vector (not used)
    nibble_index       = c.get("NibbleIndex")          .convert<int>(0, 15); // Index of the flag to check if data should be written back to the database. default=15(f_hqc)
    
    NeighbourFilename  = c.get("BestStationFilename")  .value(0, "NotSet"); // Filename containing the best station list
    ParValFile         = c.get("ParValFilename")       .value(0, "NotSet"); // Filename containing pairs of paramids and associated values
    InFlagFilename     = c.get("FlagsIn")              .value(0, "NotSet"); // Pathname for file containing controlinfo useifno test flag pairs
    OutFlagFilename    = c.get("FlagsOut")             .value(0, "NotSet"); // Pathname for results of flag tests. 
    CFAILED_STRING     = c.get("CfailedString")        .value(0, ""); // Value to add to CFAILED if the algorithm runs and writes data back to the database
    
    missing            = c.get("MissingValue")         .convert<float>(0, -32767.0); // Original Missing Data Value
    rejected           = c.get("RejectedValue")        .convert<float>(0, -32766.0); // Original Rejected Data Value
    delta              = c.get("DeltaValue")           .convert<float>(0, 0.0); // Delta Value for Dip Test (can be Øgland's Parameter for example
    MinimumValue       = c.get("MinValue")             .convert<float>(0, -32767.0); // Minimum Data Value For Some Controls
    InterpolationLimit = c.get("InterpolationDistance").convert<float>(0, 25); // Nearest Neighbour Limiting Distance
    Ngap               = c.get("MaxHalfGap")           .convert<int>(0, 0); // Maximum distance from a good neighbour for an Akima Interpolation:

    extractFlag(c, "z", zflag, zbool);
    extractFlag(c, "R", Rflag, Rbool);
    extractFlag(c, "I", Iflag, Ibool);
    extractFlag(c, "A", Aflag, Abool);
    extractFlag(c, "Not", Notflag, Notbool);
    extractFlag(c, "W", Wflag, Wbool);
    bool dummy = true;
    extractFlag(c, "change", chflag, dummy);

    Ubool    = c.get("Ubool")   .convert<bool>(0, true);
    NotUbool = c.get("NotUbool").convert<bool>(0, true);

    for(int i=0; i<16; ++i) {
        std::ostringstream key;
        key << "U_" << i;
        Uflag[i]    = c.get(        key.str()).convert<unsigned char>();
        NotUflag[i] = c.get("Not" + key.str()).convert<unsigned char>();
    }

    for(int i=0; i<16; ++i) {
        std::string key = std::string("S_") + flagnames[i];
        if( c.has(key) )
            Sflag[i] = c.get(key).convert<unsigned char>(0, '?');
    }

    for(int i=0; i<16; ++i) {
        std::string key = std::string("change_") + flagnames[i];
        if( c.has(key) )
            chflag[i] = c.get(key).values();
    }

    vector_uchar* Vflag[16] = { &Vfqclevel, &Vfr, &Vfcc, &Vfs, &Vfnum, &Vfpos, &Vfmis, &Vftime,
                                &Vfw, &Vfstat, &Vfcp, &Vfclim, &Vfd, &Vfpre, &Vfcombi, &Vfhqc };
    for(int i=0; i<16; ++i) {
        std::ostringstream key;
        key << "V_" << flagnames[i];
        *Vflag[i] = c.get(key.str()).convert<unsigned char>();
    }
    
    /// If no specific flag is set then the algorithm shall run for all flags.
    bool Aflag_all_empty = true;
    for (int i=0; Aflag_all_empty && i<16; i++)
        Aflag_all_empty &= Aflag[i].empty();
    if( Aflag_all_empty ) {
        for (int i=0; i<16; i++)
            Aflag[i] = Vfull;
    }
    
    /// If no specific flag is set then the algorithm shall run for all flags.
    bool Uflag_all_empty = true;
    for (int i=0; Uflag_all_empty && i<16; i++)
        Uflag_all_empty &= Uflag[i].empty();
    if( Uflag_all_empty ) {
        for (int i=0; i<16; i++)
            Uflag[i] = Vfull;
    }
    
    return 0;
}

int ReadProgramOptions::clear()
{
    zflag   .clear();
    Rflag   .clear();
    Iflag   .clear();
    Aflag   .clear();
    Wflag   .clear();
    Sflag   .clear();
    Uflag   .clear();
    NotUflag.clear();
    Notflag .clear();
    chflag  .clear();

    miutil::miTime UT0(1900,1,1,0,0,0);
    miutil::miTime UT1(1900,1,1,0,0,0);

    StepD=0;
    StepH=0;
    AlgoCode=-1;
    Algorithm="NotSet";
    InterpCode=-1;
    std::string ControlInfoString;       ///Check these are cleared correctly
    std::vector<int> ControlInfoVector;  ///TBD

    RunAtMinute=0;
    RunAtHour=2;
    pid=0;
    maxpid=0;
    minpid=0;
    tid=0;
    nibble_index=15;  // Always set back to HQC by default
    tids.clear();

    return 0;
}
//miutil::miTime UT0;
//miutil::miTime UT1;
//int StepD;
//int StepH;
//int AlgoCode;
//std::string ControlInfoString;
//std::vector<int> ControlInfoVector;
//int RunAtMinute;
//int RunAtHour;
//int pid;
//int tid;
//bool newfile;
