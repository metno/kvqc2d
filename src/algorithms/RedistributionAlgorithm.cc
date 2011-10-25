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

#include <milog/milog.h>
#include <puTools/miTime.h>
#include "foreach.h"

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
    return sqlA + " OR " + sqlB;
}

AndConstraint operator &&(const DBConstraint& a, const DBConstraint&  b)
{ return AndConstraint(a, b); }

OrConstraint operator ||(const DBConstraint& a, const DBConstraint& b)
{ return OrConstraint(a, b); }

std::string SQL(const DBConstraint& c)
{
    const std::string sql = c.sql();
    if( sql.empty() )
        return "";
    return "WHERE " + sql;
}

class NeighboringStationFinder {
public:
    typedef std::list<kvalobs::kvStation> stations_t;
    typedef std::map<int, kvalobs::kvStation> stationsByID_t;

    virtual ~NeighboringStationFinder() { }

    bool hashStationList() const
        { return !mStationsByID.empty(); }

    void setStationList(const stations_t& stations);

    virtual void findNeighbors(stations_t& neighbors, int aroundID, float maxdist);

private:
    stationsByID_t mStationsByID;
};

void NeighboringStationFinder::setStationList(const stations_t& stations)
{
    foreach(const kvalobs::kvStation& s, stations) {
        mStationsByID[ s.stationID() ] = s;
    }
}

void NeighboringStationFinder::findNeighbors(stations_t& neighbors, int aroundID, float maxdist)
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
        if( distance > 0 && distance < maxdist )
            neighbors.push_back( station );
    }
}

void RedistributionAlgorithm2::run(const ReadProgramOptions& params)
{
    NeighboringStationFinder nf;

    FlagMatcher fm;
    fm.permit(kvQCFlagTypes::f_fmis, 4).permit(kvQCFlagTypes::f_fd, 2).permit(kvQCFlagTypes::f_fhqc, 0);

    const std::string constraint = SQL(ControlinfoConstraint(fm)
            && ObstimeConstraint(params.UT0, params.UT1));
    std::cout << "sql='" << constraint << "'" << std::endl;
    std::list<kvalobs::kvData> allDataOneTime;
    if( !database()->selectData(allDataOneTime, constraint) ) {
        LOGERROR("Problem with query in ProcessRedistribution");
        std::cout << " => sql error" << std::endl;
        return;
    }
    std::cout << " => got " << allDataOneTime.size() << std::endl;
    foreach(const kvalobs::kvData& d, allDataOneTime) {
        std::cout << "center=" << d << std::endl;

        FlagMatcher fm1;
        fm1.permit(kvQCFlagTypes::f_fmis, 3).permit(kvQCFlagTypes::f_fd, 2).permit(kvQCFlagTypes::f_fhqc, 0);
        const std::string constraint1 = SQL(ControlinfoConstraint(fm1)
                && ObstimeConstraint(params.UT0, d.obstime())
                && StationConstraint(d.stationID()))
                + " ORDER BY obstime";
        std::cout << "sql2='" << constraint1 << "'" << std::endl;
        std::list<kvalobs::kvData> bdata;
        if( !database()->selectData(bdata, constraint1) ) {
            LOGERROR("Problem with query in ProcessRedistribution");
            std::cout << " => sql error" << std::endl;
            return;
        }
        std::cout << " => got " << bdata.size() << std::endl;
        foreach(const kvalobs::kvData& b, bdata) {
            std::cout << "before=" << b << std::endl;
        }

        if( !nf.hashStationList() ) {
            std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
            std::list<int> allStationIDs;
            fillStationLists(allStations, allStationIDs);
            nf.setStationList(allStations);
        }

        std::list<kvalobs::kvStation> neighbors;
        nf.findNeighbors(neighbors, d.stationID(), params.InterpolationLimit);

        const std::string constraint2 = SQL(UseinfoConstraint(FlagMatcher().permit(2, 0))
                && ObstimeConstraint(params.UT0, d.obstime())
                && StationConstraint(neighbors))
                + " ORDER BY obstime, stationid";
        std::cout << "sql2='" << constraint2 << "'" << std::endl;
        std::list<kvalobs::kvData> ndata;
        if( !database()->selectData(ndata, constraint2) ) {
            LOGERROR("Problem with query in ProcessRedistribution");
            std::cout << " => sql error" << std::endl;
            return;
        }
        std::cout << " => got " << ndata.size() << std::endl;
        foreach(const kvalobs::kvData& n, ndata) {
            std::cout << "neighbor=" << n << std::endl;
        }
    }
}
