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

#include "AlgorithmTestBase.h"
#include "AlgorithmHelpers.h"
#include "algorithms/GapInterpolationAlgorithm.h"

class GapInterpolationTest : public AlgorithmTestBase {
public:
    void SetUp();
};

namespace {
void INSERT_NEIGHBOR(std::ostream& sql, int stationid, int neighborid, int paramid, float offset, float slope, float sigma)
{
    sql << "INSERT INTO interpolation_best_neighbors VALUES ("
        << stationid << ',' << neighborid << ',' << paramid << ',' << offset << ',' << slope << ',' << sigma << ");";
}
}

void GapInterpolationTest::SetUp()
{
    algo = new GapInterpolationAlgorithm();
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(69150, 63.488200, 10.879500, 40, 0, 'KVITHAMAR', 69150, NULL, NULL, NULL, NULL, 2, 't', '1987-05-12 00:00:00');";

    INSERT_NEIGHBOR(sql, 18700, 211, 18210,  0.359377,  0.936164, 0.837205);
    INSERT_NEIGHBOR(sql, 18700, 211, 18230,  0.432384,  0.946009, 0.890819);
    INSERT_NEIGHBOR(sql, 18700, 211,  4780,  1.82088,   0.949237, 1.15107 );
    INSERT_NEIGHBOR(sql, 18700, 211, 17850,  1.07924,   0.974772, 1.356   );
    INSERT_NEIGHBOR(sql, 18700, 211, 26990,  0.682128,  0.971464, 1.38741 );
    INSERT_NEIGHBOR(sql, 18700, 211, 18500,  2.65349,   0.979765, 1.40503 );
    INSERT_NEIGHBOR(sql, 18700, 211, 20301,  1.57686,   0.917686, 1.40677 );
    INSERT_NEIGHBOR(sql, 18700, 211,  4200,  1.58401,   0.921502, 1.46844 );
    INSERT_NEIGHBOR(sql, 18700, 211,  4460,  2.0924,    0.928709, 1.51339 );
    INSERT_NEIGHBOR(sql, 18700, 211, 17150, -0.0143439, 1.00587,  1.53928 );

    INSERT_NEIGHBOR(sql, 69150, 211, 69100, -0.0739351, 0.999815, 0.774052);
    INSERT_NEIGHBOR(sql, 69150, 211, 68860,  0.0443254, 1.02678,  1.29483 );
    INSERT_NEIGHBOR(sql, 69150, 211, 69655, -1.04465,   1.06465,  1.32951 );
    INSERT_NEIGHBOR(sql, 69150, 211, 68290,  1.1234,    0.941565, 1.37513 );
    INSERT_NEIGHBOR(sql, 69150, 211, 69380,  1.67007,   0.907408, 1.46907 );
    INSERT_NEIGHBOR(sql, 69150, 211, 68130,  0.230591,  1.04775,  1.47233 );
    INSERT_NEIGHBOR(sql, 69150, 211, 70991,  1.08758,   0.923029, 1.72307 );
    INSERT_NEIGHBOR(sql, 69150, 211, 70990,  1.00074,   0.892268, 1.76754 );
    INSERT_NEIGHBOR(sql, 69150, 211, 67280,  2.04537,   0.926591, 1.86768 );
    INSERT_NEIGHBOR(sql, 69150, 211, 71000,  1.09259,   0.932182, 1.87565 );

    ASSERT_NO_THROW(db->exec(sql.str()));
}

TEST_F(GapInterpolationTest, test1)
{
    DataList data(68290, 211, 330);
    data.add("2012-03-25 18:00:00",      3.9,      3.9, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      3.6,      3.6, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      2.6,      2.6, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      3.0,      3.0, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      2.1,      2.1, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      0.8,      0.8, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      1.3,      1.3, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.1,      1.1, "0111000000100010", "");
    data.setStation(69100)
        .add("2012-03-25 18:00:00",      5.4,      5.4, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      5.9,      5.9, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      6.2,      6.2, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      5.9,      5.9, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      5.4,      5.4, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      5.2,      5.2, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      4.7,      4.7, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      4.1,      4.1, "0111000000100010", "");
    data.setStation(69150)
        .add("2012-03-25 18:00:00",      5.9,      5.9, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      5.7,      5.7, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      6.1,      6.1, "0111000000100010", "")
        .add("2012-03-25 21:00:00", -32767.0, -32767.0, "0000003000000000", "")
        .add("2012-03-25 22:00:00", -32767.0, -32767.0, "0000003000000000", "")
        .add("2012-03-25 23:00:00",      4.7,      4.7, "0110000000100010", "")
        .add("2012-03-26 00:00:00",      4.2,      4.2, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      3.9,      3.9, "0111000000100010", "");
    data.setStation(69380)
        .add("2012-03-25 18:00:00",      4.8,      4.8, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      4.6,      4.6, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      5.0,      5.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      4.5,      4.5, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      4.1,      4.1, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      3.8,      3.8, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      2.6,      2.6, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.8,      1.8, "0111000000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   03\n"
           << "Start_DD   =   25\n"
           << "Start_hh   =   18\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   03\n"
           << "End_DD     =   26\n"
           << "End_hh     =   01\n"
           << "TypeId     =  330\n"
           << "ParamId    =  211\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    ASSERT_RUN(algo, bc, 0);
}
