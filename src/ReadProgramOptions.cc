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
#include <iostream>
#include <sstream>
#include <math.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <kvalobs/kvPath.h>

#include "ReadProgramOptions.h"

using namespace std;

namespace po = boost::program_options;
namespace fs = boost::filesystem;


ReadProgramOptions::ReadProgramOptions()
{
   newfile=false;
}


///Scans $KVALOBS/Qc2Config and searched for configuration files "*.cfg". 

int 
ReadProgramOptions::
SelectConfigFiles( std::vector<string>& config_files)
{

try{
       //fs::path config_path(string(pKv)+"/Qc2Config/");
       fs::path config_path(kvPath("sysconfdir")+"/Qc2Config/");
       
       // scan for files
       std::cout << "Scanning For Files" << std::endl;   
       fs::directory_iterator end_iter; // By default this is after the end of a directory !
       fs::path full_path( fs::initial_path() );
       full_path = fs::system_complete( config_path );
       
       std::string filename;

       if ( !fs::exists( full_path ) ) {
           //std::cout << "Does not exist: " << full_path.native_file_string() <<std::endl;
           std::cout << "Does not exist: " << full_path.file_string() <<std::endl;
       }
       else {
           for ( fs::directory_iterator dit( full_path ); dit != end_iter; ++dit ) {
              filename=dit->path().native_file_string();
              if ( filename.substr(filename.length()-3,3) == "cfg") {
                   config_files.push_back(filename);
                   std::cout << "Configuration File Found: " << filename << std::endl; 
              } 
           }
       }
   }
   catch(exception& ecfg) { 
       std::cout << "Problem with configuration files for Qc2" << std::endl;
       std::cout << ecfg.what() << std::endl;
       return 1;
   } 
return 0;
}


///Parses the configuration files.
int 
ReadProgramOptions::
Parse(std::string filename)
{
int StartYear, StartMonth, StartDay, StartHour, StartMinute, StartSecond;
int EndYear, EndMonth, EndDay, EndHour, EndMinute, EndSecond;
int StepYear, StepMonth, StepDay, StepHour, StepMinute, StepSecond;
int RunMinute;
int RunHour;
int ParamId;
int MaxParamId;
int MinParamId;
int TypeId;
int MissingValue;
int MinValue;
std::string BestStationFilename;
std::string CfailedString;
float InterpolationDistance;
// Test control flag paramters
std::vector<unsigned char> z_fqclevel,z_fr,z_fcc,z_fs,z_fnum,z_fpos,z_fmis,z_ftime,z_fw,z_fstat,z_fcp,z_fclim,z_fd,z_fpre,z_fcombi,z_fhqc;
// CONTROL FLAGS FOR VARIOUS POSSIBLE FILTERS
std::vector<unsigned char> R_fqclevel,R_fr,R_fcc,R_fs,R_fnum,R_fpos,R_fmis,R_ftime,R_fw,R_fstat,R_fcp,R_fclim,R_fd,R_fpre,R_fcombi,R_fhqc; //READ
std::vector<unsigned char> I_fqclevel,I_fr,I_fcc,I_fs,I_fnum,I_fpos,I_fmis,I_ftime,I_fw,I_fstat,I_fcp,I_fclim,I_fd,I_fpre,I_fcombi,I_fhqc; //INTERPOLATE
std::vector<unsigned char> A_fqclevel,A_fr,A_fcc,A_fs,A_fnum,A_fpos,A_fmis,A_ftime,A_fw,A_fstat,A_fcp,A_fclim,A_fd,A_fpre,A_fcombi,A_fhqc; //ALGORITHM
std::vector<unsigned char> W_fqclevel,W_fr,W_fcc,W_fs,W_fnum,W_fpos,W_fmis,W_ftime,W_fw,W_fstat,W_fcp,W_fclim,W_fd,W_fpre,W_fcombi,W_fhqc; //WRITE
// CONTROL FLAGS TO SET 
unsigned char S_fqclevel,S_fr,S_fcc,S_fs,S_fnum,S_fpos,S_fmis,S_ftime,S_fw,S_fstat,S_fcp,S_fclim,S_fd,S_fpre,S_fcombi,S_fhqc; //SET

std::vector<std::string> change_fqclevel,change_fr,change_fcc,change_fs,change_fnum,change_fpos,change_fmis,change_ftime,change_fw,change_fstat,change_fcp,change_fclim,change_fd,change_fpre,change_fcombi,change_fhqc;
std::vector<unsigned char> V_fqclevel,V_fr,V_fcc,V_fs,V_fnum,V_fpos,V_fmis,V_ftime,V_fw,V_fstat,V_fcp,V_fclim,V_fd,V_fpre,V_fcombi,V_fhqc; //For algorithms which need multiple control options for the same flag

std::string ControlString;
std::vector<int> ControlVector;

try{
   po::variables_map vm;
   po::options_description config_file_options("Configuration File Parameters");
   // NOTE!! What follows is a very long line of code!!
   config_file_options.add_options()  
        ("RunAtMinute",po::value<int>(&RunMinute)->default_value(0),"Min at which to run (!could just use CRON)")
        ("RunAtHour",po::value<int>(&RunHour)->default_value(2),"Hour at which to run (!!!could just use CRON)")
        ("Start_YYYY",po::value<int>(&StartYear)->default_value(miutil::miTime::nowTime().year()),"Start Year")
        ("Start_MM",po::value<int> (&StartMonth)->default_value(miutil::miTime::nowTime().month()),"Start Month")
        ("Start_DD",po::value<int>  (&StartDay)->default_value(miutil::miTime::nowTime().day()), "Start Day")
        ("Start_hh",po::value<int>  (&StartHour)->default_value(miutil::miTime::nowTime().hour()),  "Start Hour")
        //("Start_mm",po::value<int>  (&StartMinute)->default_value(miutil::miTime::nowTime().min()),"Start Minute")
        //("Start_ss",po::value<int>  (&StartSecond)->default_value(miutil::miTime::nowTime().sec()),"Start Second")
        ("Start_mm",po::value<int>  (&StartMinute)->default_value(0),"Start Minute")
        ("Start_ss",po::value<int>  (&StartSecond)->default_value(0),"Start Second")
        ("End_YYYY",po::value<int>  (&EndYear)->default_value(miutil::miTime::nowTime().year()),  "End Year")
        ("End_MM",po::value<int>    (&EndMonth)->default_value(miutil::miTime::nowTime().month()),"End Month")
        ("End_DD",po::value<int>    (&EndDay)->default_value(miutil::miTime::nowTime().day()), "End Day")
        ("End_hh",po::value<int>    (&EndHour)->default_value(miutil::miTime::nowTime().hour()),    "End Hour")
        //("End_mm",po::value<int>    (&EndMinute)->default_value(miutil::miTime::nowTime().min()),  "End Minute")
        //("End_ss",po::value<int>    (&EndSecond)->default_value(miutil::miTime::nowTime().sec()),  "End Second")
        ("End_mm",po::value<int>    (&EndMinute)->default_value(0),  "End Minute")
        ("End_ss",po::value<int>    (&EndSecond)->default_value(0),  "End Second")

        ("Last_NDays",po::value<int>(&LastN)->default_value(-1),  "Last N Days to Run Algorithm")

        ("Step_YYYY",po::value<int>(&StepYear)->default_value(0),"Step Year")
        ("Step_MM",po::value<int>  (&StepMonth)->default_value(0),  "Step Minute")
        ("Step_DD",po::value<int>  (&StepDay)->default_value(0),  "Step Day")
        ("Step_hh",po::value<int>  (&StepHour)->default_value(0),  "Step Hour")
        ("Step_mm",po::value<int>  (&StepMinute)->default_value(0),  "Step Minute")
        ("Step_ss",po::value<int>  (&StepSecond)->default_value(0),  "Step Second")

        ("ParamId",po::value<int>  (&ParamId)->default_value(0),  "Parameter ID")
        ("MaxParamId",po::value<int>  (&MaxParamId)->default_value(0),  "Parameter ID for a maximum value")
        ("MinParamId",po::value<int>  (&MinParamId)->default_value(0),  "Parameter ID for a minimum value")
        ("TypeId",po::value<int>  (&TypeId),  "Type ID")
        ("AlgoCode",po::value<int>  (&AlgoCode)->default_value(-1),  "Algoritham Code")
        ("InterpCode",po::value<int>  (&InterpCode)->default_value(-1),  "Code to determine method of interpolation")
        ("ControlString",po::value<std::string>  (&ControlString),  "Control Info")
        ("ControlVector",po::value<std::vector<int> > (&ControlVector),  "Control Vector")

        ("BestStationFilename",po::value<std::string> (&BestStationFilename)->default_value("NotSet"),  "Filename containing the best station list")
        ("CfailedString",po::value<std::string> (&CfailedString)->default_value(""),  "Value to add to CFAILED if the algorithm runs and writes data back to the database")

        ("MissingValue",po::value<int>(&MissingValue)->default_value(-32767),  "Original Missing Data Value") /// could also rely on fmis here !!??
        ("MinValue",po::value<int>(&MinValue)->default_value(-32767),  "Minimum Data Value FOr Some Controls") 
        ("InterpolationDistance",po::value<float>(&InterpolationDistance)->default_value(25),  "Nearest Neighbour Limiting Distance") 

        ("z_fqclevel",po::value<std::vector<unsigned char> >  (&z_fqclevel),  "fqclevel")
        ("z_fr",po::value<std::vector<unsigned char> >  (&z_fr),  "fr")
        ("z_fcc",po::value<std::vector<unsigned char> >  (&z_fcc),  "fcc")
        ("z_fs",po::value<std::vector<unsigned char> >  (&z_fs),  "fs")
        ("z_fnum",po::value<std::vector<unsigned char> >  (&z_fnum),  "fnum")
        ("z_fpos",po::value<std::vector<unsigned char> >  (&z_fpos),  "fpos")
        ("z_fmis",po::value<std::vector<unsigned char> >  (&z_fmis),  "fmis")
        ("z_ftime",po::value<std::vector<unsigned char> >  (&z_ftime),  "ftime")
        ("z_fw",po::value<std::vector<unsigned char> >  (&z_fw),  "fw")
        ("z_fstat",po::value<std::vector<unsigned char> >  (&z_fstat),  "fstat")
        ("z_fcp",po::value<std::vector<unsigned char> >  (&z_fcp),  "fcp")
        ("z_fclim",po::value<std::vector<unsigned char> >  (&z_fclim),  "fclim")
        ("z_fd",po::value<std::vector<unsigned char> >  (&z_fd),  "fd")
        ("z_fpre",po::value<std::vector<unsigned char> >  (&z_fpre),  "fpre")
        ("z_fcombi",po::value<std::vector<unsigned char> >  (&z_fcombi),  "fcombi")
        ("z_fhqc",po::value<std::vector<unsigned char> >  (&z_fhqc),  "fhqc")

        ("zbool",po::value<bool>  (&zbool)->default_value(true),  "Option to change logic of all flag controls")

        ("R_fqclevel",po::value<std::vector<unsigned char> >  (&R_fqclevel),  "fqclevel")
        ("R_fr",po::value<std::vector<unsigned char> >  (&R_fr),  "fr")
        ("R_fcc",po::value<std::vector<unsigned char> >  (&R_fcc),  "fcc")
        ("R_fs",po::value<std::vector<unsigned char> >  (&R_fs),  "fs")
        ("R_fnum",po::value<std::vector<unsigned char> >  (&R_fnum),  "fnum")
        ("R_fpos",po::value<std::vector<unsigned char> >  (&R_fpos),  "fpos")
        ("R_fmis",po::value<std::vector<unsigned char> >  (&R_fmis),  "fmis")
        ("R_ftime",po::value<std::vector<unsigned char> >  (&R_ftime),  "ftime")
        ("R_fw",po::value<std::vector<unsigned char> >  (&R_fw),  "fw")
        ("R_fstat",po::value<std::vector<unsigned char> >  (&R_fstat),  "fstat")
        ("R_fcp",po::value<std::vector<unsigned char> >  (&R_fcp),  "fcp")
        ("R_fclim",po::value<std::vector<unsigned char> >  (&R_fclim),  "fclim")
        ("R_fd",po::value<std::vector<unsigned char> >  (&R_fd),  "fd")
        ("R_fpre",po::value<std::vector<unsigned char> >  (&R_fpre),  "fpre")
        ("R_fcombi",po::value<std::vector<unsigned char> >  (&R_fcombi),  "fcombi")
        ("R_fhqc",po::value<std::vector<unsigned char> >  (&R_fhqc),  "fhqc")

        ("Rbool",po::value<bool>  (&Rbool)->default_value(true),  "Option to change logic of all flag controls")

        ("I_fqclevel",po::value<std::vector<unsigned char> >  (&I_fqclevel),  "fqclevel")
        ("I_fr",po::value<std::vector<unsigned char> >  (&I_fr),  "fr")
        ("I_fcc",po::value<std::vector<unsigned char> >  (&I_fcc),  "fcc")
        ("I_fs",po::value<std::vector<unsigned char> >  (&I_fs),  "fs")
        ("I_fnum",po::value<std::vector<unsigned char> >  (&I_fnum),  "fnum")
        ("I_fpos",po::value<std::vector<unsigned char> >  (&I_fpos),  "fpos")
        ("I_fmis",po::value<std::vector<unsigned char> >  (&I_fmis),  "fmis")
        ("I_ftime",po::value<std::vector<unsigned char> >  (&I_ftime),  "ftime")
        ("I_fw",po::value<std::vector<unsigned char> >  (&I_fw),  "fw")
        ("I_fstat",po::value<std::vector<unsigned char> >  (&I_fstat),  "fstat")
        ("I_fcp",po::value<std::vector<unsigned char> >  (&I_fcp),  "fcp")
        ("I_fclim",po::value<std::vector<unsigned char> >  (&I_fclim),  "fclim")
        ("I_fd",po::value<std::vector<unsigned char> >  (&I_fd),  "fd")
        ("I_fpre",po::value<std::vector<unsigned char> >  (&I_fpre),  "fpre")
        ("I_fcombi",po::value<std::vector<unsigned char> >  (&I_fcombi),  "fcombi")
        ("I_fhqc",po::value<std::vector<unsigned char> >  (&I_fhqc),  "fhqc")

        ("Ibool",po::value<bool>  (&Ibool)->default_value(true),  "Option to change logic of all flag controls")

        ("A_fqclevel",po::value<std::vector<unsigned char> >  (&A_fqclevel),  "fqclevel")
        ("A_fr",po::value<std::vector<unsigned char> >  (&A_fr),  "fr")
        ("A_fcc",po::value<std::vector<unsigned char> >  (&A_fcc),  "fcc")
        ("A_fs",po::value<std::vector<unsigned char> >  (&A_fs),  "fs")
        ("A_fnum",po::value<std::vector<unsigned char> >  (&A_fnum),  "fnum")
        ("A_fpos",po::value<std::vector<unsigned char> >  (&A_fpos),  "fpos")
        ("A_fmis",po::value<std::vector<unsigned char> >  (&A_fmis),  "fmis")
        ("A_ftime",po::value<std::vector<unsigned char> >  (&A_ftime),  "ftime")
        ("A_fw",po::value<std::vector<unsigned char> >  (&A_fw),  "fw")
        ("A_fstat",po::value<std::vector<unsigned char> >  (&A_fstat),  "fstat")
        ("A_fcp",po::value<std::vector<unsigned char> >  (&A_fcp),  "fcp")
        ("A_fclim",po::value<std::vector<unsigned char> >  (&A_fclim),  "fclim")
        ("A_fd",po::value<std::vector<unsigned char> >  (&A_fd),  "fd")
        ("A_fpre",po::value<std::vector<unsigned char> >  (&A_fpre),  "fpre")
        ("A_fcombi",po::value<std::vector<unsigned char> >  (&A_fcombi),  "fcombi")
        ("A_fhqc",po::value<std::vector<unsigned char> >  (&A_fhqc),  "fhqc")

        ("Abool",po::value<bool>  (&Abool)->default_value(true),  "Option to change logic of all flag controls")

        ("W_fqclevel",po::value<std::vector<unsigned char> >  (&W_fqclevel),  "fqclevel")
        ("W_fr",po::value<std::vector<unsigned char> >  (&W_fr),  "fr")
        ("W_fcc",po::value<std::vector<unsigned char> >  (&W_fcc),  "fcc")
        ("W_fs",po::value<std::vector<unsigned char> >  (&W_fs),  "fs")
        ("W_fnum",po::value<std::vector<unsigned char> >  (&W_fnum),  "fnum")
        ("W_fpos",po::value<std::vector<unsigned char> >  (&W_fpos),  "fpos")
        ("W_fmis",po::value<std::vector<unsigned char> >  (&W_fmis),  "fmis")
        ("W_ftime",po::value<std::vector<unsigned char> >  (&W_ftime),  "ftime")
        ("W_fw",po::value<std::vector<unsigned char> >  (&W_fw),  "fw")
        ("W_fstat",po::value<std::vector<unsigned char> >  (&W_fstat),  "fstat")
        ("W_fcp",po::value<std::vector<unsigned char> >  (&W_fcp),  "fcp")
        ("W_fclim",po::value<std::vector<unsigned char> >  (&W_fclim),  "fclim")
        ("W_fd",po::value<std::vector<unsigned char> >  (&W_fd),  "fd")
        ("W_fpre",po::value<std::vector<unsigned char> >  (&W_fpre),  "fpre")
        ("W_fcombi",po::value<std::vector<unsigned char> >  (&W_fcombi),  "fcombi")
        ("W_fhqc",po::value<std::vector<unsigned char> >  (&W_fhqc),  "fhqc")

        ("Wbool",po::value<bool>  (&Wbool)->default_value(true),  "Option to change logic of all flag controls")

        ("S_fqclevel",po::value<unsigned char>  (&S_fqclevel)->default_value(0x3F),  "fqclevel")
        ("S_fr",po::value<unsigned char>  (&S_fr)->default_value(0x3F),  "fr")
        ("S_fcc",po::value<unsigned char>  (&S_fcc)->default_value(0x3F),  "fcc")
        ("S_fs",po::value<unsigned char>  (&S_fs)->default_value(0x3F),  "fs")
        ("S_fnum",po::value<unsigned char>  (&S_fnum)->default_value(0x3F),  "fnum")
        ("S_fpos",po::value<unsigned char>  (&S_fpos)->default_value(0x3F),  "fpos")
        ("S_fmis",po::value<unsigned char>  (&S_fmis)->default_value(0x3F),  "fmis")
        ("S_ftime",po::value<unsigned char>  (&S_ftime)->default_value(0x3F),  "ftime")
        ("S_fw",po::value<unsigned char>  (&S_fw)->default_value(0x3F),  "fw")
        ("S_fstat",po::value<unsigned char>  (&S_fstat)->default_value(0x3F),  "fstat")
        ("S_fcp",po::value<unsigned char>  (&S_fcp)->default_value(0x3F),  "fcp")
        ("S_fclim",po::value<unsigned char>  (&S_fclim)->default_value(0x3F),  "fclim")
        ("S_fd",po::value<unsigned char>  (&S_fd)->default_value(0x3F),  "fd")
        ("S_fpre",po::value<unsigned char>  (&S_fpre)->default_value(0x3F),  "fpre")
        ("S_fcombi",po::value<unsigned char>  (&S_fcombi)->default_value(0x3F),  "fcombi")
        ("S_fhqc",po::value<unsigned char>  (&S_fhqc)->default_value(0x3F),  "fhqc")


        ("change_fqclevel",po::value<std::vector<std::string> >  (&change_fqclevel),  "Conditional change to fqclevel")
        ("change_fr",po::value<std::vector<std::string> >  (&change_fr),  "Conditional change to fr")
        ("change_fcc",po::value<std::vector<std::string> >  (&change_fcc),  "Conditional change to fcc")
        ("change_fs",po::value<std::vector<std::string> >  (&change_fs),  "Conditional change to fs")
        ("change_fnum",po::value<std::vector<std::string> >  (&change_fnum),  "Conditional change to fnum")
        ("change_fpos",po::value<std::vector<std::string> >  (&change_fpos),  "Conditional change to fpos")
        ("change_fmis",po::value<std::vector<std::string> >  (&change_fmis),  "Conditional change to fmis")
        ("change_ftime",po::value<std::vector<std::string> >  (&change_ftime),  "Conditional change to ftime")
        ("change_fw",po::value<std::vector<std::string> >  (&change_fw),  "Conditional change to fw")
        ("change_fstat",po::value<std::vector<std::string> >  (&change_fstat),  "Conditional change to fstat")
        ("change_fcp",po::value<std::vector<std::string> >  (&change_fcp),  "Conditional change to fcp")
        ("change_fclim",po::value<std::vector<std::string> >  (&change_fclim),  "Conditional change to fclim")
        ("change_fd",po::value<std::vector<std::string> >  (&change_fd),  "Conditional change to fd")
        ("change_fpre",po::value<std::vector<std::string> >  (&change_fpre),  "Conditional change to fpre")
        ("change_fcombi",po::value<std::vector<std::string> >  (&change_fcombi),  "Conditional change to fcombi")
        ("change_fhqc",po::value<std::vector<std::string> >  (&change_fhqc),  "Conditional change to fhqc")

        ("V_fqclevel",po::value<std::vector<unsigned char> >  (&V_fqclevel),  "fqclevel")
        ("V_fr",po::value<std::vector<unsigned char> >  (&V_fr),  "fr")
        ("V_fcc",po::value<std::vector<unsigned char> >  (&V_fcc),  "fcc")
        ("V_fs",po::value<std::vector<unsigned char> >  (&V_fs),  "fs")
        ("V_fnum",po::value<std::vector<unsigned char> >  (&V_fnum),  "fnum")
        ("V_fpos",po::value<std::vector<unsigned char> >  (&V_fpos),  "fpos")
        ("V_fmis",po::value<std::vector<unsigned char> >  (&V_fmis),  "fmis")
        ("V_ftime",po::value<std::vector<unsigned char> >  (&V_ftime),  "ftime")
        ("V_fw",po::value<std::vector<unsigned char> >  (&V_fw),  "fw")
        ("V_fstat",po::value<std::vector<unsigned char> >  (&V_fstat),  "fstat")
        ("V_fcp",po::value<std::vector<unsigned char> >  (&V_fcp),  "fcp")
        ("V_fclim",po::value<std::vector<unsigned char> >  (&V_fclim),  "fclim")
        ("V_fd",po::value<std::vector<unsigned char> >  (&V_fd),  "fd")
        ("V_fpre",po::value<std::vector<unsigned char> >  (&V_fpre),  "fpre")
        ("V_fcombi",po::value<std::vector<unsigned char> >  (&V_fcombi),  "fcombi")
        ("V_fhqc",po::value<std::vector<unsigned char> >  (&V_fhqc),  "fhqc")

        ; // ***************** The end of the line is here !! ****************************

        ifstream ifs(filename.c_str());   
        store(parse_config_file(ifs, config_file_options), vm);
        notify(vm);
        if (LastN != -1) {       // Ho Ho Ho retain the option to run into the future 
            miutil::miTime TempStartTime=miutil::miTime::nowTime();
            TempStartTime.addDay(-LastN);
            StartDay=TempStartTime.day();
            StartMonth=TempStartTime.month();
            StartYear=TempStartTime.year();
         }
         miutil::miTime StartTime(StartYear,StartMonth,StartDay,StartHour,StartMinute,StartSecond);
         miutil::miTime EndTime(EndYear,EndMonth,EndDay,EndHour,EndMinute,EndSecond);
         //std::cout << config_file_options << std::endl;  // This prints all the conifg options !!!!!!!!!!!!!
         UT0=StartTime;
         UT1=EndTime;
         pid=ParamId;
         maxpid=MaxParamId;
         minpid=MinParamId;
         tid=TypeId;
         RunAtMinute=RunMinute;
         RunAtHour=RunHour;
         StepD=StepDay;
         StepH=StepHour;
         ControlInfoString=ControlString;
         ControlInfoVector=ControlVector;
         InterpolationLimit=InterpolationDistance;
         NeighbourFilename=BestStationFilename;
         CFAILED_STRING=CfailedString;
         missing=MissingValue;
         MinimumValue=MinValue;
         std::cout << miutil::miTime::nowTime() << ": " << UT0 << " -> " << UT1 << "  " << filename << std::endl;

         zflag[0]= z_fqclevel;
         zflag[1]= z_fr;
         zflag[2]= z_fcc;
         zflag[3]= z_fs;
         zflag[4]= z_fnum;
         zflag[5]= z_fpos;
         zflag[6]= z_fmis;
         zflag[7]= z_ftime;
         zflag[8]= z_fw;
         zflag[9]= z_fstat;
         zflag[10]= z_fcp;
         zflag[11]= z_fclim;
         zflag[12]= z_fd;
         zflag[13]= z_fpre;
         zflag[14]= z_fcombi;
         zflag[15]= z_fhqc;

         Rflag[0]= R_fqclevel;
         Rflag[1]= R_fr;
         Rflag[2]= R_fcc;
         Rflag[3]= R_fs;
         Rflag[4]= R_fnum;
         Rflag[5]= R_fpos;
         Rflag[6]= R_fmis;
         Rflag[7]= R_ftime;
         Rflag[8]= R_fw;
         Rflag[9]= R_fstat;
         Rflag[10]= R_fcp;
         Rflag[11]= R_fclim;
         Rflag[12]= R_fd;
         Rflag[13]= R_fpre;
         Rflag[14]= R_fcombi;
         Rflag[15]= R_fhqc;

         Iflag[0]= I_fqclevel;
         Iflag[1]= I_fr;
         Iflag[2]= I_fcc;
         Iflag[3]= I_fs;
         Iflag[4]= I_fnum;
         Iflag[5]= I_fpos;
         Iflag[6]= I_fmis;
         Iflag[7]= I_ftime;
         Iflag[8]= I_fw;
         Iflag[9]= I_fstat;
         Iflag[10]= I_fcp;
         Iflag[11]= I_fclim;
         Iflag[12]= I_fd;
         Iflag[13]= I_fpre;
         Iflag[14]= I_fcombi;
         Iflag[15]= I_fhqc;

         Aflag[0]= A_fqclevel;
         Aflag[1]= A_fr;
         Aflag[2]= A_fcc;
         Aflag[3]= A_fs;
         Aflag[4]= A_fnum;
         Aflag[5]= A_fpos;
         Aflag[6]= A_fmis;
         Aflag[7]= A_ftime;
         Aflag[8]= A_fw;
         Aflag[9]= A_fstat;
         Aflag[10]= A_fcp;
         Aflag[11]= A_fclim;
         Aflag[12]= A_fd;
         Aflag[13]= A_fpre;
         Aflag[14]= A_fcombi;
         Aflag[15]= A_fhqc;

         Wflag[0]= W_fqclevel;
         Wflag[1]= W_fr;
         Wflag[2]= W_fcc;
         Wflag[3]= W_fs;
         Wflag[4]= W_fnum;
         Wflag[5]= W_fpos;
         Wflag[6]= W_fmis;
         Wflag[7]= W_ftime;
         Wflag[8]= W_fw;
         Wflag[9]= W_fstat;
         Wflag[10]= W_fcp;
         Wflag[11]= W_fclim;
         Wflag[12]= W_fd;
         Wflag[13]= W_fpre;
         Wflag[14]= W_fcombi;
         Wflag[15]= W_fhqc;

         if (S_fqclevel != 0x3F) Sflag[0]= S_fqclevel;
         if (S_fr != 0x3F) Sflag[1]= S_fr;
         if (S_fcc != 0x3F) Sflag[2]= S_fcc;
         if (S_fs != 0x3F) Sflag[3]= S_fs;
         if (S_fnum != 0x3F) Sflag[4]= S_fnum;
         if (S_fpos != 0x3F) Sflag[5]= S_fpos;
         if (S_fmis != 0x3F) Sflag[6]= S_fmis;
         if (S_ftime != 0x3F) Sflag[7]= S_ftime;
         if (S_fw != 0x3F) Sflag[8]= S_fw;
         if (S_fstat != 0x3F) Sflag[9]= S_fstat;
         if (S_fcp != 0x3F) Sflag[10]= S_fcp;
         if (S_fclim != 0x3F) Sflag[11]= S_fclim;
         if (S_fd != 0x3F) Sflag[12]= S_fd;
         if (S_fpre != 0x3F) Sflag[13]= S_fpre;
         if (S_fcombi != 0x3F) Sflag[14]= S_fcombi;
         if (S_fhqc != 0x3F) Sflag[15]= S_fhqc;

         
         Vfqclevel = V_fqclevel;
         Vfr = V_fr;
         Vfcc = V_fcc;
         Vfs = V_fs;
         Vfnum = V_fnum;
         Vfpos = V_fpos;
         Vfmis = V_fmis;
         Vftime = V_ftime;
         Vfw = V_fw;
         Vfstat = V_fstat;
         Vfcp = V_fcp;
         Vfclim = V_fclim;
         Vfd = V_fd;
         Vfpre = V_fpre;
         Vfcombi = V_fcombi;
         Vfhqc = V_fhqc;

         std::cout << "Vector Flags Work" << std::endl;
         for (int i=0;i<Vfpre.size();i++){
           std::cout << Vfpre[i] << std::endl;
         }

         if (change_fqclevel.size() != 0) chflag[0]= change_fqclevel;
         if (change_fr.size()       != 0) chflag[1]= change_fr;
         if (change_fcc.size()      != 0) chflag[2]= change_fcc;
         if (change_fs.size()       != 0) chflag[3]= change_fs;
         if (change_fnum.size()     != 0) chflag[4]= change_fnum;
         if (change_fpos.size()     != 0) chflag[5]= change_fpos;
         if (change_fmis.size()     != 0) chflag[6]= change_fmis;
         if (change_ftime.size()    != 0) chflag[7]= change_ftime;
         if (change_fw.size()       != 0) chflag[8]= change_fw;
         if (change_fstat.size()    != 0) chflag[9]= change_fstat;
         if (change_fcp.size()      != 0) chflag[10]= change_fcp;
         if (change_fclim.size()    != 0) chflag[11]= change_fclim;
         if (change_fd.size()       != 0) chflag[12]= change_fd;
         if (change_fpre.size()     != 0) chflag[13]= change_fpre;
         if (change_fcombi.size()   != 0) chflag[14]= change_fcombi;
         if (change_fhqc.size()     != 0) chflag[15]= change_fhqc;

   }
   catch(exception& e) {
       std::cout << e.what() << std::endl;
       return 1;
   }
 return 0;
}

int
ReadProgramOptions::
clear() {

zflag.clear();
Rflag.clear();
Iflag.clear();
Aflag.clear();
Wflag.clear();
Sflag.clear();

miutil::miTime UT0(1900,1,1,0,0,0);
miutil::miTime UT1(1900,1,1,0,0,0);

StepD=0;
StepH=0;
AlgoCode=-1;
InterpCode=-1;
LastN=0;
std::string ControlInfoString;       ///Check these are cleared correctly
std::vector<int> ControlInfoVector;  ///TBD

RunAtMinute=0;
RunAtHour=2;
pid=0;
maxpid=0;
minpid=0;
tid=0;

return 0;
}
//miutil::miTime UT0;
//miutil::miTime UT1;
//int StepD;
//int StepH;
//int AlgoCode;
//int LastN;
//std::string ControlInfoString;
//std::vector<int> ControlInfoVector;
//int RunAtMinute;
//int RunAtHour;
//int pid;
//int tid;
//bool newfile;



