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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

namespace b_pt = boost::posix_time;

namespace kvtime {

time maketime(const std::string& st)
{
    if( st.size() > 10 && st[10] == 'T' ) {
        std::string t = st;
        t[10] = ' ';
        return b_pt::time_from_string(t);
    } else {
        return b_pt::time_from_string(st);
    }
}

time maketime(int year, int month, int day, int hour, int minute, int second)
{
    return time(boost::gregorian::date(year, month, day), b_pt::time_duration(hour, minute, second));
}

date makedate(int year, int month, int day)
{
    return boost::gregorian::date(year, month, day);
}

time now()
{
    return b_pt::second_clock::universal_time();
}

void addSeconds(time& t, int nSeconds)
{
    t += b_pt::seconds(nSeconds);
}

void addMinutes(time& t, int nMinutes)
{
    t += b_pt::minutes(nMinutes);
}

void addHours(time& t, int nHours)
{
    t += b_pt::hours(nHours);
}

void addDays(time& t, int nDays)
{
    t += boost::gregorian::days(nDays);
}

void addDays(date& d, int nDays)
{
    d += boost::gregorian::days(nDays);
}

int minDiff(const time& t1, const time& t0)
{
    return (t1 - t0).total_seconds() / 60;
}

int hourDiff(const time& t1, const time& t0)
{
    // TODO this is subtly different from miTime::hourDiff: difference
    // between (18:00, 17:45) is 0 hours here and 1 hour for miTime
    return (t1 - t0).hours();
}

int year(const time& t)
{
    return t.date().year();
}

int month(const time& t)
{
    return t.date().month();
}

int day(const time& t)
{
    return t.date().day();
}

int hour(const time& t)
{
    return t.time_of_day().hours();
}

int minute(const time& t)
{
    return t.time_of_day().minutes();
}

int second(const time& t)
{
    return t.time_of_day().seconds();
}

int julianDay(const date& d)
{
    return d.julian_day();
}

std::string iso(const time& t)
{
    std::string ts = b_pt::to_iso_extended_string(t);
    if( ts.size() > 10 && ts[10] == 'T' )
        ts[10] = ' ';
    return ts;
}

std::string iso(const date& pd)
{
    const int y = pd.year(), m = pd.month(), d = pd.day();
    return (boost::format("%1$04d-%2$02d-%3$02d") % y % m % d).str();
}

} // namespace kvtime

namespace Helpers {

int normalisedDayOfYear(const kvtime::date& date)
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

kvtime::time plusDay(const kvtime::time& t, int nDays)
{
    kvtime::time p(t);
    kvtime::addDays(p, nDays);
    return p;
}

kvtime::time plusHour(const kvtime::time& t, int nHours)
{
    kvtime::time p(t);
    kvtime::addHours(p, nHours);
    return p;
}

kvtime::time plusMinute(const kvtime::time& t, int nMinutes)
{
    kvtime::time p(t);
    kvtime::addMinutes(p, nMinutes);
    return p;
}

} // namespace Helpers
