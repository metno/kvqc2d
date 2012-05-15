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

#ifndef StatisticalMean_H
#define StatisticalMean_H 1

#include <boost/version.hpp>
#if BOOST_VERSION >= 104000

#include "Accumulator.h"
#include "Checker.h"
#include "DayValueExtractor.h"
#include "Instrument.h"
#include "Qc2Algorithm.h"
#include <list>
#include <map>
#include <vector>

class MeanFactory;
class QuartilesFactory;
class RedistributionNeighbors;
class SumFactory;

class StatisticalMean : public Qc2Algorithm {
public:
    StatisticalMean();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

    float getReferenceValue(int station, int dayOfYear, const std::string& key, bool& valid);

public:
    typedef std::vector<kvalobs::kvData> dlist_t;
    typedef std::map<Instrument, dlist_t, lt_Instrument> smap_t;
    typedef std::vector<DayValueP> dm_t;
    typedef std::map<Instrument, dm_t, lt_Instrument> sdm_t;
    typedef std::map<int, AccumulatedValueP> dm2_t;
    typedef std::map<Instrument, dm2_t, lt_Instrument> sd2_t;

private:
    std::list<int> findNeighbors(int stationID);

    smap_t fetchData();

    sdm_t findStationDailyMeans(DayValueExtractorP dve);

    sd2_t findStationMeansPerDay(DayValueExtractorP dve, AccumulatorP accumulator);

    void checkAllMeanValues(CheckerP checker, const sd2_t& smpd);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;
    boost::shared_ptr<MeanFactory> mMeanFactory;
    boost::shared_ptr<QuartilesFactory> mQuartilesFactory;
    boost::shared_ptr<SumFactory> mSumFactory;

    float mTolerance;
    int mDays;
    int mDaysRequired;
    int mParamid;
    float mMaxBadRatePerDay;
    std::vector<int> mTypeids;
    miutil::miTime mUT0extended;

    FlagSetCU ok_flags;

    typedef std::map<std::string, DBInterface::reference_value_map_t> referenceKeys_t;
    referenceKeys_t mReferenceKeys;
};

#endif /* BOOST_VERSION >= 104000 */

#endif /* StatisticalMean_H */
