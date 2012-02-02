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

#include "DBInterface.h"

namespace {
std::string WHERE(const Constraint::DBConstraint& c)
{
    const std::string sql = c.sql();
    if( sql.empty() )
        return "";
    return " WHERE " + sql;
}

std::string ORDER_BY(const Ordering::DBOrdering& o)
{
    return " ORDER BY " + o.sql();
}
} // anonymous namespace

void DBInterface::selectData(kvDataList_t& r, const Constraint::DBConstraint& where) throw (DBException)
{
    return selectData(r, WHERE(where));
}

void DBInterface::selectData(kvDataList_t& r, const Constraint::DBConstraint& where, const Ordering::DBOrdering& order_by) throw (DBException)
{
    return selectData(r, WHERE(where) + ORDER_BY(order_by));
}

void DBInterface::updateSingle(const kvalobs::kvData& update) throw (DBException)
{
    const kvDataList_t toUpdate(1, update), toInsert;
    return storeData(toUpdate, toInsert);
}

void DBInterface::insertSingle(const kvalobs::kvData& insert) throw (DBException)
{
    const kvDataList_t toUpdate, toInsert(1, insert);
    return storeData(toUpdate, toInsert);
}
