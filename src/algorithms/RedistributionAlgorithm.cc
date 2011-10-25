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

miutil::miTime plusDay(const miutil::miTime& t, int nDays)
{
    miutil::miTime p(t);
    p.addDay(nDays);
    return p;
}

class DBConstraint  {
public:
    virtual ~DBConstraint() { }
    virtual std::string sql() const = 0;
};

class ControlinfoConstraint : public DBConstraint {
public:
    ControlinfoConstraint(const FlagMatcher& fm)
        : mFlagMatcher(fm) { }
    virtual std::string sql() const
        { return mFlagMatcher.sql("controlinfo", false); }
private:
    FlagMatcher mFlagMatcher;
};

class UseinfoConstraint : public DBConstraint {
public:
    UseinfoConstraint(const FlagMatcher& fm)
        : mFlagMatcher(fm) { }
    virtual std::string sql() const
        { return mFlagMatcher.sql("useinfo", false); }
private:
    FlagMatcher mFlagMatcher;
};

class ObstimeConstraint : public DBConstraint {
public:
    ObstimeConstraint(const miutil::miTime& t0, const miutil::miTime& t1)
        : mT0(t0), mT1(t1) { }
    virtual std::string sql() const
        { return "obstime BETWEEN '" + mT0.isoTime() + "' AND '" + mT1.isoTime() + "'"; }
private:
    miutil::miTime mT0, mT1;
};

class StationConstraint : public DBConstraint {
public:
    StationConstraint()
        { }
    StationConstraint(const std::list<kvalobs::kvStation>& stations)
        { add(stations); }
    StationConstraint(const std::list<int>& stationIDs)
        { add(stationIDs); }
    StationConstraint(const kvalobs::kvStation& s)
        { add(s.stationID()); }
    StationConstraint(int sid)
        { add(sid); }
    StationConstraint& add(const std::list<kvalobs::kvStation>& stations)
        { foreach(const kvalobs::kvStation& s, stations) add(s); return *this; }
    StationConstraint& add(const std::list<int>& stationIDs)
        { foreach(int sid, stationIDs) add(sid); return *this; }
    StationConstraint& add(const kvalobs::kvStation& s)
        { mStationIDs.push_back(s.stationID()); return *this; }
    StationConstraint& add(int sid)
        { mStationIDs.push_back(sid); return *this; }
    virtual std::string sql() const;
private:
    std::list<int> mStationIDs;
};

std::string StationConstraint::sql() const
{
    unsigned int n = mStationIDs.size();
    if( n == 0 )
        return "";
    std::ostringstream sql;
    if( n == 1 ) {
        sql << "stationid = " << mStationIDs.front();
    } else {
        sql << "stationid IN ";
        char sep = '(';
        foreach(int sid, mStationIDs) {
            sql << sep << sid;
            sep = ',';
        }
        sql << ')';
    }
    return sql.str();
}

class ParamidConstraint : public DBConstraint {
public:
    ParamidConstraint()
        { }
    ParamidConstraint(const std::list<int>& paramIDs)
        { add(paramIDs); }
    ParamidConstraint(int pid)
        { add(pid); }
    ParamidConstraint& add(const std::list<int>& paramIDs)
        { foreach(int pid, paramIDs) add(pid); return *this; }
    ParamidConstraint& add(int pid)
        { mParamIDs.push_back(pid); return *this; }
    virtual std::string sql() const;
private:
    std::list<int> mParamIDs;
};

std::string ParamidConstraint::sql() const
{
    unsigned int n = mParamIDs.size();
    if( n == 0 )
        return "";
    std::ostringstream sql;
    if( n == 1 ) {
        sql << "paramid = " << mParamIDs.front();
    } else {
        sql << "paramid IN ";
        char sep = '(';
        foreach(int pid, mParamIDs) {
            sql << sep << pid;
            sep = ',';
        }
        sql << ')';
    }
    return sql.str();
}

class TypeidConstraint : public DBConstraint {
public:
    TypeidConstraint()
        { }
    TypeidConstraint(const std::vector<int>& typeIDs)
        { add(typeIDs); }
    TypeidConstraint(const std::list<int>& typeIDs)
        { add(typeIDs); }
    TypeidConstraint(int tid)
        { add(tid); }
    TypeidConstraint& add(const std::vector<int>& typeIDs)
        { foreach(int t, typeIDs) add(t); return *this; }
    TypeidConstraint& add(const std::list<int>& typeIDs)
        { foreach(int t, typeIDs) add(t); return *this; }
    TypeidConstraint& add(int tid)
        { mTypeIDs.push_back(tid); return *this; }
    virtual std::string sql() const;
private:
    std::list<int> mTypeIDs;
};

std::string TypeidConstraint::sql() const
{
    unsigned int n = mTypeIDs.size();
    if( n == 0 )
        return "";
    std::ostringstream sql;
    if( n == 1 ) {
        sql << "typeid = " << mTypeIDs.front();
    } else {
        sql << "typeid IN ";
        char sep = '(';
        foreach(int sid, mTypeIDs) {
            sql << sep << sid;
            sep = ',';
        }
        sql << ')';
    }
    return sql.str();
}

class AndConstraint : public DBConstraint {
public:
    AndConstraint(const DBConstraint& a, const DBConstraint& b)
        : mA(a), mB(b) { }
    virtual std::string sql() const;
private:
    const DBConstraint&  mA;
    const DBConstraint&  mB;
};

std::string AndConstraint::sql() const
{
    const std::string sqlA = mA.sql(), sqlB = mB.sql();
    if( sqlA.empty() )
        return sqlB;
    if( sqlB.empty() )
        return sqlA;
    return sqlA + " AND " + sqlB;
}

class OrConstraint : public DBConstraint {
public:
    OrConstraint(const DBConstraint&  a, const DBConstraint&  b)
        : mA(a), mB(b) { }
    virtual std::string sql() const;
private:
    const DBConstraint&  mA;
    const DBConstraint&  mB;
};

std::string OrConstraint::sql() const
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

class DBOrdering {
public:
    virtual ~DBOrdering() { }
    virtual std::string sql() const = 0;
};

class ColumnOrdering : public DBOrdering {
public:
    ColumnOrdering(const std::string& column)
        : mColumn(column), mAscending(true) { }
    virtual std::string sql() const
        { return mColumn + (mAscending ? " ASC" : " DESC"); }

    DBOrdering& asc()
        { mAscending = true; return *this; }
    DBOrdering& desc()
        { mAscending = false; return *this; }
private:
    std::string mColumn;
    bool mAscending;
};

class SequenceOrdering : public DBOrdering {
public:
    SequenceOrdering(const DBOrdering& first, const DBOrdering& second)
        : mFirst(first), mSecond(second) { }
    virtual std::string sql() const
        { return mFirst.sql() + ", " + mSecond.sql(); }
private:
    const DBOrdering& mFirst;
    const DBOrdering& mSecond;
};

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

    dataList_t allDataOneTime;
    if( !database()->selectData(allDataOneTime,
            WHERE(ControlinfoConstraint(FlagMatcher().permit(f_fmis, 4).permit(f_fd, 2).permit(f_fhqc, 0))
                    && ParamidConstraint(params.pid) && TypeidConstraint(params.tids)
                    && ObstimeConstraint(params.UT0, params.UT1))) )
    {
        LOGERROR("Problem with query in ProcessRedistribution");
        return;
    }
    foreach(const kvalobs::kvData& d, allDataOneTime) {
        //std::cout << "center=" << d << std::endl;

        dataList_t bdata;
        if( !database()->selectData(bdata,
                WHERE(ControlinfoConstraint(FlagMatcher().permit(f_fmis, 3).permit(f_fd, 2).permit(f_fhqc, 0))
                        && ParamidConstraint(params.pid) && TypeidConstraint(d.typeID())
                        && ObstimeConstraint(params.UT0, d.obstime())
                        && StationConstraint(d.stationID()))
                + ORDER_BY(ColumnOrdering("obstime").desc())) )
        {
            LOGERROR("Problem with query in ProcessRedistribution");
            std::cout << " => sql error" << std::endl;
            return;
        }
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

        if( !nf.hashStationList() ) {
            std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
            std::list<int> allStationIDs;
            fillStationLists(allStations, allStationIDs);
            nf.setStationList(allStations);
        }

        NeighboringStationFinder::stationsWithDistances_t neighbors;
        nf.findNeighbors(neighbors, d.stationID(), params.InterpolationLimit);
        StationConstraint sc;
        foreach(NeighboringStationFinder::stationsWithDistances_t::value_type& v, neighbors)
            sc.add( v.first );
        dataList_t ndata;
        if( !database()->selectData(ndata,
                WHERE(UseinfoConstraint(FlagMatcher().permit(2, 0))
                        && ParamidConstraint(params.pid) && TypeidConstraint(d.typeID())
                        && ObstimeConstraint(before.back().obstime(), d.obstime()) && sc)
                + ORDER_BY((ColumnOrdering("obstime").desc(), ColumnOrdering("stationid")))) )
        {
            LOGERROR("Problem with query in ProcessRedistribution");
            return;
        }

        float sumint = 0;
        dataList_t::const_iterator itN = ndata.begin(), itB = before.begin();
        for(; itB != before.end() && itN != ndata.end(); ++itB ) {
            //std::cout << "itb obstime=" << itB->obstime() << " itN obstime=" << itN->obstime() << std::endl;
            float sumWeights = 0.0, sumWeightedValues = 0.0;
            for( ; itN->obstime() == itB->obstime(); ++itN ) {
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
            }
            sumint += sumWeightedValues/sumWeights;
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
