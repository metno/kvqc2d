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

#include "FlagChange.h"

bool FlagChange::parse(const std::string& flagstring)
{
    if( flagstring.empty() )
        return true;

    std::size_t lastSeparator = 0;
    while( lastSeparator < flagstring.size() ) {
        std::size_t nextSeparator = flagstring.find(";", lastSeparator);
        if( nextSeparator == std::string::npos )
            nextSeparator = flagstring.size();

        FlagPattern fm;

        std::size_t nextAssign = flagstring.find("->", lastSeparator);
        if( nextAssign == std::string::npos || nextAssign >= nextSeparator ) {
            nextAssign = lastSeparator;
        } else if( nextAssign == lastSeparator ) {
            nextAssign += 2;
        } else {
            if( !fm.parseControlinfo(flagstring.substr(lastSeparator, nextAssign - lastSeparator)) )
                return false;
            nextAssign += 2;
        }

        FlagUpdate fu;
        if( !fu.parse(flagstring.substr(nextAssign, nextSeparator - nextAssign)) )
            return false;

        add(fm, fu);
        lastSeparator = nextSeparator + 1;
    }
    return true;
}

kvalobs::kvControlInfo FlagChange::apply(const kvalobs::kvControlInfo& orig) const
{
    kvalobs::kvControlInfo flag = orig;
    for(unsigned int i=0; i<mMatchers.size(); ++i) {
        if( mMatchers[i].matches(flag) )
            flag = mUpdaters[i].apply(flag);
    }
    return flag;
}
