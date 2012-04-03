/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2012 met.no

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

#ifndef DataAccessSQLQueries_h
#define DataAccessSQLQueries_h

#include <string>
#include <DBInterface.h>

namespace DataAccessSQLQueries {

std::string queryForNorwegianFixedStations() throw (DBException);

// ----------------------------------------

std::string queryForNorwegianFixedStationIDs() throw (DBException);

// ----------------------------------------

std::string queryForData(const DBInterface::StationIDList& stationIDs, int pid, const TimeRange& time, const FlagSetCU& flags);
std::string queryForData(int stationID, int paramID, const TimeRange& time);
std::string queryForData(int stationID, int paramID, int typeID, const TimeRange& time);
std::string queryForData(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t);
std::string queryForData(int stationID, int paramID, int typeID, int sensor, int level, const TimeRange& t, const FlagSetCU& flags);
std::string queryForData(const DBInterface::StationIDList& stationIDs, int paramID, const std::vector<int>& tids, const TimeRange& time);
std::string queryForData(int stationID, const std::vector<int>& pids, const std::vector<int>& tids, int sensor, int level, const TimeRange& time, const FlagSetCU& flags);
std::string queryForData(const DBInterface::StationIDList& stationIDs, int paramID, int typeID, const TimeRange& t, const FlagSetCU& flags);
std::string queryForData(const DBInterface::StationIDList& stationIDs, const std::vector<int>& pids, const std::vector<int>& tids, const TimeRange& t, const FlagSetCU& flags);

// ----------------------------------------

std::string queryForStationParams(int stationID, const miutil::miTime& time, const std::string& qcx);

// ----------------------------------------

std::string queryForStatisticalReferenceValues(int paramid, const std::string& key);

// ----------------------------------------

std::string queryForNeighborData(int stationid, int paramid);

// ----------------------------------------

std::string queryForModelData(int stationID, int paramID, int level, const TimeRange& time);

} // namespace DataAccessSQLQueries

#endif /* DataAccessSQLQueries_h */
