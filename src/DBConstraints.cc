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
    return mFlagSet.sql("controlinfo");
}

std::string UseinfoImpl::sql() const
{
    return mFlagSet.sql("useinfo");
}

std::string ControlUseinfoImpl::sql() const
{
    return mFlagSetCU.sql();
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

ParamidImpl& ParamidImpl::add(const std::vector<int>& paramIDs)
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

SensorImpl& SensorImpl::add(const std::vector<int>& sensors)
{
    foreach(int s, sensors)
        add(s);
    return *this;
}

SensorImpl& SensorImpl::add(const std::list<int>& sensors)
{
    foreach(int s, sensors)
        add(s);
    return *this;
}

SensorImpl& SensorImpl::add(int tid)
{
    IntegerColumnnImpl::add(tid);
    return *this;
}

const char* SensorImpl::sensor = "sensor";

LevelImpl& LevelImpl::add(const std::vector<int>& levels)
{
    foreach(int l, levels)
        add(l);
    return *this;
}

LevelImpl& LevelImpl::add(const std::list<int>& levels)
{
    foreach(int l, levels)
        add(l);
    return *this;
}

LevelImpl& LevelImpl::add(int lvl)
{
    IntegerColumnnImpl::add(lvl);
    return *this;
}

const char* LevelImpl::level = "level";

std::string SameDeviceImpl::sql() const
{
    std::ostringstream s;
    s << "stationid=" << mStationID << " AND "
      << "paramid="   << mParamID   << " AND "
      << "typeid="    << mTypeID    << " AND "
      << "sensor='"   << mSensor    << "' AND "
      << "level="     << mLevel;
    return s.str();
}

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

std::string NotImpl::sql() const
{
    const std::string sqlC = mC.sql();
    if( sqlC.empty() )
        return "0=1";
    if( sqlC == "0=1" )
        return "";
    return "NOT (" +sqlC + ")";
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

} // namespace Ordering
