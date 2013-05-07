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

#ifndef DipTestAlgorithm_H
#define DipTestAlgorithm_H 1

#include "Qc2Algorithm.h"
#include "DBInterface.h"

/**
 * See https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:requirements:algorithms:diptest03
 */
class DipTestAlgorithm : public Qc2Algorithm {
public:
    DipTestAlgorithm()
        : Qc2Algorithm("DipTest") { }

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    bool  fillParameterDeltaMap(const AlgorithmConfig& params, std::map<int, float>& map);
    float fetchDelta(const kvtime::time& time, int pid);
    void  checkDipAndInterpolate(const kvalobs::kvData& candidate, float delta);
    bool  tryAkima(const kvalobs::kvData& candidate, float& interpolated);
    void  writeChanges(const kvalobs::kvData& dip, const kvalobs::kvData& after, const float interpolated, bool haveAkima);

private:
    std::map<int, float> PidValMap;
    FlagSetCU akima_flags, candidate_flags, dip_before_flags, dip_after_flags, message_before_flags, message_after_flags;
    FlagChange dip_flagchange, afterdip_flagchange;
};

#endif
