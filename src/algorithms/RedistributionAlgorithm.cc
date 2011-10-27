/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
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

#include "RedistributionAlgorithm.h"

#include "AlgorithmHelpers.h"
#include "Helpers.h"
#include "FlagMatcher.h"
#include "tround.h"

#include <milog/milog.h>
#include <puTools/miTime.h>
#include "foreach.h"

static const unsigned int MIN_NEIGHBORS = 1;

miutil::miTime plusDay(const miutil::miTime& t, int nDays)
{
    miutil::miTime p(t);
    p.addDay(nDays);
    return p;
}

template<class T>
class SQLBuilderPointer {
public:
    template<class D>
    SQLBuilderPointer(const SQLBuilderPointer<D>& d)
        : p(d.p) { }

    explicit SQLBuilderPointer()
        : p( new T()) { }

    template<class P1>
    explicit SQLBuilderPointer(const P1& p1)
        : p( new T(p1)) { }

    template<class P1, class P2>
    explicit SQLBuilderPointer(const P1& p1, const P2& p2)
        : p( new T(p1, p2)) { }

    template<class P1, class P2, class P3>
    explicit SQLBuilderPointer(const P1& p1, const P2& p2, const P3& p3)
        : p( new T(p1, p2, p3)) { }

    template<class P1, class P2, class P3, class P4>
    explicit SQLBuilderPointer(const P1& p1, const P2& p2, const P3& p3, const P4& p4)
        : p( new T(p1, p2, p3, p4)) { }

    std::string sql() const
        { return p->sql(); }

protected:
    template<class D> friend class SQLBuilderPointer;

    boost::shared_ptr<T> p;
};

class DBConstraintImpl  {
public:
    virtual ~DBConstraintImpl() { }
    virtual std::string sql() const = 0;
};
typedef SQLBuilderPointer<DBConstraintImpl> DBConstraint;

class ControlinfoConstraintImpl : public DBConstraintImpl {
public:
    ControlinfoConstraintImpl(const FlagMatcher& fm)
        : mFlagMatcher(fm) { }
    virtual std::string sql() const
        { return mFlagMatcher.sql("controlinfo", false); }
private:
    FlagMatcher mFlagMatcher;
};
typedef SQLBuilderPointer<ControlinfoConstraintImpl> ControlinfoConstraint;

class UseinfoConstraintImpl : public DBConstraintImpl {
public:
    UseinfoConstraintImpl(const FlagMatcher& fm)
        : mFlagMatcher(fm) { }
    virtual std::string sql() const
        { return mFlagMatcher.sql("useinfo", false); }
private:
    FlagMatcher mFlagMatcher;
};
typedef SQLBuilderPointer<UseinfoConstraintImpl> UseinfoConstraint;

class ObstimeConstraintImpl : public DBConstraintImpl {
public:
    ObstimeConstraintImpl(const miutil::miTime& t0, const miutil::miTime& t1)
        : mT0(t0), mT1(t1) { if( mT0 > mT1 ) std::swap(mT0, mT1); }
    ObstimeConstraintImpl(const miutil::miTime& t)
        : mT0(t), mT1(t) { }
    virtual std::string sql() const;
private:
    miutil::miTime mT0, mT1;
};
typedef SQLBuilderPointer<ObstimeConstraintImpl> ObstimeConstraint;

std::string ObstimeConstraintImpl::sql() const
{
    if ( mT0 != mT1 )
        return "obstime BETWEEN '" + mT0.isoTime() + "' AND '" + mT1.isoTime() + "'";
    else
        return "obstime = '" + mT0.isoTime() + "'";
}

class IntegerColumnnConstraintImpl : public DBConstraintImpl {
protected:
    IntegerColumnnConstraintImpl(const std::string& columnName)
        : mColumnName(columnName) { }
    IntegerColumnnConstraintImpl& add(int pid)
        { mValues.push_back(pid); return *this; }
    virtual std::string sql() const;
private:
    std::string mColumnName;
    std::list<int> mValues;
};

std::string IntegerColumnnConstraintImpl::sql() const
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


class StationConstraintImpl : public IntegerColumnnConstraintImpl {
public:
    StationConstraintImpl()
        : IntegerColumnnConstraintImpl(stationid) { }
    StationConstraintImpl(const std::list<kvalobs::kvStation>& stations)
        : IntegerColumnnConstraintImpl(stationid) { add(stations); }
    StationConstraintImpl(const std::list<int>& stationIDs)
        : IntegerColumnnConstraintImpl(stationid) { add(stationIDs); }
    StationConstraintImpl(const kvalobs::kvStation& s)
        : IntegerColumnnConstraintImpl(stationid) { add(s.stationID()); }
    StationConstraintImpl(int sid)
        : IntegerColumnnConstraintImpl(stationid) { add(sid); }
    StationConstraintImpl& add(const std::list<kvalobs::kvStation>& stations)
        { foreach(const kvalobs::kvStation& s, stations) add(s); return *this; }
    StationConstraintImpl& add(const std::list<int>& stationIDs)
        { foreach(int sid, stationIDs) add(sid); return *this; }
    StationConstraintImpl& add(const kvalobs::kvStation& s)
        { add(s.stationID()); return *this; }
    StationConstraintImpl& add(int sid)
        { IntegerColumnnConstraintImpl::add(sid); return *this; }
private:
    static const char* stationid;
};
class StationConstraint : public SQLBuilderPointer<StationConstraintImpl> {
public:
    StationConstraint()
        : SQLBuilderPointer<StationConstraintImpl>() { }
    StationConstraint(int sid)
        : SQLBuilderPointer<StationConstraintImpl>() { add(sid); }
    StationConstraint& add(int s)
        { p->add(s); return *this; }
};
const char* StationConstraintImpl::stationid = "stationid";

class ParamidConstraintImpl : public IntegerColumnnConstraintImpl {
public:
    ParamidConstraintImpl()
        : IntegerColumnnConstraintImpl(paramid) { }
    ParamidConstraintImpl(const std::list<int>& paramIDs)
        : IntegerColumnnConstraintImpl(paramid) { add(paramIDs); }
    ParamidConstraintImpl(int pid)
        : IntegerColumnnConstraintImpl(paramid) { add(pid); }
    ParamidConstraintImpl& add(const std::list<int>& paramIDs)
        { foreach(int pid, paramIDs) add(pid); return *this; }
    ParamidConstraintImpl& add(int pid)
        { IntegerColumnnConstraintImpl::add(pid); return *this; }
private:
    static const char* paramid;
};
typedef SQLBuilderPointer<ParamidConstraintImpl> ParamidConstraint;
const char* ParamidConstraintImpl::paramid = "paramid";

class TypeidConstraintImpl : public IntegerColumnnConstraintImpl {
public:
    TypeidConstraintImpl()
        : IntegerColumnnConstraintImpl(typeID) { }
    TypeidConstraintImpl(const std::vector<int>& typeIDs)
        : IntegerColumnnConstraintImpl(typeID) { add(typeIDs); }
    TypeidConstraintImpl(const std::list<int>& typeIDs)
        : IntegerColumnnConstraintImpl(typeID) { add(typeIDs); }
    TypeidConstraintImpl(int tid)
        : IntegerColumnnConstraintImpl(typeID) { add(tid); }
    TypeidConstraintImpl& add(const std::vector<int>& typeIDs)
        { foreach(int t, typeIDs) add(t); return *this; }
    TypeidConstraintImpl& add(const std::list<int>& typeIDs)
        { foreach(int t, typeIDs) add(t); return *this; }
    TypeidConstraintImpl& add(int tid)
        { IntegerColumnnConstraintImpl::add(tid); return *this; }
private:
    static const char* typeID;
};
typedef SQLBuilderPointer<TypeidConstraintImpl> TypeidConstraint;
const char* TypeidConstraintImpl::typeID = "typeid";

class AndConstraintImpl : public DBConstraintImpl {
public:
    AndConstraintImpl(const DBConstraint& a, const DBConstraint& b)
        : mA(a), mB(b) { }
    virtual std::string sql() const;
private:
    const DBConstraint mA;
    const DBConstraint mB;
};
typedef SQLBuilderPointer<AndConstraintImpl> AndConstraint;

std::string AndConstraintImpl::sql() const
{
    const std::string sqlA = mA.sql(), sqlB = mB.sql();
    if( sqlA.empty() )
        return sqlB;
    if( sqlB.empty() )
        return sqlA;
    return sqlA + " AND " + sqlB;
}

class OrConstraintImpl : public DBConstraintImpl {
public:
    OrConstraintImpl(const DBConstraint&  a, const DBConstraint&  b)
        : mA(a), mB(b) { }
    virtual std::string sql() const;
private:
    const DBConstraint mA;
    const DBConstraint mB;
};
typedef SQLBuilderPointer<OrConstraintImpl> OrConstraint;

std::string OrConstraintImpl::sql() const
{
    const std::string sqlA = mA.sql(), sqlB = mB.sql();
    if( sqlA.empty() )
        return sqlB;
    if( sqlB.empty() )
        return sqlA;
    return "(" +sqlA + ") OR (" + sqlB + ")";
}

AndConstraint operator &&(const DBConstraint& a, const DBConstraint&  b)
{ return AndConstraint(a, b); }

OrConstraint operator ||(const DBConstraint& a, const DBConstraint& b)
{ return OrConstraint(a, b); }

std::string WHERE(const DBConstraint& c)
{
    const std::string sql = c.sql();
    if( sql.empty() )
        return "";
    return " WHERE " + sql;
}

class DBOrderingImpl {
public:
    virtual ~DBOrderingImpl() { }
    virtual std::string sql() const = 0;
};
typedef SQLBuilderPointer<DBOrderingImpl> DBOrdering;

class ColumnOrderingImpl : public DBOrderingImpl {
public:
    ColumnOrderingImpl(const std::string& column)
        : mColumn(column), mAscending(true) { }
    virtual std::string sql() const
        { return mColumn + (mAscending ? " ASC" : " DESC"); }

    ColumnOrderingImpl& asc()
        { mAscending = true; return *this; }
    ColumnOrderingImpl& desc()
        { mAscending = false; return *this; }
private:
    std::string mColumn;
    bool mAscending;
};
//typedef SQLBuilderPointer<ColumnOrderingImpl> ColumnOrdering;

class ColumnOrdering : public SQLBuilderPointer<ColumnOrderingImpl> {
public:
    ColumnOrdering(const std::string& column)
        : SQLBuilderPointer<ColumnOrderingImpl>(column) { }
    ColumnOrdering& asc()
        { p->asc(); return *this; }
    ColumnOrdering& desc()
        { p->desc(); return *this; }
};

class SequenceOrderingImpl : public DBOrderingImpl {
public:
    SequenceOrderingImpl(const DBOrdering& first, const DBOrdering& second)
        : mFirst(first), mSecond(second) { }
    virtual std::string sql() const
        { return mFirst.sql() + ", " + mSecond.sql(); }
private:
    const DBOrdering mFirst;
    const DBOrdering mSecond;
};
typedef SQLBuilderPointer<SequenceOrderingImpl> SequenceOrdering;

SequenceOrdering operator , (const DBOrdering& a, const DBOrdering& b)
{
    return SequenceOrdering(a, b);
}

std::string ORDER_BY(const DBOrdering& o)
{
    return " ORDER BY " + o.sql();
}

class NeighboringStationFinder {
public:
    typedef std::map<int, double> stationsWithDistances_t;
    typedef std::list<kvalobs::kvStation> stations_t;
    typedef std::map<int, kvalobs::kvStation> stationsByID_t;

    virtual ~NeighboringStationFinder() { }

    bool hashStationList() const
        { return !mStationsByID.empty(); }

    void setStationList(const stations_t& stations);

    virtual void findNeighbors(stationsWithDistances_t& neighbors, int aroundID, float maxdist);

private:
    stationsByID_t mStationsByID;
};

void NeighboringStationFinder::setStationList(const stations_t& stations)
{
    foreach(const kvalobs::kvStation& s, stations) {
        mStationsByID[ s.stationID() ] = s;
    }
}

void NeighboringStationFinder::findNeighbors(stationsWithDistances_t& neighbors, int aroundID, float maxdist)
{
    neighbors.clear();
    if( mStationsByID.find(aroundID) == mStationsByID.end() )
        return;
    const kvalobs::kvStation& around = mStationsByID.at(aroundID);
    foreach(stationsByID_t::value_type& v, mStationsByID) {
        const kvalobs::kvStation& station = v.second;
        if( station.stationID() == aroundID )
            continue;
        const double distance = Helpers::distance(around.lon(), around.lat(), station.lon(), station.lat());
        if( distance > 0 && distance < maxdist ) {
            neighbors[ station.stationID() ] = distance;
        }
    }
}

void RedistributionAlgorithm2::run(const ReadProgramOptions& params)
{
    using namespace kvQCFlagTypes;
    typedef std::list<kvalobs::kvData> dataList_t;

    NeighboringStationFinder nf;

    const ColumnOrdering order_by_time = ColumnOrdering("obstime").desc();
    const SequenceOrdering order_by_time_id = (order_by_time, ColumnOrdering("stationid"));
    const ControlinfoConstraint controli_endpoint(FlagMatcher().permit(f_fmis, 4).permit(f_fd, 2).permit(f_fhqc, 0));
    const ControlinfoConstraint controli_missing(FlagMatcher().permit(f_fmis, 3).permit(f_fd, 2).permit(f_fhqc, 0));
    const UseinfoConstraint usei_neighbors(FlagMatcher().permit(2, 0));

    dataList_t allDataOneTime;
    if( !database()->selectData(allDataOneTime,
            WHERE(controli_endpoint
                    && ParamidConstraint(params.pid)
                    && TypeidConstraint(params.tids)
                    && ObstimeConstraint(params.UT0, params.UT1))) )
    {
        LOGERROR("Problem with query in ProcessRedistribution");
        return;
    }
    foreach(const kvalobs::kvData& d, allDataOneTime) {
        //std::cout << "center=" << d << std::endl;

        dataList_t bdata;
        if( !database()->selectData(bdata,
                WHERE(controli_missing
                        && ParamidConstraint(params.pid)
                        && TypeidConstraint(d.typeID())
                        && ObstimeConstraint(params.UT0, d.obstime())
                        && StationConstraint(d.stationID()))
                + ORDER_BY(order_by_time)) )
        {
            LOGERROR("Problem with query in ProcessRedistribution");
            std::cout << " => sql error" << std::endl;
            return;
        }

        // FIXME check for start of database, too
        dataList_t before;
        before.push_back(d);
        miutil::miTime t = plusDay(d.obstime(), -1);
        foreach(const kvalobs::kvData& b, bdata) {
            if( b.obstime() != t )
                break;
            before.push_back(b);
            t.addDay(-1);
        }
        if( before.size()<=1 ) {
            //std::cout << "no data before accumulated value" << std::endl;
            continue;
        }
        if( before.back().obstime() <= params.UT0 ) {
            //std::cout << "before starts at UT0, no idea about series length" << std::endl;
            continue;
        }

        dataList_t startdata;
        if( !database()->selectData(startdata,
                WHERE(/*usei_neighbors // need to define some constraints on this value
                        &&*/ ParamidConstraint(params.pid)
                        && TypeidConstraint(d.typeID())
                        && ObstimeConstraint(t)
                        && StationConstraint(d.stationID()))
                + ORDER_BY(order_by_time)) )
        {
            LOGERROR("Problem with query in ProcessRedistribution");
            std::cout << " => sql error" << std::endl;
            return;
        }
        if( startdata.size() != 1 || startdata.front().original() == params.missing || startdata.front().original() == params.rejected ) {
            LOGINFO("value before time series not existing/missing/rejected/not usable at t=" << t << " for accumulation in " << d);
            continue;
        }

        if( !nf.hashStationList() ) {
            std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
            std::list<int> allStationIDs;
            fillStationLists(allStations, allStationIDs);
            nf.setStationList(allStations);
        }

        NeighboringStationFinder::stationsWithDistances_t neighbors;
        nf.findNeighbors(neighbors, d.stationID(), params.InterpolationLimit);
        if( neighbors.size() < MIN_NEIGHBORS ) {
            LOGINFO("less than " << MIN_NEIGHBORS << " neighbor(s) within " << params.InterpolationLimit
                    << " km for stationid=" << d.stationID());
            continue;
        }

        StationConstraint sc;
        foreach(NeighboringStationFinder::stationsWithDistances_t::value_type& v, neighbors)
            sc.add( v.first );
        dataList_t ndata;
        if( !database()->selectData(ndata,
                WHERE(usei_neighbors
                        && ParamidConstraint(params.pid)
                        && TypeidConstraint(d.typeID())
                        && ObstimeConstraint(before.back().obstime(), d.obstime())
                        && sc)
                + ORDER_BY(order_by_time_id)) )
        {
            LOGERROR("Problem with query in ProcessRedistribution");
            return;
        }

        bool neighborsMissing = false;
        float sumint = 0;
        dataList_t::const_iterator itN = ndata.begin(), itB = before.begin();
        for(; itB != before.end(); ++itB ) {
            //std::cout << "itb obstime=" << itB->obstime() << " itN obstime=" << itN->obstime() << std::endl;
            float sumWeights = 0.0, sumWeightedValues = 0.0;
            unsigned int n = 0;
            for( ; itN != ndata.end() && itN->obstime() == itB->obstime(); ++itN ) {
                //std::cout << "neighbour id = " << itN->stationID() << std::endl;
                float data_point = itN->original();
                if (data_point == -1)
                    data_point = 0; // These are bone dry measurements as opposed to days when there may have been rain but none was measurable
                if( data_point <= -1 )
                    continue;
                const double dist = neighbors.at(itN->stationID()), invDist2 = 1.0/(dist*dist);
                //std::cout << "distance =" << dist << " km" << std::endl;
                sumWeights += invDist2;
                sumWeightedValues += data_point*invDist2;
                n += 1;
            }
            if( n < MIN_NEIGHBORS ) {
                neighborsMissing = true;
                break;
            }
            sumint += sumWeightedValues/sumWeights;
        }
        if( neighborsMissing ){
            LOGERROR("not enough good neighbors d=" << d);
            continue;
        }
        std::list<kvalobs::kvData> toWrite;
        //std::cout << "accval = " << d.original() << " sumint=" << sumint << std::endl;
        itN = ndata.begin(), itB = before.begin();
        for(; itB != before.end() && itN != ndata.end(); ++itB ) {
            std::ostringstream cfailed;
            cfailed << "QC2N";
            float sumWeights = 0.0, sumWeightedValues = 0.0;
            for( ; itN->obstime() == itB->obstime(); ++itN ) {
                float data_point = itN->original();
                if (data_point == -1)
                    data_point = 0; // These are bone dry measurements as opposed to days when there may have been rain but none was measurable
                if( data_point <= -1 )
                    continue;
                const double dist = neighbors.at(itN->stationID()), invDist2 = 1.0/(dist*dist);
                sumWeights += invDist2;
                sumWeightedValues += data_point*invDist2;
                cfailed << "_" << itN->stationID();
            }
            cfailed << ",QC2-redist";
            float itB_corrected = (sumWeightedValues/sumWeights) * (d.original() / sumint);
            if( d.original() == -1 || itB_corrected == 0 )
                itB_corrected = -1; // bugzilla 1304: by default assume dry

            // TODO make sure that sum of re-distributed is the same as the original accumulated value
            //std::cout << "corrected at " << itB->obstime() << " = " << itB_corrected << std::endl;

            kvalobs::kvControlInfo fixflags = itB->controlinfo();
            checkFlags().setter(fixflags, params.Sflag);
            checkFlags().conditional_setter(fixflags, params.chflag);

            kvalobs::kvData correctedData(*itB);
            correctedData.corrected(round<float, 1>(itB_corrected));
            correctedData.controlinfo(fixflags);
            Helpers::updateUseInfo(correctedData);
            Helpers::updateCfailed(correctedData, cfailed.str(), params.CFAILED_STRING);
            toWrite.push_back(correctedData);
        }

        // we accumulated the corrections in time-reversed order, while tests expect them the other way around => foreach_r
        foreach_r(const kvalobs::kvData& corr, toWrite) {
            if( !database()->insertData(corr, true) ) {
                LOGERROR("Could not write to database");
                continue;
            }
            broadcaster()->queueChanged(corr);
        }
        broadcaster()->sendChanges();
    }
}
