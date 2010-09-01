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
#include "ProcessImpl.h"
#include "BasicStatistics.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "Qc2D.h"
#include "ReadProgramOptions.h"
#include <sstream>
#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>
#include <puTools/miTime.h>
#include <memory>
#include <stdexcept>

#include "ProcessControl.h"
#include "CheckedDataCommandBase.h"
#include "CheckedDataHelper.h"

#include "scone.h"
#include "tround.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace kvalobs;
using namespace std;
using namespace miutil;

int 
ProcessImpl::
FlagTester( ReadProgramOptions params )
{
  LOGINFO("FlagTester");
  int pid=params.pid;
  miutil::miTime stime=params.UT0;
  miutil::miTime etime=params.UT1;
  std::string CIF=params.ControlInfoString;

  ProcessControl CheckFlags;

  std::list<kvalobs::kvData> Qc2Data;
  std::list<kvalobs::kvData> ReturnData;
  bool result;

  kvalobs::kvControlInfo fixflags;
  kvData d;
  std::vector<std::string> FlagStrings;
  std::vector<kvalobs::kvControlInfo> ControlFlags; 
  std::vector<kvalobs::kvUseInfo> UseFlags; 

   std::string line;
   string valis;
   string key;

   std::ifstream ind;
   std::ofstream ond;
   //ind.open("/metno/kvalobs/kvalobs-svn/src/kvQc2/algorithms/Flags.txt");
   ind.open("/metno/kvalobs/kvqc2-svn/trunk/src/algorithms/Flags.txt");
   ond.open("/metno/kvalobs/kvqc2-svn/trunk/src/algorithms/OutFlags.txt");
   // Change the file to read both original controlinfo and useinfo
   if(ind) {
      while ( !ind.eof() ) {

         ind >> valis;
         FlagStrings.push_back(valis);

         }
   }
   else {
          LOGINFO("Could not open flag input file");
   }

  ind.close();

  if(ond) {

      int ii;
      for(ii=0; ii < FlagStrings.size()-1; ii=ii+2)
      {
        // Set the initial controlinfo
        kvalobs::kvControlInfo kbruce( FlagStrings[ii] );
        // Set the initial useinfo 
        kvalobs::kvUseInfo ubruce( FlagStrings[ii+1] );
        // Apply changes to the control infor as specified by the config file for Qc2
        CheckFlags.setter(fixflags,params.Sflag);
        CheckFlags.conditional_setter(kbruce,params.chflag);
        //Generate the corresponding useinfo file based on setUseflags from the kvalobs library
        ubruce.setUseFlags( kbruce );
        ond << FlagStrings[ii] << " "<< FlagStrings[ii+1] << " " << kbruce << " " << ubruce << std::endl;
   
      }
   }
   else {
          LOGINFO("Could not open flag output file");
          std::cout << "Could not open Flag Output File!" << std::endl;
   }

   ond.close();

return 0;
}

