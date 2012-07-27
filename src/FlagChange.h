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

#ifndef FLAGCHANGE_H_
#define FLAGCHANGE_H_

#include "FlagPattern.h"
#include "FlagUpdate.h"
#include <string>
#include <vector>

/**
 * \brief Update kvalobs controlinfo flags using \ref FlagPattern s and \ref FlagUpdate s specifying what to change.
 *
 * Patterns and updates may be specified as text as documented for parse().
 */
class FlagChange {
public:
    /**
     * \brief Construct without pattern.
     */
    FlagChange()
        { }

    /**
     * \brief Construct with a single pattern and update.
     */
    FlagChange(const FlagPattern& fm, const FlagUpdate& fu)
        : mMatchers(1, fm), mUpdaters(1, fu) { }

    /**
     * \brief Construct from a text specification.
     *
     * \see parse
     */
    FlagChange(const std::string& fs)
        { parse(fs); }

    FlagChange& add(const FlagPattern& fm, const FlagUpdate& fu)
        { mMatchers.push_back(fm); mUpdaters.push_back(fu); return *this; }

    /**
     * \brief Apply changes and return modified controlinfo.
     *
     * Starts with the original controlinfo and walks through the list of
     * patterns in the order they were specified / added. If a pattern matches,
     * the corresponding update is applied and matching/updating continues with
     * the modified pattern.
     *
     * Example: if parsed from `fmis=3->fmis=E;fmis=E->fmis=1`, apply on a
     * controlinfo with fmis=3 or one with fmis=E will in both cases yield
     * a controlinfo with fmis=1 and all other flags unchanged.
     *
     * @return the modified controlinfo structure
     */
    kvalobs::kvControlInfo apply(const kvalobs::kvControlInfo& orig) const;

    bool isEmpty() const
        { return mUpdaters.empty(); }

    /**
     * Forget all patterns and updates.
     */
    FlagChange& reset()
        { mMatchers.clear(); mUpdaters.clear(); return *this; }

    /**
     * \brief Parse an update specification.
     *
     * Format:
     *
     * <pre>
     *     FlagPattern '->' FlagUpdate (';' FlagPattern '->' FlagUpdate)*
     * </pre>
     *
     * @return true if flagstring could be parsed completely without errors
     */
    bool parse(const std::string& flagstring);

private:
    std::vector<FlagPattern> mMatchers;
    std::vector<FlagUpdate> mUpdaters;
};

#endif /* FLAGCHANGE_H_ */
