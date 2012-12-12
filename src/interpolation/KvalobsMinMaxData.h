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

#ifndef KVALOBSMINMAXDATA_H_
#define KVALOBSMINMAXDATA_H_

#include "GapData.hh"
#include "MinMaxInterpolator.h"
#include "MinMaxReconstruction.h"

class KvalobsMinMaxInterpolatorData : public Interpolation::MinMaxInterpolator::Data {
public:
    KvalobsMinMaxInterpolatorData(GapData& data)
        : mData(data) { }

    virtual int duration()
        { return mData.duration(); }

    virtual Interpolation::SeriesData parameter(int time)
        { return mData.parameter(time); }

    virtual void setParameter(int time, Interpolation::Quality q, float value)
        { mData.setParameter(time, q, value); }

    virtual Interpolation::SupportData minimum(int time)
        { return mData.minimum(time); }

    virtual Interpolation::SupportData maximum(int time)
        { return mData.maximum(time); }

private:
    GapData& mData;
};

// ========================================================================

class KvalobsMinMaxReconstructionData : public Interpolation::MinMaxReconstruction::Data {
public:
    KvalobsMinMaxReconstructionData(GapData& data)
        : mData(data) { }

    virtual int duration()
        { return mData.duration(); }

    virtual float fluctuationLevel();

    virtual Interpolation::SupportData parameter(int time);

    virtual Interpolation::SeriesData minimum(int time)
        { return mData.minimum(time); }

    virtual Interpolation::SeriesData maximum(int time)
        { return mData.maximum(time); }

    virtual void setMinimum(int time, Interpolation::Quality q, float value)
        { mData.setMinimum(time, q, value); }

    virtual void setMaximum(int time, Interpolation::Quality q, float value)
        { mData.setMaximum(time, q, value); }

private:
    GapData& mData;
    float mFluctuationLevel;
};

#endif /* KVALOBSMINMAXDATA_H_ */
