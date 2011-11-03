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

#ifndef FLAGSET_H_
#define FLAGSET_H_

#include "FlagMatcher.h"
#include <kvalobs/kvData.h>
#include <string>
#include <vector>

class FlagSet {
public:
    FlagSet()
        : mError(false), mDefaultIfEmpty(true) { }

    FlagSet(const FlagMatcher& fm)
        : mMatchers(1, fm), mError(false), mDefaultIfEmpty(true) { }

    FlagSet(const std::string& fs)
        : mError(false), mDefaultIfEmpty(true) { parse(fs); }

    FlagSet& add(const FlagMatcher& fm)
        { mMatchers.push_back(fm); return *this; }

    bool matches(const kvalobs::kvDataFlag& flags) const;

    bool isEmpty() const
        { return mMatchers.empty(); }

    FlagSet& setError()
        { mError = true; return *this; }

    bool hasError() const
        { return mError; }

    FlagSet& setDefaultIfEmpty(bool d)
        { mDefaultIfEmpty = d; return *this; }

    FlagSet& reset()
        { mMatchers.clear(); mError = false; return *this; }

    bool parse(const std::string& flagstring);

    std::string sql(const std::string& column) const;

private:
    std::vector<FlagMatcher> mMatchers;
    bool mError;
    bool mDefaultIfEmpty;
};

class FlagSetCU {
public:
    FlagSetCU& setC(const FlagSet& controlflags)
        { mControlflags = controlflags; return *this; }

    FlagSetCU& setU(const FlagSet& useflags)
        { mUseflags = useflags; return *this; }

    FlagSet& controlflags()
        { return mControlflags; }

    FlagSet& useflags()
        { return mUseflags; }

    const FlagSet& controlflags() const
        { return mControlflags; }

    const FlagSet& useflags() const
        { return mUseflags; }

    bool hasError() const
        { return mControlflags.hasError() || mUseflags.hasError(); }

    FlagSetCU& reset()
        { mControlflags.reset(); mUseflags.reset(); return *this; }

    bool matches(const kvalobs::kvData& data) const
        { return matches(data.controlinfo(), data.useinfo()); }

    bool matches(const kvalobs::kvControlInfo& c, const kvalobs::kvUseInfo& u) const
        { return mControlflags.matches(c) && mUseflags.matches(u); }

    std::string sql() const;

private:
    FlagSet mControlflags;
    FlagSet mUseflags;
};

#endif /* FLAGSET_H_ */
