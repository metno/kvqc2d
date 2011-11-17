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

#include "ProcessPlumatic.h"

#include "AlgorithmHelpers.h"
#include "Helpers.h"
#include "Plumatic.h"
#include "DBConstraints.h"
#include "scone.h"

#include <milog/milog.h>
#include <puTools/miTime.h>
#include "foreach.h"

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

PlumaticAlgorithm::Info::Info(Navigator& n)
    : nav(n)
{
}

/* return previous non-0 precipitation, or end() if that does not exist */
PlumaticAlgorithm::kvDataList_it PlumaticAlgorithm::Navigator::previousNot0(kvDataList_it it)
{
    if( it == begin() )
        return end();

    --it;
    while(it->original() == 0.0f) {
        if( it == begin() )
            return end();
        --it;
    }
    return it;
}
    
/* return next non-0 precipitation, or end() if that does not exist */
PlumaticAlgorithm::kvDataList_it PlumaticAlgorithm::Navigator::nextNot0(kvDataList_it it)
{
    if( it == end() )
        return end();
    ++it;
    while(it != end() && it->original() == 0.0f)
        ++it;
    return it;
}

void PlumaticAlgorithm::configure(const ReadProgramOptions& params)
{
    pid = params.getParameter<int>("ParamId");
    params.getFlagSetCU(discarded_flags, "discarded");
    params.getFlagChange(highstart_flagchange,  "highstart_flagchange");
    params.getFlagChange(highsingle_flagchange, "highsingle_flagchange");
    params.getFlagChange(interruptedrain_flagchange, "interruptedrain_flagchange");
    params.getFlagChange(aggregation_flagchange, "aggregation_flagchange");
    CFAILED_STRING = params.CFAILED_STRING;
    mStationlist = params.getParameter<std::string>("stations");
    mSlidingAlarms = params.getParameter<std::string>("sliding_alarms");
    UT0 = params.UT0;
    UT0extended = params.UT0;

    int lookback = maxRainInterrupt+minRainBeforeAndAfter;
    const std::vector<miutil::miString> msa = mSlidingAlarms.split(';');
    foreach(const miutil::miString& length_max, msa) {
        std::vector<miutil::miString> l_m = length_max.split('<', false);
        if( l_m.size() != 2 )
            throw ConfigException("cannot parse 'sliding_alarms' parameter");
        const int length = atoi(l_m[0].c_str());
        if( lookback < length )
            lookback = length;
    }
    UT0extended.addMin(-lookback);

    UT1 = params.UT1;
}

void PlumaticAlgorithm::run(const ReadProgramOptions& params)
{
    configure(params);

    // use script stinfosys-vipp-pluviometer.pl or change program and use "select stationid from obs_pgm where paramid = 105;"

    const std::vector<miutil::miString> msl = mStationlist.split(';');
    foreach(const miutil::miString& mmpv_stations, msl) {
        std::vector<miutil::miString> m_s = mmpv_stations.split(':', false);
        if( m_s.size() != 2 )
            throw ConfigException("cannot parse 'stations' parameter");
        const float mmpv = atof(m_s[0].c_str());
        if( mmpv <= 0.0f )
            throw ConfigException("invalid mm-per-vipp <= 0 in 'stations' parameter");
        const std::vector<miutil::miString> s = m_s[1].split(',');
        foreach(miutil::miString stationidText, s) {
            int stationid = atoi(stationidText.c_str());
            checkStation(stationid, mmpv);
        }
    }
}

void PlumaticAlgorithm::checkStation(int stationid, float mmpv)
{
    const C::DBConstraint cSeries = C::Station(stationid)
        && C::Paramid(pid) && C::Obstime(UT0extended, UT1);

    kvDataOList_t datao;
    database()->selectData(datao, cSeries, O::Obstime());
    if( datao.empty() )
        return;
    kvDataList_t data(datao.begin(), datao.end());

    Navigator nav(data.begin(), data.end());
    Info info(nav);
    info.mmpv = mmpv;
    info.beforeUT0 = UT0extended; info.beforeUT0.addMin(-1);
    info.afterUT1  = UT1;         info.afterUT1 .addMin( 1);

    for(kvDataList_it d = nav.begin(); d != nav.end(); ++d) {
        if( d->obstime() < UT0 )
            continue;

        info.d    = d;
        info.prev = nav.previousNot0(d);
        info.next = nav.nextNot0(d);
        info.dryMinutesBefore = minutesBetween(d->obstime(), info.prev != nav.end() ? info.prev->obstime() : info.beforeUT0) - 1;
        info.dryMinutesAfter  = minutesBetween(info.next != nav.end() ? info.next->obstime() : info.afterUT1, d->obstime()) - 1;

        CheckResult ri = isRainInterruption(info), hsi = isHighSingle(info), hst = isHighStart(info);

        if( ri != NO ) {
            if( ri == YES ) {
                DBG("  rain interruption since " << *info.prev << ' ' << *d);
                flagRainInterruption(info, data);
            } else {
                if( info.prev != nav.end() ) {
                    DBG("maybe rain interruption since " << *info.prev << ' ' << *d);
                } else {
                    DBG("maybe rain interruption since start (" << UT0 << ") " << *d);
                }
            }
        } else if( hsi != NO ) {
            if( hsi == YES ) {
                DBG("very unlikely value for single point " << *d);
                flagHighSingle(info);
            } else {
                DBG("maybe unlikely value for single point " << *d);
            }
        } else if( hst != NO ) {
            if( hst == YES ) {
                DBG("very unlikely value for start point " << *d);
                flagHighStart(info);
            } else {
                DBG("maybe unlikely value for start point " << *d);
            }
        }
    }
    
    checkSlidingSums(data);

    storeUpdates(data);
}

void PlumaticAlgorithm::checkSlidingSums(kvDataList_t& data)
{
    int lastlength = 2;
    const std::vector<miutil::miString> msa = mSlidingAlarms.split(';');
    foreach(const miutil::miString& length_max, msa) {
        std::vector<miutil::miString> l_m = length_max.split('<', false);
        if( l_m.size() != 2 )
            throw ConfigException("cannot parse 'sliding_alarms' parameter");
        const int length = atoi(l_m[0].c_str());
        if( length < lastlength )
            throw ConfigException("invalid length (ordering?) in 'sliding_alarms' parameter");
        const float maxi = atof(l_m[1].c_str());
        if( maxi <= 0.0f )
            throw ConfigException("invalid threshold (<=0?) in 'sliding_alarms' parameter");
        checkSlidingSum( data, length, maxi );
        lastlength = length;
    }
}

void PlumaticAlgorithm::checkSlidingSum(kvDataList_t& data, const int length, const float maxi)
{
    DBG("length=" << length << " maxi=" << maxi);
    std::ostringstream cfailed;
    cfailed << "QC2h-1-aggregation-" << length;
    kvDataList_it head = data.begin(), tail = data.begin();
    float sum = 0;
    std::list<kvDataList_it> discarded;
    //std::list<kvDataList_it> flagged;
    for(; head != data.end(); ++head) {
        DBG("head=" << *head << " tail=" << *tail << "minutes=" << minutesBetween(head->obstime(), tail->obstime()));
        for( ; minutesBetween(head->obstime(), tail->obstime()) >= length; ++tail ) {
            if( tail->original()>0 )
                sum -= tail->original();
            DBGV(sum);
            if( tail == discarded.front() ) {
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
        if( discarded_flags.matches(head->data()) ) {
            discarded.push_back(head);
            DBGV(discarded.size());
        }
        if( sum >= maxi /*&& flagged.empty()*/ && discarded.empty() ) {
            //flagged.push_back(head);
            //DBGV(flagged.size());
            kvDataList_it stop = head; stop++;
            for(kvDataList_it mark = tail; mark != stop; ++mark)
                mark->controlinfo(aggregation_flagchange.apply(mark->controlinfo()))
                    . cfailed(cfailed.str(), CFAILED_STRING);
        }
    }
}

PlumaticAlgorithm::CheckResult PlumaticAlgorithm::isRainInterruption(const Info& info)
{
    if( info.d->original() < info.mmpv*vippsRainInterrupt )
        return NO;
    if( info.prev == info.nav.end() )
        return (info.dryMinutesBefore > maxRainInterrupt ? NO : DONT_KNOW);
    if( info.dryMinutesBefore > maxRainInterrupt  || info.dryMinutesBefore < 1 )
        return NO;
    if( info.prev->original() < info.mmpv*vippsRainInterrupt )
        return NO;

    // check that there is some rain before and after the "interrruption"
    int nBefore = 1;
    kvDataList_it before = info.prev, b0;
    while( (b0 = info.nav.previousNot0(before)) != info.nav.end() && miutil::miTime::minDiff(before->obstime(), b0->obstime()) == 1 ) {
        before = b0;
        nBefore += 1;
    }
    DBG("    nBefore = " << nBefore << " start = " << (b0 == info.nav.end()));

    int nAfter = 2; // info.d and info.next are already after the gap, so we have minimum 2 after
    kvDataList_it after = info.next, a0;
    while( (a0 = info.nav.nextNot0(after)) != info.nav.end() && miutil::miTime::minDiff(a0->obstime(), after->obstime()) == 1 ) {
        after = a0;
        nAfter += 1;
    }
    DBG("    nAfter = " << nAfter << " end = " << (a0 == info.nav.end()));

    if( a0 == info.nav.end() && nAfter < minRainBeforeAndAfter )
        return DONT_KNOW;

    return (nAfter >= minRainBeforeAndAfter && nBefore >= minRainBeforeAndAfter) ? YES : NO;
}

PlumaticAlgorithm::CheckResult PlumaticAlgorithm::isHighSingle(const Info& info)
{
    if( info.d->original() < info.mmpv*vippsUnlikelySingle)
        return NO;
    if( info.dryMinutesBefore<1 || info.dryMinutesAfter<1 )
        return NO;
    if( info.prev == info.nav.end() && info.dryMinutesBefore == 1 )
        return DONT_KNOW;
    if( info.next == info.nav.end() && info.dryMinutesAfter == 1 )
        return DONT_KNOW;
    return YES;
}

PlumaticAlgorithm::CheckResult PlumaticAlgorithm::isHighStart(const Info& info)
{
    if( info.d->original() <= info.mmpv*vippsUnlikelyStart)
        return NO;
    if( info.dryMinutesBefore < 1 )
        return NO;
    if( info.dryMinutesBefore == 1 && info.prev == info.nav.end() )
        return DONT_KNOW;
    if( info.dryMinutesAfter < 1 )
        return YES;
    if( info.dryMinutesAfter == 1 && info.next == info.nav.end() )
        return DONT_KNOW;
    return YES;
}

void PlumaticAlgorithm::flagRainInterruption(Info& info, kvDataList_t& data)
{
    const miutil::miTime now = miutil::miTime::nowTime();
    miutil::miTime t = info.prev->obstime(); t.addMin(1);
    for(; t<info.d->obstime(); t.addMin(1)) {
        DBG("t=" << t);

        bool needInsert = true;
        kvDataList_t::iterator it = info.prev;
        for( ; it != info.d; ++it ) {
            if( t == it->obstime() ) {
                needInsert = false;
                break;
            }
        }
        if( needInsert ) {
            DataUpdate insert(info.d->data(), t, now, 0.0f, "0000000000000000");
            insert.controlinfo(interruptedrain_flagchange.apply(insert.controlinfo()))
                .cfailed("QC2h-1-interruptedrain", CFAILED_STRING);
            data.insert(info.d, insert);
        } else {
            it->controlinfo(interruptedrain_flagchange.apply(it->controlinfo()))
                .cfailed("QC2h-1-interruptedrain", CFAILED_STRING);
        }
    }
}

void PlumaticAlgorithm::flagHighSingle(Info& info)
{
    info.d->controlinfo(highsingle_flagchange.apply(info.d->controlinfo()))
        .cfailed("QC2h-1-highsingle", CFAILED_STRING);
}

void PlumaticAlgorithm::flagHighStart(Info& info)
{
    info.d->controlinfo(highstart_flagchange.apply(info.d->controlinfo()))
        .cfailed("QC2h-1-highstart", CFAILED_STRING);
}

void PlumaticAlgorithm::storeUpdates(const kvDataList_t& data)
{
    kvDataOList_t toInsert, toUpdate;
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

// ########################################################################

DataUpdate::DataUpdate()
    : mNew(false)
    , mOrigControl("0000000000000000")
    , mOrigCorrected(-32767.0f)
    , mOrigCfailed("")
{
}

DataUpdate::DataUpdate(const kvalobs::kvData& data)
    : mData(data)
    , mNew(false)
    , mOrigControl(mData.controlinfo())
    , mOrigCorrected(mData.corrected())
    , mOrigCfailed(mData.cfailed())
{
}

DataUpdate::DataUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
                       float corrected, const std::string& controlinfo)
    : mData(templt.stationID(), obstime, -32767, templt.paramID(), tbtime, templt.typeID(), templt.sensor(),
            templt.level(), corrected, kvalobs::kvControlInfo(controlinfo), kvalobs::kvUseInfo(), "QC2-missing-row")
    , mNew(true)
    , mOrigControl(mData.controlinfo())
    , mOrigCorrected(mData.corrected())
    , mOrigCfailed(mData.cfailed())
{
    Helpers::updateUseInfo(mData);
}

bool DataUpdate::isModified() const
{
    return mNew
        || mOrigCorrected != mData.corrected()
        || mOrigControl   != mData.controlinfo();
    // || mOrigCfailed   != mData.cfailed(); // this does not work because text is appended instead of overwritten
}

DataUpdate& DataUpdate::controlinfo(const kvalobs::kvControlInfo& ci)
{
    mData.controlinfo(ci);
    Helpers::updateUseInfo(mData);
    return *this;
}

DataUpdate& DataUpdate::cfailed(const std::string& cf, const std::string& extra)
{
    Helpers::updateCfailed(mData, cf, extra);
    return *this;
}

std::ostream& operator<<(std::ostream& out, const DataUpdate& du)
{
    out << du.data() << "[cf='" << du.data().cfailed() << ']';
    if( du.isModified() ) {
        out << '{';
        if( du.isNew() )
            out << 'n';
        else
            out << 'm';
        out << '}';
    }
    return out;
}
