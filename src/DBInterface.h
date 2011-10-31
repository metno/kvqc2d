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

#ifndef DBINTERFACE_H_
#define DBINTERFACE_H_

#include <kvalobs/kvData.h>
#include <kvalobs/kvQueries.h>
#include <kvalobs/kvStation.h>
#include <kvalobs/kvStationParam.h>

#include "DBConstraints.h"

#include <exception>

class DBException : public std::runtime_error {
public:
    DBException(const std::string& what)
        : std::runtime_error(what) { }
};

/**
 * Wrapper for kvalobs database connections.
 */
class DBInterface {
public:
    virtual ~DBInterface() { }

    typedef std::list<kvalobs::kvData> kvDataList_t;
    typedef std::list<kvalobs::kvStationParam> kvStationParamList_t;
    typedef std::list<int> kvStationIDList_t;
    typedef std::list<kvalobs::kvStation> kvStationList_t;

    bool dataForStationsParamTimerange(kvDataList_t& d, const kvStationIDList_t& s, int paramID, const miutil::miTime& first, const miutil::miTime& last)
        { return selectData(d, kvQueries::selectData(s, paramID, first, last)); }

    /**
     * Select station data for one parameter id in the specified time range.
     * Calls dataForStationsParamTimerange.
     */
    bool dataForStationParamTimerange(kvDataList_t&, int stationID, int paramID, const miutil::miTime& first, const miutil::miTime& last);

    /**
     * Select station data like in dataForStationParamTimerange, but specifying relative times.
     * Calls dataForStationsParamTimerange.
     */
    bool dataForStationParamTimerange(kvDataList_t&, int stationID, int paramID, const miutil::miTime& middle, int hoursBefore, int hoursAfter);

    virtual bool selectData(kvDataList_t&, const miutil::miString& where) = 0;

    virtual bool selectData(kvDataList_t&, const Constraint::DBConstraint& where);

    virtual bool selectData(kvDataList_t&, const Constraint::DBConstraint& where, const Ordering::DBOrdering& order_by);

    virtual bool selectStationparams(kvStationParamList_t&, int stationID, const miutil::miTime& time, const std::string& qcx) = 0;

    virtual bool selectStations(kvStationList_t&) = 0;

    /**
     * Insert data to table tblName (or main table), replacing existing data if replace is true.
     */
    virtual bool insertData(const kvDataList_t&, bool replace=false) = 0;

    /**
     * Insert single data item to table tblName (or main table), replacing existing data if replace is true.
     * Calls insert.
     */
    bool insertData(const kvalobs::kvData&, bool replace=false);

};


#endif /* DBINTERFACE_H_ */
