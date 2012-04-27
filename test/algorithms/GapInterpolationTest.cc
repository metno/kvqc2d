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
#include "helpers/AlgorithmHelpers.h"
#include "interpolation/GapInterpolationAlgorithm.h"

class GapInterpolationTest : public AlgorithmTestBase {
public:
    void SetUp();
};

namespace {
void INSERT_NEIGHBOR(std::ostream& sql, int stationid, int paramid, int neighborid, float offset=0, float slope=1, float sigma=1)
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
    sql << "INSERT INTO station VALUES( 1380, 59.376, 11.303, 130, 0, 'GAUTESTAD', NULL, 1380, NULL, NULL, NULL, 0, 't', '2010-01-21 00:00:00');"
        << "INSERT INTO station VALUES( 4460, 60.117, 10.829, 170, 0, 'HAKADAL JERNBANESTASJON', 1488, 4460, NULL, NULL, NULL, 8, 't', '2007-01-08 00:00:00');"
        << "INSERT INTO station VALUES(18700, 59.942, 10.720,  94, 0, 'OSLO - BLINDERN', 1492, 18700, NULL, NULL, NULL, 8, 't', '1937-02-25 00:00:00');\n"
        << "INSERT INTO station VALUES(69150, 63.488, 10.880,  40, 0, 'KVITHAMAR', 69150, NULL, NULL, NULL, NULL, 2, 't', '1987-05-12 00:00:00');\n"
        << "INSERT INTO station VALUES(90720, 69.986, 18.687, 47.0, 0.0, 'MÅSVIK', 1021, 90720, NULL, NULL, NULL, 8, 't', '2003-10-08 00:00:00');"
        << "INSERT INTO station VALUES(99754, 77.000, 15.500,  10, 0, 'HORNSUND', 1003, 99754, NULL, NULL, NULL, 8, 't', '1985-06-01 00:00:00');";

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

    ASSERT_NO_THROW(db->exec(sql.str()));

    NeighborDataVector n;
    ASSERT_NO_THROW(n = db->findNeighborData(18700, 211, 2.7));
    ASSERT_EQ(10, n.size());

    ASSERT_NO_THROW(n = db->findNeighborData(18700, 178, 2.7));
    ASSERT_EQ(10, n.size());

    ASSERT_NO_THROW(n = db->findNeighborData(69150, 211, 2.7));
    ASSERT_EQ(10, n.size());
}

TEST_F(GapInterpolationTest, test1)
{
    DataList data(68290, 211, 330);
    data.add("2012-03-25 18:00:00",      4.0, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      3.0, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      2.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      3.0, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      2.0, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      1.0, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      1.0, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.0, "0111000000100010", "");
    data.setStation(69100)
        .add("2012-03-25 18:00:00",      4.1, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      3.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      2.1, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      3.1, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      2.1, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      1.1, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      1.1, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.1, "0111000000100010", "");
    data.setStation(69150)
        .add("2012-03-25 18:00:00",      3.0, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      2.0, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      1.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00", -32767.0, "0000003000000000", "")
        .add("2012-03-25 22:00:00", -32767.0, "0000003000000000", "")
        .add("2012-03-25 23:00:00",      0.0, "0110000000100010", "")
        .add("2012-03-26 00:00:00",      0.0, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      0.0, "0111000000100010", "");
    data.setStation(69380)
        .add("2012-03-25 18:00:00",      4.2, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      3.2, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      2.2, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      3.2, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      2.2, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      1.2, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      1.2, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.2, "0111000000100010", "");
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
           << "Parameter  =  par=211,minVal=-100,maxVal=100,offsetCorrectionLimit=15\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);
    EXPECT_NEAR(2, bc->update(0).corrected(), 0.01);
    EXPECT_NEAR(1, bc->update(1).corrected(), 0.01);

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, GapInMultiplePar)
{
    DataList data(18700, 178, 330);
    data.add("2012-03-25 18:00:00",    1030.6, "0100000000000010", "")
        .add("2012-03-25 19:00:00",    1030.7, "0100000000000010", "")
        .add("2012-03-25 20:00:00",    1031.1, "0100000000000010", "")
        .add("2012-03-25 21:00:00",  -32767.0, "0000003000000000", "")
        .add("2012-03-25 22:00:00",  -32767.0, "0000003000000000", "")
        .add("2012-03-25 23:00:00",    1031.4, "0100000000000010", "")
        .add("2012-03-26 00:00:00",    1031.7, "0100000000000010", "")
        .add("2012-03-26 01:00:00",    1031.5, "0100000000000010", "");
    data.setParam(211);
    data.setStation(18700).setType(330)
        .add("2012-03-25 18:00:00",      11.5, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      10.1, "0111000000100010", "")
        .add("2012-03-25 20:00:00",       7.4, "0111000000100010", "")
        .add("2012-03-25 21:00:00",  -32767.0, "0000003000000000", "")
        .add("2012-03-25 22:00:00",  -32767.0, "0000003000000000", "")
        .add("2012-03-25 23:00:00",       4.4, "0110000000100010", "")
        .add("2012-03-26 00:00:00",  -32767.0, "0000003000000000", "")
        .add("2012-03-26 01:00:00",       4.4, "0111000000100010", "");

    const int neighborStations[] = { 4200, 17150, 20301, -1 };
    const int neighborTypeIds[] = { 342, 342, 330, -1 };
    for(int i=0; neighborStations[i]>0; ++i) {
        data.setParam(178);
        data.setStation(neighborStations[i]).setType(neighborTypeIds[i])
            .add("2012-03-25 18:00:00", 1031.1, "0100000000000010", "")
            .add("2012-03-25 19:00:00", 1031.6, "0100000000000010", "")
            .add("2012-03-25 20:00:00", 1031.6, "0100000000000010", "")
            .add("2012-03-25 21:00:00", 1031.4, "0100000000000010", "")
            .add("2012-03-25 22:00:00", 1031.4, "0100000000000010", "")
            .add("2012-03-25 23:00:00", 1031.6, "0100000000000010", "")
            .add("2012-03-26 00:00:00", 1032.0, "0100000000000010", "")
            .add("2012-03-26 01:00:00", 1031.9, "0100000000000010", "");
        data.setParam(211)
            .add("2012-03-25 18:00:00",   10.4, "0111000000100010", "")
            .add("2012-03-25 19:00:00",    9.1, "0111000000100010", "")
            .add("2012-03-25 20:00:00",    8.6, "0111000000100010", "")
            .add("2012-03-25 21:00:00",    4.1, "0111000000100010", "")
            .add("2012-03-25 22:00:00",    2.9, "0111000000100010", "")
            .add("2012-03-25 23:00:00",    2.6, "0111000000100010", "")
            .add("2012-03-26 00:00:00",    1.6, "0111000000100010", "")
            .add("2012-03-26 01:00:00",    2.6, "0111000000100010", "");
    }
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
           << "Parameter  =  par=211,minPar=213,maxPar=215,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  =  par=178,minVal=800,maxVal=1200,offsetCorrectionLimit=5\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 5);
    EXPECT_NEAR(1031.0, bc->update(0).corrected(), 0.01);
    EXPECT_NEAR(1031.1, bc->update(1).corrected(), 0.01);
    EXPECT_NEAR(3.9, bc->update(2).corrected(), 0.01);
    EXPECT_NEAR(3.7, bc->update(3).corrected(), 0.01);
    EXPECT_NEAR(3.4, bc->update(4).corrected(), 0.01);

    ASSERT_RUN(algo, bc, 0);
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
           << "Parameter  =  par=211,minPar=213,maxPar=215,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  =  par=178,minVal=800,maxVal=1200,offsetCorrectionLimit=5\n";
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
           << "Parameter  =  par=211,minPar=213,maxPar=215,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  =  par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);
    EXPECT_NEAR(77, bc->update(0).corrected(), 1);

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, OnlyMissing)
{
    DataList data(1380, 211, 502);
    miutil::miTime t("2012-04-01 00:00:00");
    for(int i=0; i<21*24; ++i) {
        data.add(t, -32767, "0000003000000000", "");
        t.addHour(1);
    }
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   04\n"
           << "Start_DD   =   10\n"
           << "Start_hh   =    0\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   04\n"
           << "End_DD     =   10\n"
           << "End_hh     =    3\n"
           << "TypeId     =  502\n"
           << "Parameter  =  par=211,minVal=-100,maxVal=100,offsetCorrectionLimit=15\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, DoNotSetMaxTo100)
{
    DataList data(99754, 211, 330);
    data.add("2012-04-16 05:00:00",     -10.9,     -10.9, "0111000000100010", "")
        .add("2012-04-16 06:00:00",     -10.4,     -10.4, "0111000000100010", "")
        .add("2012-04-16 07:00:00",      -7.8,      -7.8, "0111000000100010", "")
        .add("2012-04-16 08:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2012-04-16 09:00:00",      -8.0,      -8.0, "0110000000100010", "")
        .add("2012-04-16 10:00:00",      -7.0,      -7.0, "0111000000100010", "")
        .add("2012-04-16 11:00:00",      -7.5,      -7.5, "0111000000100010", "");
    data.setParam(213)
        .add("2012-04-16 05:00:00",     -11.2,     -11.2, "0111000000000000", "")
        .add("2012-04-16 06:00:00",     -10.9,     -10.9, "0111000000000000", "")
        .add("2012-04-16 07:00:00",     -10.4,     -10.4, "0111000000000000", "")
        .add("2012-04-16 08:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2012-04-16 09:00:00",      -9.0,      -9.0, "0110000000000000", "")
        .add("2012-04-16 10:00:00",      -8.0,      -8.0, "0111000000000000", "")
        .add("2012-04-16 11:00:00",      -7.7,      -7.7, "0111000000000000", "");
    data.setParam(215)
        .add("2012-04-16 05:00:00",      -9.4,      -9.4, "0111000000000000", "")
        .add("2012-04-16 06:00:00",      -9.1,      -9.1, "0111000000000000", "")
        .add("2012-04-16 07:00:00",      -7.8,      -7.8, "0111000000000000", "")
        .add("2012-04-16 08:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2012-04-16 09:00:00",      -6.0,      -6.0, "0110000000000000", "")
        .add("2012-04-16 10:00:00",      -6.9,      -6.9, "0111000000000000", "")
        .add("2012-04-16 11:00:00",      -5.7,      -5.7, "0111000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   04\n"
           << "Start_DD   =   16\n"
           << "Start_hh   =    0\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   04\n"
           << "End_DD     =   17\n"
           << "End_hh     =    0\n"
           << "TypeId     =  330\n"
           << "Parameter = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=100,offsetCorrectionLimit=15,fluctuationLevel=0.5\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);
    EXPECT_NEAR( -7.9, bc->update(0).corrected(), 0.1);
    EXPECT_NEAR(-10,   bc->update(1).corrected(), 2);
    EXPECT_NEAR( -6,   bc->update(2).corrected(), 2);
}

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, VeryLittleSnow)
{
    DataList data(4460, 112, 342); // corrected values are mostly fake
    data.add("2012-04-15 00:00:00",      1.2, "0111000000100010", "")
        .add("2012-04-15 01:00:00",      1.3, "0111000000100010", "")
        .add("2012-04-15 02:00:00",      1.5, "0111000000100010", "")
        .add("2012-04-15 03:00:00",      1.6, "0111000000100010", "")
        .add("2012-04-15 04:00:00",     -1,   "0111000000100010", "")
        .add("2012-04-15 05:00:00",     -1,   "0111000000100010", "")
        .add("2012-04-15 06:00:00",     -1,   "0111000000100010", "")
        .add("2012-04-15 07:00:00", -32767,   "0000003000000000", "")
        .add("2012-04-15 08:00:00", -32767,   "0000003000000000", "")
        .add("2012-04-15 09:00:00",     -1,   "0111000000100010", "")
        .add("2012-04-15 10:00:00",     -1,   "0111000000100010", "")
        .add("2012-04-15 11:00:00",     -1,   "0111000000100010", "")
        .add("2012-04-15 12:00:00",      1.2, "0111000000100010", "")
        .add("2012-04-15 13:00:00",      1.5, "0111000000100010", "")
        .add("2012-04-15 14:00:00",      1.4, "0111000000100010", "")
        .add("2012-04-15 15:00:00",      1.3, "0111000000100010", "")
        .add("2012-04-15 16:00:00",      1.4, "0111000000100010", "")
        .add("2012-04-15 17:00:00",      1.2, "0111000000100010", "")
        .add("2012-04-15 18:00:00",      1.3, "0111000000100010", "")
        .add("2012-04-15 19:00:00",      1.2, "0111000000100010", "")
        .add("2012-04-15 20:00:00",      1.1, "0111000000100010", "")
        .add("2012-04-15 21:00:00",      1.2, "0111000000100010", "")
        .add("2012-04-15 22:00:00",      1.2, "0111000000100010", "")
        .add("2012-04-15 23:00:00",      1.2, "0111000000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   04\n"
           << "Start_DD   =   15\n"
           << "Start_hh   =    0\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   04\n"
           << "End_DD     =   16\n"
           << "End_hh     =    0\n"
           << "TypeId     =  342\n"
           << "Parameter = par=112,minVal=0,offsetCorrectionLimit=15\n";
    AlgorithmConfig params;
    ASSERT_PARSE_CONFIG(params, config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    for(int i=0; i<bc->count(); ++i)
        EXPECT_EQ( -1, bc->update(i).corrected());
}

// ------------------------------------------------------------------------

TEST_F(GapInterpolationTest, UUNoJump1)
{
    DataList data(90720, 211, 342);
    data.add("2012-04-26 17:00:00",       8.6,       8.6, "0111000000100010", "")
        .add("2012-04-26 18:00:00",       8.2,       8.2, "0111000000100010", "")
        .add("2012-04-26 19:00:00",       9.1,       9.1, "0111000000100010", "")
        .add("2012-04-26 20:00:00",       8.2,       8.2, "0111000000100010", "")
        .add("2012-04-26 21:00:00",       8.6,       8.6, "0111000000100010", "")
        .add("2012-04-26 22:00:00",       7.3,       7.3, "0111000000100010", "")
        .add("2012-04-26 23:00:00",       7.7,       7.7, "0111000000100010", "");
    data.setParam(262);
    data.setStation(90720).setType(342)
        .add("2012-04-26 17:00:00",      89.0,      89.0, "0101000000000010", "")
        .add("2012-04-26 18:00:00",      89.8,      89.8, "0101000000000010", "")
        .add("2012-04-26 19:00:00",      90.2,      90.2, "0101000000000010", "")
        .add("2012-04-26 20:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2012-04-26 20:00:00",  -32767.0,  -32767.0, "0000003000000000", "")
        .add("2012-04-26 21:00:00",      92.4,      92.4, "0100000000000010", "")
        .add("2012-04-26 22:00:00",      93.0,      93.0, "0101000000000010", "")
        .add("2012-04-26 23:00:00",      93.0,      93.0, "0101000000000010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   04\n"
           << "Start_DD   =   25\n"
           << "Start_hh   =   17\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   04\n"
           << "End_DD     =   26\n"
           << "End_hh     =   23\n"
           << "TypeId     =  342\n"
           << "Parameter  = par=211,minPar=213,maxPar=215,minVal=-80,maxVal=80,offsetCorrectionLimit=15,fluctuationLevel=0.5\n"
           << "Parameter  = par=262,minPar=264,maxPar=265,minVal=0,maxVal=100,offsetCorrectionLimit=5,fluctuationLevel=2\n";
    AlgorithmConfig params;
    ASSERT_PARSE_CONFIG(params, config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);
    EXPECT_NEAR(91.1, bc->update(0).corrected(), 0.3);
}
