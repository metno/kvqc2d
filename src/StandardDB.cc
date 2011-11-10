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

#include "StandardDB.h"

#include <kvalobs/kvQueries.h>
#include "foreach.h"

StandardDB::StandardDB(dnmi::db::Connection* connection)
    : mDbGate(connection)
{
}

StandardDB::~StandardDB()
{
}

void StandardDB::selectData(kvDataList_t& d, const miutil::miString& where) throw (DBException)
{
    if( !mDbGate.select(d, where) )
        throw DBException("Database problem with SELECT data " + where + ": " + mDbGate.getErrorStr());
}

void StandardDB::selectStations(kvStationList_t& s) throw (DBException)
{
    if( !mDbGate.select(s) )
        throw DBException("Database problem with SELECT stations: " + mDbGate.getErrorStr());
}

void StandardDB::selectStationparams(kvStationParamList_t& s, int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException)
{
    const std::list<int> station(1, stationID);
    const std::string where = kvQueries::selectStationParam(station, time, qcx );
    if( !mDbGate.select(s, where) )
        throw DBException("Database problem with SELECT stationparam " + where);
}

void StandardDB::insertData(const kvDataList_t& d, bool replace) throw (DBException)
{
#if 1
    if( !mDbGate.insert(d, replace) )
        throw DBException("Database problem with INSERT/UPDATE: " + mDbGate.getErrorStr());
#else
    foreach(const kvalobs::kvData& data, d) {
        if( !mDbGate.insert(data, "data", replace) )
        throw DBException("Database problem with INSERT/UPDATE: " + mDbGate.getErrorStr());
    }
#endif
}
