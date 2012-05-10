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

#include "helpers/StationParamParser.h"
#include "helpers/timeutil.h"
#include "AggregatorLimits.h"
#include "DataUpdate.h"
#include "foreach.h"

#define NDEBUG 1
#include "debug.h"

namespace {

struct SPInstrument {
    int stationid, paramid;
    SPInstrument(int s, int p)
        : stationid(s), paramid(p) { }
};

struct lt_SPInstrument : public std::binary_function<SPInstrument, SPInstrument, bool> {
    bool operator () (const SPInstrument& a, const SPInstrument& b) const;
};

bool lt_SPInstrument::operator () (const SPInstrument& a, const SPInstrument& b) const
{
    if( a.stationid < b.stationid )
        return true;
    if( a.stationid > b.stationid )
        return false;
    if( a.paramid < b.paramid )
        return true;
    return false;
}


struct Limits {
    float min, max;
    int dayFrom, dayTo;

    bool valid() const;

    Limits()
        : min(0), max(0), dayFrom(0), dayTo(0) { }

    Limits(float mi, float ma, int f, int t)
        : min(mi), max(ma), dayFrom(f), dayTo(t) { }
};

bool Limits::valid() const
{
    return dayFrom >= 1 && dayFrom <= 365 && dayTo >= 1 && dayTo <= 365 && dayFrom <= dayTo;
}

struct lt_Limits : public std::binary_function<Limits, Limits, bool> {
    bool operator () (const Limits&  a, const Limits& b) const
        { return a.dayFrom < b.dayFrom; }
    bool operator () (const Limits& l, int day) const
        { return l.dayFrom < day; }
};

} // anonymous namespace

class AggregatorLimits::LimitValues {
public:
    static const Limits INVALID;
    Limits find(int stationid, int paramid, int hour, int day);
    void add(int stationid, int paramid, int hour, int dayFrom, int dayTo, float min, float max);

private:
    typedef std::list<Limits> LimitsPerDayOfYear;
    typedef std::map<int, LimitsPerDayOfYear> LimitsPerHour;
    typedef std::map<SPInstrument, LimitsPerHour, lt_SPInstrument> LimitsPerStation;

private:
    bool findInstrument(int stationid, int paramid, int hour, int day, Limits& limits);
    bool findHour(const LimitsPerHour& lph, int hour, int day, Limits& limits);
    bool findDayOfYear(const LimitsPerDayOfYear& lpd, int day, Limits& limits);

private:
    LimitsPerStation lps;
};

const Limits AggregatorLimits::LimitValues::INVALID;

bool AggregatorLimits::LimitValues::findInstrument(int stationid, int paramid, int hour, int day, Limits& limits)
{
    const int N = 2, ids[N] = { stationid, 0 };
    for(int i=0; i<N; ++i) {
        DBGV(i);
        LimitsPerStation::const_iterator its = lps.find(SPInstrument(ids[i], paramid));
        if( its != lps.end() && findHour(its->second, hour, day, limits) )
            return true;
    }
    DBGL;
    return false;
}

bool AggregatorLimits::LimitValues::findHour(const LimitsPerHour& lph, int hour, int day, Limits& limits)
{
    DBGL;
    LimitsPerHour::const_iterator ith = lph.find(hour);
    if( ith != lph.end() && findDayOfYear(ith->second, day, limits) )
        return true;
    DBGL;
    ith = lph.find(-1);
    if( ith != lph.end() && findDayOfYear(ith->second, day, limits) )
        return true;
    DBGL;
    return false;
}

bool AggregatorLimits::LimitValues::findDayOfYear(const LimitsPerDayOfYear& lpd, int day, Limits& limits)
{
    DBGL;
    if( lpd.empty() )
        return false;

    DBGL;
    LimitsPerDayOfYear::const_iterator itd = std::lower_bound(lpd.begin(), lpd.end(), day, lt_Limits());
    if( itd == lpd.end() )
        --itd;
    DBG(DBG1(itd->dayFrom) << DBG1(itd->dayTo));
    if( itd->dayFrom <= day && itd->dayTo >= day ) {
        DBGL;
        limits = *itd;
        return true;
    }
    DBGL;
    return false;
}

Limits AggregatorLimits::LimitValues::find(int stationid, int paramid, int hour, int day)
{
    DBG(DBG1(stationid) << DBG1(paramid) << DBG1(hour) << DBG1(day));
    Limits limits;
    findInstrument(stationid, paramid, hour, day, limits);
    DBG(DBG1(limits.dayFrom) << DBG1(limits.dayTo)DBG1(limits.min) << DBG1(limits.max));
    return limits;
}

void AggregatorLimits::LimitValues::add(int stationid, int paramid, int hour, int dayFrom, int dayTo, float min, float max)
{
    DBG(DBG1(stationid) << DBG1(paramid) << DBG1(hour) << DBG1(min) << DBG1(max));
    const Limits limits(min, max, dayFrom, dayTo);
    LimitsPerDayOfYear& lpd = lps[SPInstrument(stationid, paramid)][hour];
    if( lpd.empty() ) {
        lpd.push_back(limits);
        return;
    }
    lpd.insert(std::lower_bound(lpd.begin(), lpd.end(), limits, lt_Limits()), limits);
}

AggregatorLimits::AggregatorLimits()
    : Qc2Algorithm("AggregatorLimits")
{
}

void AggregatorLimits::configure(const AlgorithmConfig& config)
{
    mParameters = config.getMultiParameter<int>("ParamID");
    mMinQCX = config.getParameter<std::string>("minQCX", "min");
    mMaxQCX = config.getParameter<std::string>("maxQCX", "max");
    config.getFlagSetCU(mFlags, "aggregation", "fr=)6(", "");
    config.getFlagChange(mFlagChangeMin, "aggregation_flagchange_min", "fr=6");
    config.getFlagChange(mFlagChangeMax, "aggregation_flagchange_max", "fr=6");

    Qc2Algorithm::configure(config);
}

void AggregatorLimits::addStationLimits(LimitValues& lv, const DBInterface::StationParamList& spl)
{
    foreach(const kvalobs::kvStationParam& sp, spl) {
        const StationParamParser spp(sp);
        const float min = spp.floatValue(mMinQCX, -32767), max = spp.floatValue(mMaxQCX, 32767);
        lv.add(sp.stationID(), sp.paramID(), sp.hour(), sp.fromday(), sp.today(), min, max);
    }
}

void AggregatorLimits::run()
{
    const miutil::miTime now = miutil::miTime::nowTime();

    LimitValues lv;
    DBInterface::StationIDList allStations(1, DBInterface::ALL_STATIONS);
    addStationLimits(lv, database()->findStationParams(allStations, mParameters, "QC1-1-"));
    addStationLimits(lv, database()->findStationParams(DBInterface::StationIDList(1, 0), mParameters, "QC1-1-"));

    DBInterface::DataList updates;

    const DBInterface::DataList outOfRange = database()->findDataAggregations(allStations, mParameters, TimeRange(UT0, UT1), mFlags);
    foreach(const kvalobs::kvData& data, outOfRange) {
        DataUpdate du(data);
        DBGV(data);
        const Limits limits = lv.find(data.stationID(), data.paramID(), data.obstime().hour(), Helpers::normalisedDayOfYear(data.obstime().date()));
        if( !limits.valid() ) {
            error() << "no parameter limits found for " << data;
            continue;
        }
        if( du.original() < limits.min ) {
            du.controlinfo(mFlagChangeMin.apply(du.controlinfo()));
            du.cfailed("QC2-agglim-min");
        } else if( du.original() > limits.max ) {
            du.controlinfo(mFlagChangeMax.apply(du.controlinfo()));
            du.cfailed("QC2-agglim-max");
        } else {
            DBGL;
            continue;
        }

        updates.push_back(du.data());
    }
    const DBInterface::DataList inserts; // empty
    storeData(updates, inserts);
}
