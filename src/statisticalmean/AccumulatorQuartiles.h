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

#ifndef ACCUMULATORQUARTILES_H_
#define ACCUMULATORQUARTILES_H_

#include "Accumulator.h"
#include <vector>

class AccumulatorQuartiles : public Accumulator {
public:
    AccumulatorQuartiles(int days, int daysRequired)
        : mDays(days), mDaysRequired(daysRequired) { }
    virtual void newStation() { mValues.clear(); }
    virtual void push(DayValueP value);
    virtual void pop(DayValueP value);
    virtual AccumulatedValueP value();
private:
    int mDays, mDaysRequired;
    std::vector<float> mValues;
};

#endif /* ACCUMULATORQUARTILES_H_ */
