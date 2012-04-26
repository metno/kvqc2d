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

#ifndef SingleLinearAlgorithm_H
#define SingleLinearAlgorithm_H 1

#include "DataUpdate.h"
#include "FlagChange.h"
#include "FlagPatterns.h"
#include "Qc2Algorithm.h"

#include <vector>

/**
 * Specification:
 * https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:requirements
 * section "Simple Interpolations" links to specification article
 * https://kvalobs.wiki.met.no/lib/exe/fetch.php?media=kvoss:system:qc2:qc2-d2_flagg_08_2010_v32.pdf
 *
 * Also see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:test:algorithms:singlemissingpoint2
 */
class SingleLinearAlgorithm : public Qc2Algorithm {
public:
    SingleLinearAlgorithm();

    virtual void configure(const AlgorithmConfig& params);
    virtual void run();

private:
    bool isNeighborOk(const kvalobs::kvData& n);
    void calculateCorrected(const kvalobs::kvData& before, DataUpdate& middle, const kvalobs::kvData& after);

private:
    FlagSetCU missing_flags, neighbor_flags;
    FlagChange ftime0_flagchange, ftime1_flagchange;
    std::vector<int> pids;
};

#endif
