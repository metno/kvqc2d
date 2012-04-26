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

#include "DBInterface.h"
#include "Qc2Algorithm.h"
#include "algorithms/DataUpdate.h"
#include <boost/shared_ptr.hpp>

class RedistributionNeighbors;

class StatisticalMean : public Qc2Algorithm {
public:
    StatisticalMean();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

    float getReferenceValue(int station, int dayOfYear, const std::string& key, bool& valid);

private:
    std::list<int> findNeighbors(int stationID);

private:
    boost::shared_ptr<RedistributionNeighbors> mNeighbors;

    float mTolerance;
    int mDays;
    int mDaysRequired;
    int mParamid;
    std::vector<int> mTypeids;
    miutil::miTime mUT0extended;

    FlagSetCU ok_flags;

    typedef std::map<std::string, DBInterface::reference_value_map_t> referenceKeys_t;
    referenceKeys_t mReferenceKeys;
};

#endif /* BOOST_VERSION >= 104000 */

#endif /* StatisticalMean_H */
