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

#include "FlagMatcher.h"

#include <sstream>
#include <iostream>

namespace {

char int2char(int i)
{
    if( i<10 )
        return ('0' + i);
    else
        return ('A' + (i-10));
}

int count_bits(unsigned int x)
{
    // origin: http://bytes.com/topic/c/answers/535049-how-many-bits-1-integer-variable
    int bits = 0;
    while( x ) {
        x &= (x-1);
        bits += 1;
    }
    return bits;
}

} // anonymous namespace

FlagMatcher& FlagMatcher::reset()
{
    for(int i=0; i<N_FLAGS; ++i)
        mPermitted[i] = mForbidden[i] = 0;
    return *this;
}

std::string FlagMatcher::sql(const std::string& column, bool needSQLText) const
{
    std::ostringstream sql;
    bool enclose = false;
    for(int i=0; i<N_FLAGS; ++i) {
        const unsigned int allowed = allowedBits(i), nbits = count_bits(allowed);
        if( nbits == 0 )
            return "0=1";
        else if( nbits == N_VALUES )
            continue;
        if( !sql.str().empty() ) {
            sql << " AND ";
            enclose = true;
        }
        sql << "substr(" << column << "," << i+1 << ",1) ";
        const bool negate = nbits > N_VALUES/2;
        if( negate )
            sql << "NOT ";
        sql << "IN (";
        bool first = true;
        for(int v = 0; v<N_VALUES; ++v) {
            const bool needBit = (allowed & (1<<v)) != 0;
            if( (negate && needBit) || (!negate && !needBit) )
                continue;
            if( !first )
                sql << ',';
            sql << "'" << int2char(v) << "'";
            first = false;
        }
        sql << ")";
    }
    const std::string sql1 = sql.str();
    if( sql1.empty() ) {
        if( needSQLText )
            return "0=0";
        else
            return "";
    }
    if( !enclose )
        return sql1;
    return "(" + sql1 + ")";
}

bool FlagMatcher::matches(const kvalobs::kvDataFlag& flags) const
{
    for(int i=0; i<N_FLAGS; ++i) {
        if( !isAllowed(i, flags.flag(i)) )
            return false;
    }
    return true;
}
