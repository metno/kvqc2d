/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  Copyright (C) 2007-2012 met.no

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
#include "DBInterface.h"
#include "NeighborsDistance2.h"
#include "foreach.h"

#include <milog/milog.h>
#include <puTools/miTime.h>

#include <boost/bind.hpp>

#define NDEBUG
#include "debug.h"

namespace {

const int vippsUnlikelySingle   = 3;
const int vippsUnlikelyStart    = 3;
const int vippsRainInterrupt    = 3;
const int maxRainInterrupt      = 4;
const int minRainBeforeAndAfter = 2;
const float numericSafety = 1e-4;

}; // anonymous namespace

// ========================================================================

PlumaticAlgorithm::PlumaticAlgorithm()
    : Qc2Algorithm("Plumatic")
    , mNeighbors(new NeighborsDistance2())
{
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    pid = params.getParameter<int>("ParamId");
    mThresholdDry = params.getParameter<float>("threshold_dry", 0.5);
    mThresholdWet = params.getParameter<float>("threshold_wet", 3.0);

    params.getFlagSetCU(discarded_flags, "discarded", "fr=9|fs=8|fmis=2", "");
    params.getFlagSetCU(neighbor_flags,  "neighbor",  "", "U2=0");
    params.getFlagChange(highstart_flagchange,       "highstart_flagchange",       "fs=8,fmis=2");
    params.getFlagChange(highsingle_flagchange,      "highsingle_flagchange",      "fs=8,fmis=2");
    params.getFlagChange(interruptedrain_flagchange, "interruptedrain_flagchange", "fs=8,fmis=2");
    params.getFlagChange(aggregation_flagchange,     "aggregation_flagchange",     "fr=9");

    mNeighbors->configure(params);

    // parse 'stations'
    mStationlist.clear();
    const std::string stationlist = params.getParameter<std::string>("stations");
    const std::vector<std::string> msl = Helpers::splitN(stationlist, ";", true);
    foreach(const std::string& mmpv_stations, msl) {
        Helpers::split2_t m_s = Helpers::split2(mmpv_stations, ":");
        ResolutionStations rs(atof(m_s.first.c_str()));
        if( rs.mmpv <= 0.0f )
            throw ConfigException("invalid mm-per-vipp <= 0 in 'stations' parameter");
        const std::vector<std::string> s = Helpers::splitN(m_s.second, ",", true);
        foreach(std::string stationidText, s) {
            const int stationid = atoi(stationidText.c_str());
            if( stationid <= 0 )
                throw ConfigException("invalid stationid <= 0 in 'stations' parameter");
            rs.stationids.push_back(stationid);
        }
        mStationlist.push_back(rs);
    }

    // parse 'sliding_alarms'
    int lookback = maxRainInterrupt+minRainBeforeAndAfter;
    mSlidingAlarms.clear();
    const std::string slidingAlarms = params.getParameter<std::string>("sliding_alarms");
    if( !slidingAlarms.empty() ) {
        const std::vector<std::string> msa = Helpers::splitN(slidingAlarms, ";", true);
        foreach(const std::string& length_max, msa) {
            Helpers::split2_t l_m = Helpers::split2(length_max, "<", true);
            const int length = atoi(l_m.first.c_str());
            if( length <= 1 || (!mSlidingAlarms.empty() && length <= mSlidingAlarms.back().length) )
                throw ConfigException("invalid length (ordering?) in 'sliding_alarms' parameter");
            const float maxi = atof(l_m.second.c_str());
            if( maxi <= 0.0f )
                throw ConfigException("invalid threshold (<=0?) in 'sliding_alarms' parameter");
            mSlidingAlarms.push_back(SlidingAlarm(length, maxi));
            if( lookback < length )
                lookback = length;
        }
    }
    
    UT0extended = params.UT0;
    UT0extended.addMin(-lookback);
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::run()
{
    // use script stinfosys-vipp-pluviometer.pl or change program and use "select stationid from obs_pgm where paramid = 105;"
    foreach(const ResolutionStations& rs, mStationlist) {
        foreach(int stationid, rs.stationids) {
            checkStation(stationid, rs.mmpv);
        }
    }
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::checkStation(int stationid, float mmpv)
{
    //const C::DBConstraint cSeries = C::Station(stationid) && C::Paramid(pid) && C::Obstime(UT0extended, UT1);

    DBInterface::DataList data_orig
        = database()->findDataOrderObstime(stationid, pid, TimeRange(UT0extended, UT1));
    if( data_orig.empty() )
        return;
    kvUpdateList_t data(data_orig.begin(), data_orig.end());

    discardAllNonOperationalTimes(data);
    checkShowers(data, mmpv);
    checkSlidingSums(data);
    checkNeighborStations(stationid, data);
    storeUpdates(data);
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::checkSlidingSums(kvUpdateList_t& data)
{
    foreach(const SlidingAlarm& slal, mSlidingAlarms) {
        checkSlidingSum(data, slal);
    }
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::checkSlidingSum(kvUpdateList_t& data, const SlidingAlarm& slal)
{
    std::ostringstream cfailed;
    cfailed << "QC2h-1-aggregation-" << slal.length;
    kvUpdateList_it head = data.begin(), tail = data.begin();
    float sum = 0;
    std::list<kvUpdateList_it> discarded;
    std::list<kvUpdateList_it> flagged;
    for(; head != data.end(); ++head) {
        for( ; minutesBetween(head->obstime(), tail->obstime()) >= slal.length; ++tail ) {
            if( tail->original()>0 )
                sum -= tail->original();
            if( !discarded.empty() && tail == discarded.front() ) {
                discarded.pop_front();
            }
            if( !flagged.empty() && tail == flagged.front() ) {
                flagged.pop_front();
            }
        }
        if( head->original()>0 ) {
            // no need to look at operational start / end here as we
            // do not look at fixed time intervals
            sum += head->original();
        }
        if( isBadData(*head) ) {
            discarded.push_back(head);
        }
        if( sum >= slal.max && discarded.empty() ) {
            kvUpdateList_it stop = head; ++stop;
            kvUpdateList_it mark = tail;
            for(; mark != stop; ++mark) {
                if( std::find(flagged.begin(), flagged.end(), mark) == flagged.end() ) {
                    if( mark->original() > 0 || mark->obstime().min() != 0 )
                        mark->controlinfo(aggregation_flagchange.apply(mark->controlinfo()))
                            . cfailed(cfailed.str(), CFAILED_STRING);
                    flagged.push_back(mark);
                }
            }
        }
    }
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::discardAllNonOperationalTimes(kvUpdateList_t& data)
{
    // check for -5 / -6 markers for start / end of non-operational times
    const int ORIG_START_BAD = -5, ORIG_END_BAD = -6;
    kvUpdateList_it start_bad = data.end();
    for(kvUpdateList_it mark = data.begin(); mark != data.end(); ++mark) {
        if( discarded_flags.matches(mark->data()) )
            continue;
        if( mark->original() == ORIG_START_BAD ) {
            if( start_bad != data.end() ) {
                warning() << "Plumatic: found duplicate marker for start of non-operational period at "
                          << *mark << ", previous marker is " << *start_bad;
            } else {
                start_bad = mark;
            }
        } else if( mark->original() == ORIG_END_BAD ) {
            if( start_bad == data.end() ) {
                start_bad = data.begin();
                for(kvUpdateList_it mark2 = data.begin(); mark2 != mark; ++mark2) {
                    if ( mark->original() >= 0 && !discarded_flags.matches(mark->data()) ) {
                        warning() << "Plumatic: -6 marker (at " << mark->obstime()
                                  << ") preceeded by non-discarded data at " << *mark2;
                    }
                }
            }
            discardNonOperationalTime(data, start_bad, mark);
            start_bad = data.end();
        } else if ( mark->original() >= 0 && start_bad != data.end() && !discarded_flags.matches(mark->data()) ) {
            warning() << "Plumatic: -5 marker (at " << start_bad->obstime()
                      << ") followed by non-discarded data at " << *mark;
        }
    }
    if( start_bad != data.end() ) {
        discardNonOperationalTime(data, start_bad, data.end());
    }

    // search for hours without data and without value at :00 of the next hour => mark :01 -- :00 as bad
    const miutil::miTime now = miutil::miTime::nowTime();

    kvUpdateList_it m1 = data.begin();
    // advance m1 to the first non-discarded value
    while( m1 != data.end() && discarded_flags.matches(m1->data()) )
        ++m1;

    for(kvUpdateList_it m2 = m1; m2 != data.end(); m1 = m2 ) {
        ++m2;
        // advance m2 to the next non-discarded value
        while( m2 != data.end() && discarded_flags.matches(m2->data()) )
            ++m2;
        
        const miutil::miTime &t1 = m1->obstime(), &t2 = (m2 != data.end()) ? m2->obstime() : UT1;
        const int minDiff = miutil::miTime::minDiff(t2, t1);

        if( minDiff < 60 || (minDiff == 60 && t1.min() == 0 && t2.min() == 0) )
            continue;

        miutil::miTime tBegin = t1;
        if( tBegin.min() > 0 )
            tBegin.addMin(-t1.min());
        tBegin.addHour(1);
        miutil::miTime tEnd = t2;
        if( tEnd.min() != 0 )
            tEnd.addMin(-t2.min());
        tEnd.addMin(-1);
        
        if( tBegin >= tEnd )
            continue;

        if( tBegin > t1 ) {
            PlumaticUpdate uBegin(m1->data(), tBegin, now, 0.0, missing, "FFFFFFFFFFFFFFFF");
            uBegin.forceNoWrite();
            m1 = data.insert(m2, uBegin);
        }
        if( tEnd < t2 ) {
            PlumaticUpdate uEnd(m2->data(), tEnd, now, 0.0, missing, "FFFFFFFFFFFFFFFF");
            uEnd.forceNoWrite();
            m2 = data.insert(m2, uEnd);
        }
        discardNonOperationalTime(data, m1, m2);
    }
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::discardNonOperationalTime(kvUpdateList_t& data, kvUpdateList_it begin, kvUpdateList_it end)
{
    const bool endHasData = (end != data.end());
    const miutil::miTime beginTime = begin->obstime(), endTime = (endHasData ? end->obstime() : UT1);
    info() << "Plumatic: ignoring non-operational time for station " << begin->data().stationID()
           << " between " << beginTime << " and "
           << (endHasData ? endTime.isoTime() : UT1.isoTime() + " [end]");
    
    begin->setNotOperationalStart();
    if( begin == end ) {
        begin->setNotOperationalEnd();
        return;
    }
    if( end != data.end() )
        end->setNotOperationalEnd();
    begin++;
    data.erase(begin, end);
}

// ------------------------------------------------------------------------

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
            if( length>0 ) {
                // only flag 1st minute, more is too much noise
                flagHighStart(shower, 1);
            }
        }
        previousShower = shower;
    }
}

// ------------------------------------------------------------------------

bool PlumaticAlgorithm::isBadData(const PlumaticUpdate& data)
{
    return !Helpers::equal(data.original(), data.corrected())
        || data.isNotOperational()
        || discarded_flags.matches(data.data());
}

// ------------------------------------------------------------------------

bool PlumaticAlgorithm::checkRainInterruption(const Shower& shower, const Shower& previousShower, const float mmpv)
{
    if( shower.duration < minRainBeforeAndAfter
        || previousShower.duration < minRainBeforeAndAfter )
    {
        return false;
    }

    const float threshold = mmpv*vippsRainInterrupt - numericSafety;
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

// ------------------------------------------------------------------------

bool PlumaticAlgorithm::checkHighSingle(const Shower& shower, const float mmpv)
{
    if( shower.duration != 1 )
        return false;
    
    if( isBadData(*shower.first) )
        return false;

    const float threshold = mmpv*vippsUnlikelySingle - numericSafety;
    if( shower.first->original() < threshold )
        return false;

    return true;
}

// ------------------------------------------------------------------------

int PlumaticAlgorithm::checkHighStartLength(const Shower& shower, const float mmpv)
{
    if( shower.duration < 2 )
        return 0;

    int n = 0;
    const float threshold = mmpv*vippsUnlikelyStart - numericSafety;
    kvUpdateList_it end = shower.last; ++end;
    for(kvUpdateList_it it = shower.first; it != end && !isBadData(*it); ++it) {
        if( it->original() < threshold )
            break;
        else
            n += 1;
    }

    return n;
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::flagRainInterruption(const Shower& shower, const Shower& previousShower, kvUpdateList_t& data)
{
    const miutil::miTime now = miutil::miTime::nowTime();

    miutil::miTime t = previousShower.last->obstime();
    t.addMin(1);

    for(; t<shower.first->obstime(); t.addMin(1)) {
        bool needInsert = true;
        kvUpdateList_t::iterator it = previousShower.last;
        for( ; it != shower.first; ++it ) {
            if( t == it->obstime() ) {
                needInsert = false;
                break;
            }
        }
        if( needInsert ) {
            PlumaticUpdate insert(shower.first->data(), t, now, missing, missing, "0008003000000000");
            insert.controlinfo(interruptedrain_flagchange.apply(insert.controlinfo()))
                .cfailed("QC2h-1-interruptedrain", CFAILED_STRING);
            data.insert(shower.first, insert);
        } else {
            it->controlinfo(interruptedrain_flagchange.apply(it->controlinfo()))
                .cfailed("QC2h-1-interruptedrain", CFAILED_STRING);
        }
    }
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::flagHighSingle(const Shower& shower)
{
    shower.first->controlinfo(highsingle_flagchange.apply(shower.first->controlinfo()))
        .cfailed("QC2h-1-highsingle", CFAILED_STRING);
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::flagHighStart(const Shower& shower, int length)
{
    kvUpdateList_it it = shower.first;
    for(int i=0; i<length; ++i, ++it) {
        it->controlinfo(highstart_flagchange.apply(it->controlinfo()))
            .cfailed("QC2h-1-highstart", CFAILED_STRING);
    }
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::storeUpdates(const kvUpdateList_t& data)
{
    kvDataList_t toInsert, toUpdate;
    foreach(const DataUpdate& du, data) {
        if( du.needsWrite() ) {
            if( du.isNew() )
                toInsert.push_back(du.data());
            else
                toUpdate.push_back(du.data());
        }
    }
    storeData(toUpdate, toInsert);
}

// ------------------------------------------------------------------------

PlumaticAlgorithm::Shower PlumaticAlgorithm::findFirstShower(kvUpdateList_t& data)
{
    return findShowerForward(data.begin(), data.end());
}

// ------------------------------------------------------------------------

PlumaticAlgorithm::Shower PlumaticAlgorithm::findNextShower(const Shower& s, const kvUpdateList_it& end)
{
    kvUpdateList_it begin = s.last;
    begin++;
    return findShowerForward(begin, end);
}

// ------------------------------------------------------------------------

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
    }
    return f;
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::checkNeighborStations(int stationid, const kvUpdateList_t& data)
{
    if( data.empty() )
        return;

    miutil::miTime nextXX06 = UT0extended;
    if( nextXX06.hour() > 6 )
        nextXX06.addDay(1);
    nextXX06.addHour(6 - nextXX06.hour());
    nextXX06.addMin(-nextXX06.min());
    nextXX06.addSec(-nextXX06.sec());

    bool operational = true;

    // advance mark just beyond first ..:06 measurement
    kvUpdateList_cit mark = data.begin();
    while( mark != data.end() && mark->obstime() < nextXX06 ) {
        if( mark->isNotOperationalStart() )
            operational = false;
        if( mark->isNotOperationalEnd() )
            operational = true;
        ++mark;
    }

    while( mark != data.end() ) {
        nextXX06.addDay(1);
        bool discarded = false;
        float sum = 0;
        for( ; mark != data.end() && mark->obstime() <= nextXX06; ++mark ) {
            if( mark->isNotOperationalStart() )
                operational = false;
            if( mark->isNotOperationalEnd() )
                operational = true;
            if( isBadData(*mark) ) {
                discarded = true;
                // no break here as we have to advance to the next ..:06 observation
            } else if( mark->original() > 0 ) {
                sum = Helpers::round(sum + mark->original(), 1000);
            }
        }
        
        if( !discarded && operational )
            compareWithNeighborStations(stationid, nextXX06, sum);
    }        
}

// ------------------------------------------------------------------------

void PlumaticAlgorithm::compareWithNeighborStations(int stationid, const miutil::miTime& obstime, float sum)
{
    DBG("station=" << stationid << " obstime=" << obstime << " sum=" << sum);
    if( sum > mThresholdDry && sum < mThresholdWet ) {
        // neither dry nor wet :-) -- no need to check neighbor stations
        return;
    }

    if( !mNeighbors->hasStationList() ) {
        std::list<kvalobs::kvStation> allStations; // actually only stationary norwegian stations
        std::list<int> allStationIDs;
        fillStationLists(allStations, allStationIDs);
        mNeighbors->setStationList(allStations);
    }
    
    std::list<int> neighbors = mNeighbors->findNeighbors(stationid);
    std::vector<int> neighborsSorted;
    foreach(int n, neighbors) {
        if( mNeighbors->getWeight(n) > 0 )
            neighborsSorted.push_back(n);
    }
    // sort neighbors by decreasing weight
    std::sort(neighborsSorted.begin(), neighborsSorted.end(),
              boost::bind( &RedistributionNeighbors::getWeight, mNeighbors, _1 ) > boost::bind( &RedistributionNeighbors::getWeight, mNeighbors, _2 ));

    // const C::DBConstraint cNeighbors = C::ControlUseinfo(neighbor_flags)
    //     && C::Paramid( 110 /* RR_24 */ ) //&& C::Typeid(endpoint.typeID())
    //     && C::Obstime(obstime)
    //     && C::Station(neighbors);
    const DBInterface::DataList ndata
        = database()->findDataOrderNone(neighbors, 110 /* RR_24 */, TimeRange(obstime, obstime), neighbor_flags);
    if( ndata.empty() ) {
        info() << "no neighbor stations with data near " << stationid << " (sum=" << sum << ") at " << obstime;
        return;
    }
    
    typedef std::map<int, kvalobs::kvData> neighborDataByID_t;
    neighborDataByID_t neighborDataByID;
    foreach(const kvalobs::kvData& n, ndata) {
        neighborDataByID[n.stationID()] = n;
    }

    int nNeighbors = 0, nNeighborsDry = 0, nNeighborsWet = 0;
    std::ostringstream textNeighbors;
    float highestDryNeighbor = 0, lowestWetNeighbor = 0;
    foreach(int n, neighborsSorted) {
        neighborDataByID_t::const_iterator itN = neighborDataByID.find(n);
        if( itN == neighborDataByID.end() )
            continue;
        const float nOriginal = itN->second.original();
        if( nOriginal <= mThresholdDry ) {
            if( nNeighborsDry == 0 || nOriginal > highestDryNeighbor )
                highestDryNeighbor = nOriginal;
            nNeighborsDry += 1;
            textNeighbors << ", " << n;
        }
        if( nOriginal >= mThresholdWet ) {
            if( nNeighborsWet == 0 || nOriginal < lowestWetNeighbor )
                lowestWetNeighbor = nOriginal;
            nNeighborsWet += 1;
            textNeighbors << ", " << n;
        }
        if( ++nNeighbors >= 6 )
            break;
    }
    DBG("sum=" << sum << " n wet=" << nNeighborsWet << " dry=" << nNeighborsDry);
    if( nNeighbors >= 3 ) {
        if( sum <= mThresholdDry && nNeighborsWet == nNeighbors ) {
            warning() << "Plumatic: station " << stationid << " is dry (" << sum
                      << ") while " << nNeighbors << " neighbors (" << textNeighbors.str().substr(2)
                      << ") are wet (lowest:" << lowestWetNeighbor << ") in 24h before " << obstime;
        } else if( sum >= mThresholdWet && nNeighborsDry == nNeighbors ) {
            warning() << "Plumatic: station " << stationid << " is wet (" << sum
                      << ") while " << nNeighbors << " neighbors (" << textNeighbors.str().substr(2)
                      << ") are dry (highest=" << highestDryNeighbor << ") in 24h before " << obstime;
        }
    } else {
        info() << "found only " << nNeighbors << " neighbor stations with data near " << stationid << " (sum=" << sum << ") at " << obstime;
        return;
    }
}
