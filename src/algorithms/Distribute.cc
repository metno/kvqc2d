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

#include "Distribute.h"

#include "AlgorithmHelpers.h"
#include "tround.h"
#include "foreach.h"

///Add a row to the data object holding items to be redistributed.
void Distribute::add_element(const StationData& sd)
{
    int sid = sd.mObservation.stationID();
    mStationsByID[ sid ].push_back(sd);
}

/// Clear all data from the redistribution data object.
void Distribute::clear_all()
{
    mStationsByID.clear();
}

/// Clear single station entry from the redistribution data object.
void Distribute::clean_station_entry(int sid)
{
    mStationsByID[ sid ].clear();
}

/// Algorithm to redistribute data based on interpolated model data.
void Distribute::RedistributeStationData(int stid, std::list<kvalobs::kvData>& ReturnData, const ReadProgramOptions& params)
{
    std::vector<StationData>& svec = mStationsByID[ stid ];
    // need to keep accval -- if it is -1 need to set all redistributed elements to -1
    const float original_accval = svec.back().mObservation.original();
    float accval = original_accval;
    if (accval == -1.0)
        accval=0.0;

    int irun = 0;
    int index = svec.size()-1 ;
    const int sindex=index;
    float missing_val = params.missing;
    while (missing_val == params.missing && index != 0) { //works out how long the series to redistribute is
        missing_val = svec[ index - 1 ].mObservation.original();
        ++irun;
        --index;
    }

    if( irun<1 )
        return;

    if( svec[ sindex-irun ].mObservation.obstime() == params.UT0 ) {
        // NB if the available data starts at the first time
        // we cannot redistribute since there might be times
        // earlier!!!
        return;
    }

    float sumint = 0.0;
    for( int k=sindex; k>=sindex-irun ; --k ) {
        if( svec[ k ].mInterpolated == -10.0 )
            return;  // this is set if any of the points are unavailable
        sumint += svec[ k ].mInterpolated;

        // Also a check for time continuity
        if( k != sindex-irun ) {
            const miutil::miTime& d_now = svec[k].mObservation.obstime();
            miutil::miTime d_test = d_now;
            d_test.addDay(-1);
            const miutil::miTime& d_next = svec[k-1].mObservation.obstime();
            if ( d_next != d_test )
                return;
        }
    }

    if( sumint <= 0 ) {
        // XXX what is this condition for?
        return;
    }

    float normaliser = accval/sumint;
    float roundSum = 0.0;
    //for (int k=sindex-irun+1; k<=sindex ; ++k) {
    for (int k=sindex-irun; k<=sindex ; ++k) {

        // Perform redistribution.
        float roundVal = round<float,1>(svec[k].mInterpolated * normaliser);
        roundSum += roundVal;  // Need to check roundSum does not deviate too much from accval
        if (original_accval == -1.0)
            roundVal=-1.0;
        if (roundVal == 0.0)
            roundVal=-1.0;   // BUG1304 ... By default assume dry.

        // Set flags and do housecleaning indicating that a redistribution has been done
        kvalobs::kvControlInfo fixflags = svec[ k ].mObservation.controlinfo();
        ControlFlag.setter(fixflags,params.Sflag);
        ControlFlag.conditional_setter(fixflags,params.chflag);

        kvalobs::kvData correctedData(svec[k].mObservation);
        correctedData.corrected(roundVal);
        correctedData.controlinfo(fixflags);
        Helpers::updateUseInfo(correctedData);
        Helpers::updateCfailed(correctedData, "QC2-redist", params.CFAILED_STRING);
        ReturnData.push_back(correctedData);
    }

    // Check ReturnData
    // IF VALUES ARE DIFFERENT
    float compareSum = round<float,1>(roundSum-accval);
    if (compareSum != 0.0) {
        foreach(kvalobs::kvData& d, ReturnData) {
            if ((d.corrected() - compareSum) > 0.0) {
                d.corrected(d.corrected() - compareSum);
                compareSum=0.0;
            }
        }
    }
}

