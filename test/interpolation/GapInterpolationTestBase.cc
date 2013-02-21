/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2012 met.no

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

#include "GapInterpolationTestBase.hh"

#include "interpolation/GapInterpolationAlgorithm.h"

void INSERT_NEIGHBOR(std::ostream& sql, int stationid, int paramid, int neighborid, float offset, float slope, float sigma)
{
    sql << "INSERT INTO qc2_interpolation_best_neighbors VALUES ("
        << stationid << ',' << neighborid << ',' << paramid << ", 0, " << offset << ',' << slope << ',' << sigma << ");\n";
}

void GapInterpolationTest::SetUp()
{
    algo = new GapInterpolationAlgorithm();
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    INSERT_STATION(sql,  1380, "GAUTESTAD",               59.376, 11.303, 130);
    INSERT_STATION(sql,  4460, "HAKADAL JERNBANESTASJON", 60.117, 10.829, 170);
    INSERT_STATION(sql, 18700, "OSLO - BLINDERN",         59.942, 10.720,  94);
    INSERT_STATION(sql, 69150, "KVITHAMAR",               63.488, 10.880,  40);
    INSERT_STATION(sql, 90720, "MÅSVIK",                  69.986, 18.687,  47);
    INSERT_STATION(sql, 99754, "HORNSUND",                77.000, 15.500,  10);
    INSERT_STATION(sql, 99927, "VERLEGENHUKEN",           80.059, 16.250,   8);

    INSERT_NEIGHBOR(sql, 18700, 211, 18210);
    INSERT_NEIGHBOR(sql, 18700, 211, 18230);
    INSERT_NEIGHBOR(sql, 18700, 211,  4780);
    INSERT_NEIGHBOR(sql, 18700, 211, 17850);
    INSERT_NEIGHBOR(sql, 18700, 211, 26990);
    INSERT_NEIGHBOR(sql, 18700, 211, 18500);
    INSERT_NEIGHBOR(sql, 18700, 211, 20301);
    INSERT_NEIGHBOR(sql, 18700, 211,  4200);
    INSERT_NEIGHBOR(sql, 18700, 211,  4460);
    INSERT_NEIGHBOR(sql, 18700, 211, 17150);

    INSERT_NEIGHBOR(sql, 18700, 178,  4200);
    INSERT_NEIGHBOR(sql, 18700, 178,  2540);
    INSERT_NEIGHBOR(sql, 18700, 178,  4780);
    INSERT_NEIGHBOR(sql, 18700, 178, 17150);
    INSERT_NEIGHBOR(sql, 18700, 178, 20301);
    INSERT_NEIGHBOR(sql, 18700, 178, 28380);
    INSERT_NEIGHBOR(sql, 18700, 178, 27470);
    INSERT_NEIGHBOR(sql, 18700, 178, 30650);
    INSERT_NEIGHBOR(sql, 18700, 178, 30420);
    INSERT_NEIGHBOR(sql, 18700, 178,  6020);

    INSERT_NEIGHBOR(sql, 18700, 262, 18230);
    INSERT_NEIGHBOR(sql, 18700, 262,  4780);
    INSERT_NEIGHBOR(sql, 18700, 262,  4200);
    INSERT_NEIGHBOR(sql, 18700, 262, 17850);
    INSERT_NEIGHBOR(sql, 18700, 262,  4460);
    INSERT_NEIGHBOR(sql, 18700, 262, 18950);
    INSERT_NEIGHBOR(sql, 18700, 262, 20301);
    INSERT_NEIGHBOR(sql, 18700, 262,  4920);
    INSERT_NEIGHBOR(sql, 18700, 262, 17150);
    INSERT_NEIGHBOR(sql, 18700, 262, 26990);

    INSERT_NEIGHBOR(sql, 69150, 211, 69100);
    INSERT_NEIGHBOR(sql, 69150, 211, 68860);
    INSERT_NEIGHBOR(sql, 69150, 211, 69655);
    INSERT_NEIGHBOR(sql, 69150, 211, 68290);
    INSERT_NEIGHBOR(sql, 69150, 211, 69380);
    INSERT_NEIGHBOR(sql, 69150, 211, 68130);
    INSERT_NEIGHBOR(sql, 69150, 211, 70991);
    INSERT_NEIGHBOR(sql, 69150, 211, 70990);
    INSERT_NEIGHBOR(sql, 69150, 211, 67280);
    INSERT_NEIGHBOR(sql, 69150, 211, 71000);

    ASSERT_NO_THROW_X(db->exec(sql.str()));

    NeighborDataVector n;
    ASSERT_NO_THROW(n = db->findNeighborData(18700, 211, 2.7));
    ASSERT_EQ(10, n.size());

    ASSERT_NO_THROW(n = db->findNeighborData(18700, 178, 2.7));
    ASSERT_EQ(10, n.size());

    ASSERT_NO_THROW(n = db->findNeighborData(69150, 211, 2.7));
    ASSERT_EQ(10, n.size());
}
