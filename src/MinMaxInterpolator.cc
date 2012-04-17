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

#include "MinMaxInterpolator.h"

#include "Akima.h"
#include "AlgorithmHelpers.h"
#include "FormulaUU.h"
#include "Helpers.h"

#define NDEBUG 1
#include "debug.h"

// ########################################################################

#if 0
MinMaxInterpolator::MinMaxInterpolator(DataAccess* dax)
    : mDax(dax)
{
}

// ------------------------------------------------------------------------

::Interpolator::ValuesWithQualities_t MinMaxInterpolator::interpolate(const Instrument& instrument, const TimeRange& t)
{
    if( mMinMaxParameters.find(instrument.paramid) != mMinMaxParameters.end() ) {
        const MinMaxParameters& mmp =  mMinMaxParameters[instrument.paramid];
        MinMaxInterpolate(mDax, instrument, mmp.minParameter, mmp.maxParameter, mmp.noiseLevel, t);
    }
}

// ------------------------------------------------------------------------

void MinMaxInterpolator::configure(const AlgorithmConfig& config)
{
    mMinMaxParameters.clear();
    const std::string parameters = params.getParameter<std::string>("Parameters");
    const std::vector<std::string> pl = Helpers::splitN(parameters, ";", true);
    foreach(const std::string& parminmax, pl) {
        const std::vector<std::string> pmm = Helpers::splitN(parminmax, "/", true);
        if( pmm.size() != 4 )
            throw ConfigException("invalid parameter/minParameter/maxParameter/noiselevel specification");
        const int par = std::atoi(pmm[0].c_str()), mini = std::atoi(pmm[1].c_str()), maxi = std::atoi(pmm[2].c_str());
        const float noiselevel = std::atof(pmm[3].c_str());
        if( par <= 0 || mini <= 0 || maxi <= 0 || par == mini || par == maxi || mini == maxi )
            throw ConfigException("invalid parameter ids in parameter/minParameter/maxParameter specification");
        if( noiselevel <= 0  )
            throw ConfigException("invalid noiselevel <= 0 in parameter/minParameter/maxParameter specification");
        if( mMinMaxParameters.find(par) != mMinMaxParameters.end() )
            throw ConfigException("duplicate parameter id in parameter/minParameter/maxParameter specification");
        mMinMaxParameters[par] = MinMaxParameters(mini, maxi, noiselevel);
    }
}
#endif

// ########################################################################

namespace {
bool mmwq_helper(Interpolator::ValuesWithQualities_t& vq, float obs)
{
    const bool obsValid = (obs != Interpolator::INVALID);
    const int quality = obsValid ? Interpolator::QUALITY_OBS : Interpolator::QUALITY_INTER_FAILED;
    vq.push_back(Interpolator::ValueWithQuality(obs, quality));
    return obsValid;
}
}

// ------------------------------------------------------------------------

MinMaxValuesWithQualities_t MinMaxInterpolateI(Interpolator* inter, CorrelatedNeighbors::DataAccess* dax, const Instrument& instrument,
                                               const ParameterInfo& parameterInfo, const TimeRange& t)
{
    Instrument instMin = instrument, instMax = instrument;
    instMin.paramid = parameterInfo.minParameter;
    instMax.paramid = parameterInfo.maxParameter;
    const int N = 3;

    TimeRange tex(Helpers::plusHour(t.t0, -N+1), Helpers::plusHour(t.t1, N-1));
    const std::vector<float> observationsPar = dax->fetchObservations(instrument, tex);
    const std::vector<float> observationsMin = dax->fetchObservations(instMin, tex);
    const std::vector<float> observationsMax = dax->fetchObservations(instMax, tex);

    MinMaxValuesWithQualities_t mmwq;
    bool completePar = true, completeMin = true, completeMax = true;
    for(unsigned int i=0; i<observationsPar.size(); ++i) {
        completePar &= mmwq_helper(mmwq.par, observationsPar[i]);
        completeMin &= mmwq_helper(mmwq.min, observationsMin[i]);
        completeMax &= mmwq_helper(mmwq.max, observationsMax[i]);
    }
    DBG("complete par=" << completePar << " min=" << completeMin << " max=" << completeMax);

    if( completePar and completeMin and completeMax ) {
        // nothing to do
        return mmwq;
    }

    ::Interpolator::ValuesWithQualities_t interpolatedPar;
    if( !completePar ) {
        if( completeMin and completeMax ) {
            // reconstruct parameter from complete min and max
            for(unsigned int i=N; i<observationsPar.size()-N; ++i) {
                if( observationsPar[i] == Interpolator::INVALID ) {
                    const float maxi = std::min(observationsMax[i], observationsMax[i+1]);
                    const float mini = std::max(observationsMin[i], observationsMin[i+1]);

                    const float value = (maxi + mini)/2;
                    // if observations for min and max are inside the
                    // allowed parameter min and max value, value
                    // cannot be outside either

                    DBG("reconstruction from ..N/::X i=" << i << " value=" << value);
                    mmwq.par[i] = Interpolator::ValueWithQuality(value, Interpolator::QUALITY_INTER_BAD);
                }
            }
            return mmwq;
        }

        // parameter and min and max incomplete => interpolate
        // parameter with other interpolator
        mmwq.par.clear();
        std::vector<float>::const_iterator it = observationsPar.begin();
        for(int i=0; i<N; ++i)
            mmwq_helper(mmwq.par, *it++);

        Interpolator::ValuesWithQualities_t interpol = inter->interpolate(instrument, t);
        mmwq.par.insert(mmwq.par.end(), interpol.begin(), interpol.end());

        it = observationsPar.end();
        for(int i=0; i<N; ++i)
            mmwq_helper(mmwq.par, *(--it));

        bool newCompletePar = true;
        for(unsigned int i=N; i<mmwq.par.size()-N; ++i) {
            const bool parValid = mmwq.par[i].valid();
            if( parValid ) {
                if( mmwq.min[i].quality == Interpolator::QUALITY_OBS && mmwq.par[i].value < mmwq.min[i].value )
                    mmwq.par[i] = Interpolator::ValueWithQuality(mmwq.min[i].value, Interpolator::QUALITY_INTER_BAD);
                if( mmwq.max[i].quality == Interpolator::QUALITY_OBS && mmwq.par[i].value > mmwq.max[i].value )
                    mmwq.par[i] = Interpolator::ValueWithQuality(mmwq.max[i].value, Interpolator::QUALITY_INTER_BAD);
            }
            newCompletePar &= mmwq.par[i].valid();
        }
        DBGV(newCompletePar);
        if( !newCompletePar )
            return mmwq;
    }
    // here, mmwq.par is complete, although maybe not so good quality

    if( !completeMin || !completeMax ) {
        DBG("reconstruction ..N/..X");
        Akima akima;
        for(unsigned int i=0; i<mmwq.par.size(); ++i) {
            akima.add(i, mmwq.par[i].value);
            DBG(DBG1(i) << DBG1(mmwq.par[i].value));
        }
        for(unsigned int i=N; i<mmwq.par.size()-N; ++i) {
            float mini = parameterInfo.constrained(std::min(mmwq.par[i-1].value, mmwq.par[i].value));
            float maxi = parameterInfo.constrained(std::max(mmwq.par[i-1].value, mmwq.par[i].value));
            if( akima.interpolate(i+0.5) != Akima::INVALID ) {
                const int Nbetween = 20;
                for(int j=1; j<Nbetween; ++j) {
                    const float x = i + j/float(Nbetween);
                    const float noise = parameterInfo.fluctuationLevel * Helpers::randNormal();
                    const float akimaValue = akima.interpolate(x);
                    const float value = parameterInfo.constrained(akimaValue + noise);
                    DBG(DBG1(akimaValue) << "(at " << x << ") " << DBG1(noise) << DBG1(value));
                    Helpers::minimize(mini, value);
                    Helpers::maximize(maxi, value);
                }
                DBG(DBG1(mini) << DBG1(maxi));
                if( observationsMin[i] == Interpolator::INVALID )
                    mmwq.min[i] = Interpolator::ValueWithQuality(mini, Interpolator::QUALITY_INTER_BAD);
                if( observationsMax[i] == Interpolator::INVALID )
                    mmwq.max[i] = Interpolator::ValueWithQuality(maxi, Interpolator::QUALITY_INTER_BAD);
            } else {
                const Interpolator::ValueWithQuality  failed(Interpolator::INVALID, Interpolator::QUALITY_INTER_FAILED);
                mmwq.min[i] = mmwq.max[i] = failed;
            }
        }
    }
    return mmwq;
}

// ------------------------------------------------------------------------

MinMaxValuesWithQualities_t MinMaxInterpolate(Interpolator* inter, CorrelatedNeighbors::DataAccess* dax, const Instrument& instrument,
                                              const ParameterInfo& parameterInfo, const TimeRange& t)
{
    //TimeRange tex(Helpers::plusHour(t.t0, -2), Helpers::plusHour(t.t1, 2));
    MinMaxValuesWithQualities_t mmwq = MinMaxInterpolateI(inter, dax, instrument, parameterInfo, t);
    DBGV(mmwq.par.size());
    DBGV(mmwq.min.size());
    DBGV(mmwq.max.size());
    mmwq.par = Interpolator::ValuesWithQualities_t(++ ++ ++mmwq.par.begin(), -- -- --mmwq.par.end());
    mmwq.min = Interpolator::ValuesWithQualities_t(++ ++ ++mmwq.min.begin(), -- -- --mmwq.min.end());
    mmwq.max = Interpolator::ValuesWithQualities_t(++ ++ ++mmwq.max.begin(), -- -- --mmwq.max.end());
    return mmwq;
}

// ########################################################################

std::vector<float> DataAccessUU::fetchObservations(const Instrument& instrumentUU, const TimeRange& t)
{
    Instrument instrumentTA = instrumentUU;
    instrumentTA.paramid = 211;

    const std::vector<float> seriesUU = mDax->fetchObservations(instrumentUU, t);
    const std::vector<float> seriesTA = mDax->fetchObservations(instrumentTA, t);

    std::vector<float> series(seriesUU.size());
    for(unsigned int i=0; i<series.size(); ++i) {
        series[i] = formulaTD(seriesTA[i], seriesUU[i]);
        DBG("UU=" << seriesUU[i] << " TA=" << seriesTA[i] << " => UU=" << series[i]);
    }

    return series;
}

// ------------------------------------------------------------------------

std::vector<float> DataAccessUU::fetchModelValues (const Instrument&, const TimeRange& t)
{
    return std::vector<float>(t.hours()+1, ::Interpolator::INVALID);
}

// ------------------------------------------------------------------------

CorrelatedNeighbors::neighbors_t DataAccessUU::findNeighbors(const Instrument& instrument, double maxsigma)
{
    return mDax->findNeighbors(instrument, maxsigma);
}

// ========================================================================

Interpolator::ValuesWithQualities_t InterpolatorUU::interpolate(const Instrument& instrumentUU, const TimeRange& t)
{
    CorrelatedNeighbors::DataAccess* dax = mInterpolator->getDataAccess();
    mInterpolator->setDataAccess(mDataAccess);
    ValuesWithQualities_t interpolatedTD = mInterpolator->interpolate(instrumentUU, t);
    DBGV(interpolatedTD.size());
    mInterpolator->setDataAccess(dax);

    Instrument instrumentTA = instrumentUU;
    instrumentTA.paramid = 211;

    const std::vector<float> seriesTA = dax->fetchObservations(instrumentTA, t);
    DBGV(seriesTA.size());
    for(unsigned int i=0; i<interpolatedTD.size(); ++i) {
        const float TD = interpolatedTD[i].value, TA = seriesTA[i+1];
        const float UU = formulaUU(TA, TD);
        interpolatedTD[i].value = UU;
        DBG("TD=" << TD << " TA=" << TA << " => UU=" << UU);
    }

    return interpolatedTD;
}
