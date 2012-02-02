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

#include "DBConstraintsBase.h"

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

    virtual void selectData(kvDataList_t&, const miutil::miString& where) throw (DBException) = 0;

    virtual void selectData(kvDataList_t&, const Constraint::DBConstraint& where) throw (DBException);

    virtual void selectData(kvDataList_t&, const Constraint::DBConstraint& where, const Ordering::DBOrdering& order_by) throw (DBException);

    virtual void selectStationparams(kvStationParamList_t&, int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException) = 0;

    virtual void selectStations(kvStationList_t&) throw (DBException) = 0;

    /**
     * Update and insert data.
     */
    virtual void storeData(const kvDataList_t& toUpdate, const kvDataList_t& toInsert) throw (DBException) = 0;

    /**
     * Updates single data item in its table.
     * Calls storeData.
     */
    void updateSingle(const kvalobs::kvData& toUpdate) throw (DBException);

    /**
     * Insert a single data item to its table.
     * Calls storeData.
     */
    void insertSingle(const kvalobs::kvData& toInsert) throw (DBException);

};


#endif /* DBINTERFACE_H_ */
