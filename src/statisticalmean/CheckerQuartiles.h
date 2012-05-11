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

#ifndef CHECKERQUARTILES_H_
#define CHECKERQUARTILES_H_

#include "Checker.h"
#include <vector>

class CheckerQuartiles : public Checker {
public:
    CheckerQuartiles(StatisticalMean* sm, int days, const std::vector<float>& tolerances);
    bool newCenter(int id, int dayOfYear, AccumulatedValueP accumulated);
    bool checkNeighbor(int nbr, AccumulatedValueP accumulated);
    bool pass();
private:
    bool calculateDiffsToReferences(int station, int dOy, float diffs[3], AccumulatedValueP accumulated);
private:
    int mDays, mDayOfYear, mCenter, mCountNeighborsWithError;
    std::vector<float> mTolerances;
    float mDiffsQ[3];
};

#endif /* CHECKERQUARTILES_H_ */
