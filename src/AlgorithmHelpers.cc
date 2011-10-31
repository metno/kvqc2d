/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011 met.no

  Contact information:
  Norwegian Meteorological Institute
  Postboks 43 Blindern
  N-0313 OSLO
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

#include "AlgorithmHelpers.h"

#include "DBInterface.h"
#include <milog/milog.h>
#include "foreach.h"

namespace Helpers {

bool checkContinuousHourAndSameTypeID(const std::vector<kvalobs::kvData>& series)
{
    if( series.size() < 2 )
        return false;
    for(unsigned int i=1; i<series.size(); ++i) {
        if( series[i].obstime().hour() != ((series[i-1].obstime().hour() + 1)%24) )
            return false;
        if( series[i].typeID() != series[i-1].typeID() )
            return false;
    }
    return true;
}

void updateCfailed(kvalobs::kvData& data, const miutil::miString& add, const miutil::miString& extra)
{
    miutil::miString new_cfailed = data.cfailed();
    if( new_cfailed.length() > 0 )
        new_cfailed += ",";
    new_cfailed += add;
    if( extra.length() > 0)
        new_cfailed += ","+extra;
    data.cfailed(new_cfailed);
}

void updateUseInfo(kvalobs::kvData& data)
{
    kvalobs::kvUseInfo ui = data.useinfo();
    ui.setUseFlags( data.controlinfo() );
    data.useinfo( ui );
}

void GetNorwegianFixedStations(DBInterface* db, std::list<kvalobs::kvStation>& stations)
{
    std::list<kvalobs::kvStation> tempStations;
    GetAllStations(db, tempStations);

    // Make Qc2 specific selection on the StationList here
    // Only use stations less than 100000 i.e. only Norwegian stations
    // Also remove stations that are moving, e.g. ships.
    stations.clear();
    foreach( const kvalobs::kvStation& s, tempStations ) {
        if( s.stationID() >= 60 && s.stationID() <= 99999 &&  s.maxspeed()==0.0 )
            stations.push_back(s);
    }
}

void GetAllStations(DBInterface* db, std::list<kvalobs::kvStation>& stations)
{
    stations.clear();
    db->selectStations( stations );
}

double distance(double lon1, double lat1, double lon2, double lat2)
{
    const double DEG_RAD = M_PI/180, EARTH_RADIUS = 6371.0;
    const double delta_lon=(lon1 - lon2)*DEG_RAD, slon = std::sin(delta_lon/2);
    const double delta_lat=(lat1 - lat2)*DEG_RAD, slat = std::sin(delta_lat/2);
    const double a = slat*slat + cos(lat1*DEG_RAD)*cos(lat2*DEG_RAD)*slon*slon;
    const double c =2.0 * atan2(sqrt(a), sqrt(1-a));
    return EARTH_RADIUS*c;
}

miutil::miTime plusDay(const miutil::miTime& t, int nDays)
{
    miutil::miTime p(t);
    p.addDay(nDays);
    return p;
}

} // namespace Helpers
