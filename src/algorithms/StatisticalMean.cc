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

#include "StatisticalMean.h"

#include "AlgorithmHelpers.h"
#include "algorithms/NeighborsDistance2.h"
#include "DBConstraints.h"
#include "DBInterface.h"
#include "foreach.h"

#include <milog/milog.h>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#define NDEBUG
#include "debug.h"

namespace C = Constraint;
namespace O = Ordering;
using Helpers::equal;

// ========================================================================

class DayMean {
public:
    DayMean(int d, float m)
        : mDay(d), mMean(m) { }
    int day() const
        { return mDay; }
    float mean() const
        { return mMean; }

    int normalisedDayOfYear(const miutil::miDate& day0) const;
    
private:
    int mDay;
    float mMean;
};

// ------------------------------------------------------------------------

int DayMean::normalisedDayOfYear(const miutil::miDate& day0) const
{
    miutil::miDate date(day0);
    date.addDay(mDay);
    return Helpers::normalisedDayOfYear(date);
}

// ========================================================================

StatisticalMean::StatisticalMean()
    : Qc2Algorithm("StatisticalMean")
    , mNeighbors(new NeighborsDistance2())
{
}

// ------------------------------------------------------------------------

std::list<int> StatisticalMean::findNeighbors(int stationID)
{
    if( !mNeighbors->hasStationList() ) {
        std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
        std::list<int> allStationIDs;
        fillStationLists(allStations, allStationIDs);
        mNeighbors->setStationList(allStations);
    }
    
    return mNeighbors->findNeighbors(stationID);
}

// ------------------------------------------------------------------------

void StatisticalMean::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    params.getFlagSetCU(ok_flags,      "ok", "", "U2=0");

    mTolerance = params.getParameter<int>("tolerance", 10.0f);
    mDays      = params.getParameter<int>("days", 30);
    mDaysRequired = params.getParameter<int>("days_required", int(0.9*mDays + 0.5));
    mParamid   = params.getParameter<int>("ParamId");
    mTypeids   = params.getMultiParameter<int>("TypeIds");

    mUT0extended = UT0;
    mUT0extended.addDay(-mDays);

    mNeighbors->configure(params);
}

// ------------------------------------------------------------------------

class DailyValueExtractor {
public:
    void newDay() { mHours = 0; mCountHours = 0; mMean = 0; }
    void addObservation(const miutil::miTime& obstime, float original)
        { int h = obstime.hour(); if( h == 6 || h == 12 || h == 18 ) { mHours |= (1 << h); mCountHours += 1; mMean += original; } }
    bool completeDay()
        { return (mHours == 1<<6 || mHours == (1<<6 | 1<<12 | 1<<18)); }
    float value()
        { return mMean / mCountHours; }
private:
    int mHours, mCountHours;
    float mMean;
};

// ========================================================================

class AccumulatedValue {
public:
    virtual ~AccumulatedValue() = 0;
};

AccumulatedValue::~AccumulatedValue()
{
}

typedef boost::shared_ptr<AccumulatedValue> AccumulatedValueP;

// ========================================================================

class Accumulator {
public:
    virtual void newStation() =0;
    virtual void push(float value) = 0;
    virtual void pop(float value) = 0;
    virtual AccumulatedValueP value() = 0;
    virtual ~Accumulator() { }
};

typedef boost::shared_ptr<Accumulator> AccumulatorP;

// ========================================================================

class Checker {
public:
    Checker(StatisticalMean* sm)
        : mStatisticalMean(sm) { }

    // indicates new center station; return true if ok, i.e. no neighbors need to be checked
    virtual bool newCenter(int id, int dayOfYear, AccumulatedValueP value) = 0;
    // return true if enough neighbors have been seen
    virtual bool checkNeighbor(int nbr, AccumulatedValueP value) = 0;
    // return true if the center station passes the test
    virtual bool pass() = 0;

    float getReference(int stationid, int day, const std::string& key, bool& valid)
        { return mStatisticalMean->getReferenceValue(stationid, day, key, valid); }

    virtual ~Checker() { }
private:
    StatisticalMean* mStatisticalMean;
};

typedef boost::shared_ptr<Checker> CheckerP;

// ========================================================================

struct AccumulatedFloat : public AccumulatedValue {
    float value;
    AccumulatedFloat(float v) : value(v) { }
};

// ========================================================================

class AccumulatorMeanOrSum : public Accumulator {
public:
    AccumulatorMeanOrSum(bool calculateMean, int days, int daysRequired)
        : mCalculateMean(calculateMean), mDays(days), mDaysRequired(daysRequired) { }
    void newStation() { mSum = 0; mCountDays = 0; }
    void push(float value) { mCountDays += 1; mSum += value; }
    void pop(float value)  { mCountDays -= 1; mSum -= value; }
    AccumulatedValueP value();
private:
    bool mCalculateMean;
    int mDays, mDaysRequired;
    double mSum;
    int mCountDays;
};

// ------------------------------------------------------------------------

AccumulatedValueP AccumulatorMeanOrSum::value()
{
    if( mCountDays<=0 || mCountDays < mDaysRequired )
        return AccumulatedValueP();
    if( mCalculateMean )
        return boost::make_shared<AccumulatedFloat>(mSum / mCountDays);
    else
        return boost::make_shared<AccumulatedFloat>(mSum * float(mDays) / float(mCountDays));
}

// ========================================================================

class CheckerMeanOrSum : public Checker {
public:
    CheckerMeanOrSum(StatisticalMean* sm, int paramid, float tolerance)
        : Checker(sm), mParamid(paramid), mTolerance(tolerance) { }
    bool newCenter(int id, int dayOfYear, AccumulatedValueP accumulated);
    bool checkNeighbor(int nbr, AccumulatedValueP accumulated);
    bool pass();
private:
    int mParamid, mDayOfYear, mCenter, mCountNeighborsBelowTolerance;
    float mTolerance;
};

// ------------------------------------------------------------------------

bool CheckerMeanOrSum::newCenter(int id, int dayOfYear, AccumulatedValueP accumulated)
{
    mCenter = id;
    mDayOfYear = dayOfYear;
    mCountNeighborsBelowTolerance = 0;

    bool referenceValid;
    const float reference = getReference(id, dayOfYear, "ref_value", referenceValid);
    //bool toleranceValid;
    //const float tolerance = getReference(id, dayOfYear, "tolerance", referenceValid);
    const float value = boost::static_pointer_cast<AccumulatedFloat>(accumulated)->value;
    return !referenceValid || fabs(value - reference) < mTolerance;
}

// ------------------------------------------------------------------------

bool CheckerMeanOrSum::checkNeighbor(int nbr, AccumulatedValueP accumulated)
{
    bool referenceValid;
    const float reference = getReference(nbr, mDayOfYear, "ref_value", referenceValid);
    const float value = boost::static_pointer_cast<AccumulatedFloat>(accumulated)->value;
    if( referenceValid && fabs(value - reference) < mTolerance )
        mCountNeighborsBelowTolerance += 1;
    return mCountNeighborsBelowTolerance <= 3;
}

// ------------------------------------------------------------------------

bool CheckerMeanOrSum::pass()
{
    return mCountNeighborsBelowTolerance < 3;
}

// ========================================================================
// ========================================================================

struct AccumulatedQuartiles : public AccumulatedValue {
    float q1, q2, q3;
    AccumulatedQuartiles(float qq1, float qq2, float qq3)
        : q1(qq1), q2(qq2), q3(qq3) { }
};

typedef boost::shared_ptr<AccumulatedQuartiles> AccumulatedQuartilesP;

// ========================================================================

class AccumulatorQuartiles : public Accumulator {
public:
    AccumulatorQuartiles(int days, int daysRequired)
        : mDays(days), mDaysRequired(daysRequired) { }
    void newStation() { mValues.clear(); }
    void push(float value) { mValues.push_back(value); }
    void pop(float value);
    AccumulatedValueP value();
private:
    int mDays, mDaysRequired;
    std::vector<float> mValues;
    int mCountDays;
};

// ------------------------------------------------------------------------

void AccumulatorQuartiles::pop(float value)
{
    std::vector<float>::iterator it = std::find(mValues.begin(), mValues.end(), value);
    if( it != mValues.end() )
        mValues.erase(it);
}

// ------------------------------------------------------------------------

AccumulatedValueP AccumulatorQuartiles::value()
{
    if( mCountDays<=0 || mCountDays < mDaysRequired )
        return AccumulatedValueP();

    double q1, q2, q3;
    Helpers::quartiles(mValues.begin(), mValues.end(), q1, q2, q3);

    return boost::make_shared<AccumulatedQuartiles>(q1, q2, q3);
}

// ========================================================================

class CheckerQuartiles : public Checker {
public:
    CheckerQuartiles(StatisticalMean* sm, std::vector<float>& tolerances)
        : Checker(sm), mTolerances(tolerances) { }
    bool newCenter(int id, int dayOfYear, AccumulatedValueP accumulated);
    bool checkNeighbor(int nbr, AccumulatedValueP accumulated);
    bool pass();
private:
    int mDayOfYear, mCenter, mCountNeighborsWithError;
    std::vector<float>& mTolerances;
    float mDiffQ1, mDiffQ2, mDiffQ3;
};

// ------------------------------------------------------------------------

bool CheckerQuartiles::newCenter(int id, int dayOfYear, AccumulatedValueP accumulated)
{
    mCenter = id;
    mDayOfYear = dayOfYear;
    mCountNeighborsWithError = 0;

    AccumulatedQuartilesP quartiles = boost::static_pointer_cast<AccumulatedQuartiles>(accumulated);

    bool referenceValid;
    const float reference_q1 = getReference(id, dayOfYear, "ref_q1", referenceValid);
    mDiffQ1 = referenceValid ? fabs(quartiles->q1 - reference_q1) : 0;
    const float reference_q2 = getReference(id, dayOfYear, "ref_q2", referenceValid);
    mDiffQ2 = referenceValid ? fabs(quartiles->q2 - reference_q2) : 0;
    const float reference_q3 = getReference(id, dayOfYear, "ref_q3", referenceValid);
    mDiffQ3 = referenceValid ? fabs(quartiles->q3 - reference_q3) : 0;

    return mDiffQ1 <= mTolerances[1] && mDiffQ2 <= mTolerances[3] && mDiffQ3 <= mTolerances[5];
}

// ------------------------------------------------------------------------

bool CheckerQuartiles::checkNeighbor(int nbr, AccumulatedValueP accumulated)
{
    AccumulatedQuartilesP quartiles = boost::static_pointer_cast<AccumulatedQuartiles>(accumulated);

    bool referenceValid;
    const float reference_q1 = getReference(nbr, mDayOfYear, "ref_q1", referenceValid);
    const float nDiffQ1 = referenceValid ? fabs(quartiles->q1 - reference_q1) : 0;
    const float reference_q2 = getReference(nbr, mDayOfYear, "ref_q2", referenceValid);
    const float nDiffQ2 = referenceValid ? fabs(quartiles->q2 - reference_q2) : 0;
    const float reference_q3 = getReference(nbr, mDayOfYear, "ref_q3", referenceValid);
    const float nDiffQ3 = referenceValid ? fabs(quartiles->q3 - reference_q3) : 0;

    const bool error = (mDiffQ1 > mTolerances[1] && mDiffQ1 - nDiffQ1 > mTolerances[0])
        || (mDiffQ2 > mTolerances[3] && mDiffQ2 - nDiffQ2 > mTolerances[2])
        || (mDiffQ3 > mTolerances[5] && mDiffQ3 - nDiffQ3 > mTolerances[4]);

    if( error )
        mCountNeighborsWithError += 1;
    return mCountNeighborsWithError <= 3;
}

// ------------------------------------------------------------------------

bool CheckerQuartiles::pass()
{
    return mCountNeighborsWithError < 3;
}

// ========================================================================
// ========================================================================

void StatisticalMean::run()
{
    std::list<kvalobs::kvData> sdata;
    const C::DBConstraint cData = C::Paramid(mParamid) && C::Typeid(mTypeids)
            && C::Obstime(mUT0extended, UT1);
    database()->selectData(sdata, cData, O::Obstime().asc());
    
    typedef std::vector<kvalobs::kvData> dlist_t;
    typedef std::map<int, dlist_t > smap_t;
    smap_t smap;
    foreach(const kvalobs::kvData& d, sdata) {
        if( !Helpers::equal(d.original(), missing) && !Helpers::equal(d.original(), rejected) )
            smap[d.stationID()].push_back(d);
    }

    sdata.clear(); // release memory

    typedef std::vector<DayMean> dm_t;
    typedef std::map<int, dm_t> sdm_t;
    sdm_t stationDailyMeans;

    const miutil::miDate date0 = mUT0extended.date();
    const int day0 = date0.julianDay();

    DailyValueExtractor dve;

    // calculate daily mean values
    foreach(const smap_t::value_type& sd, smap) {
        const dlist_t& dl = sd.second;
        for(dlist_t::const_iterator itB = dl.begin(), itE=itB; itB != dl.end(); itB = itE) {
            dve.newDay();
            const int day = itB->obstime().date().julianDay() - day0;
            for( ; itE != dl.end() && (itE->obstime().date().julianDay() - day0) == day; itE++ )
                dve.addObservation(itE->obstime(), itE->original());
            if( dve.completeDay() )
                stationDailyMeans[sd.first].push_back(DayMean(day, dve.value()));
        }
    }

    smap.clear(); // release memory

    typedef std::map<int, AccumulatedValueP> dm2_t;
    typedef std::map<int, dm2_t> sd2_t;
    sd2_t stationMeansPerDay;

    AccumulatorP accumulator = boost::make_shared<AccumulatorMeanOrSum>(true, mDays, mDaysRequired);
    foreach(const sdm_t::value_type& sd, stationDailyMeans) {
        accumulator->newStation();
        const dm_t& dml = sd.second;
        dm_t::const_iterator tail = dml.begin(), head = tail;

        const int d0 = UT0.date().julianDay() - day0, d1 = UT1.date().julianDay() - day0;
        for(int day=d0; day<=d1; ++day) {
            const int dfront = day - mDays;
            while( head != dml.end() && head->day() <= day ) {
                accumulator->push(head->mean());
                head++;
            }
            while( tail != head && tail->day() <= dfront ) {
                accumulator->pop(tail->mean());
                tail++;
            }
            AccumulatedValueP value = accumulator->value();
            if( value != 0 )
                stationMeansPerDay[sd.first][day] = value;
        }
    }

    stationDailyMeans.clear(); // release memory

    CheckerP checker = boost::make_shared<CheckerMeanOrSum>(this, mParamid, mTolerance);
    foreach(const sd2_t::value_type& sd, stationMeansPerDay) {
        const int center = sd.first;
        foreach(const dm2_t::value_type& dm, sd.second) {
            const int day = dm.first;

            miutil::miDate date(date0);
            date.addDay(day);
            if( date < UT0.date() )
                continue;
            
            if( checker->newCenter(center, Helpers::normalisedDayOfYear(date), dm.second) )
                continue;

            std::list<int> neighbors = findNeighbors(center);
            foreach(int n, neighbors) {
                sd2_t::const_iterator itN = stationMeansPerDay.find(n);
                if( itN == stationMeansPerDay.end() )
                    continue;

                const dm2_t& ndata = itN->second;
                dm2_t::const_iterator itD = ndata.find(day);
                if( itD == ndata.end() )
                    continue;

                if( !checker->checkNeighbor(n, itD->second) )
                    break;
            }
            if( !checker->pass() ) {
                warning() << "statistical test failed for station " << center
                          << " for series ending at " << date;
            }
        }
    }
}

// ------------------------------------------------------------------------

float StatisticalMean::getReferenceValue(int station, int dayOfYear, const std::string& key, bool& valid)
{
    if( mParamid == 178 ) {
        // pressure
        valid = true;
        return 1014;
    }

    float rv;
    database()->selectStatisticalReferenceValue(station, mParamid, dayOfYear, key, valid, rv);
    return rv;
}
