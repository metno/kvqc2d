/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

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

#include "SingleLinearAlgorithm.h"

#include "AlgorithmHelpers.h"
#include "DBInterface.h"
#include "foreach.h"

#include <milog/milog.h>
#include <puTools/miTime.h>

#define NDEBUG
#include "debug.h"

using Helpers::equal;

// ########################################################################

SingleLinearAlgorithm::SingleLinearAlgorithm()
    : Qc2Algorithm("SingleLinear")
{
}

// ------------------------------------------------------------------------

bool SingleLinearAlgorithm::isNeighborOk(const kvalobs::kvData& n)
{
    return neighbor_flags.matches(n) && n.original() != missing && n.original() != rejected;
}

// ------------------------------------------------------------------------

void SingleLinearAlgorithm::configure(const AlgorithmConfig& params)
{
    Qc2Algorithm::configure(params);

    params.getFlagSetCU(missing_flags,  "missing", "ftime=0&fmis=[1234]&fhqc=0|ftime=1&fmis=[14]&fhqc=0", "");
    params.getFlagSetCU(neighbor_flags, "neighbor", "fmis=0", "U0=[37]&U2=0");
    params.getFlagChange(ftime0_flagchange, "ftime0_flagchange", "ftime=1;fmis=3->fmis=1;fmis=2->fmis=4");
    params.getFlagChange(ftime1_flagchange, "ftime1_flagchange", "fmis=1->fmis=3;fmis=4->fmis=2");
    pids = params.getMultiParameter<int>("ParamId");
}

// ------------------------------------------------------------------------

void SingleLinearAlgorithm::run()
{
    const DBInterface::StationIDList stationIDs = database()->findNorwegianFixedStationIDs();
    DBGV(stationIDs.size());
    foreach(int pid, pids) {
        const DBInterface::DataList Qc2Data
            = database()->findDataOrderObstime(stationIDs, pid, TimeRange(UT0, UT1), missing_flags);
        DBGV(Qc2Data.size());

        foreach(const kvalobs::kvData& d, Qc2Data) {
            miutil::miTime timeBefore = d.obstime(), timeAfter = d.obstime();
            timeBefore.addHour(-1);
            timeAfter.addHour(1);

            const DBInterface::DataList series
                = database()->findDataOrderObstime(d.stationID(), d.paramID(), d.typeID(), d.sensor(), d.level(), TimeRange(timeBefore, timeAfter));
            DBGV(series.size());
            if( series.size() != 3 ) {
                DBG("got " << series.size() << " neighbors at d=" << d);
                continue;
            }

            DataUpdate update(d);
            calculateCorrected(series.front(), update, series.back());

            if( update.needsWrite() )
                updateSingle(update.data());
        }
    }
}

// ------------------------------------------------------------------------
            
void SingleLinearAlgorithm::calculateCorrected(const kvalobs::kvData& before, DataUpdate& middle, const kvalobs::kvData& after)
{
    DBG("before corr=" << middle.data());
    // check that the neighbours are good
    const int ftime = middle.controlinfo().flag(7);
    if( isNeighborOk(before) && isNeighborOk(after) ) {
        DBGV(ftime);
        if( ftime == 0 || ftime == 1 ) {
            middle.corrected(Helpers::round1( 0.5*(before.original()+after.original()) ));
            if( ftime == 0 )
                middle.controlinfo(ftime0_flagchange.apply(middle.controlinfo()));
            middle.cfailed("QC2d-2", CFAILED_STRING);
        }
    } else if( ftime == 1) {
        const int fmis = middle.controlinfo().flag(6);
        DBG("ftime=" << ftime << " fmis=" << fmis);
        if( fmis == 1 || fmis == 3 ) {
            middle.corrected(missing);
        } else if( fmis == 2 || fmis == 4 ) {
            middle.corrected(rejected);
        } else {
            // XXX this is not in the specification
            middle.corrected(Helpers::round1( 0.5*(before.original()+after.original()) ));
        }
        middle.controlinfo(ftime1_flagchange.apply(middle.controlinfo()));
        middle.cfailed("QC2d-2", CFAILED_STRING);
    }
    DBG("after corr=" << middle.data() << " mod=" << middle.needsWrite());
}
