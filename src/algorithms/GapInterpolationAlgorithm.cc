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

#include "GapInterpolationAlgorithm.h"

#include "AkimaSpline.h"
#include "AlgorithmHelpers.h"
#include "DBConstraints.h"
#include "Helpers.h"
#include "scone.h"
#include "tround.h"

#include <milog/milog.h>
#include "foreach.h"

namespace C = Constraint;
namespace O = Ordering;

void GapInterpolationAlgorithm::configure( const ReadProgramOptions& params )
{
    Ngap = params.getParameter("MaxHalfGap", 0);
}

void GapInterpolationAlgorithm::run( const ReadProgramOptions& params )
{
    const std::vector<int> pids = params.getMultiParameter<int>("ParamId");
    const int tid = params.getParameter<int>("TypeId");

    const long StartDay = params.UT0.date().julianDay();

    std::list<kvalobs::kvStation> StationList;
    std::list<int> StationIds;
    fillStationLists(StationList, StationIds);

    const C::DBConstraint cGeneral = (C::Paramid(pids) && C::Typeid(tid)
                && C::Obstime(params.UT0, params.UT1) && C::Sensor(0) && C::Level(0));

    foreach(const kvalobs::kvStation& station, StationList) {
        std::list<kvalobs::kvData> Qc2SeriesData;
        const C::DBConstraint cStation = C::Station(station.stationID()) && cGeneral;
        database()->selectData(Qc2SeriesData, cStation);

        std::vector<double> xt,yt;
        // Go through the data and fit an Akima Spline to the good points
        foreach(const kvalobs::kvData& d, Qc2SeriesData) {
            if( d.useinfo().flag(2)==0 ) {
                const miutil::miDate PDate = d.obstime().date();
                const double JulDec = PDate.julianDay()+d.obstime().hour()/24.0 +
                    d.obstime().min()/(24.0*60)+d.obstime().sec()/(24.0*60.0*60.0);
                const double HourDec = (PDate.julianDay()-StartDay)*24.0 + d.obstime().hour() +
                    d.obstime().min()/60.0+d.obstime().sec()/3600.0;
                xt.push_back(HourDec);
                yt.push_back(d.original());
            }
        }
        // Calculate the Akima Spline if there are enough points and the fill missing points
        if( xt.size() <= 4 )
            continue;

        AkimaSpline AkimaX(xt, yt);
        // Find the missing points and their distance from a good point etc ..
        foreach(const kvalobs::kvData& d, Qc2SeriesData) {
            const int flag6 = d.controlinfo().flag(6);
            if( flag6==1 || flag6==2 || flag6==3 || flag6==4 ) {
                //value is missing so find the time stamp wrt Akima fit
                const miutil::miDate PDate = d.obstime().date();
                const double JulDec = PDate.julianDay()+d.obstime().hour()/24.0 +
                    d.obstime().min()/(24.0*60)+d.obstime().sec()/(24.0*60.0*60.0);
                const double HourDec = (PDate.julianDay()-StartDay)*24.0 + d.obstime().hour() +
                    d.obstime().min()/60.0+d.obstime().sec()/3600.0;
                // Check to see how many hours the point is away from good data ... in the past ...
                double lowHour, highHour; // FIXME not initialised
                foreach(double v, xt) {
                    if( v < HourDec )
                        lowHour = v;
                }
                // ... and in the future
                foreach_r(double v, xt) {
                    if( v > HourDec )
                        highHour = v;
                }
                //std::cout << lowHour << " :: " << HourDec << " :: " << highHour << std::endl;
                if ( ( std::find(xt.begin(), xt.end(), highHour+1) != xt.end() ) &&
                     ( std::find(xt.begin(), xt.end(), lowHour-1)  != xt.end() ) &&
                     ( std::find(xt.begin(), xt.end(), lowHour-2)  != xt.end() || std::find(xt.begin(), xt.end(), highHour+2)  != xt.end() ) &&
                     ( HourDec - lowHour <= Ngap ) &&
                     ( highHour - HourDec  < Ngap ) )
                {
                    // Do Akima Interpolation
                    std::cout << d.stationID() << " " << d.obstime() << " " << d.original() << " " << d.corrected() << " Sub Akima " << AkimaX.AkimaPoint(HourDec) << std::endl;
                    const float NewCorrected = round<float,1>(AkimaX.AkimaPoint(HourDec));

                    // Push the data back
                    FlagChange fc;
                    params.getFlagChange(fc, "gap_flagchange");

                    kvalobs::kvData dwrite(d);
                    dwrite.corrected(NewCorrected);
                    dwrite.controlinfo(fc.apply(dwrite.controlinfo()));
                    Helpers::updateCfailed(dwrite, "QC2d-2-A", params.CFAILED_STRING);
                    Helpers::updateUseInfo(dwrite);
                    updateData(dwrite);
                }
            }
        }
    }
}
