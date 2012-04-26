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

#include "FlagPatterns.h"

bool FlagPatterns::matches(const kvalobs::kvDataFlag& flags) const
{
    if( mError )
        return false;
    if( mMatchers.empty() )
        return mDefaultIfEmpty;
    for(unsigned int i=0; i<mMatchers.size(); ++i)
        if( mMatchers[i].matches(flags) )
            return true;
    return false;
}

bool FlagPatterns::parse(const std::string& flagstring, FlagPattern::FlagType type)
{
    if( flagstring.empty() )
        return true;

    std::size_t lastSeparator = 0;
    while( lastSeparator < flagstring.size() ) {
        std::size_t nextSeparator = flagstring.find("|", lastSeparator);
        if( nextSeparator == std::string::npos )
            nextSeparator = flagstring.size();
        FlagPattern fm;
        if( !fm.parse(flagstring.substr(lastSeparator, nextSeparator - lastSeparator), type) ) {
            mError = true;
            return false;
        }
        add(fm);
        lastSeparator = nextSeparator + 1;
    }
    return true;
}

std::string FlagPatterns::sql(const std::string& column) const
{
    if( mError )
        return "0=1";
    if( mMatchers.empty() )
        return mDefaultIfEmpty ? "" : "0=1";

    std::ostringstream sql;
    if( mMatchers.size()>1 )
        sql << '(';
    sql << mMatchers.front().sql(column);
    for(unsigned int i=1; i<mMatchers.size(); ++i)
        sql << " OR " << mMatchers[i].sql(column);
    if( mMatchers.size()>1 )
        sql << ')';
    return sql.str();
}

std::string FlagSetCU::sql() const
{
    const std::string sqlC = mControlflags.sql("controlinfo"), sqlU = mUseflags.sql("useinfo");
    const bool emptyC = sqlC.empty(), emptyU = sqlU.empty();
    if( emptyC && emptyU )
        return "0=0";
    if( emptyC )
        return sqlU;
    if( emptyU )
        return sqlC;
    return "(" + sqlC + " AND " + sqlU + ")";
}
