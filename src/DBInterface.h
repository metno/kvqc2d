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
#include <kvalobs/kvStationParam.h>

/**
 * Wrapper for kvalobs database connections.
 */
class DBInterface {
public:
    virtual ~DBInterface() { }

    typedef std::list<kvalobs::kvData> kvDataList_t;
    typedef std::list<kvalobs::kvStationParam> kvStationParamList_t;
    typedef std::list<int> StationList_t;

    virtual bool dataForStationsParamTimerange(kvDataList_t&, const StationList_t&, int paramID, const miutil::miTime& first, const miutil::miTime& last) = 0;

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

    virtual bool data(kvDataList_t&, const miutil::miString& where) = 0;

    virtual bool queryStationparams(kvStationParamList_t&, int stationID, const miutil::miTime& time, const std::string& qcx) = 0;

    /**
     * Insert data to table tblName (or main table), replacing existing data if replace is true.
     */
    virtual bool insert(const kvDataList_t&, bool replace=false, const miutil::miString &tblName="") = 0;

    /**
     * Insert single data item to table tblName (or main table), replacing existing data if replace is true.
     * Calls insert.
     */
    bool insert(const kvalobs::kvData&, bool replace=false, const miutil::miString &tblName="");

};


#endif /* DBINTERFACE_H_ */
