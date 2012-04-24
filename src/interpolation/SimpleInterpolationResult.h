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

#ifndef SIMPLEINTERPOLATIONRESULT_H_
#define SIMPLEINTERPOLATIONRESULT_H_

#include "InterpolationData.h"
#include <vector>

namespace Interpolation {

struct SimpleResult {
    int time;
    Quality quality;
    float value;

    SimpleResult(int t) : time(t), quality(FAILED), value(-32767) { }
    SimpleResult(int t, Quality q, float v) : time(t), quality(q), value(v) { }
};

typedef std::vector<SimpleResult> SimpleResultVector;

} // namespace Interpolation

#endif /* SIMPLEINTERPOLATIONRESULT_H_ */
