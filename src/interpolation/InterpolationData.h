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

#ifndef INTERPOLATIONDATA_H_
#define INTERPOLATIONDATA_H_

#include <iosfwd>
#include <vector>

namespace Interpolation {

enum Quality { OBSERVATION, GOOD, BAD, FAILED, MISSING, UNUSABLE, NO_ROW };
enum Values { MISSING_VALUE = -32767, INVALID_VALUE=-32765 };

class SupportData {
public:
    SupportData()
        : mUsable(false), mValue(MISSING_VALUE) { }
    SupportData(float v, bool usable=true)
        : mUsable(usable), mValue(v) { }

    bool usable() const
        { return mUsable; }

    float value() const
        { return mValue; }

protected:
    bool mUsable;
    float mValue;
};

class SeriesData : public SupportData {
public:
    SeriesData()
        : SupportData(), mQuality(MISSING) { }

    SeriesData(float v, Quality q, bool usable)
        : SupportData(v, usable), mQuality(q) { }

    bool needsInterpolation() const;

    Quality quality() const
        { return mQuality; }

private:
    Quality mQuality;
};

class Summary {
public:
    Summary(int nOk, int nFailed)
        : mOk(nOk), mFailed(nFailed) { }

    Summary()
        : mOk(0), mFailed(0) { }

    bool failed() const
        { return mFailed > 0; }

    bool ok() const
        { return mFailed == 0; }

    bool partial() const
        { return mFailed>0 and mOk>0; }

    int nFailed() const
        { return mFailed; }

    int nOk() const
        { return mOk; }

    void addFailed()
        { mFailed += 1; }

    void addOk()
        { mOk += 1; }

private:
    int mOk;
    int mFailed;
};

typedef std::vector<SupportData> SupportDataList;
typedef std::vector<SeriesData> SeriesDataList;

} // namespace Interpolation

std::ostream& operator<<(std::ostream& out, const Interpolation::SupportData& s);
std::ostream& operator<<(std::ostream& out, const Interpolation::SeriesData& s);

#endif /* INTERPOLATIONDATA_H_ */
