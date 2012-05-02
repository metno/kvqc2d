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

#ifndef KVALOBSSINGLEPARAMETERDATA_H_
#define KVALOBSSINGLEPARAMETERDATA_H_

#include "DBInterface.h"
#include "FlagPatterns.h"
#include "Instrument.h"
#include "KvalobsDataList.h"
#include "NeighborInterpolator.h"
#include "ParameterInfo.h"
#include "SimpleInterpolationResult.h"
#include "TimeRange.h"

class KvalobsNeighborData : public Interpolation::NeighborInterpolator::Data {
public:
    KvalobsNeighborData(DBInterface* db, const Instrument& i, const TimeRange& t, const ParameterInfo& pi);

    virtual int duration();

    virtual Interpolation::SeriesData parameter(int time);

    virtual void setInterpolated(int time, Interpolation::Quality q, float value);

    virtual int neighbors();

    virtual float maximumOffset();

    virtual Interpolation::SupportData neighbor(int n, int time);
    virtual Interpolation::SupportData model(int time);
    virtual Interpolation::SupportData transformedNeighbor(int n, int t);
    virtual float neighborWeight(int n);

    DBInterface* getDatabase()
        { return mDB; }

    const Instrument& getInstrument() const
        { return mInstrument; }

    const TimeRange& getTimeRange() const
        { return mTimeRange; }

    miutil::miTime timeAtOffset(int time) const;

    const NeighborData& getNeighborData(int n) const
        { return neighborCorrelations[n]; }

    const Interpolation::SimpleResultVector& getInterpolated()
        { return interpolations; }

    Interpolation::SimpleResult getInterpolated(int time);

    const ParameterInfo& getParameterInfo() const
        { return mParameterInfo; }

private:
    DBInterface* mDB;
    const TimeRange& mTimeRange;
    const Instrument& mInstrument;
    const ParameterInfo& mParameterInfo;

    KvalobsSeriesDataList centerObservations;
    KvalobsSupportModelDataList centerModel;
    bool mFetchedNeighborCorrelations;
    NeighborDataVector neighborCorrelations;
    typedef std::vector<KvalobsSupportDataList> NeighborObservations;
    NeighborObservations neighborObservations;
    FlagSetCU mNeighborFlags;

    Interpolation::SimpleResultVector interpolations;
};

#endif /* KVALOBSSINGLEPARAMETERDATA_H_ */
