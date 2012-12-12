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

#ifndef NEIGHBORINTERPOLATION_H_
#define NEIGHBORINTERPOLATION_H_

#include "SingleParameterInterpolator.h"

namespace Interpolation {

class NeighborInterpolator {
public:
    class Data : public SingleParameterInterpolator::Data {
    public:
        virtual int neighborCount() = 0;

        virtual float maximumOffset() = 0;

        virtual SupportData model(int time) = 0;

        virtual SupportData transformedNeighbor(int n, int time) = 0;

        virtual float neighborWeight(int neighbor) = 0;
    };

    /** amount of extra data required before and after the gap for Akima interpolation. */
    static const int EXTRA_DATA;

    Interpolation::Summary run(Data& data);

};

} // namespace Interpolation

#endif /* NEIGHBORINTERPOLATION_H_ */
