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

#include "AlgorithmTestBase.h"
#include "AlgorithmHelpers.h"

class AlgorithmHelpersTest : public AlgorithmTestBase {
};

#if 0
TEST_F(AlgorithmHelpersTest, testGetNorwegianFixedStations)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES (180, 61.2944, 12.2719, 360, 0.0, 'TRYSIL VEGSTASJON', 1397, 180, '', '', '', 8, 1, '1993-11-10 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::list<kvalobs::kvStation> stations;
    Helpers::GetNorwegianFixedStations(db, stations);
    ASSERT_EQ(1, stations.size());
    ASSERT_EQ(180, stations.begin()->stationID());
}
#endif
