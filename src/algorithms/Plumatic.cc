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
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "Plumatic.h"
#include "tround.h"
#include <dnmithread/mtcout.h>
#include <milog/milog.h>
#include <map>
#include <numeric>

using namespace std;
using namespace miutil;
using namespace dnmi;


Plumatic::Plumatic(const std::list<kvalobs::kvStation> & slist,ReadProgramOptions PPP)
{
    params=PPP;
} 

Plumatic::Plumatic(std::list<kvalobs::kvData> & QD, ReadProgramOptions PPP)
{
    params=PPP;
	for (std::list<kvalobs::kvData>::const_iterator id = QD.begin(); id != QD.end(); ++id) {
			std::cout << *id << std::endl;
	}
    std::cout << " - - - - - - - - - - - - - - - " << std::endl;
    std::cout << " - - - - - - - - - - - - - - - " << std::endl;
    std::cout << " - - - - - - - - - - - - - - - " << std::endl;
    std::cout << " - - - - - - - - - - - - - - - " << std::endl;
    std::cout << " - - - - - - - - - - - - - - - " << std::endl;
	std::list<kvalobs::kvData>::const_iterator id = QD.begin();
	Start_Time_Interval=id->obstime();
	id = QD.end();
	--id;
	Stop_Time_Interval=id->obstime();
	std::cout << Start_Time_Interval << std::endl;
	std::cout << Stop_Time_Interval << std::endl;

    miutil::miTime Tstamp;

    Tstamp=Start_Time_Interval;
// First write an empty map with all of the time stamps and a "no-measurement" placeholder (i.e. -10.0) for each value
	while (Tstamp <= Stop_Time_Interval) {
            pluvi_data[Tstamp]=-10.0;
			Tstamp.addMin(); // default is to add one minute unless specified
	}
// Now add a data value where there is a some data
	for (std::list<kvalobs::kvData>::const_iterator id = QD.begin(); id != QD.end(); ++id) {
            pluvi_data[id->obstime()]=id->original();
	}
// For now just print out the results 
    Tstamp=Start_Time_Interval;
	while (Tstamp <= Stop_Time_Interval) {
            if (pluvi_data[Tstamp]==-10.0) {
					std::cout << " . "; 
			} 
			else { 
					std::cout << pluvi_data[Tstamp]; 
			}
			Tstamp.addMin(); // default is to add one minute unless specified
	}
	std::cout << endl;
} 

int 
Plumatic::aggregate_window()
{

    miutil::miTime Tstamp;
	float PluviSum;
    std::vector<miutil::miTime> pluvi_time;
    std::vector<float> pluvi_local;
    Tstamp=Start_Time_Interval;
	while (Tstamp <= Stop_Time_Interval) {
            pluvi_time.push_back(Tstamp);
            if (pluvi_data[Tstamp]==-10.0) {;
               pluvi_time.push_back(0.0);
			}
			else
			{
               pluvi_time.push_back(pluvi_data[Tstamp]);
			}
			Tstamp.addMin(); // default is to add one minute unless specified
	}

	int WindowSize=2;
	long offset;

    std::vector<float>::iterator ipl=pluvi_local.begin();
    //for (offset=0; offset < pluvi_local.size()-WindowSize-1 ; offset++) {
	   //PluviSum=accumulate(pluvi_local.begin() + offset, pluvi_local.begin() + WindowSize + offset, 0);
	   PluviSum=accumulate(pluvi_local.begin(), pluvi_local.end(), 0);
	   if (PluviSum > 0.2) {
	         std::cout <<  PluviSum << std::endl; 
			 sleep(1);
	   }
	//}



return 0;
}

/// Clear all data from the redistribution data object.
void
Plumatic::
clear_all()
{
}


