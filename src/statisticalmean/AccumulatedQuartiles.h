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

#ifndef ACCUMULATEDQUARTILES_H_
#define ACCUMULATEDQUARTILES_H_

#include "AccumulatedValue.h"

struct AccumulatedQuartiles : public AccumulatedValue {
    float q1, q2, q3;
    float q(int i) const { if(i==0) return q1; else if(i==1) return q2; else if(i==2) return q3; else return -9999; }
    AccumulatedQuartiles(float qq1, float qq2, float qq3)
        : q1(qq1), q2(qq2), q3(qq3) { }
};

typedef boost::shared_ptr<AccumulatedQuartiles> AccumulatedQuartilesP;

#endif /* ACCUMULATEDQUARTILES_H_ */
