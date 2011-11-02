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

#include "FlagUpdater.h"

#include <sstream>
#include <iostream>

FlagUpdater& FlagUpdater::reset()
{
    for(int i=0; i<N_FLAGS; ++i)
        mSet[i] = NO_CHANGE;
    return *this;
}

static int char2int(char c)
{
    return (c>='0' && c<='9') ? (c - '0') : (c - 'A' + 10);
}

bool FlagUpdater::parse(const std::string& flagstring)
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

kvalobs::kvDataFlag FlagUpdater::apply(const kvalobs::kvDataFlag& orig) const
{
    kvalobs::kvDataFlag flag = orig;
    for(int i=0; i<N_FLAGS; ++i) {
        if( mSet[i] != NO_CHANGE )
            flag.set(i, mSet[i]);
    }
    return flag;
}
