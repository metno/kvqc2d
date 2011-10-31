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

#include "DBConstraints.h"
#include "foreach.h"

namespace Constraint {

std::string ControlinfoImpl::sql() const
{
    return mFlagMatcher.sql("controlinfo", false);
}

std::string UseinfoImpl::sql() const
{
    return mFlagMatcher.sql("useinfo", false);
}

std::string ObstimeImpl::sql() const
{
    if ( mT0 != mT1 )
        return "obstime BETWEEN '" + mT0.isoTime() + "' AND '" + mT1.isoTime() + "'";
    else
        return "obstime = '" + mT0.isoTime() + "'";
}

std::string IntegerColumnnImpl::sql() const
{
    unsigned int n = mValues.size();
    if( n == 0 )
        return "";
    std::ostringstream sql;
    if( n == 1 ) {
        sql << mColumnName << " = " << mValues.front();
    } else {
        sql << mColumnName << " IN ";
        char sep = '(';
        foreach(int v, mValues) {
            sql << sep << v;
            sep = ',';
        }
        sql << ')';
    }
    return sql.str();
}

StationImpl& StationImpl::add(const std::list<kvalobs::kvStation>& stations)
{
    foreach(const kvalobs::kvStation& s, stations)
        add(s);
    return *this;
}

StationImpl& StationImpl::add(const std::list<int>& stationIDs)
{
    foreach(int sid, stationIDs)
        add(sid);
    return *this;
}

const char* StationImpl::stationid = "stationid";

ParamidImpl& ParamidImpl::add(const std::list<int>& paramIDs)
{
    foreach(int pid, paramIDs)
        add(pid);
    return *this;
}

ParamidImpl& ParamidImpl::add(int pid)
{
    IntegerColumnnImpl::add(pid);
    return *this;
}

const char* ParamidImpl::paramid = "paramid";

TypeidImpl& TypeidImpl::add(const std::vector<int>& typeIDs)
{
    foreach(int t, typeIDs)
        add(t);
    return *this;
}

TypeidImpl& TypeidImpl::add(const std::list<int>& typeIDs)
{
    foreach(int t, typeIDs)
        add(t);
    return *this;
}

TypeidImpl& TypeidImpl::add(int tid)
{
    IntegerColumnnImpl::add(tid);
    return *this;
}

const char* TypeidImpl::typeID = "typeid";

std::string AndImpl::sql() const
{
    const std::string sqlA = mA.sql(), sqlB = mB.sql();
    if( sqlA.empty() )
        return sqlB;
    if( sqlB.empty() )
        return sqlA;
    return sqlA + " AND " + sqlB;
}

std::string OrImpl::sql() const
{
    const std::string sqlA = mA.sql(), sqlB = mB.sql();
    if( sqlA.empty() )
        return sqlB;
    if( sqlB.empty() )
        return sqlA;
    return "(" +sqlA + ") OR (" + sqlB + ")";
}

std::string WHERE(const DBConstraint& c)
{
    const std::string sql = c.sql();
    if( sql.empty() )
        return "";
    return " WHERE " + sql;
}

} // namespace Contraint

namespace Ordering {

std::string ColumnImpl::sql() const
{
    return mColumn + (mAscending ? " ASC" : " DESC");
}

Obstime::Obstime()
    : Column("obstime")
{
}

Stationid::Stationid()
    : Column("stationid")
{
}

std::string SequenceImpl::sql() const
{
    return mFirst.sql() + ", " + mSecond.sql();
}

std::string ORDER_BY(const DBOrdering& o)
{
    return " ORDER BY " + o.sql();
}
} // namespace Ordering
