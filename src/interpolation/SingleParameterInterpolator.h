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

#ifndef SINGLEPARAMETERINTERPOLATOR_H
#define SINGLEPARAMETERINTERPOLATOR_H 1

#include "InterpolationData.h"

namespace Interpolation {

class SingleParameterInterpolator {
public:
    class Data {
    public:
        virtual ~Data();

        virtual int duration() = 0;

        virtual SeriesData parameter(int time) = 0;

        virtual void setParameter(int time, Quality q, float value) = 0;
    };

};

} // namespace Interpolation

#endif /* SINGLEPARAMETERINTERPOLATOR_H */
