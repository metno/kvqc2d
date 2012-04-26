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

#ifndef KvalobsDB_h
#define KvalobsDB_h 1

#include "SQLDataAccess.h"
#include "KvalobsDbGate.h"

class Qc2App;

class KvalobsDB : public SQLDataAccess {
public:
    KvalobsDB(Qc2App& app);
    virtual ~KvalobsDB();

protected:
    virtual StationList extractStations(const std::string& sql) throw (DBException);
    virtual StationIDList extractStationIDs(const std::string& sql) throw (DBException);
    virtual StationParamList extractStationParams(const std::string& sql) throw (DBException);
    virtual DataList extractData(const std::string& sql) throw (DBException);
    virtual reference_value_map_t extractStatisticalReferenceValues(const std::string& sql, float missingValue) throw (DBException);
    virtual NeighborDataVector extractNeighborData(const std::string& sql) throw (DBException);
    virtual ModelDataList extractModelData(const std::string& sql) throw (DBException);
    virtual void execSQLUpdate(const std::string& sql) throw (DBException);

    virtual void formatStationIDList(std::ostream& sql, const StationIDList& stationIDs);

private:
    void connect();
    void disconnect();

private:
    Qc2App& mApp;
    KvalobsDbGate mDbGate;
};

#endif /* KvalobsDB_h */
