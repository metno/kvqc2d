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

namespace Interpolation {

enum Quality { OBSERVATION=0, GOOD=1, BAD=2, FAILED=3 };

class SupportData {
public:
    SupportData()
        : mUsable(false), mValue(-32766.5) { }
    SupportData(float v)
        : mUsable(true), mValue(v) { }

    bool usable() const
        { return mUsable; }

    float value() const
        { return mValue; }

private:
    bool mUsable;
    float mValue;
};

class SeriesData : public SupportData {
public:
    SeriesData()
        : SupportData(), mNeedsInterpolation(true) { }

    SeriesData(float v)
        : SupportData(v), mNeedsInterpolation(false) { }

    explicit SeriesData(const SupportData& sd)
        : SupportData(sd), mNeedsInterpolation(!sd.usable()) { }

    bool needsInterpolation() const
        { return mNeedsInterpolation; }

private:
    bool mNeedsInterpolation;
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

} // namespace Interpolation

#endif /* INTERPOLATIONDATA_H_ */
