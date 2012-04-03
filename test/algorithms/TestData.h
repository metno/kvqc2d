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

#ifndef TestData_h
#define TestData_h 1

#include <kvalobs/kvData.h>
#include <list>
class SqliteTestDB;

// #######################################################################

class DataList : public std::list<kvalobs::kvData> {
public:
    DataList(int stationid, int paramid, int tid)
        : mStationId(stationid), mParamId(paramid), mTypeId(tid) { }

    DataList& setStation(int sid)
        { mStationId = sid; return *this; }
    DataList& setParam(int pid)
        { mParamId = pid; return *this; }
    DataList& setType(int tid)
        { mTypeId = tid; return *this; }

    DataList& add(int stationid, const miutil::miTime& obstime, float original, int paramid, int type, float corrected, const std::string& controlinfo, const std::string& cfailed);

    DataList& add(int stationid, const miutil::miTime& obstime, float original, int paramid, int type, const std::string& controlinfo, const std::string& cfailed)
        { return add(stationid, obstime, original, paramid, type, original, controlinfo, cfailed); }

    DataList& add(const miutil::miTime& obstime, float original, float corrected, const std::string& controlinfo, const std::string& cfailed)
        { return add(mStationId, obstime, original, mParamId, mTypeId, corrected, controlinfo, cfailed); }

    DataList& add(const miutil::miTime& obstime, float original, const std::string& controlinfo, const std::string& cfailed="")
        { return add(obstime, original, original, controlinfo, cfailed); }

    DataList& add(int stationid, const miutil::miTime& obstime, float original, const std::string& controlinfo, const std::string& cfailed)
        { return add(stationid, obstime, original, mParamId, mTypeId, original, controlinfo, cfailed); }

    void insert(SqliteTestDB* db);
    void update(SqliteTestDB* db);

private:
    int mStationId, mParamId, mTypeId;
};

#endif /* ALGORITHMTESTBASE_H_ */
