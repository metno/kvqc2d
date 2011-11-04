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

#include "SingleLinearAlgorithm.h"

#include "AlgorithmHelpers.h"
#include "DBConstraints.h"
#include "Helpers.h"
#include "tround.h"
#include "scone.h"

#include <milog/milog.h>
#include <puTools/miTime.h>
#include "foreach.h"

namespace C = Constraint;
namespace O = Ordering;
using Helpers::equal;

namespace {

const float NO_UPDATE = -99999.0;

} // anonymous namespace

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

void SingleLinearAlgorithm::configure(const ReadProgramOptions& params)
{
    params.getFlagSetCU(missing_flags, "missing");
    params.getFlagSetCU(neighbor_flags, "neighbor");
    params.getFlagChange(update_flagchange, "update_flagchange");
    params.getFlagChange(missing_flagchange, "missing_flagchange");
    missing = params.missing;
    rejected = params.rejected;
    CFAILED_STRING = params.CFAILED_STRING;
}

// ------------------------------------------------------------------------

void SingleLinearAlgorithm::run(const ReadProgramOptions& params)
{
    configure(params);

    for(miutil::miTime ProcessTime = params.UT1; ProcessTime >= params.UT0; ProcessTime.addHour(-1)) {
        const C::DBConstraint cSingleMissing =
            C::ControlUseinfo(missing_flags)
            && C::Paramid(params.pid)
            && C::Obstime(ProcessTime); // TODO AND stationid BETWEEN 60 and 99999"?
        std::list<kvalobs::kvData> Qc2Data;
        database()->selectData(Qc2Data, cSingleMissing);
        if( Qc2Data.empty() )
            continue;

        miutil::miTime timeBefore = ProcessTime, timeAfter = ProcessTime;
        timeBefore.addHour(-1);
        timeAfter.addHour(1);

        foreach(const kvalobs::kvData& d, Qc2Data) {
            // may not check neighbor flags here, as the corrected value from calculateCorrected depends on neighbor availability
            const C::DBConstraint cNeighbors = C::Station(d.stationID()) && C::Paramid(d.paramID()) && C::Typeid(d.typeID())
                && (C::Obstime(timeBefore) || C::Obstime(timeAfter));

            std::list<kvalobs::kvData> series;
            database()->selectData(series, cNeighbors, O::Obstime());
            if( series.size() != 2 ) {
                LOGDEBUG("got " << series.size() << " neighbors at d=" << d);
                continue;
            }

            const float NewCorrected = calculateCorrected(series.front(), d, series.back());
            if( equal(NewCorrected, NO_UPDATE) ) {
                LOGDEBUG("no update for d=" << d);
                continue;
            }

            LOGDEBUG("about to store corrected=" << NewCorrected << " for d=" << d);
            writeChanges(d, NewCorrected);
        }
    }
}

// ------------------------------------------------------------------------
            
float SingleLinearAlgorithm::calculateCorrected(const kvalobs::kvData& before, const kvalobs::kvData& middle, const kvalobs::kvData& after)
{
    float NewCorrected = NO_UPDATE;

    // check that the neighbours are good
    const int flag7 = middle.controlinfo().flag(7);
    if( isNeighborOk(before) && isNeighborOk(after) ) {
        if( flag7 == 0 || flag7 == 1 ) {
            NewCorrected = round<float,1>( 0.5*(before.original()+after.original()) );
            // std::cout << "heiho cc" << std::endl;
            if( (flag7 == 1) && equal(middle.corrected(), NewCorrected) ) {
                NewCorrected = NO_UPDATE;
            }
        }
    } else {
        //NB for ftime=0 ... do nothing
        
        //For ftime=1, reset to missing value ...
        if( flag7 == 1 ) {
            const int flag6 = middle.controlinfo().flag(6);
            if( flag6 == 1 || flag6 == 3 ) {
                if( !equal(middle.corrected(), missing) ) // XXX this is new compared to Paul's version, but necessary to pass his test
                    NewCorrected = missing;
            } else if( flag6 == 2 || flag6 == 4 ) {
                if( !equal(middle.corrected(), rejected) ) // XXX this is new compared to Paul's version, but necessary to pass his test
                    NewCorrected = rejected;
            } else {
                NewCorrected = round<float,1>( (before.original()+after.original())/2 );
            }
        }
    }
    return NewCorrected;
}

// ------------------------------------------------------------------------
            
void SingleLinearAlgorithm::writeChanges(const kvalobs::kvData& middle, const float NewCorrected)
{
    kvalobs::kvControlInfo fixflags = update_flagchange.apply(middle.controlinfo());
    if( equal(NewCorrected, missing) || equal(NewCorrected, rejected) )
        fixflags = missing_flagchange.apply(fixflags);
        
    kvalobs::kvData dwrite(middle);
    dwrite.corrected(NewCorrected);
    dwrite.controlinfo(fixflags);
    Helpers::updateCfailed(dwrite, "QC2d-2", CFAILED_STRING);
    Helpers::updateUseInfo(dwrite);

    updateData(dwrite);
}
