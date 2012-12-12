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
    class Data : public SingleParameterInterpolator::Data {
    public:
        virtual SupportData minimum(int t) = 0;
        virtual SupportData maximum(int t) = 0;
    };

    Summary run(Data& data);

private:
    Summary interpolateFromMinMax(Data& data);
};

} // namespace Interpolation

#endif /* MINMAXINTERPOLATOR_H */
