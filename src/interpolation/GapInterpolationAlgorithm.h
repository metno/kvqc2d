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

#ifndef GapInterpolationAlgorithm_H
#define GapInterpolationAlgorithm_H 1

#include "Instrument.h"
#include "interpolation/NeighborInterpolator.h"
#include "interpolation/MinMaxInterpolator.h"
#include "interpolation/ParameterInfo.h"
#include "interpolation/SimpleInterpolationResult.h"
#include "Qc2Algorithm.h"

#include <boost/shared_ptr.hpp>

class GapInterpolationAlgorithm : public Qc2Algorithm {
public:
    GapInterpolationAlgorithm();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    struct ParamGroupMissingRange {
        TimeRange range;
        typedef std::vector<kvalobs::kvData> MissingRange;
        typedef std::map<int, MissingRange> ParamMissingRanges;
        ParamMissingRanges paramMissingRanges;

        bool tryExtend(const kvalobs::kvData& missing);
        ParamGroupMissingRange(const kvalobs::kvData& missing);
    };

private:
    Instrument getMasterInstrument(const kvalobs::kvData& data);
    void makeUpdates(const ParamGroupMissingRange::MissingRange& mr,
                     const Interpolation::SimpleResultVector& interpolated,
                     const TimeRange& range,
                     DBInterface::DataList& updates,
                     const ParameterInfo& parameterInfo);

private:
    typedef DBInterface::DataList    DataList;
    typedef DataList::iterator       DataList_it;
    typedef DataList::const_iterator DataList_cit;

private:
    boost::shared_ptr<Interpolation::NeighborInterpolator> mNeighborInterpolator;
    boost::shared_ptr<Interpolation::MinMaxInterpolator> mMinMaxInterpolator;

    typedef std::vector<ParameterInfo> ParameterInfos;
    typedef ParameterInfos::const_iterator ParameterInfos_it;
    ParameterInfos mParameterInfos;

    std::vector<int> tids;
    float mRAThreshold;

    FlagSetCU missing_flags;
    FlagChange missing_flagchange_good, missing_flagchange_bad, missing_flagchange_common;
};

#endif
