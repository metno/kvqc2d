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

#ifndef FLAGUPDATER_H_
#define FLAGUPDATER_H_

#include <kvalobs/kvDataFlag.h>
#include <string>

class FlagUpdate {

public:
    enum { N_FLAGS = 16, NO_CHANGE = -1 };

    FlagUpdate()
        { reset(); }

    FlagUpdate(const std::string& flagstring)
        { parse(flagstring); }

    FlagUpdate& set(int flag, int value)
        { mSet[flag] = value; return *this; }

    int get(int flag) const
        { return mSet[flag]; }

    FlagUpdate& reset();

    bool parse(const std::string& flagstring)
        { return parseNames(flagstring) || parsePattern(flagstring); }

    bool parsePattern(const std::string& flagstring);

    bool parseNames(const std::string& flagstring);

    kvalobs::kvControlInfo apply(const kvalobs::kvControlInfo& flag) const;

private:
    int mSet[N_FLAGS];
};


#endif /* FLAGUPDATER_H_ */
