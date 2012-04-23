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

#include "KvalobsData.h"
#include "SimpleData.h"

class KvalobsDataUU : public KvalobsData {
public:
    KvalobsDataUU(DBInterface* db, const Instrument& i, const TimeRange& t);

    virtual NeighborInterpolation::SeriesData center(int time);

    virtual NeighborInterpolation::SupportData model(int time);

    virtual NeighborInterpolation::SupportData neighbor(int n, int time);

    virtual void setInterpolated(int time, NeighborInterpolation::Quality q, float value)
    { interpolations.push_back(NeighborInterpolation::Result(time, q, value)); }

private:
    DBInterface::DataList centerObservationsTA;
    std::vector<DBInterface::DataList> neighborObservationsTA;
    NeighborInterpolation::SimpleData::InterpolationVector interpolations;
};

class KvalobsDataUU2 : public NeighborInterpolation::Data {
public:
    KvalobsDataUU2(KvalobsData& dUU, KvalobsData& dTA);

    virtual NeighborInterpolation::SeriesData center(int time);

    virtual NeighborInterpolation::SupportData model(int time);

    virtual NeighborInterpolation::SupportData transformedNeighbor(int n, int time);

    virtual void setInterpolated(int time, NeighborInterpolation::Quality q, float value)
    { interpolations.push_back(NeighborInterpolation::Result(time, q, value)); }

private:
    KvalobsData& dataUU;
    KvalobsData& dataTA;
    NeighborInterpolation::SimpleData::InterpolationVector interpolations;
};

#endif /* KVALOBSDATAUU_H_ */
