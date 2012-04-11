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

#include "FlagUpdate.h"

#include "FlagPattern.h"
#include <sstream>
#include <iostream>

FlagUpdate& FlagUpdate::reset()
{
    for(int i=0; i<N_FLAGS; ++i)
        mSet[i] = NO_CHANGE;
    return *this;
}

static int char2int(char c)
{
    return (c>='0' && c<='9') ? (c - '0') : (c - 'A' + 10);
}

bool FlagUpdate::parseNames(const std::string& flagstring)
{
    reset();

    unsigned int start=0, usedflags=0;
    while( start < flagstring.size() ) {
        const size_t equal = flagstring.find('=', start);
        if( equal == std::string::npos )
            return false;
        const std::string flagname = flagstring.substr(start, equal - start);
        int flag = 0;
        for(; flag<N_FLAGS; ++flag) {
            if( flagname == FlagPattern::CONTROLINFO_NAMES[flag] )
                break;
        }
        if( flag == N_FLAGS || (usedflags & (1<<flag)) != 0 )
            return false;
        usedflags |= (1<<flag);
        start = equal + 1;
        if( start >= flagstring.size() )
            return false;

        char fc = flagstring[start++];
        if( (fc>='0' && fc<='9') || (fc>='A' && fc<='F') ) {
            mSet[flag] = char2int(fc);
        } else {
            return false;
        }

        if( start < flagstring.size() ) {
            if( flagstring[start++] != ',' )
                break;
            if( start == flagstring.size() )
                return false;
        }
    }
    return start == flagstring.size();
}

bool FlagUpdate::parsePattern(const std::string& flagstring)
{
    reset();

    unsigned int c=0, f=0;
    for(; f<16 && c<flagstring.size(); ++f) {
        char fc = flagstring[c++];
        if( fc == '_' || fc == '.' ) {
            // no update on flag f
        } else if( (fc>='0' && fc<='9') || (fc>='A' && fc<='F') ) {
            // update flag f to fc
            mSet[f] = char2int(fc);
        } else {
            return false;
        }
    }
    if( c != flagstring.size() || f != 16 )
        return false;
    return true;
}

kvalobs::kvControlInfo FlagUpdate::apply(const kvalobs::kvControlInfo& orig) const
{
    kvalobs::kvControlInfo flag = orig;
    for(int i=0; i<N_FLAGS; ++i) {
        if( mSet[i] != NO_CHANGE )
            flag.set(i, mSet[i]);
    }
    return flag;
}
