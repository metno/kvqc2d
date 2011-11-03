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
    std::vector<std::string> setmissing_fmis;
    setmissing_fmis.push_back("1->3");
    setmissing_fmis.push_back("4->2");
    setmissing_chflag[6]=setmissing_fmis;
}

// ------------------------------------------------------------------------

bool SingleLinearAlgorithm::isNeighborOk(const ReadProgramOptions& params, const kvalobs::kvData& n)
{
    FlagSet cflags, uflags;
    params.getFlagSet(cflags, "neighbor_cflags");
    params.getFlagSet(uflags, "neighbor_uflags");
    return cflags.matches(n.controlinfo())
        && uflags.matches(n.useinfo())
        && n.original() != params.missing
        && n.original() != params.rejected;
}

// ------------------------------------------------------------------------

void SingleLinearAlgorithm::run(const ReadProgramOptions& params)
{
    LOGINFO("Single Linear");

    FlagSet missingdata_cflags, missingdata_uflags;
    params.getFlagSet(missingdata_cflags, "missingdata_cflags");
    params.getFlagSet(missingdata_uflags, "missingdata_uflags");

    for(miutil::miTime ProcessTime = params.UT1; ProcessTime >= params.UT0; ProcessTime.addHour(-1)) {
        LOGDEBUG("time=" << ProcessTime);

        const C::DBConstraint cSingleMissing =
            C::Controlinfo(missingdata_cflags) && C::Useinfo(missingdata_uflags)
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
            std::list<kvalobs::kvData> Qc2SeriesData;
            database()->dataForStationParamTimerange(Qc2SeriesData, d.stationID(), params.pid, timeBefore, timeAfter);
            if( Qc2SeriesData.size() != 3 )
                continue;

            std::vector<kvalobs::kvData> Tseries(Qc2SeriesData.begin(), Qc2SeriesData.end());
            if( !Helpers::checkContinuousHourAndSameTypeID(Tseries) || Tseries[1] != d ) {
                LOGDEBUG("cancel continuous");
                continue;
            }

            const float NewCorrected = calculateCorrected(params, Tseries);
            if( equal(NewCorrected, NO_UPDATE) ) {
                LOGDEBUG("cancel no update");
                continue;
            }

            LOGDEBUG("about to store");
            storeUpdate(params, Tseries[1], NewCorrected);
        }
    }
}

// ------------------------------------------------------------------------
            
float SingleLinearAlgorithm::calculateCorrected(const ReadProgramOptions& params, const std::vector<kvalobs::kvData>& Tseries)
{
    float NewCorrected = NO_UPDATE;
    const kvalobs::kvData before = Tseries[0], middle = Tseries[1], after = Tseries[2];

    // check that the neighbours are good
    const int flag7 = middle.controlinfo().flag(7);
    if( isNeighborOk(params, before) && isNeighborOk(params, after) ) {
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
                if( !equal(middle.corrected(), params.missing) ) // XXX this is new compared to Paul's version, but necessary to pass his test
                    NewCorrected = params.missing;
            } else if( flag6 == 2 || flag6 == 4 ) {
                if( !equal(middle.corrected(), params.rejected) ) // XXX this is new compared to Paul's version, but necessary to pass his test
                    NewCorrected = params.rejected;
            } else {
                NewCorrected = round<float,1>( (before.original()+after.original())/2 );
            }
        }
    }
    return NewCorrected;
}

// ------------------------------------------------------------------------
            
void SingleLinearAlgorithm::storeUpdate(const ReadProgramOptions& params, const kvalobs::kvData& middle, const float NewCorrected)
{
    kvalobs::kvControlInfo fixflags = middle.controlinfo();
    checkFlags().setter(fixflags, params.Sflag);
    checkFlags().conditional_setter(fixflags, params.chflag);
    if( equal(NewCorrected, params.missing) || equal(NewCorrected, params.rejected) )
        checkFlags().conditional_setter(fixflags, setmissing_chflag);
        
    kvalobs::kvData dwrite(middle);
    dwrite.corrected(NewCorrected);
    dwrite.controlinfo(fixflags);
    Helpers::updateCfailed(dwrite, "QC2d-2", params.CFAILED_STRING);
    Helpers::updateUseInfo(dwrite);

    updateData(dwrite);
}
