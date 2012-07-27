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

#ifndef DataUpdate_H
#define DataUpdate_H 1

#include <kvalobs/kvData.h>
#include <kvalobs/kvDataFlag.h>
#include <puTools/miTime.h>
#include <iosfwd>

// ########################################################################

/**
 * \brief Wrapper around kvalobs::kvData to remember any changes.
 *
 * Also automates updating useinfo after a change in controlinfo before write,
 * and adding a ',' to cfailed if it was not empty before appending.
 */
class DataUpdate {
private:
    enum eForced { FORCED_NOTHING = 0,
                   FORCED_WRITE = 1,
                   FORCED_NOWRITE = 2 };

public:
    DataUpdate();

    DataUpdate(const kvalobs::kvData& data);

    DataUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
               float original, float corrected, const std::string& controlinfo);

    /**
     * \brief Determine whether writing to database is necessary.
     *
     * The return value can be forced to true (with forceWrite()) or false (with
     * forceNoWrite()).
     *
     * Otherwise, this will disregard changes in cfailed and check for
     * value changes in controlinfo or corrected -- return will be true if
     * controlinfo or corrected have actually changed.
     */
    bool needsWrite() const;

    bool isNew() const
        { return mNew; }

    /// \return the 'original' data before any modification
    kvalobs::kvData& data()
        { return mData; }

    /// \return the 'original' data before any modification
    const kvalobs::kvData& data() const
        { return mData; }

    float original() const
        { return mData.original(); }

    /// \return the possibly modified 'corrected' observation value
    float corrected() const
        { return mData.corrected(); }

    miutil::miTime obstime() const
        { return mData.obstime(); }

    kvalobs::kvControlInfo controlinfo() const
        { return mData.controlinfo(); }

    /// \brief Set a new 'corrected' observation value.
    DataUpdate& corrected(float c)
        { mData.corrected(c); return *this; }

    DataUpdate& controlinfo(const kvalobs::kvControlInfo& ci);

    DataUpdate& cfailed(const std::string& cf, const std::string& extra="");

    DataUpdate& forceWrite()
        { mForced = FORCED_WRITE; return *this; }

    DataUpdate& forceNoWrite()
        { mForced = FORCED_NOWRITE; return *this; }

    bool operator<(const DataUpdate& other) const
        { return obstime() < other.obstime(); }

    /**
     * \brief Format data as text.
     *
     * \param hoursBefore if obstime should be returned with BETWEEN
     * \param modified whether to include modified values
     * \return text, partly suitable for copy&paste to SQL
     */
    std::string text(int hoursBefore=0, bool modified=true) const;

    /// \see text(int,bool)
    std::string text(const miutil::miTime& start, bool modified=true) const;

private:
    kvalobs::kvData mData;
    bool mNew;
    eForced mForced;
    kvalobs::kvControlInfo mOrigControl;
    float mOrigCorrected;
    std::string mOrigCfailed;
};

std::ostream& operator<<(std::ostream& out, const DataUpdate& du);

// ########################################################################

#endif /* DataUpdate */
