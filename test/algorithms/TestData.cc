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

#include "TestData.h"

#include "foreach.h"
#include "TestDB.h"

DataList& DataList::add(int stationid, const miutil::miTime& obstime, float original, int paramid,
                        int type, float corrected, const std::string& controlinfo, const std::string& cfailed)
{
    kvalobs::kvControlInfo ci(controlinfo);
    kvalobs::kvUseInfo ui;
    ui.setUseFlags(ci);
    push_back(kvalobs::kvData(stationid, obstime, original, paramid, obstime, type, 0, 0, corrected, ci, ui, cfailed));
    return *this;
}

// ------------------------------------------------------------------------

void DataList::insert(SqliteTestDB* db)
{
    std::list<kvalobs::kvData> toUpdate;
    db->storeData(toUpdate, *this);
    clear();
}

// ------------------------------------------------------------------------

void DataList::update(SqliteTestDB* db)
{
    // cannot use storeData because it does not modify the original value
    std::ostringstream sql;
    sql << "BEGIN;";
    foreach(const kvalobs::kvData& d, *this) {
        sql << "UPDATE data SET original=" << d.original() << ", corrected=" << d.corrected()
            << ", controlinfo='" << d.controlinfo().flagstring() << "', useinfo='" << d.useinfo().flagstring()
            << "', cfailed='" << d.cfailed() << "' " << d.uniqueKey() << ';';
    }
    sql << "COMMIT;";
    db->exec(sql.str());
    clear();
}
