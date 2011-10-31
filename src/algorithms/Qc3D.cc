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
#include "Qc3D.h"
#include "StationSelection.h"
#include "BasicStatistics.h"
#include <dnmithread/mtcout.h>
#include <kvalobs/kvData.h>
#include "kvQABaseTypes.h"
#include <map>

#include <netcdfcpp.h>

#include <milog/milog.h>

#include "ProcessControl.h"
#include "proj++.h"

#include "table_delaunay.h"

using namespace std;
using namespace miutil;
using namespace dnmi;


///Method to clear() all of the vectors held in the Qc3D data structure.
void Qc3D::clean()
{
  stid_.clear();
  obstime_.clear();
  original_.clear();
  paramid_.clear();
  tbtime_.clear();
  typeid_.clear();
  sensor_.clear();
  level_.clear();
  corrected_.clear();
  controlinfo_.clear();
  useinfo_.clear();
  cfailed_.clear();

  intp_.clear();
  redis_.clear();
  lat_.clear();
  lon_.clear();
  ht_.clear();
  CP_.clear();

  stindex.clear();
}

Qc3D::
Qc3D(std::list<kvalobs::kvData>& QD, std::list<kvalobs::kvStation>& SL, const ReadProgramOptions& PPP)
    : params(PPP)
{
  std::map<int, kvalobs::kvStation> Gsid;
  for ( std::list<kvalobs::kvStation>::const_iterator it = SL.begin(); it != SL.end(); ++it ) {
       Gsid[ it->stationID() ] = *it;
  }
  for (std::list<kvalobs::kvData>::const_iterator id = QD.begin(); id != QD.end(); ++id) {
          istid( id->stationID() );
          iobstime( id->obstime() );
          ioriginal( id->original() );
          iparamid( id->paramID() );
          itbtime( id->tbtime() );
          itypeid( id->typeID() );
          isensor( id->sensor() );
          ilevel( id->level() );
          icorrected( id->corrected() );
          icontrolinfo( id->controlinfo() );
          iuseinfo( id->useinfo() );
          icfailed( id->cfailed() );
          iintp( -10.0 );
          iredis( -10.0 );
          icp( -10.0 );
          iht(Gsid[ id->stationID() ].height());
          ilat(Gsid[ id->stationID() ].lat());
          ilon(Gsid[ id->stationID() ].lon());
          istindex( id->stationID() );
  }
}

                          
std::ostream& operator<<(std::ostream& stm, const Qc3D &Q)
{
    stm << "Qc2 Data:";
    for (unsigned int i=0; i<Q.stid_.size(); i++){
       stm <<"{"<< Q.stid_[i]
           <<","<< Q.obstime_[i]
           <<","<< Q.original_[i]
           <<","<< Q.paramid_[i]
           <<","<< Q.tbtime_[i]
           <<","<< Q.typeid_[i]
           <<","<< Q.sensor_[i]
           <<","<< Q.level_[i]
           <<","<< Q.corrected_[i]
           <<","<< Q.controlinfo_[i]
           <<","<< Q.useinfo_[i]
           <<","<< Q.cfailed_[i]
           <<","<< Q.intp_[i]
           <<","<< Q.redis_[i]
           <<","<< Q.lat_[i]
           <<","<< Q.lon_[i]
           <<","<< Q.ht_[i]
           <<","<< Q.CP_[i]
           <<"}"<< std::endl;
    }
    return stm;
}

