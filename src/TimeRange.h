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

#ifndef TIMERANGE_H
#define TIMERANGE_H 1

#include "helpers/timeutil.h"
#include <iosfwd>

struct TimeRange {
    kvtime::time t0;
    kvtime::time t1;
    TimeRange(const kvtime::time& T0, const kvtime::time& T1)
        : t0(T0), t1(T1) { }
    int days() const;
    int hours() const;
    TimeRange extendedByHours(int nHours) const
        { TimeRange t(*this); t.extendByHours(nHours); return t; }
    void extendByHours(int nHours);
};

std::ostream& operator<<(std::ostream& out, const TimeRange& tr);

#endif /* TIMERANGE_H */
