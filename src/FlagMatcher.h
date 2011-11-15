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

#ifndef FLAGMATCH_H_
#define FLAGMATCH_H_

#include <kvalobs/kvDataFlag.h>
#include <string>

class FlagMatcher {

public:
    enum { N_FLAGS = 16, N_VALUES = 16 };
    enum FlagType { USEINFO, CONTROLINFO };

    FlagMatcher()
        { reset(); }

    FlagMatcher(const std::string& flagstring, FlagType type)
        { parse(flagstring, type); }

    FlagMatcher& permit(int flag, int value)
        { mPermitted[flag] |= (1<<value); return *this; }

    FlagMatcher& forbid(int flag, int value)
        { mForbidden[flag] |= (1<<value); return *this; }

    bool isPermitted(int flag, int value) const
        { const unsigned int r = mPermitted[flag], bit = 1<<value; return r != 0 && (r & bit) != 0; }

    bool isForbidden(int flag, int value) const
        { const unsigned int e = mForbidden[flag], bit = 1<<value; return e != 0 && (e & bit) != 0; }

    bool isAllowed(int flag, int value) const
        { return (allowedBits(flag) & (1<<value)) != 0; }

    FlagMatcher& reset();

    std::string sql(const std::string& column, bool needSQLText=false) const;

    std::string sql(const char* column) const
        { return sql(std::string(column), false); }

    std::string sql(bool needSQLText=false) const
        { return sql("controlinfo", needSQLText); }

    bool matches(const kvalobs::kvDataFlag& flags) const;

    bool parseControlinfo(const std::string& flagstring)
        { return parseNames(flagstring, CONTROLINFO_NAMES) || parsePattern(flagstring); }
    bool parseUseinfo(const std::string& flagstring)
        { return parseNames(flagstring, USEINFO_NAMES) || parsePattern(flagstring); }
    bool parse(const std::string& flagstring, FlagType type)
        { return type == CONTROLINFO ? parseControlinfo(flagstring) : parseUseinfo(flagstring); }

    static const char* CONTROLINFO_NAMES[N_FLAGS];
    static const char* USEINFO_NAMES[N_FLAGS];

private:
    unsigned int allowedBits(int flag) const
        { const unsigned int p = mPermitted[flag]; return (p ? p : (1<<N_VALUES)-1) & ~mForbidden[flag]; }

    bool parsePattern(const std::string& flagstring);
    bool parsePermittedValues(int flag, const std::string& flagstring, unsigned int& pos);
    bool parseNames(const std::string& flagstring, const char* flagnames[]);
    
    unsigned int mPermitted[N_FLAGS];
    unsigned int mForbidden[N_FLAGS];
};


#endif /* FLAGMATCH_H_ */
