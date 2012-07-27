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

#ifndef FLAGMATCH_H_
#define FLAGMATCH_H_

#include <kvalobs/kvDataFlag.h>
#include <string>

/**
 * \brief A pattern for matching kvalobs controlinfo flags.
 */
class FlagPattern {

public:
    enum { N_FLAGS = 16, N_VALUES = 16 };
    enum FlagType { USEINFO, CONTROLINFO };

    FlagPattern()
        { reset(); }

    FlagPattern(const std::string& flagstring, FlagType type)
        { reset(); parse(flagstring, type); }

    /// \brief Match a controlinfo if flag equals value.
    FlagPattern& permit(int flag, int value)
        { mPermitted[flag] |= (1<<value); return *this; }

    /// \brief Do not match a controlinfo if flag equals value.
    FlagPattern& forbid(int flag, int value)
        { mForbidden[flag] |= (1<<value); return *this; }

    bool isPermitted(int flag, int value) const
        { const unsigned int r = mPermitted[flag], bit = 1<<value; return r != 0 && (r & bit) != 0; }

    bool isForbidden(int flag, int value) const
        { const unsigned int e = mForbidden[flag], bit = 1<<value; return e != 0 && (e & bit) != 0; }

    /**
     * \brief Check if a controlinfo flag with the specified value should be accepted.
     *
     * If some values are specifically permitted for this flag, it will check if
     * value is amongst them. Otherwise it will check if this value is not
     * explicitly forbidden.
     */
    bool isAllowed(int flag, int value) const
        { return (allowedBits(flag) & (1<<value)) != 0; }

    FlagPattern& reset();

    /**
     * \brief Create a SQL constraint for the kvalobs database.
     *
     * \param column the column to match in the kvalobs database
     * \param needSQLText if true, the SQL constraint will not be empty even if
     *                    the pattern matches all possible flag combinations.
     */
    std::string sql(const std::string& column, bool needSQLText=false) const;

    std::string sql(const char* column) const
        { return sql(std::string(column), false); }

    /// \brief Create a SQL constraint for the controlinfo column.
    std::string sql(bool needSQLText=false) const
        { return sql("controlinfo", needSQLText); }

    /// \brief Match against the specified kvalobs controlinfo or useinfo.
    bool matches(const kvalobs::kvDataFlag& flags) const;

    /// \brief Tries parseNames() for controlinfo first, if that fails parsePattern().
    bool parseControlinfo(const std::string& flagstring)
        { return parseNames(flagstring, CONTROLINFO_NAMES) || parsePattern(flagstring); }

    /// \brief Tries parseNames() for useinfo first, if that fails parsePattern().
    bool parseUseinfo(const std::string& flagstring)
        { return parseNames(flagstring, USEINFO_NAMES) || parsePattern(flagstring); }

    /**
     * \brief Parse a flag pattern.
     *
     * A pattern specification by name is 'all' or 'always' for explicitly
     * allowing all flag combinations, 'none' or 'never' for explicitly
     * forbidding all flag combinations, or:
     *
     * <pre>
     *     text_pattern := 'all' | 'always' | 'none' | 'never' | name_pattern
     *     name_pattern := single_flag ( '&' single_flag )+
     *     single_flag := flag '=' ( value | ( '[' value + ']' ) | ')' value+ '(' )
     * </pre>
     *
     * where <code>[045]</code> allows any of 0, 4, and 5 and <code>)137(</code>
     * forbids any of 1, 3, and 7. Each flag name may appear only once, but the
     * ordering is not important.
     *
     * A pattern specification by value is
     * <pre>
     *     value_pattern := ( any_value | single_flag ){16}
     *     any_value := '_' | '.'
     * </pre>
     *
     * \param type whether to parse for useinfo or controlinfo
     * \return true if either parsing by name or by pattern was successful
     */
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
