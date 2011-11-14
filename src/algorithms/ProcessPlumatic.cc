/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id$                                                       

  Copyright (C) 2007 met.no

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

namespace C = Constraint;
namespace O = Ordering;

namespace {

const float veryUnlikelySingle = 0.2;
const float veryUnlikelyStart  = 0.3;
const int   maxRainInterrupt = 5;
const int   minRainBeforeAndAfter = 2;
const float rainInterruptValue = 0.3;

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

void PlumaticAlgorithm::run(const ReadProgramOptions& params)
{
    const int pid = params.getParameter<int>("ParamId");
    FlagChange highsingle_flagchange, highstart_flagchange, interruptedrain_flagchange;
    params.getFlagChange(highstart_flagchange,  "highstart_flagchange");
    params.getFlagChange(highsingle_flagchange, "highsingle_flagchange");
    params.getFlagChange(interruptedrain_flagchange, "interruptedrain_flagchange");
    const std::string CFAILED_STRING = params.CFAILED_STRING;

    LOGINFO("Plumatic Control");

    std::list<kvalobs::kvStation> StationList;
    std::list<int> StationIds;
    fillStationLists(StationList, StationIds);

    miutil::miTime beforeUT0 = params.UT0, afterUT1 = params.UT1;
    beforeUT0.addMin(-1);
    afterUT1 .addMin( 1);

    foreach(const kvalobs::kvStation& station, StationList) {
        const C::DBConstraint cSeries = C::Station(station.stationID())
            && C::Paramid(pid) && C::Obstime(params.UT0 ,params.UT1);

        kvDataList_t data;
        database()->selectData(data, cSeries, O::Obstime());
        if( data.empty() )
            continue;
        
        Navigator nav(data.begin(), data.end());
        Info info(nav);
        info.beforeUT0 = beforeUT0;
        info.afterUT1  = afterUT1;

        for(kvDataList_it d = nav.begin(); d != nav.end(); ++d) {
            std::cout << "data point = " << *d << std::endl;

            info.d = d;
            info.prev = nav.previousNot0(d);
            info.next = nav.nextNot0(d);
            info.dryMinutesBefore = minutesBetween(d->obstime(), info.prev != nav.end() ? info.prev->obstime() : beforeUT0) - 1;
            info.dryMinutesAfter  = minutesBetween(info.next != nav.end() ? info.next->obstime() : afterUT1, d->obstime()) - 1;

            CheckResult ri = isRainInterruption(info), hsi = isHighSingle(info), hst = isHighStart(info);

//
// KNOWN PROBLEMS: device out of service, not clear how to detect this
// MISSING: sliding aggregation + flagging of large aggregation values
// WISHLIST: list of stations to avoid querying all stations
//

            if( ri != NO ) {
                if( ri == YES ) {
                    std::cout << "  rain interruption since " << *info.prev << std::endl;
                    
                    info.prev->controlinfo(interruptedrain_flagchange.apply(info.prev->controlinfo()));
                    Helpers::updateUseInfo(*info.prev);
                    Helpers::updateCfailed(*info.prev, "QC2-plu-interruptedrain-0", CFAILED_STRING);
                    
                    info.d->controlinfo(interruptedrain_flagchange.apply(info.d->controlinfo()));
                    Helpers::updateUseInfo(*info.d);
                    Helpers::updateCfailed(*info.d, "QC2-plu-interruptedrain-1", CFAILED_STRING);
                    
                    kvDataList_t toWrite;
                    toWrite.push_back(*info.prev);
                    toWrite.push_back(*info.d);
                    storeData(toWrite);
                } else {
                    std::cout << "  maybe rain interruption since " << std::flush;
                    if( info.prev != nav.end())
                        std::cout << *info.prev << std::endl;
                    else
                        std::cout << params.UT0 << std::endl;
                }
            } else if( hsi != NO ) {
                if( hsi == YES ) {
                    std::cout << "  very unlikely value for single point" << std::endl;
                    
                    info.d->controlinfo(highsingle_flagchange.apply(info.d->controlinfo()));
                    Helpers::updateUseInfo(*info.d);
                    Helpers::updateCfailed(*info.d, "QC2-plu-highsingle", CFAILED_STRING);
                    updateSingle(*info.d);
                } else {
                    std::cout << "  maybe unlikely value for single point" << std::endl;
                }
            } else if( hst != NO ) {
                if( hst == YES ) {
                    std::cout << "  very unlikely value for start point" << std::endl;
                    
                    info.d->controlinfo(highstart_flagchange.apply(info.d->controlinfo()));
                    Helpers::updateUseInfo(*info.d);
                    Helpers::updateCfailed(*info.d, "QC2-plu-highstart", CFAILED_STRING);
                    updateSingle(*info.d);
                } else {
                    std::cout << "  maybe unlikely value for start point" << std::endl;
                }
            }
            std::cout << " -- done" << std::endl;
        }
        std::cout << " -- finished" << std::endl;
    }
}

PlumaticAlgorithm::CheckResult PlumaticAlgorithm::isRainInterruption(const Info& info)
{
    // std::cout << "    checking for interrupted rain" << std::endl;
    if( info.d->original() < rainInterruptValue )
        return NO;
    // std::cout << "    above threshold" << std::endl;
    if( info.prev == info.nav.end() )
        return (info.dryMinutesBefore > maxRainInterrupt ? NO : DONT_KNOW);
    // std::cout << "    not first" << std::endl;
    if( info.dryMinutesBefore > maxRainInterrupt  || info.dryMinutesBefore < 1 )
        return NO;
    // std::cout << "    short enough interrupt" << std::endl;
    if( info.prev->original() < rainInterruptValue )
        return NO;

    // std::cout << "    looks good" << std::endl;

    // check that there is some rain before and after the "interrruption"
    int nBefore = 1;
    kvDataList_it before = info.prev, b0;
    while( (b0 = info.nav.previousNot0(before)) != info.nav.end() && miutil::miTime::minDiff(before->obstime(), b0->obstime()) == 1 ) {
        before = b0;
        nBefore += 1;
    }
    // std::cout << "    nBefore = " << nBefore << " start = " << (b0 == info.nav.end()) << std::endl;
    if( b0 == info.nav.end() && nBefore < minRainBeforeAndAfter )
        return DONT_KNOW;

    int nAfter = 2; // info.d and info.next are already after the gap
    kvDataList_it after = info.next, a0;
    while( (a0 = info.nav.nextNot0(after)) != info.nav.end() && miutil::miTime::minDiff(a0->obstime(), after->obstime()) == 1 ) {
        after = a0;
        nAfter += 1;
    }
    // std::cout << "    nAfter = " << nAfter << " end = " << (a0 == info.nav.end()) << std::endl;
    if( a0 == info.nav.end() && nAfter < minRainBeforeAndAfter )
        return DONT_KNOW;

    return (nAfter >= minRainBeforeAndAfter && nBefore >= minRainBeforeAndAfter) ? YES : NO;
}

PlumaticAlgorithm::CheckResult PlumaticAlgorithm::isHighSingle(const Info& info)
{
    // std::cout << "    checking for high single" << std::endl;
    if( info.d->original() < veryUnlikelySingle)
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
    // std::cout << "    checking for high start" << std::endl;
    if( info.d->original() <= veryUnlikelyStart)
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
