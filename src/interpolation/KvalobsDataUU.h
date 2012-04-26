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

#ifndef KVALOBSDATAUU_H_
#define KVALOBSDATAUU_H_

#include "KvalobsMinMaxData.h"

class KvalobsUUNeighborData : public Interpolation::NeighborInterpolator::Data {
public:
    KvalobsUUNeighborData(KvalobsNeighborData& dUU, KvalobsNeighborData& dTA);

    virtual int duration()
        { return dataUU.duration(); }

    virtual Interpolation::SeriesData parameter(int time);

    virtual Interpolation::SupportData transformedNeighbor(int n, int time);

    virtual Interpolation::SupportData model(int)
        { return Interpolation::SupportData(); }

    virtual int neighbors()
        { return dataUU.neighbors(); }

    virtual float maximumOffset()
        { return dataUU.neighbors(); }

    virtual float neighborWeight(int neighbor)
        { return dataUU.neighborWeight(neighbor); }

    virtual void setInterpolated(int time, Interpolation::Quality q, float value);

    virtual Interpolation::SimpleResult getInterpolated(int time)
        { return dataUU.getInterpolated(time); }

private:
    KvalobsNeighborData& dataUU;
    KvalobsNeighborData& dataTA;
};



class KvalobsUUMinMaxData : public Interpolation::MinMaxInterpolator::Data {
public:
    KvalobsUUMinMaxData(KvalobsUUNeighborData& ndata, KvalobsMinMaxData& dUU, KvalobsMinMaxData& dTA);

    virtual Interpolation::SeriesData minimum(int t);
    virtual Interpolation::SeriesData maximum(int t);

    virtual void setMinimum(int time, Interpolation::Quality q, float value);

    virtual void setMaximum(int time, Interpolation::Quality q, float value);

    virtual float fluctuationLevel()
        { return dataUU.fluctuationLevel(); }

private:
    KvalobsMinMaxData& dataUU;
    KvalobsMinMaxData& dataTA;
};

#endif /* KVALOBSDATAUU_H_ */
