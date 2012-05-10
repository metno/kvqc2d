/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

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

#include "TimeRange.h"
#include <kvalobs/kvData.h>
#include <kvalobs/kvModelData.h>
#include <kvalobs/kvStation.h>
#include <kvalobs/kvStationParam.h>

#include <exception>

class DBException : public std::runtime_error {
public:
    DBException(const std::string& what)
        : std::runtime_error(what) { }
};

class FlagSetCU;

struct NeighborData {
    int neighborid; // TODO maybe include typeid, level, sensor here?
    double offset, slope, sigma;
    NeighborData(int xid, double xoffset, double xslope, double xsigma)
        : neighborid(xid), offset(xoffset), slope(xslope), sigma(xsigma) { }
};

typedef std::vector<NeighborData> NeighborDataVector;

/**
 * Wrapper for kvalobs database connections.
 */
class DBInterface {
public:
    virtual ~DBInterface() { }

    enum { INVALID_ID = -32767, ALL_STATIONS = -1 };

    // ----------------------------------------

    typedef std::list<kvalobs::kvStation> StationList;
    virtual StationList findNorwegianFixedStations() throw (DBException) = 0;

    // ----------------------------------------

    typedef std::list<int> StationIDList;
    virtual StationIDList findNorwegianFixedStationIDs() throw (DBException) = 0;

    // ----------------------------------------

    typedef std::list<kvalobs::kvStationParam> StationParamList;
    virtual StationParamList findStationParams(int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException) = 0;

    // ----------------------------------------

    typedef std::list<kvalobs::kvData> DataList;
    virtual DataList findDataOrderObstime(const StationIDList& stationIDs, int pid, const TimeRange& time, const FlagSetCU& flags) throw (DBException) = 0;
    virtual DataList findDataOrderObstime(int stationID, int paramID, const TimeRange& time) throw (DBException) = 0;
    virtual DataList findDataOrderObstime(int stationID, int paramID, int typeID, const TimeRange& time) throw (DBException) = 0;
    virtual DataList findDataOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t) throw (DBException) = 0;
    virtual DataList findDataMaybeTSLOrderObstime(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t, const FlagSetCU& flags) throw (DBException) = 0;
    virtual DataList findDataOrderObstime(const StationIDList& stationIDs, int paramID, const std::vector<int>& tids, const TimeRange& time) throw (DBException) = 0;
    virtual DataList findDataOrderObstime(int stationID, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags) throw (DBException) = 0;
    virtual DataList findDataOrderObstime(const StationIDList& stationIDs, int paramID, int typeID, const TimeRange& t, const FlagSetCU& flags) throw (DBException) = 0;
    virtual DataList findDataOrderStationObstime(const StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, const TimeRange& t, const FlagSetCU& flags) throw (DBException) = 0;
    virtual DataList findAggregationOutsideRange(const StationIDList& stationIDs, const std::vector<int>& pids, const TimeRange& t, const FlagSetCU& flags, float min, float max) throw (DBException) = 0;

    // ----------------------------------------

    typedef std::vector<float> reference_values_t;
    typedef std::map<int, reference_values_t> reference_value_map_t;
    virtual reference_value_map_t findStatisticalReferenceValues(int paramid, const std::string& key, float missingValue) throw (DBException) = 0;

    // ----------------------------------------

    virtual NeighborDataVector findNeighborData(int stationid, int paramid, float maxsigma) throw (DBException) = 0;

    // ----------------------------------------

    typedef std::list<kvalobs::kvModelData> ModelDataList;
    virtual ModelDataList findModelData(int stationID, int paramID, int level, const TimeRange& time) throw (DBException) = 0;

    // ----------------------------------------

    /** Update and/or insert data. */
    virtual void storeData(const DataList& toUpdate, const DataList& toInsert) throw (DBException) = 0;

};


#endif /* DBINTERFACE_H_ */
