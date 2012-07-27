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

#ifndef FLAGSET_H_
#define FLAGSET_H_

#include "FlagPattern.h"
#include <kvalobs/kvData.h>
#include <string>
#include <vector>

/**
 * \brief An 'or' combination of FlagPattern s.
 */
class FlagPatterns {
public:
    FlagPatterns()
        : mError(false), mDefaultIfEmpty(true) { }

    FlagPatterns(const FlagPattern& fm)
        : mMatchers(1, fm), mError(false), mDefaultIfEmpty(true) { }

    FlagPatterns(const std::string& fs, FlagPattern::FlagType type)
        : mError(false), mDefaultIfEmpty(true) { parse(fs, type); }

    FlagPatterns& add(const FlagPattern& fm)
        { mMatchers.push_back(fm); return *this; }

    /// \return true if any of the FlagPattern s matched
    bool matches(const kvalobs::kvDataFlag& flags) const;

    bool isEmpty() const
        { return mMatchers.empty(); }

    FlagPatterns& setError()
        { mError = true; return *this; }

    bool hasError() const
        { return mError; }

    FlagPatterns& setDefaultIfEmpty(bool d)
        { mDefaultIfEmpty = d; return *this; }

    FlagPatterns& reset()
        { mMatchers.clear(); mError = false; return *this; }

    /**
     * \brief Parse alternative patterns from text.
     *
     * \param flagstring is split at '|' and each part is parsed as a FlagPattern
     * \return if parsing was successful
     */
    bool parse(const std::string& flagstring, FlagPattern::FlagType type);

    std::string sql(const std::string& column) const;

private:
    std::vector<FlagPattern> mMatchers;
    bool mError;
    bool mDefaultIfEmpty;
};

/**
 * \brief Two FlagPatterns, one for controlinfo and one for useinfo flags.
 */
class FlagSetCU {
public:
    FlagSetCU& setC(const FlagPatterns& controlflags)
        { mControlflags = controlflags; return *this; }

    FlagSetCU& setU(const FlagPatterns& useflags)
        { mUseflags = useflags; return *this; }

    FlagPatterns& controlflags()
        { return mControlflags; }

    FlagPatterns& useflags()
        { return mUseflags; }

    const FlagPatterns& controlflags() const
        { return mControlflags; }

    const FlagPatterns& useflags() const
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
    FlagPatterns mControlflags;
    FlagPatterns mUseflags;
};

#endif /* FLAGSET_H_ */
