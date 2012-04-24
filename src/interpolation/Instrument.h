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

#ifndef INSTRUMENT_H_
#define INSTRUMENT_H_

#include <functional>

namespace kvalobs {
class kvData;
}

struct Instrument {
    int stationid;
    int paramid;
    int sensor;
    int type;
    int level;
    Instrument(int st, int pa, int se, int ty, int le);
    explicit Instrument(const kvalobs::kvData& d);
};

struct lt_Instrument : public std::binary_function<Instrument, Instrument, bool>
{
    bool operator()(const Instrument& a, const Instrument& b) const;
};

#endif /* INSTRUMENT_H_ */
