/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  Copyright (C) 2007-2011 met.no

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

#include "PlumaticAlgorithm.h"

#include "AlgorithmHelpers.h"
#include "DBConstraints.h"
#include "DBInterface.h"
#include "foreach.h"

#include <milog/milog.h>
#include <puTools/miTime.h>

#define NDEBUG
#include "debug.h"

namespace C = Constraint;
namespace O = Ordering;

namespace {

const int vippsUnlikelySingle   = 2;
const int vippsUnlikelyStart    = 3;
const int vippsRainInterrupt    = 3;
const int maxRainInterrupt      = 4;
const int minRainBeforeAndAfter = 2;

}; // anonymous namespace

void PlumaticAlgorithm::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    pid = params.getParameter<int>("ParamId");

    params.getFlagSetCU(discarded_flags, "discarded", "fr=9|fs=8|fmis=2", "");
    params.getFlagChange(highstart_flagchange,       "highstart_flagchange",       "fs=8,fmis=2");
    params.getFlagChange(highsingle_flagchange,      "highsingle_flagchange",      "fs=8,fmis=2");
    params.getFlagChange(interruptedrain_flagchange, "interruptedrain_flagchange", "fs=8,fmis=2");
    params.getFlagChange(aggregation_flagchange,     "aggregation_flagchange",     "fr=9");

    // parse 'stations'
    mStationlist.clear();
    const miutil::miString stationlist = params.getParameter<std::string>("stations");
    const std::vector<miutil::miString> msl = stationlist.split(';');
    foreach(const miutil::miString& mmpv_stations, msl) {
        std::vector<miutil::miString> m_s = mmpv_stations.split(':', false);
        if( m_s.size() != 2 )
            throw ConfigException("cannot parse 'stations' parameter");
        ResolutionStations rs(atof(m_s[0].c_str()));
        if( rs.mmpv <= 0.0f )
            throw ConfigException("invalid mm-per-vipp <= 0 in 'stations' parameter");
        const std::vector<miutil::miString> s = m_s[1].split(',');
        foreach(miutil::miString stationidText, s) {
            int stationid = atoi(stationidText.c_str());
            if( stationid <= 0 )
                throw ConfigException("invalid stationid <= 0 in 'stations' parameter");
            rs.stationids.push_back(stationid);
        }
        mStationlist.push_back(rs);
    }

    // parse 'sliding_alarms'
    int lookback = maxRainInterrupt+minRainBeforeAndAfter;
    mSlidingAlarms.clear();
    const miutil::miString slidingAlarms = params.getParameter<std::string>("sliding_alarms");
    const std::vector<miutil::miString> msa = slidingAlarms.split(';');
    foreach(const miutil::miString& length_max, msa) {
        std::vector<miutil::miString> l_m = length_max.split('<', false);
        if( l_m.size() != 2 )
            throw ConfigException("cannot parse 'sliding_alarms' parameter");
        const int length = atoi(l_m[0].c_str());
        if( length <= 1 || (!mSlidingAlarms.empty() && length <= mSlidingAlarms.back().length) )
            throw ConfigException("invalid length (ordering?) in 'sliding_alarms' parameter");
        const float maxi = atof(l_m[1].c_str());
        if( maxi <= 0.0f )
            throw ConfigException("invalid threshold (<=0?) in 'sliding_alarms' parameter");
        mSlidingAlarms.push_back(SlidingAlarm(length, maxi));
        if( lookback < length )
            lookback = length;
    }

    UT0extended = params.UT0;
    UT0extended.addMin(-lookback);
}

void PlumaticAlgorithm::run()
{
    // use script stinfosys-vipp-pluviometer.pl or change program and use "select stationid from obs_pgm where paramid = 105;"
    foreach(const ResolutionStations& rs, mStationlist) {
        foreach(int stationid, rs.stationids) {
            checkStation(stationid, rs.mmpv);
        }
    }
}

void PlumaticAlgorithm::checkStation(int stationid, float mmpv)
{
    const C::DBConstraint cSeries = C::Station(stationid)
        && C::Paramid(pid) && C::Obstime(UT0extended, UT1);

    kvDataList_t data_orig;
    database()->selectData(data_orig, cSeries, O::Obstime());
    if( data_orig.empty() )
        return;
    kvUpdateList_t data(data_orig.begin(), data_orig.end());

    checkShowers(data, mmpv);
    checkSlidingSums(data);
    storeUpdates(data);
}

void PlumaticAlgorithm::checkSlidingSums(kvUpdateList_t& data)
{
    foreach(const SlidingAlarm& slal, mSlidingAlarms) {
        checkSlidingSum(data, slal);
    }
}

void PlumaticAlgorithm::checkSlidingSum(kvUpdateList_t& data, const SlidingAlarm& slal)
{
    DBG("length=" << slal.length << " maxi=" << slal.max);
    std::ostringstream cfailed;
    cfailed << "QC2h-1-aggregation-" << slal.length;
    kvUpdateList_it head = data.begin(), tail = data.begin();
    float sum = 0;
    std::list<kvUpdateList_it> discarded;
    //std::list<kvUpdateList_it> flagged;
    for(; head != data.end(); ++head) {
        DBG("head=" << *head << " tail=" << *tail << "minutes=" << minutesBetween(head->obstime(), tail->obstime()));
        for( ; minutesBetween(head->obstime(), tail->obstime()) >= slal.length; ++tail ) {
            if( tail->original()>0 )
                sum -= tail->original();
            DBGV(sum);
            if( !discarded.empty() && tail == discarded.front() ) {
                discarded.pop_front();
                DBGV(discarded.size());
            }
            //if( tail == flagged.front() ) {
            //    flagged.pop_front();
            //    DBGV(flagged.size());
            //}
        }
        if( head->original()>0 )
            sum += head->original();
        DBGV(sum);
        if( isBadData(*head) ) {
            discarded.push_back(head);
            DBGV(discarded.size());
        }
        if( sum >= slal.max /*&& flagged.empty()*/ && discarded.empty() ) {
            //flagged.push_back(head);
            //DBGV(flagged.size());
            kvUpdateList_it stop = head; stop++;
            for(kvUpdateList_it mark = tail; mark != stop; ++mark)
                mark->controlinfo(aggregation_flagchange.apply(mark->controlinfo()))
                    . cfailed(cfailed.str(), CFAILED_STRING);
        }
    }
}

void PlumaticAlgorithm::checkShowers(kvUpdateList_t& data, float mmpv)
{
    Shower previousShower = { data.end(), data.end(), 0 };
    for(Shower shower = findFirstShower(data); shower.first != data.end(); shower = findNextShower(shower, data.end())) {
        if( previousShower.duration != 0 && checkRainInterruption(shower, previousShower, mmpv) ) {
            flagRainInterruption(shower, previousShower, data);
        } else if( checkHighSingle(shower, mmpv) ) {
            flagHighSingle(shower);
        } else {
            int length = checkHighStartLength(shower, mmpv);
            if( length>0 )
                flagHighStart(shower, length);
        }
        previousShower = shower;
    }
}

bool PlumaticAlgorithm::isBadData(const DataUpdate& data)
{
    return !Helpers::equal(data.original(), data.corrected())
        || discarded_flags.matches(data.data());
}

bool PlumaticAlgorithm::checkRainInterruption(const Shower& shower, const Shower& previousShower, const float mmpv)
{
    if( shower.duration < minRainBeforeAndAfter
        || previousShower.duration < minRainBeforeAndAfter )
    {
        return false;
    }

    const float threshold = mmpv*vippsRainInterrupt;
    if( shower.first->original() < threshold || previousShower.last->original() < threshold )
        return false;

    const int interruption = miutil::miTime::minDiff(shower.first->obstime(), previousShower.last->obstime());
    if( interruption > maxRainInterrupt )
        return false;

    // check for bad data in the two minutes before after the interruption
    kvUpdateList_it it = shower.first;
    if( isBadData(*it) )
        return false;
    ++it;
    if( isBadData(*it) )
        return false;
    it = previousShower.last;
    if( isBadData(*it) )
        return false;
    --it;
    if( isBadData(*it) )
        return false;

    return true;
}

bool PlumaticAlgorithm::checkHighSingle(const Shower& shower, const float mmpv)
{
    if( shower.duration != 1 )
        return false;
    
    if( isBadData(*shower.first) )
        return false;

    const float threshold = mmpv*vippsUnlikelySingle;
    if( shower.first->original() < threshold )
        return false;

    return true;
}

int PlumaticAlgorithm::checkHighStartLength(const Shower& shower, const float mmpv)
{
    if( shower.duration < 2 )
        return 0;

    int n = 0;
    const float threshold = mmpv*vippsUnlikelyStart-0.05;
    kvUpdateList_it end = shower.last; ++end;
    for(kvUpdateList_it it = shower.first; it != end && !isBadData(*it); ++it) {
        if( it->original() < threshold )
            break;
        else
            n += 1;
    }

    return n;
}

void PlumaticAlgorithm::flagRainInterruption(const Shower& shower, const Shower& previousShower, kvUpdateList_t& data)
{
    const miutil::miTime now = miutil::miTime::nowTime();

    miutil::miTime t = previousShower.last->obstime();
    t.addMin(1);

    for(; t<shower.first->obstime(); t.addMin(1)) {
        DBG("t=" << t);
        
        bool needInsert = true;
        kvUpdateList_t::iterator it = previousShower.last;
        for( ; it != shower.first; ++it ) {
            if( t == it->obstime() ) {
                needInsert = false;
                break;
            }
        }
        if( needInsert ) {
            DataUpdate insert(shower.first->data(), t, now, 0.0, missing, "0008002000000000");
            insert.controlinfo(interruptedrain_flagchange.apply(insert.controlinfo()))
                .cfailed("QC2h-1-interruptedrain", CFAILED_STRING);
            data.insert(shower.first, insert);
        } else {
            it->controlinfo(interruptedrain_flagchange.apply(it->controlinfo()))
                .cfailed("QC2h-1-interruptedrain", CFAILED_STRING);
        }
    }
}

void PlumaticAlgorithm::flagHighSingle(const Shower& shower)
{
    shower.first->controlinfo(highsingle_flagchange.apply(shower.first->controlinfo()))
        .cfailed("QC2h-1-highsingle", CFAILED_STRING);
}

void PlumaticAlgorithm::flagHighStart(const Shower& shower, int length)
{
    kvUpdateList_it it = shower.first;
    for(int i=0; i<length; ++i, ++it) {
        it->controlinfo(highstart_flagchange.apply(it->controlinfo()))
            .cfailed("QC2h-1-highstart", CFAILED_STRING);
    }
}

void PlumaticAlgorithm::storeUpdates(const kvUpdateList_t& data)
{
    kvDataList_t toInsert, toUpdate;
    foreach(const DataUpdate& du, data) {
        if( du.isModified() ) {
            DBG(du);
            if( du.isNew() )
                toInsert.push_back(du.data());
            else
                toUpdate.push_back(du.data());
        }
    }
#ifndef NDEBUG
    if( toInsert.empty() && toUpdate.empty() )
        DBG("no updates/inserts");
#endif
    storeData(toUpdate, toInsert);
}

PlumaticAlgorithm::Shower PlumaticAlgorithm::findFirstShower(kvUpdateList_t& data)
{
    return findShowerForward(data.begin(), data.end());
}

PlumaticAlgorithm::Shower PlumaticAlgorithm::findNextShower(const Shower& s, const kvUpdateList_it& end)
{
    kvUpdateList_it begin = s.last;
    begin++;
    return findShowerForward(begin, end);
}

PlumaticAlgorithm::Shower PlumaticAlgorithm::findShowerForward(const kvUpdateList_it& begin, const kvUpdateList_it& end)
{
    Shower f = { begin, end, 0 };
    while(f.first != end && (f.first->obstime() < UT0 || f.first->original() < 0.05))
        ++f.first;
    f.last = f.first;
    if( f.last != end ) {
        ++f.last;
        miutil::miTime t = f.first->obstime();
        t.addMin(1);
        while( f.last != end && f.first->original() >= 0.05 && f.last->obstime() == t ) {
            ++f.last;
            t.addMin(1);
        }
        --f.last;
        f.duration = 1 + minutesBetween(f.last->obstime(), f.first->obstime());
        DBG("last=" << f.last->obstime() << " first=" << f.first->obstime() << " duration=" << f.duration);
    }
    return f;
}
