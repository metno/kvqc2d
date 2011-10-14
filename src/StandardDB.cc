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

StandardDB::StandardDB(dnmi::db::Connection* connection)
    : mDbGate(connection)
{
}

StandardDB::~StandardDB()
{
}

bool StandardDB::selectData(kvDataList_t& d, const miutil::miString& where)
{
    return mDbGate.select(d, where);
}

bool StandardDB::selectStationparams(kvStationParamList_t& s, int stationID, const miutil::miTime& time, const std::string& qcx)
{
    const std::list<int> station(1, stationID);
    return mDbGate.select(s, kvQueries::selectStationParam(station, time, qcx ));
}

bool StandardDB::insertData(const kvDataList_t& d, bool replace)
{
    return mDbGate.insert(d, replace, "data");
}
