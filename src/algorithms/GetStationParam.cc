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

/****************************************************************
**
** Implementation GetStationParam class
**
****************************************************************/

#include "GetStationParam.h"


GetStationParam::GetStationParam(std::list<kvalobs::kvStationParam> splist)
{

   for (std::list<kvalobs::kvStationParam>::const_iterator lst=splist.begin(); lst!=splist.end(); ++lst) {
       std::cout << lst->metadata() << std::endl;
	   std::cout << lst->descMetadata() << std::endl;
       std::cout << "...................." << std::endl;
       miutil::miString dougal=lst->metadata(); 
	   std::vector<miutil::miString> vs,names,vs2;
	   vs = dougal.split("\n");

       // just want one table entry

	   if (vs.size() == 2) {
			  names = vs[0].split(";"); 
			  vs2 = vs[1].split(";"); 
              for (size_t j=0; j<names.size(); j++){
				      spMap[ names[j] ] = vs2[j];
	          }
	   }
   }

}

miutil::miString
GetStationParam::ValueOf(miutil::miString name) {
	   std::cout << spMap[name] << std::endl;
	   return spMap[name];
}

