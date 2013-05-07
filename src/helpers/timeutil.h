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

#ifndef HELPERS_TIME_H_
#define HELPERS_TIME_H_

#if 1
#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace kvtime {
typedef boost::posix_time::ptime time;
typedef boost::gregorian::date   date;
}
#else
#include <puTools/miTime.h>
namespace kvtime {
typedef miutil::miTime time;
typedef miutil::miDate date;
}
#endif

namespace kvtime {

time maketime(const std::string& time_text);
time maketime(int Year, int Month, int Day, int Hour, int Minute, int Second);
date makedate(int Year, int Month, int Day);
time now();

void addSeconds(time& t, int nSeconds);
void addMinutes(time& t, int nMinutes);
void addHours(time& t, int nHours);
void addDays(time& t, int nDays);
void addDays(date& d, int nDays);

int minDiff(const time& t1, const time& t0);
int hourDiff(const time& t1, const time& t0);

int year  (const time& t);
int month (const time& t);
int day   (const time& t);
int hour  (const time& t);
int minute(const time& t);
int second(const time& t);
int julianDay(const date& t);

std::string iso(const time& t);
std::string iso(const date& d);

} // namespace kvtime

namespace Helpers {

/**
 * Returns the normalised day of the year, that is 28.2. = 29.2. = day
 * 59, 1.3. = 60, ...
 */
int normalisedDayOfYear(const kvtime::date& date);

kvtime::time plusDay(const kvtime::time& t, int nDays);
kvtime::time plusHour(const kvtime::time& t, int nHours);
kvtime::time plusMinute(const kvtime::time& t, int nMinutes);

} // namespace Helpers

#endif /* HELPERS_TIME_H_ */
