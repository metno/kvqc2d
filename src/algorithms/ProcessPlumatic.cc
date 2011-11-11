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
        for(kvDataList_it d = nav.begin(); d != nav.end(); ++d) {
            Info info(nav);

            info.d = d;
            info.prev = nav.previousNot0(d);
            info.next = nav.nextNot0(d);
            info.dryMinutesBefore = miutil::miTime::minDiff(d->obstime(), info.prev != nav.end() ? info.prev->obstime() : beforeUT0) - 1;
            info.dryMinutesAfter  = miutil::miTime::minDiff(info.next != nav.end() ? info.next->obstime() : afterUT1, d->obstime()) - 1;

            if( isRainInterruption(info) ) {
                std::cout << "rain interruption between " << *info.prev << " and " << *info.d << std::endl;

                info.prev->controlinfo(interruptedrain_flagchange.apply(info.prev->controlinfo()));
                Helpers::updateUseInfo(*info.prev);
                Helpers::updateCfailed(*info.prev, "QC2-plu-interruptedrain-0", CFAILED_STRING);

                info.d->controlinfo(interruptedrain_flagchange.apply(info.d->controlinfo()));
                Helpers::updateUseInfo(*info.d);
                Helpers::updateCfailed(*info.d, "QC2-plu-interruptedrain-1", CFAILED_STRING);

                kvDataList_t toWrite;
                toWrite.push_back(*info.prev);
                toWrite.push_back(*info.d);
                updateData(toWrite);
            } else if( isHighSingle(info) ) {
                std::cout << "very unlikely value for single point " << *info.d << std::endl;

                info.d->controlinfo(highsingle_flagchange.apply(info.d->controlinfo()));
                Helpers::updateUseInfo(*info.d);
                Helpers::updateCfailed(*info.d, "QC2-plu-highsingle", CFAILED_STRING);
                updateData(*info.d);
            } else if( isHighStart(info) ) {
                std::cout << "very unlikely value for start point " << *info.d << std::endl;

                info.d->controlinfo(highstart_flagchange.apply(info.d->controlinfo()));
                Helpers::updateUseInfo(*info.d);
                Helpers::updateCfailed(*info.d, "QC2-plu-highstart", CFAILED_STRING);
                updateData(*info.d);
            }
        }
    }
}

bool PlumaticAlgorithm::isRainInterruption(const Info& info)
{
    if( !(info.dryMinutesAfter<1
          && info.dryMinutesBefore <= maxRainInterrupt
          && info.prev != info.nav.end()
          && info.prev->original() >= rainInterruptValue
          && info.d->original() >= rainInterruptValue) )
        return false;

    // check that there is some rain before and after the "interrruption"
    int nBefore = 1;
    kvDataList_it before = info.prev, b0;
    while( (b0 = info.nav.previousNot0(before)) != info.nav.end() && miutil::miTime::minDiff(before->obstime(), b0->obstime()) == 1 ) {
        before = b0;
        nBefore += 1;
    }

    int nAfter = 2; // info.d and info.next are already after the gap
    kvDataList_it after = info.next, a0;
    while( (a0 = info.nav.nextNot0(after)) != info.nav.end() && miutil::miTime::minDiff(a0->obstime(), after->obstime()) == 1 ) {
        after = a0;
        nAfter += 1;
    }
    std::cout << "nBefore = " << nBefore << " nAfter = " << nAfter << " for " << *info.d << std::endl;
    return (nAfter >= minRainBeforeAndAfter && nBefore >= minRainBeforeAndAfter);
}

bool PlumaticAlgorithm::isHighSingle(const Info& info)
{
    return info.dryMinutesBefore>=1
        && info.dryMinutesAfter>=1
        && info.d->original() >= veryUnlikelySingle;
}

bool PlumaticAlgorithm::isHighStart(const Info& info)
{
    return info.dryMinutesBefore>=1
        && info.dryMinutesAfter<1
        && info.d->original() > veryUnlikelyStart;
}
