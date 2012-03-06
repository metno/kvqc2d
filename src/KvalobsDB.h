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

#ifndef STANDARDDB_H_
#define STANDARDDB_H_

#include "DBInterface.h"
#include <kvalobs/kvDbGate.h>

class Qc2App;

class KvalobsDB: public DBInterface {
public:
    KvalobsDB(Qc2App& app);
    virtual ~KvalobsDB();

    virtual void selectData(kvDataList_t&, const std::string& where) throw (DBException);
    virtual void selectStations(kvStationList_t&) throw (DBException);
    virtual void selectStationparams(kvStationParamList_t&, int stationID, const miutil::miTime& time, const std::string& qcx) throw (DBException);
    virtual void storeData(const kvDataList_t& toUpdate, const kvDataList_t& toInsert) throw (DBException);
    virtual reference_value_map_t selectStatisticalReferenceValues(int paramid, const std::string& key, float missingValue);

private:
    void connect();
    void disconnect();
    
private:
    Qc2App& mApp;
    dnmi::db::Connection* mConnection;
    kvalobs::kvDbGate mDbGate;
};

#endif /* STANDARDDB_H_ */
