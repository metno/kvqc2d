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
           //std::cout << "Does not exist: " << full_path.native_file_string() <<std::endl; //ETCH
           std::cout << "Does not exist: " << full_path.file_string() <<std::endl; //HARDY
       }
       else {
           for ( fs::directory_iterator dit( full_path ); dit != end_iter; ++dit ) {
              filename=dit->path().native_file_string(); //HARDY
              //filename=dit->native_file_string(); // ETCH
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
std::vector<unsigned char> Not_fqclevel,Not_fr,Not_fcc,Not_fs,Not_fnum,Not_fpos,Not_fmis,Not_ftime,Not_fw,Not_fstat,Not_fcp,Not_fclim,Not_fd,Not_fpre,Not_fcombi,Not_fhqc; // Restrictions on the ALGORITHM
std::vector<unsigned char> U_0,U_1,U_2,U_3,U_4,U_5,U_6,U_7,U_8,U_9,U_10,U_11,U_12,U_13,U_14,U_15; //ALGORITHM Use flags control
std::vector<unsigned char> NotU_0,NotU_1,NotU_2,NotU_3,NotU_4,NotU_5,NotU_6,NotU_7,NotU_8,NotU_9,NotU_10,NotU_11,NotU_12,NotU_13,NotU_14,NotU_15; //ALGORITHM Not Use flags control
std::vector<unsigned char> W_fqclevel,W_fr,W_fcc,W_fs,W_fnum,W_fpos,W_fmis,W_ftime,W_fw,W_fstat,W_fcp,W_fclim,W_fd,W_fpre,W_fcombi,W_fhqc; //WRITE
// CONTROL FLAGS TO SET 
unsigned char S_fqclevel,S_fr,S_fcc,S_fs,S_fnum,S_fpos,S_fmis,S_ftime,S_fw,S_fstat,S_fcp,S_fclim,S_fd,S_fpre,S_fcombi,S_fhqc; //SET

std::vector<std::string> change_fqclevel,change_fr,change_fcc,change_fs,change_fnum,change_fpos,change_fmis,change_ftime,change_fw,change_fstat,change_fcp,change_fclim,change_fd,change_fpre,change_fcombi,change_fhqc;
std::vector<unsigned char> V_fqclevel,V_fr,V_fcc,V_fs,V_fnum,V_fpos,V_fmis,V_ftime,V_fw,V_fstat,V_fcp,V_fclim,V_fd,V_fpre,V_fcombi,V_fhqc; //For algorithms which need multiple control options for the same flag

std::string ControlString;
std::vector<int> ControlVector;

Vfull.push_back('0');
Vfull.push_back('1');
Vfull.push_back('2');
Vfull.push_back('3');
Vfull.push_back('4');
Vfull.push_back('5');
Vfull.push_back('6');
Vfull.push_back('7');
Vfull.push_back('8');
Vfull.push_back('9');
Vfull.push_back('A');
Vfull.push_back('B');
Vfull.push_back('C');
Vfull.push_back('D');
Vfull.push_back('E');
Vfull.push_back('F');

try{
   po::variables_map vm;
   po::options_description config_file_options("Configuration File Parameters");
   // NOTE!! What follows is a very long line of code!!
   config_file_options.add_options()  
        ("RunAtMinute",po::value<int>(&RunMinute)->default_value(0),"Minute at which to run the algorithm.")    //DOCME
        ("RunAtHour",po::value<int>(&RunHour)->default_value(2),"Hour at which to run the algorithm.")     //DOCME
        ("Start_YYYY",po::value<int>(&StartYear)->default_value(miutil::miTime::nowTime().year()),"Start Year (of the data to process...)")     //DOCME
        ("Start_MM",po::value<int> (&StartMonth)->default_value(miutil::miTime::nowTime().month()),"Start Month")     //DOCME
        ("Start_DD",po::value<int>  (&StartDay)->default_value(miutil::miTime::nowTime().day()), "Start Day")     //DOCME
        ("Start_hh",po::value<int>  (&StartHour)->default_value(miutil::miTime::nowTime().hour()),  "Start Hour")     //DOCME
        ("Start_mm",po::value<int>  (&StartMinute)->default_value(0),"Start Minute")     //DOCME
        ("Start_ss",po::value<int>  (&StartSecond)->default_value(0),"Start Second")     //DOCME
        ("End_YYYY",po::value<int>  (&EndYear)->default_value(miutil::miTime::nowTime().year()),  "End Year (of the data to process...)")     //DOCME
        ("End_MM",po::value<int>    (&EndMonth)->default_value(miutil::miTime::nowTime().month()),"End Month")     //DOCME
        ("End_DD",po::value<int>    (&EndDay)->default_value(miutil::miTime::nowTime().day()), "End Day")     //DOCME
        ("End_hh",po::value<int>    (&EndHour)->default_value(miutil::miTime::nowTime().hour()),    "End Hour")     //DOCME
        ("End_mm",po::value<int>    (&EndMinute)->default_value(0),  "End Minute")     //DOCME
        ("End_ss",po::value<int>    (&EndSecond)->default_value(0),  "End Second")     //DOCME

        ("Last_NDays",po::value<int>(&LastN)->default_value(-1),  "Last N Days to Run Algorithm (from the latest time)")     //DOCME

        ("Step_YYYY",po::value<int>(&StepYear)->default_value(0),"Step Year (to step through the data interval ...)")     //DOCME
        ("Step_MM",po::value<int>  (&StepMonth)->default_value(0),  "Step Minute")     //DOCME
        ("Step_DD",po::value<int>  (&StepDay)->default_value(0),  "Step Day")     //DOCME
        ("Step_hh",po::value<int>  (&StepHour)->default_value(0),  "Step Hour")     //DOCME
        ("Step_mm",po::value<int>  (&StepMinute)->default_value(0),  "Step Minute")     //DOCME
        ("Step_ss",po::value<int>  (&StepSecond)->default_value(0),  "Step Second")     //DOCME

        ("ParamId",po::value<int>  (&ParamId)->default_value(0),  "Parameter ID")     //DOCME
        ("MaxParamId",po::value<int>  (&MaxParamId)->default_value(0),  "Parameter ID for a maximum value")     //DOCME
        ("MinParamId",po::value<int>  (&MinParamId)->default_value(0),  "Parameter ID for a minimum value")     //DOCME
        ("TypeId",po::value<int>  (&TypeId),  "Type ID")     //DOCME
        ("AlgoCode",po::value<int>  (&AlgoCode)->default_value(-1),  "Algoritham Code")     //DOCME
        ("InterpCode",po::value<int>  (&InterpCode)->default_value(-1),  "Code to determine method of interpolation")     //DOCME
        ("ControlString",po::value<std::string>  (&ControlString),  "Control Info (not used)")     //DOCME
        ("ControlVector",po::value<std::vector<int> > (&ControlVector),  "Control Vector (not used)")     //DOCME

        ("BestStationFilename",po::value<std::string> (&BestStationFilename)->default_value("NotSet"),  "Filename containing the best station list")     //DOCME
        ("CfailedString",po::value<std::string> (&CfailedString)->default_value(""),  "Value to add to CFAILED if the algorithm runs and writes data back to the database")     //DOCME

        ("MissingValue",po::value<int>(&MissingValue)->default_value(-32767),  "Original Missing Data Value")      //DOCME
        ("MinValue",po::value<int>(&MinValue)->default_value(-32767),  "Minimum Data Value For Some Controls")      //DOCME
        ("InterpolationDistance",po::value<float>(&InterpolationDistance)->default_value(25),  "Nearest Neighbour Limiting Distance")      //DOCME

        ("z_fqclevel",po::value<std::vector<unsigned char> >  (&z_fqclevel),  "fqclevel [GENERAL FILTER]")     //DOCME
        ("z_fr",po::value<std::vector<unsigned char> >  (&z_fr),  "fr")     //DOCME
        ("z_fcc",po::value<std::vector<unsigned char> >  (&z_fcc),  "fcc")     //DOCME
        ("z_fs",po::value<std::vector<unsigned char> >  (&z_fs),  "fs")     //DOCME
        ("z_fnum",po::value<std::vector<unsigned char> >  (&z_fnum),  "fnum")     //DOCME
        ("z_fpos",po::value<std::vector<unsigned char> >  (&z_fpos),  "fpos")     //DOCME
        ("z_fmis",po::value<std::vector<unsigned char> >  (&z_fmis),  "fmis")     //DOCME
        ("z_ftime",po::value<std::vector<unsigned char> >  (&z_ftime),  "ftime")     //DOCME
        ("z_fw",po::value<std::vector<unsigned char> >  (&z_fw),  "fw")     //DOCME
        ("z_fstat",po::value<std::vector<unsigned char> >  (&z_fstat),  "fstat")     //DOCME
        ("z_fcp",po::value<std::vector<unsigned char> >  (&z_fcp),  "fcp")     //DOCME
        ("z_fclim",po::value<std::vector<unsigned char> >  (&z_fclim),  "fclim")     //DOCME
        ("z_fd",po::value<std::vector<unsigned char> >  (&z_fd),  "fd")     //DOCME
        ("z_fpre",po::value<std::vector<unsigned char> >  (&z_fpre),  "fpre")     //DOCME
        ("z_fcombi",po::value<std::vector<unsigned char> >  (&z_fcombi),  "fcombi")     //DOCME
        ("z_fhqc",po::value<std::vector<unsigned char> >  (&z_fhqc),  "fhqc")     //DOCME

        ("zbool",po::value<bool>  (&zbool)->default_value(true),  "Option to change logic of all flag controls")     //DOCME

        ("R_fqclevel",po::value<std::vector<unsigned char> >  (&R_fqclevel),  "fqclevel [FILTER for Reading Data]")     //DOCME
        ("R_fr",po::value<std::vector<unsigned char> >  (&R_fr),  "fr")     //DOCME
        ("R_fcc",po::value<std::vector<unsigned char> >  (&R_fcc),  "fcc")     //DOCME
        ("R_fs",po::value<std::vector<unsigned char> >  (&R_fs),  "fs")     //DOCME
        ("R_fnum",po::value<std::vector<unsigned char> >  (&R_fnum),  "fnum")     //DOCME
        ("R_fpos",po::value<std::vector<unsigned char> >  (&R_fpos),  "fpos")     //DOCME
        ("R_fmis",po::value<std::vector<unsigned char> >  (&R_fmis),  "fmis")     //DOCME
        ("R_ftime",po::value<std::vector<unsigned char> >  (&R_ftime),  "ftime")     //DOCME
        ("R_fw",po::value<std::vector<unsigned char> >  (&R_fw),  "fw")     //DOCME
        ("R_fstat",po::value<std::vector<unsigned char> >  (&R_fstat),  "fstat")     //DOCME
        ("R_fcp",po::value<std::vector<unsigned char> >  (&R_fcp),  "fcp")     //DOCME
        ("R_fclim",po::value<std::vector<unsigned char> >  (&R_fclim),  "fclim")     //DOCME
        ("R_fd",po::value<std::vector<unsigned char> >  (&R_fd),  "fd")     //DOCME
        ("R_fpre",po::value<std::vector<unsigned char> >  (&R_fpre),  "fpre")     //DOCME
        ("R_fcombi",po::value<std::vector<unsigned char> >  (&R_fcombi),  "fcombi")     //DOCME
        ("R_fhqc",po::value<std::vector<unsigned char> >  (&R_fhqc),  "fhqc")     //DOCME

        ("Rbool",po::value<bool>  (&Rbool)->default_value(true),  "Option to change logic of all flag controls")     //DOCME

        ("I_fqclevel",po::value<std::vector<unsigned char> >  (&I_fqclevel),  "fqclevel [FILTER for Intrpolating Data]")     //DOCME
        ("I_fr",po::value<std::vector<unsigned char> >  (&I_fr),  "fr")     //DOCME
        ("I_fcc",po::value<std::vector<unsigned char> >  (&I_fcc),  "fcc")     //DOCME
        ("I_fs",po::value<std::vector<unsigned char> >  (&I_fs),  "fs")     //DOCME
        ("I_fnum",po::value<std::vector<unsigned char> >  (&I_fnum),  "fnum")     //DOCME
        ("I_fpos",po::value<std::vector<unsigned char> >  (&I_fpos),  "fpos")     //DOCME
        ("I_fmis",po::value<std::vector<unsigned char> >  (&I_fmis),  "fmis")     //DOCME
        ("I_ftime",po::value<std::vector<unsigned char> >  (&I_ftime),  "ftime")     //DOCME
        ("I_fw",po::value<std::vector<unsigned char> >  (&I_fw),  "fw")     //DOCME
        ("I_fstat",po::value<std::vector<unsigned char> >  (&I_fstat),  "fstat")     //DOCME
        ("I_fcp",po::value<std::vector<unsigned char> >  (&I_fcp),  "fcp")     //DOCME
        ("I_fclim",po::value<std::vector<unsigned char> >  (&I_fclim),  "fclim")     //DOCME
        ("I_fd",po::value<std::vector<unsigned char> >  (&I_fd),  "fd")     //DOCME
        ("I_fpre",po::value<std::vector<unsigned char> >  (&I_fpre),  "fpre")     //DOCME
        ("I_fcombi",po::value<std::vector<unsigned char> >  (&I_fcombi),  "fcombi")     //DOCME
        ("I_fhqc",po::value<std::vector<unsigned char> >  (&I_fhqc),  "fhqc")     //DOCME

        ("Ibool",po::value<bool>  (&Ibool)->default_value(true),  "Option to change logic of all flag controls")     //DOCME

        ("A_fqclevel",po::value<std::vector<unsigned char> >  (&A_fqclevel),  "fqclevel [FILTER for applying Algorithm]")     //DOCME
        ("A_fr",po::value<std::vector<unsigned char> >  (&A_fr),  "fr")     //DOCME
        ("A_fcc",po::value<std::vector<unsigned char> >  (&A_fcc),  "fcc")     //DOCME
        ("A_fs",po::value<std::vector<unsigned char> >  (&A_fs),  "fs")     //DOCME
        ("A_fnum",po::value<std::vector<unsigned char> >  (&A_fnum),  "fnum")     //DOCME
        ("A_fpos",po::value<std::vector<unsigned char> >  (&A_fpos),  "fpos")     //DOCME
        ("A_fmis",po::value<std::vector<unsigned char> >  (&A_fmis),  "fmis")     //DOCME
        ("A_ftime",po::value<std::vector<unsigned char> >  (&A_ftime),  "ftime")     //DOCME
        ("A_fw",po::value<std::vector<unsigned char> >  (&A_fw),  "fw")     //DOCME
        ("A_fstat",po::value<std::vector<unsigned char> >  (&A_fstat),  "fstat")     //DOCME
        ("A_fcp",po::value<std::vector<unsigned char> >  (&A_fcp),  "fcp")     //DOCME
        ("A_fclim",po::value<std::vector<unsigned char> >  (&A_fclim),  "fclim")     //DOCME
        ("A_fd",po::value<std::vector<unsigned char> >  (&A_fd),  "fd")     //DOCME
        ("A_fpre",po::value<std::vector<unsigned char> >  (&A_fpre),  "fpre")     //DOCME
        ("A_fcombi",po::value<std::vector<unsigned char> >  (&A_fcombi),  "fcombi")     //DOCME
        ("A_fhqc",po::value<std::vector<unsigned char> >  (&A_fhqc),  "fhqc")     //DOCME

        ("Abool",po::value<bool>  (&Abool)->default_value(true),  "Option to change logic of all flag controls")     //DOCME

        ("Not_fqclevel",po::value<std::vector<unsigned char> >  (&Not_fqclevel),  "fqclevel [FILTER for not applying the Algorithm]")     //DOCME
        ("Not_fr",po::value<std::vector<unsigned char> >  (&Not_fr),  "fr")     //DOCME
        ("Not_fcc",po::value<std::vector<unsigned char> >  (&Not_fcc),  "fcc")     //DOCME
        ("Not_fs",po::value<std::vector<unsigned char> >  (&Not_fs),  "fs")     //DOCME
        ("Not_fnum",po::value<std::vector<unsigned char> >  (&Not_fnum),  "fnum")     //DOCME
        ("Not_fpos",po::value<std::vector<unsigned char> >  (&Not_fpos),  "fpos")     //DOCME
        ("Not_fmis",po::value<std::vector<unsigned char> >  (&Not_fmis),  "fmis")     //DOCME
        ("Not_ftime",po::value<std::vector<unsigned char> >  (&Not_ftime),  "ftime")     //DOCME
        ("Not_fw",po::value<std::vector<unsigned char> >  (&Not_fw),  "fw")     //DOCME
        ("Not_fstat",po::value<std::vector<unsigned char> >  (&Not_fstat),  "fstat")     //DOCME
        ("Not_fcp",po::value<std::vector<unsigned char> >  (&Not_fcp),  "fcp")     //DOCME
        ("Not_fclim",po::value<std::vector<unsigned char> >  (&Not_fclim),  "fclim")     //DOCME
        ("Not_fd",po::value<std::vector<unsigned char> >  (&Not_fd),  "fd")     //DOCME
        ("Not_fpre",po::value<std::vector<unsigned char> >  (&Not_fpre),  "fpre")     //DOCME
        ("Not_fcombi",po::value<std::vector<unsigned char> >  (&Not_fcombi),  "fcombi")     //DOCME
        ("Not_fhqc",po::value<std::vector<unsigned char> >  (&Not_fhqc),  "fhqc")     //DOCME

        ("Notbool",po::value<bool>  (&Notbool)->default_value(true),  "Option to change logic of all flag controls")     //DOCME

        ("U_0",po::value<std::vector<unsigned char> >  (&U_0),  "f0 [FILTER applied to the Useinfo for applying the ALgorithm.]")     //DOCME
        ("U_1",po::value<std::vector<unsigned char> >  (&U_1),  "f1")     //DOCME
        ("U_2",po::value<std::vector<unsigned char> >  (&U_2),  "f2")     //DOCME
        ("U_3",po::value<std::vector<unsigned char> >  (&U_3),  "f3")     //DOCME
        ("U_4",po::value<std::vector<unsigned char> >  (&U_4),  "f4")     //DOCME
        ("U_5",po::value<std::vector<unsigned char> >  (&U_5),  "f5")     //DOCME
        ("U_6",po::value<std::vector<unsigned char> >  (&U_6),  "f6")     //DOCME
        ("U_7",po::value<std::vector<unsigned char> >  (&U_7),  "f7")     //DOCME
        ("U_8",po::value<std::vector<unsigned char> >  (&U_8),  "f8")     //DOCME
        ("U_9",po::value<std::vector<unsigned char> >  (&U_9),  "f9")     //DOCME
        ("U_10",po::value<std::vector<unsigned char> >  (&U_10),  "f10")     //DOCME
        ("U_11",po::value<std::vector<unsigned char> >  (&U_11),  "f11")     //DOCME
        ("U_12",po::value<std::vector<unsigned char> >  (&U_12),  "f12")     //DOCME
        ("U_13",po::value<std::vector<unsigned char> >  (&U_13),  "f13")     //DOCME
        ("U_14",po::value<std::vector<unsigned char> >  (&U_14),  "f14")     //DOCME
        ("U_15",po::value<std::vector<unsigned char> >  (&U_15),  "f15")     //DOCME

        ("Ubool",po::value<bool>  (&Ubool)->default_value(true),  "Option to change logic of all flag controls")     //DOCME

        ("NotU_0",po::value<std::vector<unsigned char> >  (&NotU_0),  "f0 [FILTER applied to the Useinfo for not applying the ALgorithm.]")     //DOCME
        ("NotU_1",po::value<std::vector<unsigned char> >  (&NotU_1),  "f1")     //DOCME
        ("NotU_2",po::value<std::vector<unsigned char> >  (&NotU_2),  "f2")     //DOCME
        ("NotU_3",po::value<std::vector<unsigned char> >  (&NotU_3),  "f3")     //DOCME
        ("NotU_4",po::value<std::vector<unsigned char> >  (&NotU_4),  "f4")     //DOCME
        ("NotU_5",po::value<std::vector<unsigned char> >  (&NotU_5),  "f5")     //DOCME
        ("NotU_6",po::value<std::vector<unsigned char> >  (&NotU_6),  "f6")     //DOCME
        ("NotU_7",po::value<std::vector<unsigned char> >  (&NotU_7),  "f7")     //DOCME
        ("NotU_8",po::value<std::vector<unsigned char> >  (&NotU_8),  "f8")     //DOCME
        ("NotU_9",po::value<std::vector<unsigned char> >  (&NotU_9),  "f9")     //DOCME
        ("NotU_10",po::value<std::vector<unsigned char> >  (&NotU_10),  "f10")     //DOCME
        ("NotU_11",po::value<std::vector<unsigned char> >  (&NotU_11),  "f11")     //DOCME
        ("NotU_12",po::value<std::vector<unsigned char> >  (&NotU_12),  "f12")     //DOCME
        ("NotU_13",po::value<std::vector<unsigned char> >  (&NotU_13),  "f13")     //DOCME
        ("NotU_14",po::value<std::vector<unsigned char> >  (&NotU_14),  "f14")     //DOCME
        ("NotU_15",po::value<std::vector<unsigned char> >  (&NotU_15),  "f15")     //DOCME

        ("NotUbool",po::value<bool>  (&NotUbool)->default_value(true),  "Option to change logic of all flag controls")     //DOCME

        ("W_fqclevel",po::value<std::vector<unsigned char> >  (&W_fqclevel),  "fqclevel [FILTER for Writing results back to the DataBase]")     //DOCME
        ("W_fr",po::value<std::vector<unsigned char> >  (&W_fr),  "fr")     //DOCME
        ("W_fcc",po::value<std::vector<unsigned char> >  (&W_fcc),  "fcc")     //DOCME
        ("W_fs",po::value<std::vector<unsigned char> >  (&W_fs),  "fs")     //DOCME
        ("W_fnum",po::value<std::vector<unsigned char> >  (&W_fnum),  "fnum")     //DOCME
        ("W_fpos",po::value<std::vector<unsigned char> >  (&W_fpos),  "fpos")     //DOCME
        ("W_fmis",po::value<std::vector<unsigned char> >  (&W_fmis),  "fmis")     //DOCME
        ("W_ftime",po::value<std::vector<unsigned char> >  (&W_ftime),  "ftime")     //DOCME
        ("W_fw",po::value<std::vector<unsigned char> >  (&W_fw),  "fw")     //DOCME
        ("W_fstat",po::value<std::vector<unsigned char> >  (&W_fstat),  "fstat")     //DOCME
        ("W_fcp",po::value<std::vector<unsigned char> >  (&W_fcp),  "fcp")     //DOCME
        ("W_fclim",po::value<std::vector<unsigned char> >  (&W_fclim),  "fclim")     //DOCME
        ("W_fd",po::value<std::vector<unsigned char> >  (&W_fd),  "fd")     //DOCME
        ("W_fpre",po::value<std::vector<unsigned char> >  (&W_fpre),  "fpre")     //DOCME
        ("W_fcombi",po::value<std::vector<unsigned char> >  (&W_fcombi),  "fcombi")     //DOCME
        ("W_fhqc",po::value<std::vector<unsigned char> >  (&W_fhqc),  "fhqc")     //DOCME

        ("Wbool",po::value<bool>  (&Wbool)->default_value(true),  "Option to change logic of all flag controls")     //DOCME

        ("S_fqclevel",po::value<unsigned char>  (&S_fqclevel)->default_value(0x3F),  "fqclevel [Controlinfo to set for the controlled datum]")     //DOCME
        ("S_fr",po::value<unsigned char>  (&S_fr)->default_value(0x3F),  "fr")     //DOCME
        ("S_fcc",po::value<unsigned char>  (&S_fcc)->default_value(0x3F),  "fcc")     //DOCME
        ("S_fs",po::value<unsigned char>  (&S_fs)->default_value(0x3F),  "fs")     //DOCME
        ("S_fnum",po::value<unsigned char>  (&S_fnum)->default_value(0x3F),  "fnum")     //DOCME
        ("S_fpos",po::value<unsigned char>  (&S_fpos)->default_value(0x3F),  "fpos")     //DOCME
        ("S_fmis",po::value<unsigned char>  (&S_fmis)->default_value(0x3F),  "fmis")     //DOCME
        ("S_ftime",po::value<unsigned char>  (&S_ftime)->default_value(0x3F),  "ftime")     //DOCME
        ("S_fw",po::value<unsigned char>  (&S_fw)->default_value(0x3F),  "fw")     //DOCME
        ("S_fstat",po::value<unsigned char>  (&S_fstat)->default_value(0x3F),  "fstat")     //DOCME
        ("S_fcp",po::value<unsigned char>  (&S_fcp)->default_value(0x3F),  "fcp")     //DOCME
        ("S_fclim",po::value<unsigned char>  (&S_fclim)->default_value(0x3F),  "fclim")     //DOCME
        ("S_fd",po::value<unsigned char>  (&S_fd)->default_value(0x3F),  "fd")     //DOCME
        ("S_fpre",po::value<unsigned char>  (&S_fpre)->default_value(0x3F),  "fpre")     //DOCME
        ("S_fcombi",po::value<unsigned char>  (&S_fcombi)->default_value(0x3F),  "fcombi")     //DOCME
        ("S_fhqc",po::value<unsigned char>  (&S_fhqc)->default_value(0x3F),  "fhqc")     //DOCME


        ("change_fqclevel",po::value<std::vector<std::string> >  (&change_fqclevel),  "Conditional change to fqclevel")     //DOCME
        ("change_fr",po::value<std::vector<std::string> >  (&change_fr),  "Conditional change to fr")     //DOCME
        ("change_fcc",po::value<std::vector<std::string> >  (&change_fcc),  "Conditional change to fcc")     //DOCME
        ("change_fs",po::value<std::vector<std::string> >  (&change_fs),  "Conditional change to fs")     //DOCME
        ("change_fnum",po::value<std::vector<std::string> >  (&change_fnum),  "Conditional change to fnum")     //DOCME
        ("change_fpos",po::value<std::vector<std::string> >  (&change_fpos),  "Conditional change to fpos")     //DOCME
        ("change_fmis",po::value<std::vector<std::string> >  (&change_fmis),  "Conditional change to fmis")     //DOCME
        ("change_ftime",po::value<std::vector<std::string> >  (&change_ftime),  "Conditional change to ftime")     //DOCME
        ("change_fw",po::value<std::vector<std::string> >  (&change_fw),  "Conditional change to fw")     //DOCME
        ("change_fstat",po::value<std::vector<std::string> >  (&change_fstat),  "Conditional change to fstat")     //DOCME
        ("change_fcp",po::value<std::vector<std::string> >  (&change_fcp),  "Conditional change to fcp")     //DOCME
        ("change_fclim",po::value<std::vector<std::string> >  (&change_fclim),  "Conditional change to fclim")     //DOCME
        ("change_fd",po::value<std::vector<std::string> >  (&change_fd),  "Conditional change to fd")     //DOCME
        ("change_fpre",po::value<std::vector<std::string> >  (&change_fpre),  "Conditional change to fpre")     //DOCME
        ("change_fcombi",po::value<std::vector<std::string> >  (&change_fcombi),  "Conditional change to fcombi")     //DOCME
        ("change_fhqc",po::value<std::vector<std::string> >  (&change_fhqc),  "Conditional change to fhqc")     //DOCME

        ("V_fqclevel",po::value<std::vector<unsigned char> >  (&V_fqclevel),  "fqclevel [Additional vector for controlinfo controls (not used)]")     //DOCME
        ("V_fr",po::value<std::vector<unsigned char> >  (&V_fr),  "fr")     //DOCME
        ("V_fcc",po::value<std::vector<unsigned char> >  (&V_fcc),  "fcc")     //DOCME
        ("V_fs",po::value<std::vector<unsigned char> >  (&V_fs),  "fs")     //DOCME
        ("V_fnum",po::value<std::vector<unsigned char> >  (&V_fnum),  "fnum")     //DOCME
        ("V_fpos",po::value<std::vector<unsigned char> >  (&V_fpos),  "fpos")     //DOCME
        ("V_fmis",po::value<std::vector<unsigned char> >  (&V_fmis),  "fmis")     //DOCME
        ("V_ftime",po::value<std::vector<unsigned char> >  (&V_ftime),  "ftime")     //DOCME
        ("V_fw",po::value<std::vector<unsigned char> >  (&V_fw),  "fw")     //DOCME
        ("V_fstat",po::value<std::vector<unsigned char> >  (&V_fstat),  "fstat")     //DOCME
        ("V_fcp",po::value<std::vector<unsigned char> >  (&V_fcp),  "fcp")     //DOCME
        ("V_fclim",po::value<std::vector<unsigned char> >  (&V_fclim),  "fclim")     //DOCME
        ("V_fd",po::value<std::vector<unsigned char> >  (&V_fd),  "fd")     //DOCME
        ("V_fpre",po::value<std::vector<unsigned char> >  (&V_fpre),  "fpre")     //DOCME
        ("V_fcombi",po::value<std::vector<unsigned char> >  (&V_fcombi),  "fcombi")     //DOCME
        ("V_fhqc",po::value<std::vector<unsigned char> >  (&V_fhqc),  "fhqc")     //DOCME

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

         /// If no specific flag is set then the algorithm shall run for all flags.
         if ( Aflag[0].empty() && Aflag[1].empty() && Aflag[2].empty() && Aflag[3].empty() && Aflag[4].empty() && Aflag[5].empty() && Aflag[6].empty() && Aflag[7].empty() && Aflag[8].empty() && Aflag[9].empty() && Aflag[10].empty() && Aflag[11].empty() && Aflag[12].empty() && Aflag[13].empty() && Aflag[14].empty() && Aflag[15].empty()  ) {
            for (int i=0;i<16;i++) Aflag[i]=Vfull;
         }

         Notflag[0]= Not_fqclevel;
         Notflag[1]= Not_fr;
         Notflag[2]= Not_fcc;
         Notflag[3]= Not_fs;
         Notflag[4]= Not_fnum;
         Notflag[5]= Not_fpos;
         Notflag[6]= Not_fmis;
         Notflag[7]= Not_ftime;
         Notflag[8]= Not_fw;
         Notflag[9]= Not_fstat;
         Notflag[10]= Not_fcp;
         Notflag[11]= Not_fclim;
         Notflag[12]= Not_fd;
         Notflag[13]= Not_fpre;
         Notflag[14]= Not_fcombi;
         Notflag[15]= Not_fhqc;

         Uflag[0]= U_0;
         Uflag[1]= U_1;
         Uflag[2]= U_2;
         Uflag[3]= U_3;
         Uflag[4]= U_4;
         Uflag[5]= U_5;
         Uflag[6]= U_6;
         Uflag[7]= U_7;
         Uflag[8]= U_8;
         Uflag[9]= U_9;
         Uflag[10]= U_10;
         Uflag[11]= U_11;
         Uflag[12]= U_12;
         Uflag[13]= U_13;
         Uflag[14]= U_14;
         Uflag[15]= U_15;
         /// If no specific flag is set then the algorithm shall run for all flags.
         if ( Uflag[0].empty() && Uflag[1].empty() && Uflag[2].empty() && Uflag[3].empty() && Uflag[4].empty() && Uflag[5].empty() && Uflag[6].empty() && Uflag[7].empty() && Uflag[8].empty() && Uflag[9].empty() && Uflag[10].empty() && Uflag[11].empty() && Uflag[12].empty() && Uflag[13].empty() && Uflag[14].empty() && Uflag[15].empty()  ) {
            for (int i=0;i<16;i++) Uflag[i]=Vfull;
         }

         NotUflag[0]= NotU_0;
         NotUflag[1]= NotU_1;
         NotUflag[2]= NotU_2;
         NotUflag[3]= NotU_3;
         NotUflag[4]= NotU_4;
         NotUflag[5]= NotU_5;
         NotUflag[6]= NotU_6;
         NotUflag[7]= NotU_7;
         NotUflag[8]= NotU_8;
         NotUflag[9]= NotU_9;
         NotUflag[10]= NotU_10;
         NotUflag[11]= NotU_11;
         NotUflag[12]= NotU_12;
         NotUflag[13]= NotU_13;
         NotUflag[14]= NotU_14;
         NotUflag[15]= NotU_15;

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

         //std::cout << "Vector Flags Work" << std::endl;
         //for (int i=0;i<Vfpre.size();i++){
           //std::cout << Vfpre[i] << std::endl;
         //}

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
Uflag.clear();
NotUflag.clear();
Notflag.clear();
chflag.clear();
Vfull.clear();

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



