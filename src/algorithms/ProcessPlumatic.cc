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
const int   maxRainInterrupt = 6;
const float rainInterruptValue = 0.3;

typedef std::list<kvalobs::kvData> kvDataList_t;
typedef std::list<kvalobs::kvData>::iterator kvDataList_it;

inline kvDataList_it before(kvDataList_it it)
{ return --it; }

inline kvDataList_it after(kvDataList_it it)
{ return ++it; }

kvDataList_it before(const kvDataList_it& begin, kvDataList_it it)
{
    --it;
    while( it->obstime().min() == 0 && it->original() == 0.0f && it != begin )
        --it;
    return it;
}

kvDataList_it after(const kvDataList_it& end, kvDataList_it it)
{
    ++it;
    while( it->obstime().min() == 0 && it->original() == 0.0f && after(it) != end )
        ++it;
    return it;
}

}; // anonymous namespace

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

    foreach(const kvalobs::kvStation& station, StationList) {
        const C::DBConstraint cSeries = C::Station(station.stationID())
            && C::Paramid(pid) && C::Obstime(params.UT0 ,params.UT1);
        std::list<kvalobs::kvData> series;
        database()->selectData(series, cSeries, O::Obstime());
        if( series.empty() )
            continue;

        for(kvDataList_it d = series.begin(); d != series.end(); ++d) {
            const bool notFirst = (d != series.begin()), notLast = (after(d) != series.end());
            const kvDataList_it befr = notFirst ? before(series.begin(), d) : series.begin();
            const kvDataList_it aftr = notLast  ? after (series.end(),   d) : before(series.end());
            //std::cout << "d= " << *d << "\n   befr=" << *befr << "\n   aftr=" << *aftr << std::endl;
            
            const int tDiffBefore = miutil::miTime::minDiff(d->obstime(), notFirst ? befr->obstime() : params.UT0);
            const int tDiffAfter  = miutil::miTime::minDiff(notLast ? aftr->obstime() : params.UT1, d->obstime());

            const bool dryBefore = tDiffBefore > 1 || (notFirst && befr->original() == 0.0f);
            const bool dryAfter  = tDiffAfter  > 1 || (notLast  && aftr->original() == 0.0f);
            //std::cout << "td b=" << tDiffBefore << " a=" << tDiffAfter << " db=" << dryBefore << " da=" << dryAfter << std::endl;

            if( notFirst && dryBefore && !dryAfter && tDiffBefore <= maxRainInterrupt && befr->original() >= rainInterruptValue && d->original() >= rainInterruptValue ) {
                std::cout << "rain interruption between " << *befr << " and " << *d << std::endl;

                befr->controlinfo(interruptedrain_flagchange.apply(befr->controlinfo()));
                Helpers::updateUseInfo(*befr);
                Helpers::updateCfailed(*befr, "QC2-plu-interruptedrain-0", CFAILED_STRING);

                d->controlinfo(interruptedrain_flagchange.apply(d->controlinfo()));
                Helpers::updateUseInfo(*d);
                Helpers::updateCfailed(*d, "QC2-plu-interruptedrain-1", CFAILED_STRING);

                kvDataList_t toWrite;
                toWrite.push_back(*befr);
                toWrite.push_back(*d);
                updateData(toWrite);
            } else if( dryBefore && dryAfter && d->original() >= veryUnlikelySingle ) {
                std::cout << "very unlikely value for single point " << *d << std::endl;

                d->controlinfo(highsingle_flagchange.apply(d->controlinfo()));
                Helpers::updateUseInfo(*d);
                Helpers::updateCfailed(*d, "QC2-plu-highsingle", CFAILED_STRING);
                updateData(*d);
            } else if( dryBefore && !dryAfter && d->original() > veryUnlikelyStart ) {
                std::cout << "very unlikely value for start point " << *d << std::endl;

                d->controlinfo(highstart_flagchange.apply(d->controlinfo()));
                Helpers::updateUseInfo(*d);
                Helpers::updateCfailed(*d, "QC2-plu-highstart", CFAILED_STRING);
                updateData(*d);
            }
        }
    }
}
