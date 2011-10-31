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

#include "DBInterface.h"

void DBInterface::dataForStationParamTimerange(kvDataList_t& r, int stationID, int paramID, const miutil::miTime& first, const miutil::miTime& last) throw (DBException)
{
    const kvStationIDList_t stations(1, stationID);
    return dataForStationsParamTimerange(r, stations, paramID, first, last);
}

void DBInterface::dataForStationParamTimerange(kvDataList_t& r, int stationID, int paramID, const miutil::miTime& middle, int hoursBefore, int hoursAfter) throw (DBException)
{
    const kvStationIDList_t stations(1, stationID);
    miutil::miTime first=middle, last=middle;
    first.addHour( hoursBefore );
    last.addHour( hoursAfter );
    return dataForStationsParamTimerange(r, stations, paramID, first, last);
}

void DBInterface::selectData(kvDataList_t& r, const Constraint::DBConstraint& where) throw (DBException)
{
    return selectData(r, Constraint::WHERE(where));
}

void DBInterface::selectData(kvDataList_t& r, const Constraint::DBConstraint& where, const Ordering::DBOrdering& order_by) throw (DBException)
{
    return selectData(r, Constraint::WHERE(where) + Ordering::ORDER_BY(order_by));
}

void DBInterface::insertData(const kvalobs::kvData& d, bool replace) throw (DBException)
{
    const kvDataList_t datalist(1, d);
    return insertData(datalist, replace);
}
