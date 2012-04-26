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

#ifndef MINMAXINTERPOLATOR_H
#define MINMAXINTERPOLATOR_H 1

#include "SingleParameterInterpolator.h"

namespace Interpolation {

class MinMaxInterpolator {
public:
    class Data {
    public:
        Data(SingleParameterInterpolator::Data& centerData)
                : mCenterData(centerData) { }

        virtual ~Data() { }

        SingleParameterInterpolator::Data& centerData()
            { return mCenterData; }

        const SingleParameterInterpolator::Data& centerData() const
            { return mCenterData; }

        virtual int duration()
            { return mCenterData.duration(); }

        virtual Interpolation::SeriesData parameter(int time)
            { return mCenterData.parameter(time); }

        virtual void setInterpolated(int time, Quality q, float value)
            { mCenterData.setInterpolated(time, q, value); }

        virtual SeriesData minimum(int t) = 0;
        virtual SeriesData maximum(int t) = 0;
        virtual void setMinimum(int time, Quality q, float value) = 0;
        virtual void setMaximum(int time, Quality q, float value) = 0;
        virtual float fluctuationLevel() = 0;

    private:
        SingleParameterInterpolator::Data& mCenterData;
    };

    Summary interpolate(Data& data, SingleParameterInterpolator& spi);
};

} // namespace Interpolation

#endif /* MINMAXINTERPOLATOR_H */
