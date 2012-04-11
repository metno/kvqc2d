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

#include "FlagPattern.h"

#include <sstream>
#include <iostream>

#include "config.h"
#include "debug.h"

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

FlagPattern& FlagPattern::reset()
{
    for(int i=0; i<N_FLAGS; ++i)
        mPermitted[i] = mForbidden[i] = 0;
    return *this;
}

std::string FlagPattern::sql(const std::string& column, bool needSQLText) const
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
        std::ostringstream sqlColumn;
        sqlColumn << "substr(" << column << "," << i+1 << ",1)";
        const std::string sqlC = sqlColumn.str();
#ifdef HAVE_SQL_WITH_WORKING_SUBSTR_IN
        sql << sqlC;
        const bool negate = nbits > N_VALUES/2;
        if( negate )
            sql << " NOT";
        sql << " IN (";
#else /* !HAVE_SQL_WITH_WORKING_SUBSTR_IN */
        if( nbits>1 )
            sql << '(';
#endif /* !HAVE_SQL_WITH_WORKING_SUBSTR_IN */
        bool first = true;
        for(int v = 0; v<N_VALUES; ++v) {
            const bool needBit = (allowed & (1<<v)) != 0;
#ifdef HAVE_SQL_WITH_WORKING_SUBSTR_IN
            if( (negate && needBit) || (!negate && !needBit) )
                continue;
            if( !first )
                sql << ',';
            sql << "'" << int2char(v) << "'";
#else /* !HAVE_SQL_WITH_WORKING_SUBSTR_IN */
            if( !needBit )
                continue;
            if( !first )
                sql << " OR ";
            sql << sqlC << "='" << int2char(v) << "'";
#endif /* !HAVE_SQL_WITH_WORKING_SUBSTR_IN */
            first = false;
        }
#ifdef HAVE_SQL_WITH_WORKING_SUBSTR_IN
        sql << ')'; // closing the "IN("
#else /* !HAVE_SQL_WITH_WORKING_SUBSTR_IN */
        if( nbits>1 )
            sql << ')';
#endif /* !HAVE_SQL_WITH_WORKING_SUBSTR_IN */
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

bool FlagPattern::matches(const kvalobs::kvDataFlag& flags) const
{
    for(int i=0; i<N_FLAGS; ++i) {
        if( !isAllowed(i, flags.flag(i)) )
            return false;
    }
    return true;
}

static int char2int(char c)
{
    return (c>='0' && c<='9') ? (c - '0') : (c - 'A' + 10);
}

bool FlagPattern::parsePattern(const std::string& flagstring)
{
    reset();

    unsigned int c=0;
    int f=0;
    for(; f<16 && c<flagstring.size(); ++f) {
        char fc = flagstring[c];
        if( fc == '_' || fc == '.' ) {
            // no constraint on flag f
            c += 1;
        } else if( !parsePermittedValues(f, flagstring, c) ) {
            return false;
        }
    }
    if( c != flagstring.size() || f != 16 )
        return false;
    return true;
}

bool FlagPattern::parsePermittedValues(int f, const std::string& flagstring, unsigned int& c)
{
    char fc = flagstring[c++];
    if( fc == '[' || fc == ')' ) {
        // multiple permitted / forbidden flags
        while(c<flagstring.size()) {
            char fcc = flagstring[c++];
            if( (fc == ')' && fcc == '(') || (fc=='[' && fcc==']') ) {
                // end of inclusion/exclusion
                break;
            }
            if( (fcc>='0' && fcc<='9') || (fcc>='A' && fcc<='F') ) {
                if( fc == '[' )
                    permit(f, char2int(fcc));
                else
                    forbid(f, char2int(fcc));
            } else {
                return false;
            }
        }
        return true;
    } else if( (fc>='0' && fc<='9') || (fc>='A' && fc<='F') ) {
        // single permitted flag
        permit(f, char2int(fc));
        return true;
    } else {
        return false;
    }
 }

bool FlagPattern::parseNames(const std::string& flagstring, const char* flagnames[])
{
    reset();

    const bool all = (flagstring == "all" || flagstring == "always");
    const bool none = (flagstring == "none" || flagstring == "never");
    if( all || none ) {
        const unsigned int allBits = (1<<N_VALUES)-1;
        for(int i=0; i<N_FLAGS; ++i)
            (all ? mPermitted : mForbidden)[i] = allBits;
        return true;
    }

    unsigned int start=0, usedflags=0;
    while( start < flagstring.size() ) {
        const size_t equal = flagstring.find('=', start);
        if( equal == std::string::npos )
            return false;
        const std::string flagname = flagstring.substr(start, equal - start);
        int flag = 0;
        for(; flag<N_FLAGS; ++flag) {
            if( flagname == flagnames[flag] )
                break;
        }
        if( flag == N_FLAGS || (usedflags & (1<<flag)) != 0 )
            return false;
        usedflags |= (1<<flag);
        start = equal + 1;
        if( start >= flagstring.size() )
            return false;
        if( !parsePermittedValues(flag, flagstring, start) ) {
            return false;
        }
        if( start < flagstring.size() ) {
            if( flagstring[start++] != '&' )
                return false;
            if( start == flagstring.size() )
                return false;
        }
    }
    return start == flagstring.size();
}

const char* FlagPattern::CONTROLINFO_NAMES[N_FLAGS] = {
    "fqclevel", "fr", "fcc", "fs", "fnum", "fpos", "fmis", "ftime",
    "fw", "fstat", "fcp", "fclim", "fd", "fpre", "fcombi", "fhqc"
};

const char* FlagPattern::USEINFO_NAMES[N_FLAGS] = {
    "U0", "U1", "U2", "U3", "U4", "U5", "U6", "U7",
    "U8", "U9", "U10", "U11", "U12", "U13", "U14", "U15"
};
