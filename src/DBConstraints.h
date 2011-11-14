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

#ifndef DBCONSTRAINTS_H_
#define DBCONSTRAINTS_H_

#include "DBConstraintsBase.h"
#include "FlagSet.h"
#include <kvalobs/kvStation.h>
#include <puTools/miTime.h>

namespace Constraint {

class ControlinfoImpl : public DBConstraintImpl {
public:
    ControlinfoImpl(const FlagMatcher& fm)
        : mFlagSet(fm) { }
    ControlinfoImpl(const FlagSet& fs)
        : mFlagSet(fs) { }
    virtual std::string sql() const;
private:
    FlagSet mFlagSet;
};
typedef SQLBuilderPointer<ControlinfoImpl> Controlinfo;

class UseinfoImpl : public DBConstraintImpl {
public:
    UseinfoImpl(const FlagMatcher& fm)
        : mFlagSet(fm) { }
    UseinfoImpl(const FlagSet& fs)
        : mFlagSet(fs) { }
    virtual std::string sql() const;
private:
    FlagSet mFlagSet;
};
typedef SQLBuilderPointer<UseinfoImpl> Useinfo;

class ControlUseinfoImpl : public DBConstraintImpl {
public:
    ControlUseinfoImpl(const FlagSetCU& fcu)
        : mFlagSetCU(fcu) { }
    virtual std::string sql() const;
private:
    FlagSetCU mFlagSetCU;
};
typedef SQLBuilderPointer<ControlUseinfoImpl> ControlUseinfo;

class ObstimeImpl : public DBConstraintImpl {
public:
    ObstimeImpl(const miutil::miTime& t0, const miutil::miTime& t1)
        : mT0(t0), mT1(t1) { if( mT0 > mT1 ) std::swap(mT0, mT1); }
    ObstimeImpl(const miutil::miTime& t)
        : mT0(t), mT1(t) { }
    virtual std::string sql() const;
private:
    miutil::miTime mT0, mT1;
};
typedef SQLBuilderPointer<ObstimeImpl> Obstime;

class IntegerColumnnImpl : public DBConstraintImpl {
protected:
    IntegerColumnnImpl(const std::string& columnName)
        : mColumnName(columnName) { }
    IntegerColumnnImpl& add(int pid)
        { mValues.push_back(pid); return *this; }
    virtual std::string sql() const;
private:
    std::string mColumnName;
    std::list<int> mValues;
};

class StationImpl : public IntegerColumnnImpl {
public:
    StationImpl()
        : IntegerColumnnImpl(stationid) { }
    StationImpl(const std::list<kvalobs::kvStation>& stations)
        : IntegerColumnnImpl(stationid) { add(stations); }
    StationImpl(const std::list<int>& stationIDs)
        : IntegerColumnnImpl(stationid) { add(stationIDs); }
    StationImpl(const kvalobs::kvStation& s)
        : IntegerColumnnImpl(stationid) { add(s.stationID()); }
    StationImpl(int sid)
        : IntegerColumnnImpl(stationid) { add(sid); }
    StationImpl& add(const std::list<kvalobs::kvStation>& stations);
    StationImpl& add(const std::list<int>& stationIDs);
    StationImpl& add(const kvalobs::kvStation& s)
        { add(s.stationID()); return *this; }
    StationImpl& add(int sid)
        { IntegerColumnnImpl::add(sid); return *this; }
private:
    static const char* stationid;
};
class Station : public SQLBuilderPointer<StationImpl> {
public:
    Station()
        : SQLBuilderPointer<StationImpl>() { }
    Station(int sid)
        : SQLBuilderPointer<StationImpl>(sid) { }
    Station(const std::list<kvalobs::kvStation>& stations)
        : SQLBuilderPointer<StationImpl>(stations) { }
    Station(const std::list<int>& stationIDs)
        : SQLBuilderPointer<StationImpl>(stationIDs) { }
    Station& add(int s)
        { p->add(s); return *this; }
};

class ParamidImpl : public IntegerColumnnImpl {
public:
    ParamidImpl()
        : IntegerColumnnImpl(paramid) { }
    ParamidImpl(const std::list<int>& paramIDs)
        : IntegerColumnnImpl(paramid) { add(paramIDs); }
    ParamidImpl(const std::vector<int>& paramIDs)
        : IntegerColumnnImpl(paramid) { add(paramIDs); }
    ParamidImpl(int pid)
        : IntegerColumnnImpl(paramid) { add(pid); }
    ParamidImpl& add(const std::list<int>& paramIDs)
        { return add(std::vector<int>(std::vector<int>(paramIDs.begin(), paramIDs.end()))); }
    ParamidImpl& add(const std::vector<int>& paramIDs);
    ParamidImpl& add(int pid);
private:
    static const char* paramid;
};
typedef SQLBuilderPointer<ParamidImpl> Paramid;

class TypeidImpl : public IntegerColumnnImpl {
public:
    TypeidImpl()
        : IntegerColumnnImpl(typeID) { }
    TypeidImpl(const std::vector<int>& typeIDs)
        : IntegerColumnnImpl(typeID) { add(typeIDs); }
    TypeidImpl(const std::list<int>& typeIDs)
        : IntegerColumnnImpl(typeID) { add(typeIDs); }
    TypeidImpl(int tid)
        : IntegerColumnnImpl(typeID) { add(tid); }
    TypeidImpl& add(const std::vector<int>& typeIDs);
    TypeidImpl& add(const std::list<int>& typeIDs);
    TypeidImpl& add(int tid);
private:
    static const char* typeID;
};
typedef SQLBuilderPointer<TypeidImpl> Typeid;

class SensorImpl : public IntegerColumnnImpl {
public:
    SensorImpl()
        : IntegerColumnnImpl(sensor) { }
    SensorImpl(const std::vector<int>& sensors)
        : IntegerColumnnImpl(sensor) { add(sensors); }
    SensorImpl(const std::list<int>& sensors)
        : IntegerColumnnImpl(sensor) { add(sensors); }
    SensorImpl(int sens)
        : IntegerColumnnImpl(sensor) { add(sens); }
    SensorImpl& add(const std::vector<int>& sensors);
    SensorImpl& add(const std::list<int>& sensors);
    SensorImpl& add(int sens);
private:
    static const char* sensor;
};
typedef SQLBuilderPointer<SensorImpl> Sensor;

class LevelImpl : public IntegerColumnnImpl {
public:
    LevelImpl()
        : IntegerColumnnImpl(level) { }
    LevelImpl(const std::vector<int>& levels)
        : IntegerColumnnImpl(level) { add(levels); }
    LevelImpl(const std::list<int>& levels)
        : IntegerColumnnImpl(level) { add(levels); }
    LevelImpl(int lvl)
        : IntegerColumnnImpl(level) { add(lvl); }
    LevelImpl& add(const std::vector<int>& levels);
    LevelImpl& add(const std::list<int>& levels);
    LevelImpl& add(int lvl);
private:
    static const char* level;
};
typedef SQLBuilderPointer<LevelImpl> Level;

class SameDeviceImpl : public DBConstraintImpl {
public:
    SameDeviceImpl(const kvalobs::kvData& d)
        : mStationID(d.stationID()), mParamID(d.paramID()), mTypeID(d.typeID()), mSensor(d.sensor()), mLevel(d.level()) { }
    virtual std::string sql() const;
private:
    int mStationID, mParamID, mTypeID, mSensor, mLevel;
};
typedef SQLBuilderPointer<SameDeviceImpl> SameDevice;

class AndImpl : public DBConstraintImpl {
public:
    AndImpl(const DBConstraint& a, const DBConstraint& b)
        : mA(a), mB(b) { }
    virtual std::string sql() const;
private:
    const DBConstraint mA;
    const DBConstraint mB;
};
typedef SQLBuilderPointer<AndImpl> And;

class OrImpl : public DBConstraintImpl {
public:
    OrImpl(const DBConstraint&  a, const DBConstraint&  b)
        : mA(a), mB(b) { }
    virtual std::string sql() const;
private:
    const DBConstraint mA;
    const DBConstraint mB;
};
typedef SQLBuilderPointer<OrImpl> Or;

class NotImpl : public DBConstraintImpl {
public:
    NotImpl(const DBConstraint& c, bool)
        : mC(c) { }
    virtual std::string sql() const;
private:
    const DBConstraint mC;
};
typedef SQLBuilderPointer<NotImpl> Not;

inline And operator &&(const DBConstraint& a, const DBConstraint&  b)
{ return And(a, b); }

inline Or operator ||(const DBConstraint& a, const DBConstraint& b)
{ return Or(a, b); }

inline Not operator !(const DBConstraint& c)
{ return Not(c, true); }

} // namespace Constraint

namespace Ordering {

class ColumnImpl : public DBOrderingImpl {
public:
    ColumnImpl(const std::string& column)
        : mColumn(column), mAscending(true) { }
    virtual std::string sql() const;

    ColumnImpl& asc()
        { mAscending = true; return *this; }
    ColumnImpl& desc()
        { mAscending = false; return *this; }
private:
    std::string mColumn;
    bool mAscending;
};

class Column : public SQLBuilderPointer<ColumnImpl> {
protected:
    Column(const std::string& column)
        : SQLBuilderPointer<ColumnImpl>(column) { }
public:
    Column& asc()
        { p->asc(); return *this; }
    Column& desc()
        { p->desc(); return *this; }
};

class Obstime : public Column {
public:
    Obstime();
};

class Stationid : public Column {
public:
    Stationid();
};

class SequenceImpl : public DBOrderingImpl {
public:
    SequenceImpl(const DBOrdering& first, const DBOrdering& second)
        : mFirst(first), mSecond(second) { }
    virtual std::string sql() const;
private:
    const DBOrdering mFirst;
    const DBOrdering mSecond;
};
typedef SQLBuilderPointer<SequenceImpl> Sequence;

inline Sequence operator , (const DBOrdering& a, const DBOrdering& b)
{
    return Sequence(a, b);
}

} // namespace Ordering

#endif /* DBCONSTRAINTS_H_ */
