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

#include "algorithms/DataUpdate.h"
#include "Instrument.h"
#include "interpolation/ParameterInfo.h"
#include "KvalobsMinMaxData.h"
#include "Qc2Algorithm.h"

namespace Interpolation {
class MinMaxInterpolator;
class MinMaxReconstruction;
class NeighborInterpolator;
} // namespace Interpolation

class GapData;
class GapUpdate;

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

    typedef std::vector<ParamGroupMissingRange> MissingRanges;
    typedef std::map<Instrument, MissingRanges, lt_Instrument> InstrumentMissingRanges;

private:
    void interpolateMissingRange(const Instrument& instrument, const MissingRanges& mr);
    GapDataPtr findSeriesData(const Instrument& instrument, const TimeRange& t, const ParameterInfo& pi);
    bool seriesHasMissingRows(GapData& data);
    void discardUnreliableMinMax(GapData& data);
    bool hasRADownStep(GapData& data);
    bool replaceFromOtherTypeid(GapData& data);
    bool interpolateFromMinMax(GapData& data);
    bool interpolateFromNeighbors(GapData& data);
    bool reconstructMinMax(GapData& data);

    Instrument getMasterInstrument(const kvalobs::kvData& data);
    void makeUpdates(GapData& data);
    void makeUpdates(std::vector<GapUpdate>& dul, const ParameterInfo& pi, DBInterface::DataList& updates);
    InstrumentMissingRanges findMissing();

    const ParameterInfo& findParameterInfo(int parameter);
    bool checkTimeRangeLimits(const Instrument& instrument, const ParamGroupMissingRange& pgmr);
    bool replaceFromOtherTypeid(GapUpdate& data);

public:
    NeighborDataVector findNeighborData(int stationid, int paramid, float maxsigma);
    Interpolation::SupportDataList getNeighborData(const TimeRange& t, int neighborid, int paramid);

private:
    typedef DBInterface::DataList DataList;

private:
    typedef std::vector<ParameterInfo> ParameterInfos;
    typedef ParameterInfos::const_iterator ParameterInfos_it;
    ParameterInfos mParameterInfos;

    std::vector<int> tids;
    float mRAThreshold;

    FlagSetCU missing_flags, mNeighborFlags, mDataFlagsUUTA;
    FlagChange missing_flagchange_good, missing_flagchange_bad, missing_flagchange_failed, missing_flagchange_common;
};

#endif
