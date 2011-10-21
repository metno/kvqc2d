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

#ifndef ALGORITHMHELPERS_H_
#define ALGORITHMHELPERS_H_

#include <kvalobs/kvData.h>
#include <kvalobs/kvStation.h>
#include <puTools/miString.h>
#include <list>
#include <vector>

class DBInterface;

namespace Helpers {

/**
 * Checks that all data entries in series have the same typeid and that the
 * observation times increase by 1 hour.
 */
bool checkContinuousHourAndSameTypeID(const std::vector<kvalobs::kvData>& series);

/**
 * Adds add to data's cfailed(), and also extra if extra is not empty.
 */
void updateCfailed(kvalobs::kvData& data, const miutil::miString& add, const miutil::miString& extra);

/**
 * Updates data's useinfo.
 */
void updateUseInfo(kvalobs::kvData& data);

void GetNorwegianFixedStations(DBInterface* db, std::list<kvalobs::kvStation>& stations);
void GetAllStations(DBInterface* db, std::list<kvalobs::kvStation>& stations);

/**
 * AFAIK this does not work well for points near the poles.
 */
double distance(double lon1, double lat1, double lon2, double lat2);

} // namespace Helpers

#endif /* ALGORITHMHELPERS_H_ */
