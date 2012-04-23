/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011 met.no

  Contact information:
  Norwegian Meteorological Institute
  Postboks 43 Blindern
  N-0313 OSLO
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

#include "KvalobsDataUU.h"

#include "helpers/AlgorithmHelpers.h"
#include "helpers/FormulaUU.h"
#include "foreach.h"

using NeighborInterpolation::SeriesData;
using NeighborInterpolation::SupportData;

KvalobsDataUU::KvalobsDataUU(DBInterface* db, const Instrument& i, const TimeRange& t)
: KvalobsData(db, i, t)
{
}

SeriesData KvalobsDataUU::center(int time)
{
    SeriesData sd = KvalobsData::center(time);
    if( !sd.usable() )
        return sd;

    if( centerObservationsTA.empty() ) {
        FlagSetCU all;
        centerObservationsTA = mDB->findDataMaybeTSLOrderObstime(mInstrument.stationid,
                211, DBInterface::INVALID_ID, DBInterface::INVALID_ID,
                DBInterface::INVALID_ID, mTimeRangeExtended, all);
    }

    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvData& ta, centerObservationsTA) {
        if( ta.obstime() == t && !Helpers::isMissingOrRejected(ta)) {
            return SeriesData(Helpers::formulaTD(ta.original(), sd.value()));
        }
    }
    return SeriesData();
}

SupportData KvalobsDataUU::model(int)
{
    return SupportData();
}

SupportData KvalobsDataUU::neighbor(int n, int time)
{
    const SupportData sd = KvalobsData::neighbor(n, time);
    if (!sd.usable())
        return sd;

    if (neighborObservationsTA.empty())
        neighborObservationsTA = std::vector<DBInterface::DataList>(neighbors());

    const DBInterface::DataList& noTA = neighborObservationsTA[n];
    if( noTA.empty() ) {
        const NeighborData& nd = neighborCorrelations[n];
        FlagSetCU all;
        neighborObservationsTA[n] =
                mDB->findDataMaybeTSLOrderObstime(nd.neighborid, 211,
                        DBInterface::INVALID_ID, DBInterface::INVALID_ID,
                        DBInterface::INVALID_ID, mTimeRangeExtended, all);
    }

    const miutil::miTime t = timeAtOffset(time);
    foreach(const kvalobs::kvData& ta, neighborObservationsTA[n]) {
        if( ta.obstime() == t ) {
            if( mNeighborFlags.matches(ta) )
                return SupportData(Helpers::formulaTD(ta.original(), sd.value()));
            else
                return SupportData();
        }
    }
    return SupportData();
}

KvalobsDataUU2::KvalobsDataUU2(KvalobsData& dUU, KvalobsData& dTA)
: dataUU(dUU), dataTA(dTA)
{
}

SeriesData KvalobsDataUU2::center(int time)
{
    const SeriesData sdUU = dataUU.center(time), sdTA = dataTA.center(time);
    if( !sdUU.usable() || !sdTA.usable() )
        return sdUU;

    return SeriesData(Helpers::formulaTD(sdTA.value(), sdUU.value()));
}

SupportData KvalobsDataUU2::model(int)
{
    return SupportData();
}

SupportData KvalobsDataUU2::transformedNeighbor(int n, int time)
{
    const SupportData sdUU = dataUU.neighbor(n, time), sdTA = dataTA.neighbor(n, time);
    if( !sdUU.usable() || !sdTA.usable() )
        return sdUU;

    const NeighborData& nd = dataUU.getNeighborData(n);
    const float value = nd.offset +
            (nd.slope * Helpers::formulaTD(sdTA.value(), sdUU.value()));
    return SupportData(value);
}
