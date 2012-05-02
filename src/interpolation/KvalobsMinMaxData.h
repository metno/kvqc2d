/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

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

#ifndef KVALOBSDATA_H_
#define KVALOBSDATA_H_

#include "FlagPatterns.h"
#include "KvalobsNeighborData.h"
#include "MinMaxInterpolator.h"

class KvalobsMinMaxData : public Interpolation::MinMaxInterpolator::Data {
public:
    KvalobsMinMaxData(KvalobsNeighborData& nd);

    virtual Interpolation::SeriesData minimum(int time);
    virtual Interpolation::SeriesData maximum(int time);
    virtual void setMinimum(int time, Interpolation::Quality q, float value);
    virtual void setMaximum(int time, Interpolation::Quality q, float value);
    virtual float fluctuationLevel();

    const Interpolation::SimpleResultVector& getInterpolatedMin()
        { return interpolationsMin; }

    const Interpolation::SimpleResultVector& getInterpolatedMax()
        { return interpolationsMax; }

private:
    miutil::miTime timeAtOffset(int time) const
        { return neighborData().timeAtOffset(time); }

    DBInterface* database()
        { return neighborData().getDatabase(); }

    KvalobsNeighborData& neighborData()
        { return static_cast<KvalobsNeighborData&>(centerData()); }

    const KvalobsNeighborData& neighborData() const
        { return static_cast<const KvalobsNeighborData&>(centerData()); }

    Interpolation::SeriesData minmax(int time, int paramid, KvalobsSeriesDataList& data);

private:
    FlagSetCU mNeighborFlags;
    KvalobsSeriesDataList minimumData, maximumData;
    Interpolation::SimpleResultVector interpolationsMin, interpolationsMax;
};

#endif /* KVALOBSDATA_H_ */
