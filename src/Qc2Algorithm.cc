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

#include "Qc2Algorithm.h"

#include "helpers/AlgorithmHelpers.h"
#include "Broadcaster.h"
#include "DBInterface.h"
#include "foreach.h"

#include <milog/milog.h>

Qc2Algorithm::Qc2Algorithm(const std::string& name)
    : missing(-32767)
    , rejected(-32766)
    , mDatabase(0)
    , mBroadcaster(0)
    , mNotifier(0)
    , mName(name)
{
}

Qc2Algorithm::~Qc2Algorithm()
{
}

void Qc2Algorithm::fillStationLists(DBInterface::StationList& stations, DBInterface::StationIDList& idList)
{
    stations = mDatabase->findNorwegianFixedStations();

    idList.clear();
    foreach(const kvalobs::kvStation& s, stations) {
        idList.push_back( s.stationID() );
    }
}

void Qc2Algorithm::fillStationIDList(std::list<int>& idList)
{
    std::list<kvalobs::kvStation> stations;
    fillStationLists(stations, idList);
}

void Qc2Algorithm::updateSingle(const kvalobs::kvData& update)
{
    const DBInterface::DataList toUpdate(1, update);
    storeData(toUpdate);
}

void Qc2Algorithm::storeData(const DBInterface::DataList& toUpdate, const DBInterface::DataList& toInsert)
{
    database()->storeData(toUpdate, toInsert);
    foreach(const kvalobs::kvData& i, toInsert) {
        broadcaster()->queueChanged(i);
        info() << mName << " NEW ROW: " << Helpers::datatext(i);
    }
    foreach(const kvalobs::kvData& u, toUpdate) {
        broadcaster()->queueChanged(u);
        info() << mName << " UPDATE:  " << Helpers::datatext(u);
    }
    broadcaster()->sendChanges();
}

void Qc2Algorithm::configure(const AlgorithmConfig& params)
{
    UT0            = params.UT0;
    UT1            = params.UT1;
    CFAILED_STRING = params.CFAILED_STRING;
    missing        = params.missing;
    rejected       = params.rejected;
}
