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

#include "FlagMatch.h"

#include <sstream>
#include <iostream>

static char int2char(int i)
{
    if( i<10 )
        return ('0' + i);
    else
        return ('A' + (i-10));
}

FlagMatch& FlagMatch::reset()
{
    for(int i=0; i<N_FLAGS; ++i)
        mRequired[i] = mExcluded[i] = 0;
    return *this;
}

std::string FlagMatch::sql(const std::string& column, bool needSQLText) const
{
    std::ostringstream sql;
    for(int i=0; i<N_FLAGS; ++i) {
        const unsigned int r = mRequired[i], e = mExcluded[i];
        if( r == 0 && e == 0 )
            continue;
        if( !sql.str().empty() )
            sql << " AND ";
        sql << "substr(" << column << "," << i+1 << ",1) IN (";
        bool first = true;
        for(int v = 0; v<N_VALUES; ++v) {
            const unsigned int bit = (1<<v);
            if( (r == 0 || ( r & bit ) == 0) && (e == 0 || ( e & bit ) != 0) )
                continue;
            if( !first )
                sql << ',';
            sql << "'" << int2char(v) << "'";
            first = false;
        }
        sql << ")";
    }
    if( needSQLText && sql.str().empty() )
        sql << "0=0";
    return sql.str();
}

bool FlagMatch::matches(const kvalobs::kvDataFlag& flags) const
{
    for(int i=0; i<N_FLAGS; ++i) {
        const unsigned int f = flags.flag(i), bit = (1<<f), r = mRequired[i], e = mExcluded[i];
        if( r != 0 && ( r & bit ) == 0 )
            return false;
        if( e != 0 && ( e & bit ) != 0 )
            return false;
    }
    return true;
}

