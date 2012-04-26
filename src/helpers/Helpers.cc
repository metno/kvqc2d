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

#include "Helpers.h"

#include "mathutil.h"
#include <kvalobs/kvDataOperations.h>

#define NDEBUG 1
#include "debug.h"

namespace Helpers {

int hexCharToInt(char n)
{
    if (n >= '0' && n <= '9')
        return (n-'0');
    if (n >= 'A' && n <= 'F')
        return (n-'A'+10);
    // if (n >= 'a' && n <= 'f')
    //    return (n-'a'+10);
    return 0;
}

bool fillMapFromList(const std::string& list, std::map<int, float>& map, const char separator)
{
    map.clear();
    bool error = false;
    std::istringstream parse(list);
    while( parse ) {
        int key = 0;
        float value = 0;
        parse >> key >> value;
        if( !parse ) {
            error = true;
            break;
        }
        map[key] = value;
        char sep = ' ';
        parse >> sep;
        if( parse.eof() ) {
            break;
        } else if( sep != separator ) {
            error = true;
            break;
        }
    }
    if( error )
        map.clear();
    return !error;
}

// ------------------------------------------------------------------------

bool isMissingOrRejected(const kvalobs::kvData& data)
{
    return equal(data.original(), -32767)
        || equal(data.original(), -32766)
        || kvalobs::missing(data) || kvalobs::rejected(data);
}

// ------------------------------------------------------------------------

std::ostream& digits1(std::ostream& out)
{
    out << std::setiosflags(std::ios::fixed) << std::setprecision(1);
    return out;
}

// ------------------------------------------------------------------------

std::string datatext(const kvalobs::kvData& data, int hoursBefore)
{
    miutil::miTime start = data.obstime();
    if( hoursBefore > 0 )
        start.addHour( -hoursBefore );
    return datatext(data, start);
}

// ------------------------------------------------------------------------

std::string datatext(const kvalobs::kvData& data, const miutil::miTime& start)
{
    std::ostringstream out;
    out << "[stationid=" << data.stationID() << " AND ";
    if( start >= data.obstime() ) {
        out << "obstime='" << data.obstime().isoTime() << '\'';
    } else {
        out << "obstime BETWEEN '" << start.isoTime() << "' AND '" << data.obstime().isoTime() << '\'';
    }
    out << " AND paramid="  << data.paramID()
        << " AND typeid="   << data.typeID()
        << " AND sensor='"  << data.sensor() << "'"
        << " AND level="    << data.level()
        << "; original=" << digits1 << data.original()
        << " corr="        << digits1 << data.corrected()
        << " controlinfo="  << data.controlinfo().flagstring()
        << " cfailed='"     << data.cfailed() << "']";
    return out.str();
}

} // namespace Helpers
