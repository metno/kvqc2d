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
void INSERT_NEIGHBOR(std::ostream& sql, int stationid, int paramid, int neighborid, float offset, float slope, float sigma)
{
    sql << "INSERT INTO interpolation_best_neighbors VALUES ("
        << stationid << ',' << neighborid << ',' << paramid << ',' << offset << ',' << slope << ',' << sigma << ");\n";
}
}

void GapInterpolationTest::SetUp()
{
    algo = new GapInterpolationAlgorithm();
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(69150, 63.488200, 10.879500, 40, 0, 'KVITHAMAR', 69150, NULL, NULL, NULL, NULL, 2, 't', '1987-05-12 00:00:00');\n"
        << "INSERT INTO station VALUES(18700, 59.942, 10.720, 94.0, 0.0, 'OSLO - BLINDERN', 1492, 18700, NULL, NULL, NULL, 8, 't', '1937-02-25 00:00:00');\n";

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

    INSERT_NEIGHBOR(sql, 18700, 178,  4200,   9.97152, 0.989645, 0.460573);
    INSERT_NEIGHBOR(sql, 18700, 178,  2540, -14.8821,  1.01493,  0.631187);
    INSERT_NEIGHBOR(sql, 18700, 178,  4780,   3.81164, 0.996068, 0.77165);
    INSERT_NEIGHBOR(sql, 18700, 178, 17150,  -7.60093, 1.00703,  0.775405);
    INSERT_NEIGHBOR(sql, 18700, 178, 20301,  14.7874,  0.98496,  0.805516);
    INSERT_NEIGHBOR(sql, 18700, 178, 28380,   8.25995, 0.991409, 0.917459);
    INSERT_NEIGHBOR(sql, 18700, 178, 27470,  -3.98669, 1.00356,  0.928841);
    INSERT_NEIGHBOR(sql, 18700, 178, 30650,   8.03378, 0.991527, 1.09321);
    INSERT_NEIGHBOR(sql, 18700, 178, 30420,   0.48351, 0.999162, 1.10026);
    INSERT_NEIGHBOR(sql, 18700, 178,  6020,  15.0857,  0.984536, 1.14429);

    INSERT_NEIGHBOR(sql, 18700, 262, 18230,  0.213783,  0.966444, 0.841243);
    INSERT_NEIGHBOR(sql, 18700, 262,  4780,  0.12849,   0.97245,  1.32818);
    INSERT_NEIGHBOR(sql, 18700, 262,  4200,  0.116539,  0.963722, 1.33405);
    INSERT_NEIGHBOR(sql, 18700, 262, 17850, -0.136953,  0.975423, 1.34007);
    INSERT_NEIGHBOR(sql, 18700, 262,  4460,  0.608171,  0.96986,  1.35606);
    INSERT_NEIGHBOR(sql, 18700, 262, 18950,  1.0629,    1.01281,  1.4048);
    INSERT_NEIGHBOR(sql, 18700, 262, 20301,  1.22629,   0.958088, 1.54374);
    INSERT_NEIGHBOR(sql, 18700, 262,  4920,  1.44802,   0.948424, 1.55581);
    INSERT_NEIGHBOR(sql, 18700, 262, 17150, -1.16232,   0.974988, 1.5631);
    INSERT_NEIGHBOR(sql, 18700, 262, 26990, -0.0554939, 0.953904, 1.5643);

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

    ASSERT_EQ(10, db->findNeighborData(18700, 211, 2.7).size());
    ASSERT_EQ(10, db->findNeighborData(18700, 178, 2.7).size());
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

    // should be 5.77525, 5.14475
}

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, GapInMultiplePar)
{
    DataList data(4200, 178, 342);
    data.add("2012-03-25 18:00:00",    1030.7,    1030.7, "0100000000000010", "")
        .add("2012-03-25 19:00:00",    1031.1,    1031.1, "0100000000000010", "")
        .add("2012-03-25 20:00:00",    1031.2,    1031.2, "0100000000000010", "")
        .add("2012-03-25 21:00:00",    1031.7,    1031.7, "0100000000000010", "")
        .add("2012-03-25 22:00:00",    1031.9,    1031.9, "0100000000000010", "")
        .add("2012-03-25 23:00:00",    1032.0,    1032.0, "0100000000000010", "")
        .add("2012-03-26 00:00:00",    1032.1,    1032.1, "0100000000000010", "")
        .add("2012-03-26 01:00:00",    1032.0,    1032.0, "0100000000000010", "");
    data.setStation(17150).setType(342)
        .add("2012-03-25 18:00:00",    1031.1,    1031.1, "0100000000000010", "")
        .add("2012-03-25 19:00:00",    1031.6,    1031.6, "0100000000000010", "")
        .add("2012-03-25 20:00:00",    1031.6,    1031.6, "0100000000000010", "")
        .add("2012-03-25 21:00:00",    1031.4,    1031.4, "0100000000000010", "")
        .add("2012-03-25 22:00:00",    1031.4,    1031.4, "0100000000000010", "")
        .add("2012-03-25 23:00:00",    1031.7,    1031.7, "0100000000000010", "")
        .add("2012-03-26 00:00:00",    1032.0,    1032.0, "0100000000000010", "")
        .add("2012-03-26 01:00:00",    1031.9,    1031.9, "0100000000000010", "");
    data.setStation(18700).setType(330)
        .add("2012-03-25 18:00:00",    1030.6,    1030.6, "0100000000000010", "")
        .add("2012-03-25 19:00:00",    1030.7,    1030.7, "0100000000000010", "")
        .add("2012-03-25 20:00:00",    1031.1,    1031.1, "0100000000000010", "")
        .add("2012-03-25 21:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // fake
        .add("2012-03-25 22:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // not fake
        .add("2012-03-25 23:00:00",    1031.5,    1031.5, "0100000000000010", "")
        .add("2012-03-26 00:00:00",    1031.7,    1031.7, "0100000000000010", "")
        .add("2012-03-26 01:00:00",    1031.5,    1031.5, "0100000000000010", "");
    data.setStation(20301).setType(330)
        .add("2012-03-25 18:00:00",    1030.4,    1030.4, "0100000000000010", "")
        .add("2012-03-25 19:00:00",    1031.1,    1031.1, "0100000000000010", "")
        .add("2012-03-25 20:00:00",    1031.4,    1031.4, "0100000000000010", "")
        .add("2012-03-25 21:00:00",    1031.8,    1031.8, "0100000000000010", "")
        .add("2012-03-25 22:00:00",    1032.3,    1032.3, "0100000000000010", "")
        .add("2012-03-25 23:00:00",    1032.5,    1032.5, "0100000000000010", "")
        .add("2012-03-26 00:00:00",    1032.4,    1032.4, "0100000000000010", "")
        .add("2012-03-26 01:00:00",    1032.4,    1032.4, "0100000000000010", "");
    data.setParam(211);
    data.setStation(4200).setType(342)
        .add("2012-03-25 18:00:00",      10.4,      10.4, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       8.6,       8.6, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       4.1,       4.1, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       2.9,       2.9, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       2.0,       2.0, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       1.6,       1.6, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       0.1,       0.1, "0111000000100010", "");
    data.setStation(17150).setType(342)
        .add("2012-03-25 18:00:00",      10.2,      10.2, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.8,       7.8, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       7.4,       7.4, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       6.8,       6.8, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       4.8,       4.8, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       4.1,       4.1, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       2.4,       2.4, "0111000000100010", "");
    data.setStation(18700).setType(330)
        .add("2012-03-25 18:00:00",      11.5,      11.5, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      10.1,      10.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.4,       7.4, "0111000000100010", "")
        .add("2012-03-25 21:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // fake
        .add("2012-03-25 22:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // not fake
        .add("2012-03-25 23:00:00",       5.3,       5.3, "0110000000100010", "")
#if 1
        .add("2012-03-26 00:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // fake
#else
        .add("2012-03-26 00:00:00",       4.0,       4.0, "0111000000100010", "")
#endif
        .add("2012-03-26 01:00:00",       4.0,       4.0, "0111000000100010", "");
    data.setStation(20301).setType(330)
        .add("2012-03-25 18:00:00",      11.7,      11.7, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.0,       7.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       5.2,       5.2, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       4.4,       4.4, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       3.7,       3.7, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       3.3,       3.3, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       2.8,       2.8, "0111000000100010", "");
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
           << "ParamId    =  178\n"
           << "ParamId    =  211\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 5);

    ASSERT_RUN(algo, bc, 0);

    // should be 1031.37, 1031.47 for paramid=178 and 5.92607, 5.75307, 4.98127 for paramid=211
}

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, TAGroup)
{
    DataList data(4200, 211, 342);
    data.add("2012-03-25 18:00:00",      10.4,      10.4, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       8.6,       8.6, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       4.1,       4.1, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       2.9,       2.9, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       2.0,       2.0, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       1.6,       1.6, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       0.1,       0.1, "0111000000100010", "");
    data.setStation(17150).setType(342)
        .add("2012-03-25 18:00:00",      10.2,      10.2, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.8,       7.8, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       7.4,       7.4, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       6.8,       6.8, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       4.8,       4.8, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       4.1,       4.1, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       2.4,       2.4, "0111000000100010", "");
    data.setStation(18700).setType(330)
        .add("2012-03-25 18:00:00",      11.5,      11.5, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      10.1,      10.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.4,       7.4, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       6.1,       6.1, "0111000000100010", "")
        .add("2012-03-25 22:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // not fake
        .add("2012-03-25 23:00:00",       5.3,       5.3, "0110000000100010", "")
        .add("2012-03-26 00:00:00",       4.6,       4.6, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       4.0,       4.0, "0111000000100010", "");
    data.setStation(20301).setType(330)
        .add("2012-03-25 18:00:00",      11.7,      11.7, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.0,       7.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       5.2,       5.2, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       4.4,       4.4, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       3.7,       3.7, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       3.3,       3.3, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       2.8,       2.8, "0111000000100010", "");
    data.setParam(213);
    data.setStation(4200).setType(342)
        .add("2012-03-25 18:00:00",      10.1,      10.1, "0111000000000000", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000000000", "")
        .add("2012-03-25 20:00:00",       8.2,       8.2, "0111000000000000", "")
        .add("2012-03-25 21:00:00",       4.1,       4.1, "0111000000000000", "")
        .add("2012-03-25 22:00:00",       2.6,       2.6, "0111000000000000", "")
        .add("2012-03-25 23:00:00",       2.0,       2.0, "0111000000000000", "")
        .add("2012-03-26 00:00:00",       1.6,       1.6, "0111000000000000", "")
        .add("2012-03-26 01:00:00",       0.0,       0.0, "0111000000000000", "");
    data.setStation(17150).setType(342)
        .add("2012-03-25 18:00:00",      10.2,      10.2, "0111000000000000", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000000000", "")
        .add("2012-03-25 20:00:00",       7.8,       7.8, "0111000000000000", "")
        .add("2012-03-25 21:00:00",       7.4,       7.4, "0111000000000000", "")
        .add("2012-03-25 22:00:00",       6.8,       6.8, "0111000000000000", "")
        .add("2012-03-25 23:00:00",       4.8,       4.8, "0111000000000000", "")
        .add("2012-03-26 00:00:00",       3.5,       3.5, "0111000000000000", "")
        .add("2012-03-26 01:00:00",       2.3,       2.3, "0111000000000000", "");
    data.setStation(18700).setType(330)
        .add("2012-03-25 18:00:00",      11.5,      11.5, "0111000000000000", "")
        .add("2012-03-25 19:00:00",      10.1,      10.1, "0111000000000000", "")
        .add("2012-03-25 20:00:00",       7.4,       7.4, "0111000000000000", "")
        .add("2012-03-25 21:00:00",       6.1,       6.1, "0111000000000000", "")
        .add("2012-03-25 22:00:00",       5.7,       5.7, "0110000000000000", "") // fake, original missing
        .add("2012-03-25 23:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // fake, original = 5.0
        .add("2012-03-26 00:00:00",       4.6,       4.6, "0111000000000000", "")
        .add("2012-03-26 01:00:00",       4.0,       4.0, "0111000000000000", "");
    data.setStation(20301).setType(330)
        .add("2012-03-25 18:00:00",      11.7,      11.7, "0111000000000000", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000000000", "")
        .add("2012-03-25 20:00:00",       7.0,       7.0, "0111000000000000", "")
        .add("2012-03-25 21:00:00",       5.1,       5.1, "0111000000000000", "")
        .add("2012-03-25 22:00:00",       4.3,       4.3, "0111000000000000", "")
        .add("2012-03-25 23:00:00",       3.6,       3.6, "0111000000000000", "")
        .add("2012-03-26 00:00:00",       3.2,       3.2, "0111000000000000", "")
        .add("2012-03-26 01:00:00",       2.6,       2.6, "0111000000000000", "");
    data.setParam(215);
    data.setStation(4200).setType(342)
        .add("2012-03-25 18:00:00",      12.9,      12.9, "0111000000000000", "")
        .add("2012-03-25 19:00:00",      10.5,      10.5, "0111000000000000", "")
        .add("2012-03-25 20:00:00",      10.3,      10.3, "0111000000000000", "")
        .add("2012-03-25 21:00:00",       8.5,       8.5, "0111000000000000", "")
        .add("2012-03-25 22:00:00",       4.8,       4.8, "0111000000000000", "")
        .add("2012-03-25 23:00:00",       3.8,       3.8, "0111000000000000", "")
        .add("2012-03-26 00:00:00",       2.9,       2.9, "0111000000000000", "")
        .add("2012-03-26 01:00:00",       1.8,       1.8, "0111000000000000", "");
    data.setStation(17150).setType(342)
        .add("2012-03-25 18:00:00",      11.2,      11.2, "0111000000000000", "")
        .add("2012-03-25 19:00:00",      10.2,      10.2, "0111000000000000", "")
        .add("2012-03-25 20:00:00",       9.1,       9.1, "0111000000000000", "")
        .add("2012-03-25 21:00:00",       7.8,       7.8, "0111000000000000", "")
        .add("2012-03-25 22:00:00",       7.4,       7.4, "0111000000000000", "")
        .add("2012-03-25 23:00:00",       6.9,       6.9, "0111000000000000", "")
        .add("2012-03-26 00:00:00",       4.8,       4.8, "0111000000000000", "")
        .add("2012-03-26 01:00:00",       4.2,       4.2, "0111000000000000", "");
    data.setStation(18700).setType(330)
        .add("2012-03-25 18:00:00",      13.0,      13.0, "0111000000000000", "")
        .add("2012-03-25 19:00:00",      11.6,      11.6, "0111000000000000", "")
        .add("2012-03-25 20:00:00",      10.1,      10.1, "0111000000000000", "")
        .add("2012-03-25 21:00:00",       7.3,       7.3, "0111000000000000", "")
        .add("2012-03-25 22:00:00",       6.1,       6.1, "0110000000000000", "") // fake, original missing
        .add("2012-03-25 23:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // fake, original = 5.7
        .add("2012-03-26 00:00:00",       5.4,       5.4, "0111000000000000", "")
        .add("2012-03-26 01:00:00",       4.6,       4.6, "0111000000000000", "");
    data.setStation(20301).setType(330)
        .add("2012-03-25 18:00:00",      13.5,      13.5, "0111000000000000", "")
        .add("2012-03-25 19:00:00",      11.7,      11.7, "0111000000000000", "")
        .add("2012-03-25 20:00:00",       9.1,       9.1, "0111000000000000", "")
        .add("2012-03-25 21:00:00",       7.0,       7.0, "0111000000000000", "")
        .add("2012-03-25 22:00:00",       5.4,       5.4, "0111000000000000", "")
        .add("2012-03-25 23:00:00",       5.1,       5.1, "0111000000000000", "")
        .add("2012-03-26 00:00:00",       4.0,       4.0, "0111000000000000", "")
        .add("2012-03-26 01:00:00",       3.3,       3.3, "0111000000000000", "");
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
           << "ParamId    =  211\n"
           << "ParamId    =  213\n"
           << "ParamId    =  215\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, UUGroup)
{
    DataList data(4200, 211, 342);
    data
        .add("2012-03-25 18:00:00",      10.4,      10.4, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       8.6,       8.6, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       4.1,       4.1, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       2.9,       2.9, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       2.0,       2.0, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       1.6,       1.6, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       0.1,       0.1, "0111000000100010", "");
    data.setStation(17150).setType(342)
        .add("2012-03-25 18:00:00",      10.2,      10.2, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.8,       7.8, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       7.4,       7.4, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       6.8,       6.8, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       4.8,       4.8, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       4.1,       4.1, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       2.4,       2.4, "0111000000100010", "");
    data.setStation(18700).setType(330)
        .add("2012-03-25 18:00:00",      11.5,      11.5, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      10.1,      10.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.4,       7.4, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       6.1,       6.1, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       5.7,       5.7, "0111000000100010", "") // fake
        .add("2012-03-25 23:00:00",       5.3,       5.3, "0110000000100010", "")
        .add("2012-03-26 00:00:00",       4.6,       4.6, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       4.0,       4.0, "0111000000100010", "");
    data.setStation(20301).setType(330)
        .add("2012-03-25 18:00:00",      11.7,      11.7, "0111000000100010", "")
        .add("2012-03-25 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.0,       7.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00",       5.2,       5.2, "0111000000100010", "")
        .add("2012-03-25 22:00:00",       4.4,       4.4, "0111000000100010", "")
        .add("2012-03-25 23:00:00",       3.7,       3.7, "0111000000100010", "")
        .add("2012-03-26 00:00:00",       3.3,       3.3, "0111000000100010", "")
        .add("2012-03-26 01:00:00",       2.8,       2.8, "0111000000100010", "");
    data.setParam(262);
    data.setStation(4200).setType(342)
        .add("2012-03-25 18:00:00",      63.0,      63.0, "0101000000000010", "")
        .add("2012-03-25 19:00:00",      65.0,      65.0, "0101000000000010", "")
        .add("2012-03-25 20:00:00",      67.0,      67.0, "0101000000000010", "")
        .add("2012-03-25 21:00:00",      83.0,      83.0, "0101000000000010", "")
        .add("2012-03-25 22:00:00",      91.0,      91.0, "0101000000000010", "")
        .add("2012-03-25 23:00:00",      89.0,      89.0, "0101000000000010", "")
        .add("2012-03-26 00:00:00",      94.0,      94.0, "0101000000000010", "")
        .add("2012-03-26 01:00:00",      96.0,      96.0, "0101000000000010", "");
    data.setStation(17150).setType(342)
        .add("2012-03-25 18:00:00",      54.0,      54.0, "0101000000000010", "")
        .add("2012-03-25 19:00:00",      56.0,      56.0, "0101000000000010", "")
        .add("2012-03-25 20:00:00",      61.0,      61.0, "0101000000000010", "")
        .add("2012-03-25 21:00:00",      61.0,      61.0, "0101000000000010", "")
        .add("2012-03-25 22:00:00",      65.0,      65.0, "0101000000000010", "")
        .add("2012-03-25 23:00:00",      70.0,      70.0, "0101000000000010", "")
        .add("2012-03-26 00:00:00",      77.0,      77.0, "0101000000000010", "")
        .add("2012-03-26 01:00:00",      83.0,      83.0, "0101000000000010", "");
    data.setStation(18700).setType(330)
        .add("2012-03-25 18:00:00",      53.0,      53.0, "0101000000000010", "")
        .add("2012-03-25 19:00:00",      57.0,      57.0, "0101000000000010", "")
        .add("2012-03-25 20:00:00",      69.0,      69.0, "0101000000000010", "")
        .add("2012-03-25 21:00:00",      74.0,      74.0, "0101000000000010", "")
        .add("2012-03-25 22:00:00",  -32767.0,  -32767.0, "0000003000000000", "") // not fake
        .add("2012-03-25 23:00:00",      79.0,      79.0, "0100000000000010", "")
        .add("2012-03-26 00:00:00",      80.0,      80.0, "0101000000000010", "")
        .add("2012-03-26 01:00:00",      82.0,      82.0, "0101000000000010", "");
    data.setStation(20301).setType(330)
        .add("2012-03-25 18:00:00",      49.0,      49.0, "0101000000000010", "")
        .add("2012-03-25 19:00:00",      60.0,      60.0, "0101000000000010", "")
        .add("2012-03-25 20:00:00",      67.0,      67.0, "0101000000000010", "")
        .add("2012-03-25 21:00:00",      73.0,      73.0, "0101000000000010", "")
        .add("2012-03-25 22:00:00",      76.0,      76.0, "0101000000000010", "")
        .add("2012-03-25 23:00:00",      78.0,      78.0, "0101000000000010", "")
        .add("2012-03-26 00:00:00",      80.0,      80.0, "0101000000000010", "")
        .add("2012-03-26 01:00:00",      82.0,      82.0, "0101000000000010", "");
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
           << "TypeId     =  342\n"
           << "ParamId    =  211\n"
           << "ParamId    =  262\n"
           << "ParamId    =  264\n"
           << "ParamId    =  265\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);

    ASSERT_RUN(algo, bc, 0);
}
