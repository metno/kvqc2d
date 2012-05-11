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

#ifndef CHECKER_H_
#define CHECKER_H_

#include "AccumulatedValue.h"
#include "Notifier.h"
class StatisticalMean;

class Checker {
public:
    Checker(StatisticalMean* sm);

    // indicates new center station; return true if ok, i.e. no neighbors need to be checked
    virtual bool newCenter(int id, int dayOfYear, AccumulatedValueP value) = 0;
    // return true if enough neighbors have been seen
    virtual bool checkNeighbor(int nbr, AccumulatedValueP value) = 0;
    // return true if the center station passes the test
    virtual bool pass() = 0;

    float getReference(int stationid, int day, const std::string& key, bool& valid);

    Message warning();

    virtual ~Checker();
private:
    StatisticalMean* mStatisticalMean;
};

typedef boost::shared_ptr<Checker> CheckerP;

#endif /* CHECKER_H_ */
