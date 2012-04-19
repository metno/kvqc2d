/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2012 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
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

#include "CorrelatedNeighborInterpolator.h"

#include "AlgorithmConfig.h"
#include "AlgorithmHelpers.h"
#include "DBInterface.h"
#include "NeighborInterpolation.h"
#include "foreach.h"

#include <kvalobs/kvData.h>
#include <boost/make_shared.hpp>
#include <vector>

#define NDEBUG 1
#include "debug.h"

namespace CorrelatedNeighbors {

DataAccess::~DataAccess()
{
}

DataFilter::~DataFilter()
{
}

// ========================================================================

NeighborInterpolation::Data KvalobsFilter::toNumerical(int paramid, float storage) const
{
    if( storage <= -32766 )
        return NeighborInterpolation::Data();
    if( paramid == 262 && (storage==-1 || storage==-3) )
        storage = 0;
    return NeighborInterpolation::Data(storage);
}

float KvalobsFilter::toStorage(int paramid, NeighborInterpolation::Interpolation::Quality quality, float numerical) const
{
    if( quality == NeighborInterpolation::Interpolation::FAILED )
        return -32766;
    if( paramid == 262 && numerical < 0.5 )
        return 0;
    return numerical;
}

// ========================================================================

Interpolator::Interpolator(DataAccess* dax)
    : mDax(dax)
    , mFilter(boost::make_shared<KvalobsFilter>())
{
}

struct Kvalobs2Data : public std::unary_function<NeighborInterpolation::Data, float> {
    NeighborInterpolation::Data operator() (float f) const {
        return filter->toNumerical(paramid, f);
    }
    CorrelatedNeighbors::DataFilterP filter;
    int paramid;
    Kvalobs2Data(const CorrelatedNeighbors::DataFilterP& f, int p )
    : filter(f), paramid(p) { }
};

// ------------------------------------------------------------------------

::Interpolator::ValuesWithQualities_t Interpolator::interpolate(const Instrument& ctr, const TimeRange& t)
{
    DBGV(ctr.stationid);
    DBGV(ctr.paramid);

    ValuesWithQualities_t interpolated;

    const int gapLength = t.hours() - 1;
    if( gapLength < 1 )
        return interpolated;

    const ParamInfos_cit pi_it = mParamInfos.find(ctr.paramid);
    if( pi_it == mParamInfos.end() )
        return interpolated;
    const ParamInfo& pi = pi_it->second;

    NeighborInterpolation::InterpolationData data;
    data.maxOffset = pi.offsetCorrectionLimit;

    const int NA = NeighborInterpolation::extraData;
    const TimeRange tExtended(Helpers::plusHour(t.t0, -NA), Helpers::plusHour(t.t1, NA));
    const std::vector<float> observations = mDax->fetchObservations(ctr, tExtended);
    const std::vector<float> modelvalues  = mDax->fetchModelValues (ctr, tExtended);

    Kvalobs2Data k2d(mFilter, ctr.paramid);
    std::transform(observations.begin(), observations.end(), std::back_inserter(data.centerObservations), k2d);
    std::transform(modelvalues .begin(), modelvalues .end(), std::back_inserter(data.centerModel),        k2d);
#ifndef NDEBUG
    foreach(const NeighborInterpolation::Data& d, data.centerObservations)
        DBG(DBG1(d.usable) << DBG1(d.value));
#endif

    const neighbors_t& neighbors = find_neighbors(ctr, mMaxSigma);
    DBGV(neighbors.size());

    foreach(const NeighborData& nd, neighbors) {
        NeighborInterpolation::Correlation nc;
        nc.sigma  = nd.sigma;
        nc.slope  = nd.slope;
        nc.offset = nd.offset;
        data.neighborCorrelations.push_back(nc);

        const Instrument nbr(nd.neighborid, ctr.paramid, DBInterface::INVALID_ID, DBInterface::INVALID_ID, DBInterface::INVALID_ID);
        const std::vector<float> nData = mDax->fetchObservations(nbr, tExtended);

        data.neighborObservations.push_back(std::vector<NeighborInterpolation::Data>());
        std::transform(nData.begin(), nData.end(), std::back_inserter(data.neighborObservations.back()), k2d);
    }

    const std::vector<NeighborInterpolation::Interpolation> inter = NeighborInterpolation::interpolate(data);
    for(unsigned idx=NA; idx<observations.size()-NA; ++idx) {
        const NeighborInterpolation::Interpolation& i = inter[idx];
        DBG(DBG1(i.quality) << DBG1(i.value));
        interpolated.push_back(ValueWithQuality(mFilter->toStorage(ctr.paramid, i.quality, pi.constrained(i.value)), i.quality));
    }

    return interpolated;
}

// ------------------------------------------------------------------------

void Interpolator::configure(const AlgorithmConfig& config)
{
    mMaxSigma = config.getParameter<float>("maxSigma", 2.7);

    mParamInfos.clear();
    const std::vector<std::string> parameters = config.getMultiParameter<std::string>("Parameter");
    foreach(const std::string& p, parameters) {
        ParamInfo pi(p);
        mParamInfos.insert(ParamInfos::value_type(pi.parameter, pi));
    }

    neighbor_map.clear();
}

// ------------------------------------------------------------------------

ParamInfo::ParamInfo(const std::string& pi)
    : BasicParameterInfo(pi)
    , offsetCorrectionLimit(1000)
{
    const Helpers::splitN_t items = Helpers::splitN(pi, ",", true);
    foreach(const std::string& item, items) {
        Helpers::split2_t kv = Helpers::split2(item, "=");
        if( kv.first == "offsetCorrectionLimit" ) {
            offsetCorrectionLimit = std::atof(kv.second.c_str());
        }
    }
}

// ------------------------------------------------------------------------

const neighbors_t& Interpolator::find_neighbors(const Instrument& instrument, float maxsigma)
{
    neighbor_map_t::iterator it = neighbor_map.find(instrument);
    if( it != neighbor_map.end() )
        return it->second;

    neighbor_map[instrument] = mDax->findNeighbors(instrument, maxsigma);
    return neighbor_map[instrument];
}

} // namespace CorrelatedNeigbors
