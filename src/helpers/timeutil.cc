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

#include "timeutil.h"

namespace Helpers {

int normalisedDayOfYear(const miutil::miDate& date)
{
    // February 29 is the same as February 28
    const int daysFromPreviousMonths[12] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };
    int day = date.day(), month = date.month();
    if( month == 2 && day == 29 )
        day = 28;
    return daysFromPreviousMonths[month-1] + day;
}

miutil::miTime plusDay(const miutil::miTime& t, int nDays)
{
    miutil::miTime p(t);
    p.addDay(nDays);
    return p;
}

miutil::miTime plusHour(const miutil::miTime& t, int nHours)
{
    miutil::miTime p(t);
    p.addHour(nHours);
    return p;
}

miutil::miTime plusMinute(const miutil::miTime& t, int nMinutes)
{
    miutil::miTime p(t);
    p.addMin(nMinutes);
    return p;
}

} // namespace Helpers
