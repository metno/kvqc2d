/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011-2012 met.no

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
#include "algorithms/RedistributionAlgorithm.h"
#include "helpers/mathutil.h"
#include "foreach.h"

#include <boost/algorithm/string/predicate.hpp>
#include <algorithm>
#include <numeric>

#define FD8 1
#ifdef FD8
#define END_FD "8"
#else
#define END_FD "7"
#endif

class RedistributionTest : public AlgorithmTestBase {
public:
    void SetUp();
    void Configure(AlgorithmConfig& params, int startDay, int endDay);
    void RoundingTest(const float* values, const float* expected, const int N);
};

// ========================================================================

void RedistributionTest::SetUp()
{
    algo = new RedistributionAlgorithm();
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(83880, 68.0645, 16.663,    3, 0, 'SØRFJORD KRAFTVERK',    NULL, 83880, NULL, NULL, NULL, 10, 't', '1985-01-01 00:00:00');"
        << "INSERT INTO station VALUES(83520, 67.8977, 15.8673,  70, 0, 'TØMMERNESET',           NULL, 83520, NULL, NULL, NULL,  9, 't', '1985-07-01 00:00:00');"
        << "INSERT INTO station VALUES(84070, 68.3302, 16.7883,  53, 0, 'BJØRKÅSEN',             NULL, 84070, NULL, NULL, NULL, 10, 't', '1964-01-01 00:00:00');"
        << "INSERT INTO station VALUES(84190, 68.2082, 17.5157,  29, 0, 'SKJOMEN - STIBERG',     NULL, 84190, NULL, NULL, NULL,  9, 't', '1987-09-01 00:00:00');"
        << "INSERT INTO station VALUES( 1230, 59.1223, 11.3865,   8, 0, 'HALDEN',                NULL,  1230, NULL, NULL, NULL,  9, 't', '1882-12-01 00:00:00');"
        << "INSERT INTO station VALUES( 3200, 59.3072, 11.1338,  31, 0, 'BATERØD',               NULL,  3200, NULL, NULL, NULL,  9, 't', '1942-01-01 00:00:00');"
        << "INSERT INTO station VALUES( 4040, 59.7512, 11.1532, 164, 0, 'ENEBAKK - BARBØL',      NULL,  4040, NULL, NULL, NULL,  9, 't', '1998-05-15 00:00:00');"
        << "INSERT INTO station VALUES(88100, 68.6457, 18.2455, 230, 0, 'BONES I BARDU',         NULL, 88100, NULL, NULL, NULL,  9, 't', '1907-05-01 00:00:00');"
        << "INSERT INTO station VALUES(89650, 68.6577, 18.8193, 314, 0, 'INNSET I BARDU',        NULL, 89650, NULL, NULL, NULL, 10, 't', '1907-05-01 00:00:00');"
        << "INSERT INTO station VALUES( 3780,  59.636, 11.0468, 144, 0, 'IGSI I HOBAAL',         NULL,  3780, NULL, NULL, NULL,  9, 't', '1908-01-01 00:00:00');"
        << "INSERT INTO station VALUES(17251, 59.4428, 10.6842,  32, 0, 'MOSS BRANNSTASJON',     NULL, 17251, NULL, NULL, NULL,  9, 't', '2004-04-10 00:00:00');"
        << "INSERT INTO station VALUES(17780, 59.8098, 10.6325, 104, 0, 'BLEKSLITJERN',          NULL, 17780, NULL, NULL, NULL, 10, 't', '1967-06-01 00:00:00');"
        << "INSERT INTO station VALUES(18160,  59.873, 10.7912, 118, 0, 'NORDSTRAND',            NULL, 18160, NULL, NULL, NULL,  9, 't', '1935-01-01 00:00:00');"
        << "INSERT INTO station VALUES(18450, 59.9719, 10.7894, 173, 0, 'MARIDALSOSET',          NULL, 18450, NULL, NULL, NULL, 10, 't', '1972-07-06 00:00:00');"
        << "INSERT INTO station VALUES(19930, 59.8683, 10.0563, 375, 0, 'GLITRE',                NULL, 19930, NULL, NULL, NULL, 10, 't', '1979-10-01 00:00:00');"
        << "INSERT INTO station VALUES(20520, 60.2943, 10.5803, 372, 0, 'LUNNER',                NULL, 20520, NULL, NULL, NULL,  9, 't', '1895-01-01 00:00:00');"
        << "INSERT INTO station VALUES(24600, 60.1377,   9.597, 367, 0, 'GRIMELI I KROEDSHERAD', NULL, 24600, NULL, NULL, NULL,  9, 't', '1895-07-01 00:00:00');"
        << "INSERT INTO station VALUES(27301, 59.3562, 10.2501,  30, 0, 'RAMNES - BERG',         NULL, 27301, NULL, NULL, NULL,  9, 't', '2003-05-20 00:00:00');"
        << "INSERT INTO station VALUES(30380, 59.4558,  9.5337, 475, 0, 'GODAL',                 NULL, 30380, NULL, NULL, NULL,  9, 't', '2011-01-17 00:00:00');"
        << "INSERT INTO station VALUES(30530, 59.5688,  9.2643,  34, 0, 'NOTODDEN',              NULL, 30530, NULL, NULL, NULL, 10, 't', '1923-01-01 00:00:00');";

    sql << "INSERT INTO station_param VALUES(0, 110, 0, 0,   1, 365, -1, 'QC1-1-110', 'max;highest;high;low;lowest;min\n150;120.0;100.0;-1.0;-1.0;-1', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,   1, 365, -1, 'QC1-1-110x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 274, 304, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 305, 334, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 335, 365, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,   1,  31, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  32,  59, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  60,  90, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  91, 120, -1, 'QC1-2-72.b4', 'R1\n5',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 121, 151, -1, 'QC1-2-72.b4', 'R1\n3',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 152, 181, -1, 'QC1-2-72.b4', 'R1\n1',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 182, 212, -1, 'QC1-2-72.b4', 'R1\n0',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 213, 243, -1, 'QC1-2-72.b4', 'R1\n0',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 244, 273, -1, 'QC1-2-72.b4', 'R1\n3',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 274, 304, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 305, 334, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 335, 365, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,   1,  31, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  32,  59, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  60,  90, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  91, 120, -1, 'QC1-2-72.c4', 'R1\n5',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 121, 151, -1, 'QC1-2-72.c4', 'R1\n3',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 152, 181, -1, 'QC1-2-72.c4', 'R1\n1',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 182, 212, -1, 'QC1-2-72.c4', 'R1\n0',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 213, 243, -1, 'QC1-2-72.c4', 'R1\n0',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 244, 273, -1, 'QC1-2-72.c4', 'R1\n3',  '', '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));
}

// ------------------------------------------------------------------------

void RedistributionTest::Configure(AlgorithmConfig& params, int startDay, int endDay)
{
    std::stringstream config;
    config << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   10" << std::endl
           << "Start_DD   =   " << startDay << std::endl
           << "Start_hh   =   06" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   10" << std::endl
           << "End_DD     =   " << endDay << std::endl
           << "End_hh     =   06" << std::endl
           << "ParamId=110"  << std::endl
           << "TypeIds=302"  << std::endl
           << "TypeIds=402"  << std::endl
           << "InterpolationDistance=50.0"  << std::endl;
    ASSERT_PARSE_CONFIG(params, config);
}

// ------------------------------------------------------------------------

void RedistributionTest::RoundingTest(const float* values, const float* expected, const int N)
{
    const float rounded_acc = Helpers::round1(std::accumulate(values, values+N, 0.0));

    float rounded_values[N];
    std::transform(values, values+N, rounded_values, Helpers::round1);
    const float acc_of_rounded = Helpers::round1(std::accumulate(rounded_values, rounded_values+N, 0.0)), scaling = rounded_acc / acc_of_rounded;

    float interpolated_values[N];
    for(int i=0; i<N; ++i)
        interpolated_values[i] = Helpers::round1(rounded_values[i] * scaling);
    const float acc_of_interpolated = Helpers::round1(std::accumulate(interpolated_values, interpolated_values+N, 0.0));
    const float delta = acc_of_interpolated - rounded_acc;
    ASSERT_LE(0.05, fabs(delta)) << "values do not expose problem under test";

    DataList data(83880, 110, 302);
    data.add("2011-10-13 06:00:00",    0.5,      "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-14 06:00:00", -32767,      "0000003000002000", "QC1-7-110")
        .add("2011-10-15 06:00:00", -32767,      "0000003000002000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767,      "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00", -32767,      "0000003000002000", "QC1-7-110")
        .add("2011-10-18 06:00:00", -32767,      "0000003000002000", "QC1-7-110")
        .add("2011-10-19 06:00:00", rounded_acc, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-13 06:00:00",    0.5,      "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-13 06:00:00",    0.1,      "0110000000001000", "");

    for(int i=0; i<N; ++i) {
        const kvtime::time t = kvtime::maketime(2011, 10, 14 + i, 6, 0, 0);
        data.setStation(83520)
            .add(t, rounded_values[i], "0110000000001000", "")
            .setStation(84190)
            .add(t, rounded_values[i], "0110000000001000", "");
    }
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 19);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 6);

    float acc_of_corrected = 0;
    for(int i=0; i<bc->count(); ++i) {
        const kvalobs::kvData& d = bc->updates()[i];
        SCOPED_TRACE(testing::Message() << "Update #" << i);
        EXPECT_EQ(83880, d.stationID());
        EXPECT_NEAR(expected[i], d.corrected(), 0.15) << "aor=" << acc_of_rounded << " ra=" << rounded_acc << " v[" << i << "]=" << values[i] << " interp=" << interpolated_values[i];
        EXPECT_EQ(i==bc->count()-1 ? "014000400000" END_FD "000" : "0000001000007000", d.controlinfo().flagstring());
        acc_of_corrected += d.corrected();
    }
    EXPECT_FLOAT_EQ(rounded_acc, acc_of_corrected);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, Station83880History2011117)
{
    DataList data(83880, 110, 302);
    data.add("2011-10-10 06:00:00",   16.9, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00",    6.5, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-14 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00",   38.3, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-18 06:00:00", -32767, "0000003000000000", "")
        .add("2011-10-19 06:00:00",    0.6, "0140000000000000", "QC1-2-72.b12")
        .setStation(83520)
        .add("2011-10-10 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    5.7, "0110000000001000", "")
        .add("2011-10-16 06:00:00",   54.2, "0110000000001000", "")
        .add("2011-10-17 06:00:00",   11.4, "0110000000001000", "")
        .add("2011-10-18 06:00:00",    6.7, "0110000000001000", "")
        .add("2011-10-19 06:00:00",    0.7, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-10 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    4.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    0.2, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    6.4, "0110000000001000", "")
        .add("2011-10-17 06:00:00",      2, "0110000000001000", "")
        .add("2011-10-18 06:00:00",    0.3, "0110000000001000", "")
        .add("2011-10-19 06:00:00",    7.4, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-10 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.6, "0110000000001000", "")
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-18 06:00:00",   66.7, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-19 06:00:00",    1.4, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 13, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 4);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0000001000007000",  0.9, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-15 06:00:00", "0000001000007000",  2.8, bc->update(1));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-16 06:00:00", "0000001000007000", 28.3, bc->update(2));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "014000400000" END_FD "000",  6.3, bc->update(3));
    ASSERT_EQ(1, logs->count("QC2-redist-endpoint"));

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, SeriesPossiblyIncomplete)
{
    DataList data(83880, 110, 302);
    data.add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-18 06:00:00", -32767, "0000003000000000", "")
        .add("2011-10-19 06:00:00",    0.6, "0140000000000000", "QC1-2-72.b12")
        .setStation(83520)
        .add("2011-10-16 06:00:00",   54.2, "0110000000001000", "")
        .add("2011-10-17 06:00:00",   11.4, "0110000000001000", "")
        .add("2011-10-18 06:00:00",    6.7, "0110000000001000", "")
        .add("2011-10-19 06:00:00",    0.7, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-16 06:00:00",    6.4, "0110000000001000", "")
        .add("2011-10-17 06:00:00",      2, "0110000000001000", "")
        .add("2011-10-18 06:00:00",    0.3, "0110000000001000", "")
        .add("2011-10-19 06:00:00",    7.4, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-18 06:00:00",   66.7, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-19 06:00:00",    1.4, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 15, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    data.setStation(83880)
        .add("2011-10-15 06:00:00",    6.7, "0140000000001000", "QC1-2-72.b12")
        .setStation(83520)
        .add("2011-10-15 06:00:00",    5.7, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-15 06:00:00",    0.2, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    ASSERT_RUN(algo, bc, 2);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, StartOfDatabase)
{
    // may not redistribute if first missing value is start of database
    DataList data(83880, 110, 302);
    data.add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-18 06:00:00",    0.6, "0140000000000000", "QC1-2-72.b12")
        .setStation(83520)
        .add("2011-10-16 06:00:00",   54.2, "0110000000001000", "")
        .add("2011-10-17 06:00:00",   11.4, "0110000000001000", "")
        .add("2011-10-18 06:00:00",    6.7, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-16 06:00:00",    6.4, "0110000000001000", "")
        .add("2011-10-17 06:00:00",      2, "0110000000001000", "")
        .add("2011-10-18 06:00:00",    0.3, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-18 06:00:00",   66.7, "0140004000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 14, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    data.add(83880, "2011-10-15 06:00:00", 54.2, "0110000000001000", "")
        .add(83520, "2011-10-15 06:00:00", 54.2, "0110000000001000", "")
        .add(84190, "2011-10-15 06:00:00", 54.2, "0110000000001000", "")
        .add(84070, "2011-10-15 06:00:00", 54.2, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    ASSERT_RUN(algo, bc, 2);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, TwoSeries)
{
    // redistribute twice if there are two accumulations
   DataList data(83880, 110, 302);
   data.add("2011-10-10 06:00:00",   16.9, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
       .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
       .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
       .add("2011-10-14 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
       .add("2011-10-15 06:00:00",    6.5, "0140000000001000", "QC1-2-72.b12")
       .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
       .add("2011-10-17 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
       .add("2011-10-18 06:00:00", -32767, "0000003000000000", "")
       .add("2011-10-19 06:00:00",    0.6, "0140000000000000", "QC1-2-72.b12")
        .setStation(83520)
       .add("2011-10-10 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
       .add("2011-10-13 06:00:00",    2.5, "0110000000001000", "")
       .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
       .add("2011-10-15 06:00:00",    5.7, "0110000000001000", "")
       .add("2011-10-16 06:00:00",   54.2, "0110000000001000", "")
       .add("2011-10-17 06:00:00",   11.4, "0110000000001000", "")
       .add("2011-10-18 06:00:00",    6.7, "0110000000001000", "")
       .add("2011-10-19 06:00:00",    0.7, "0110000000001000", "")
        .setStation(84190)
       .add("2011-10-10 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-13 06:00:00",    4.5, "0110000000001000", "")
       .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
       .add("2011-10-15 06:00:00",    0.2, "0110000000001000", "")
       .add("2011-10-16 06:00:00",    6.4, "0110000000001000", "")
       .add("2011-10-17 06:00:00",      2, "0110000000001000", "")
       .add("2011-10-18 06:00:00",    0.3, "0110000000001000", "")
       .add("2011-10-19 06:00:00",    7.4, "0110000000001000", "")
        .setStation(84070)
       .add("2011-10-10 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
       .add("2011-10-13 06:00:00",    2.1, "0110000000001000", "")
       .add("2011-10-14 06:00:00",    0.6, "0110000000001000", "")
       .add("2011-10-15 06:00:00",   1.00, "0110000000001000", "")
       .add("2011-10-16 06:00:00",   1.00, "0110000000001000", "")
       .add("2011-10-17 06:00:00",   1.00, "0110000000001000", "")
       .add("2011-10-18 06:00:00",   1.00, "0110000000001000", "")
       .add("2011-10-19 06:00:00",    1.4, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 4);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000",  9.4, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000",  3.4, bc->update(1));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-16 06:00:00", "0000001000007000", 10.3, bc->update(2));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "014000400000" END_FD "000",  2.5, bc->update(3));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, MissingRows)
{
    // redistribute also if station has missing rows
   DataList data(83880, 110, 302);
   data .add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        // missing some rows here
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00",   10.0, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    2.0, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    2.0, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    2.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 12, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 5);

    for(int i=0; i<bc->count()-1; ++i)
        EXPECT_STATION_OBS_CONTROL_CORR(83880, kvtime::maketime(2011, 10, 13+i, 6, 0, 0), "0000001000007000", 2.0, bc->update(i));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "014000400000" END_FD "000", 2.0, bc->update(4));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, ReRun)
{
    // redistribute also if station has missing rows
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        // missing some rows here
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00",   10.0, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    2.0, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    2.0, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    2.0, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    2.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 12, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 5);

    std::list<kvalobs::kvData> series;
    ASSERT_NO_THROW(series = db->extractData("SELECT * FROM data WHERE stationid = 83880 AND obstime BETWEEN '2011-10-12 06:00:00' AND '2011-10-17 06:00:00';"));
    ASSERT_EQ(6, series.size());

    for(int i=0; i<bc->count()-1; ++i)
        EXPECT_STATION_OBS_CONTROL_CORR(83880, kvtime::maketime(2011, 10, 13+i, 6, 0, 0), "0000001000007000", 2.0, bc->update(i));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "014000400000" END_FD "000", 2.0, bc->update(4));

    std::stringstream config;
    config << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   10" << std::endl
           << "Start_DD   =   12" << std::endl
           << "Start_hh   =   06" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   10" << std::endl
           << "End_DD     =   17" << std::endl
           << "End_hh     =   06" << std::endl
           << "ParamId=110"  << std::endl
           << "TypeIds=302"  << std::endl
           << "TypeIds=402"  << std::endl
#ifndef FD8
           << "endpoint_cflags     = fmis=4&fd=7" << std::endl //changed wrt. default configuration
#else /* FD8 */
           << "endpoint_cflags     = fmis=4&fd=8" << std::endl //changed wrt. default configuration
#endif /* FD8 */
           << "missingpoint_cflags = fmis=1&fd=7" << std::endl //changed wrt. default configuration
           << "InterpolationDistance=50.0"  << std::endl;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    ASSERT_NO_THROW(series = db->extractData("SELECT * FROM data WHERE stationid = 83880 AND obstime BETWEEN '2011-10-12 06:00:00' AND '2011-10-17 06:00:00';"));
    ASSERT_EQ(6, series.size());

    std::ostringstream sql;
    sql << "UPDATE data SET original = 3.0, corrected = 3.0 WHERE obstime = '2011-10-13 06:00:00' AND stationid IN (83520, 84190, 84070);"
        << "UPDATE data SET original = 1.0, corrected = 1.0 WHERE obstime = '2011-10-14 06:00:00' AND stationid IN (83520, 84190, 84070);";
    ASSERT_NO_THROW(db->exec(sql.str()));

    ASSERT_NO_THROW(series = db->extractData("SELECT * FROM data WHERE stationid = 83880 AND obstime BETWEEN '2011-10-12 06:00:00' AND '2011-10-17 06:00:00';"));
    ASSERT_EQ(6, series.size());

    ASSERT_RUN(algo, bc, 2);

    ASSERT_NO_THROW(series = db->extractData("SELECT * FROM data WHERE stationid = 83880 AND obstime BETWEEN '2011-10-12 06:00:00' AND '2011-10-17 06:00:00';"));
    ASSERT_EQ(6, series.size());

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 3.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0000001000007000", 1.0, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, OneOfTwoTypeids)
{
    // may only redistribute among same typeid
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setType(402)
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    4.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",     10, "0140004000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(84070, "2011-10-13 06:00:00", "0000001000007000", 9.8, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(84070, "2011-10-14 06:00:00", "014000400000" END_FD "000", 0.2, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, NoGoodNeighbors)
{
    // may not redistribute if there are no neighbors with good data
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    ASSERT_EQ(0, logs->count(Message::INFO));
    ASSERT_EQ(1, logs->count(Message::WARNING));
    ASSERT_EQ(0, logs->find("not enough", Message::WARNING));

    // now give the neighbors good data, re-run, and make sure
    // Redistribution runs

    data.setStation(83520).add("2011-10-13 06:00:00", 1.0, "0110000000001000", "")
        .setStation(84190).add("2011-10-13 06:00:00", 1.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.update(db));

    ASSERT_RUN(algo, bc, 2);

    ASSERT_EQ(2, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, NoGoodNeighborsForOnePoint)
{
    // may not redistribute if for one point in the time series, there are no neighbors
    DataList data(83880, 110, 302);
    data.add("2011-10-11 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-12 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-11 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",    0.2, "0110000000001000", "")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-11 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-12 06:00:00",    0.4, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",     10, "0140004000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    std::ostringstream sql;
    sql << "UPDATE data SET original = 1.0, corrected = 1.0, controlinfo = '0110000000001000',"
        << " useinfo='7000000000000000', cfailed='' WHERE stationid IN (83520, 84190) AND obstime = '2011-10-13 06:00:00';";
    ASSERT_NO_THROW(db->exec(sql.str()));

    // two accumulations, one for 83880 and one for 84070
    ASSERT_RUN(algo, bc, 3+2);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, NeighborsTooFar)
{
    // may not redistribute if for one point in the time series, there are no neighbors
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    4.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",     10, "0140004000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   10" << std::endl
           << "Start_DD   =   11" << std::endl
           << "Start_hh   =   06" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   10" << std::endl
           << "End_DD     =   18" << std::endl
           << "End_hh     =   06" << std::endl
           << "#InterpCode=2"      << std::endl
           << "#Step_DD=1"         << std::endl
           << "ParamId=110"       << std::endl
           << "TypeIds=302"       << std::endl
           << "TypeIds=402"       << std::endl
        // only neighbors within 5 km => none
           << "InterpolationDistance=5.0" << std::endl;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    std::stringstream confiG;
    confiG << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   10" << std::endl
           << "Start_DD   =   11" << std::endl
           << "Start_hh   =   06" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   10" << std::endl
           << "End_DD     =   18" << std::endl
           << "End_hh     =   06" << std::endl
           << "ParamId=110"       << std::endl
           << "TypeIds=302"       << std::endl
           << "TypeIds=402"       << std::endl
        // neighbors within 50 km => action
           << "InterpolationDistance=50.0" << std::endl;
    params.Parse(confiG);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 4);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BoneDrySomeNeighbors)
{
    // check correct redistribution of -1 values
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    0.1, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",     -1, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    0.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 0.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000", 0.1, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, DrySomeNeighborsBoneDry)
{
    // check correct redistribution of -1 values
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    0.0, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",     -1, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    0.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.0, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 0.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000", 0.0, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, DryAllNeighborsBoneDry)
{
    // check correct redistribution of -1 values
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    0.0, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",     -1, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",     -1, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",     -1, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000",  -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000",  -1, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BoneDryAllNeighbors)
{
    // check correct redistribution of -1 values
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    0.1, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",     -1, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000",  -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000", 0.1, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BoneDryMissingRows)
{
    // check correct redistribution of -1 values
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        //.add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    0.1, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",     -1, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    0.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 0.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000", 0.1, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, IncompleteSeries)
{
    // may not redistribute if no accumulated value yet
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00",    6.5, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-14 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    5.7, "0110000000001000", "")
        .add("2011-10-16 06:00:00",   54.2, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    4.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    0.2, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    6.4, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.6, "0110000000001000", "")
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    data.setStation(83880).add("2011-10-17 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520).add("2011-10-17 06:00:00",    8.2, "0110000000001000", "")
        .setStation(84190).add("2011-10-17 06:00:00",    2.4, "0110000000001000", "")
        .setStation(84070).add("2011-10-17 06:00:00", -32767, "0000003000002000", "QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    ASSERT_RUN(algo, bc, 4);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, Release113)
{
    // origin of test case: https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:user:version:kvqc2d-1.1.3
    DataList data(66100, 110, 402);
    data.add("2011-05-05 06:00:00",    5.8, "0110000000001000", "")
        .add("2011-05-06 06:00:00",    0.9, "0110000000001000", "")
        .add("2011-05-07 06:00:00", -32767, "0000003000000000", "")
        .add("2011-05-08 06:00:00", -32767, "0000003000000000", "")
        .add("2011-05-09 06:00:00", -32767, "0000003000000000", "")
        .add("2011-05-10 06:00:00", -32767, "0000003000000000", "")
        .add("2011-05-11 06:00:00", -32767, "0000003000000000", "")
        .add("2011-05-12 06:00:00", -32767, "0000003000000000", "")
        .add("2011-05-13 06:00:00", -32767, "0000003000002000", "QC1-7-110b")
        .add("2011-05-14 06:00:00", -32767, "0000003000002000", "QC1-7-110b")
        .add("2011-05-15 06:00:00", -32767, "0000003000002000", "QC1-7-110b")
        .add("2011-05-16 06:00:00",   27.1, "0140004000002000", "QC1-7-110b");

    // fake, did not find actual data for stations 66070 and 65270
    const int neighbors[4] = { 65270, 66070, 67150, 68270 };
    for(int n=0; n<4; ++n) {
        data.setStation(neighbors[n]);
        for(int d=5; d<=16; ++d)
            data.add(kvtime::maketime(2011, 5, d, 6, 0, 0), 2.7, "0110000000001000", "");
    }
    ASSERT_NO_THROW(data.insert(db));

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(65270,  63.2302,  9.3488, 306, 0,                'S_VATNET', NULL, 65270, NULL, NULL, NULL,  9, 't', '1965-07-01 00:00:00');"
        << "INSERT INTO station VALUES(66070,  63.2941,  9.7291,  84, 0, 'SKJENALDFOSSEN I ORKDAL', NULL, 66070, NULL, NULL, NULL,  9, 't', '1907-01-01 00:00:00');"
        << "INSERT INTO station VALUES(66100, 63.33085,  9.6489, 300, 0,                  'SONGLI', NULL, 66100, NULL, NULL, NULL, 10, 't', '1988-07-03 00:00:00');"
        << "INSERT INTO station VALUES(67150,   63.328, 10.2737,  13, 0,              'LEINSTRAND', NULL, 67150, NULL, NULL, NULL,  9, 't', '1960-01-01 00:00:00');"
        << "INSERT INTO station VALUES(68270,  63.2315, 10.4369, 173, 0,                 'L_KSMYR', NULL, 68270, NULL, NULL, NULL,  9, 't', '1960-01-01 00:00:00');"

        << "INSERT INTO station_param VALUES(0, 110, 0, 0,   1, 365, -1, 'QC1-1-110', 'max;highest;high;low;lowest;min\n150;120.0;100.0;-1.0;-1.0;-1', "
        <<            "NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 274, 304, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 305, 334, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 335, 365, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,   1,  31, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  32,  59, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  60,  90, -1, 'QC1-2-72.b4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  91, 120, -1, 'QC1-2-72.b4', 'R1\n5',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 121, 151, -1, 'QC1-2-72.b4', 'R1\n3',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 152, 181, -1, 'QC1-2-72.b4', 'R1\n1',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 182, 212, -1, 'QC1-2-72.b4', 'R1\n0',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 213, 243, -1, 'QC1-2-72.b4', 'R1\n0',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 244, 273, -1, 'QC1-2-72.b4', 'R1\n3',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 274, 304, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 305, 334, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 335, 365, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,   1,  31, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  32,  59, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  60,  90, -1, 'QC1-2-72.c4', 'R1\n10', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,  91, 120, -1, 'QC1-2-72.c4', 'R1\n5',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 121, 151, -1, 'QC1-2-72.c4', 'R1\n3',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 152, 181, -1, 'QC1-2-72.c4', 'R1\n1',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 182, 212, -1, 'QC1-2-72.c4', 'R1\n0',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 213, 243, -1, 'QC1-2-72.c4', 'R1\n0',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0, 244, 273, -1, 'QC1-2-72.c4', 'R1\n3',  '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 110, 0, 0,   1, 365, -1, 'QC1-1-110x',  '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    AlgorithmConfig params;
    Configure(params, 5, 16);
    params.UT0 = kvtime::maketime("2011-05-05 06:00:00");
    params.UT1 = kvtime::maketime("2011-05-16 06:00:00");

    // with bad fd flags, make sure that the redistribution does not run
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    // now fix the fd flags and make sure that the redistribution runs
    sql.str("");
    sql << "UPDATE data SET controlinfo = '0000003000002000' WHERE stationid = 66100 AND obstime BETWEEN '2011-05-07 06:00:00' AND '2011-05-12 06:00:00';";
    ASSERT_NO_THROW(db->exec(sql.str()));

    ASSERT_RUN(algo, bc, 10);
}

#if 0
TEST_F(RedistributionTest, Bugzilla1296)
{
    // should be covered by SeriesPossiblyIncomplete,, IncompleteSeries, StartOfDatabase
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, Bugzilla1304)
{
    // by default assume dry
    // should be covered by BoneDry
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, Bugzilla1322)
{
    // redistribution run starts in the middle of a sequence of missing values
    // should be covered by SeriesPossiblyIncomplete,, IncompleteSeries, StartOfDatabase
    FAIL() << "test not implemented, no example data found";
}
#endif

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, Bugzilla1325)
{
    // consistency of accumulated value with sum of redistributed values
    const int N = 6;
    const float values[N]   = { 1.07, 2.07, 3.07, 2.07, 1.07, 1.07 };
    const float expected[N] = { 1.0,  2.1,  3.0,  2.1,  1.1,  1.1 };

    RoundingTest(values, expected, N);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, Bugzilla1325Comment4)
{
    const int N = 6;
    const float values[N]   = { 0.07, 0.17, 0.17, 0.17, 0.17, 0.07 };
    const float expected[N] = { 0.0,  0.1,  0.2,  0.2,  0.2,  0.1 };

    RoundingTest(values, expected, N);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, RoundingForVerySmallValues)
{
    // keep sum of distributed values identical to accumulated value; see bugzilla 1325 and 1320
    const int N = 6;
    const float values[N]   = { 0.07, 0.07, 0.07, 0.07, 0.07, 0.07 };
    const float expected[N] = { 0.0,  0.0,  0.1,  0.1,  0.1,  0.1 };

    RoundingTest(values, expected, N);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, Bugzilla1333)
{
    DataList data(83880, 110, 302);
    data.add("2011-10-10 06:00:00",   16.9, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00",    6.5, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-14 06:00:00",    6.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-15 06:00:00",    6.7, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-18 06:00:00", -32767, "0000003000000000", "")
        .add("2011-10-19 06:00:00",    0.6, "0140000000000000", "QC1-2-72.b12")
        .setStation(83520)
        .add("2011-10-10 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    5.7, "0110000000001000", "")
        .add("2011-10-16 06:00:00",   54.2, "0110000000001000", "")
        .add("2011-10-17 06:00:00",   11.4, "0110000000001000", "")
        .add("2011-10-18 06:00:00",    6.7, "0110000000001000", "")
        .add("2011-10-19 06:00:00",    0.7, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-10 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    4.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    0.2, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    6.4, "0110000000001000", "")
        .add("2011-10-17 06:00:00",      2, "0110000000001000", "")
        .add("2011-10-18 06:00:00",    0.3, "0110000000001000", "")
        .add("2011-10-19 06:00:00",    7.4, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-11 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.6, "0110000000001000", "")
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-18 06:00:00",   66.7, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .add("2011-10-19 06:00:00",    1.4, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 13, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-16 06:00:00", "0000001000007000", 10.5, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "014000400000" END_FD "000",  2.3, bc->update(1));

    EXPECT_CFAILED(",QC2N_83520_84190,QC2-redist", bc->update(0));
    EXPECT_CFAILED(",QC2N_83520_84190,QC2-redist", bc->update(1));

    std::list<kvalobs::kvData> cfailedWithQC2;
    ASSERT_NO_THROW(cfailedWithQC2 = db->extractData("SELECT * FROM data  WHERE cfailed LIKE '%QC2%' AND stationid != 83880"));
    ASSERT_TRUE(cfailedWithQC2.empty());

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BadMeasurementHour)
{
   DataList data(83880, 110, 302);
   data.add("2011-10-12 07:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
       .add("2011-10-13 07:00:00", -32767, "0000003000002000", "QC1-7-110")
       .add("2011-10-14 07:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
       .setStation(83520)
       .add("2011-10-12 07:00:00",    0.1, "0110000000001000", "")
       .add("2011-10-13 07:00:00",    2.5, "0110000000001000", "")
       .add("2011-10-14 07:00:00",    2.6, "0110000000001000", "")
       .setStation(84190)
       .add("2011-10-12 07:00:00",     -1, "0110000000001000", "")
       .add("2011-10-13 07:00:00",    4.5, "0110000000001000", "")
       .add("2011-10-14 07:00:00",    0.1, "0110000000001000", "")
       .setStation(84070)
       .add("2011-10-12 07:00:00",     -1, "0110000000001000", "")
       .add("2011-10-13 07:00:00",    2.1, "0110000000001000", "")
       .add("2011-10-14 07:00:00",    0.6, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    std::ostringstream sql;
    sql << "UPDATE data SET obstime = '2011-10-12 06:00:00' WHERE obstime = '2011-10-12 07:00:00';"
        << "UPDATE data SET obstime = '2011-10-13 06:00:00' WHERE obstime = '2011-10-13 07:00:00';"
        << "UPDATE data SET obstime = '2011-10-14 06:00:00' WHERE obstime = '2011-10-14 07:00:00';";
    ASSERT_NO_THROW(db->exec(sql.str()));

    ASSERT_RUN(algo, bc, 2);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, NonzeroAccumulationButNeighborsBonedry)
{
    DataList data(89650, 110, 302);
    data.add("2011-12-20 06:00:00",      3, "0110000000001000", "")
        .add("2011-12-21 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-12-22 06:00:00",   11.2, "0110004000002000", "QC1-7-110")
        .setStation(88100)
        .add("2011-12-20 06:00:00",     -1, "0110000000001000")
        .add("2011-12-21 06:00:00",     -1, "0110000000001000")
        .add("2011-12-22 06:00:00",     -1, "0110000000001000");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2011\n"
           << "Start_MM   =   12\n"
           << "Start_DD   =   20\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2011\n"
           << "End_MM     =   12\n"
           << "End_DD     =   22\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n"
           << "InterpolationDistance=50.0\n";
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(1, logs->count(Message::WARNING));
    ASSERT_EQ(0, logs->find("non-0 accumulation with dry neighbors"));

    data.setStation(88100)
        .add("2011-12-21 06:00:00", 5, "0110000000001000")
        .add("2011-12-22 06:00:00", 5, "0110000000001000");
    ASSERT_NO_THROW(data.update(db));
    ASSERT_RUN(algo, bc, 2);
    EXPECT_STATION_OBS_CONTROL_CORR(89650, "2011-12-21 06:00:00", "0000001000007000", 5.6, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(89650, "2011-12-22 06:00:00", "011000400000" END_FD "000", 5.6, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, DryNoNeighbors)
{
   DataList data(83880, 110, 302);
   data.add("2011-10-12 06:00:00",    0.0, "0140000000001000", "QC1-2-72.b12")
       .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
       .add("2011-10-14 06:00:00",    0.0, "0140004000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000",  -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000",  -1, bc->update(1));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BoneDryNoNeighbors)
{
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.0, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",     -1, "0140000000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000",  -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000000000" END_FD "000",  -1, bc->update(1));
    ASSERT_EQ(1, logs->count("QC2-redist-endpoint"));

    // run again without modifications or messages
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BadWarning)
{
    DataList data(3200, 110, 302);
    data.add("2011-10-28 06:00:00",    0.5, "0110000000001000", "")
        .add("2011-10-29 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-30 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-31 06:00:00",     14, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(1230)
        .add("2011-10-28 06:00:00",      1, "0110000000001000", "")
        .add("2011-10-29 06:00:00",      6, "0110000000001000", "")
        .add("2011-10-30 06:00:00",      6, "0110000000001000", "")
        .add("2011-10-31 06:00:00",      2, "0110000000001000", "")
        .setStation(4040)
        .add("2011-10-28 06:00:00",      1, "0110000000001000", "")
        .add("2011-10-29 06:00:00",      6, "0110000000001000", "")
        .add("2011-10-30 06:00:00",      6, "0110000000001000", "")
        .add("2011-10-31 06:00:00",      2, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2011\n"
           << "Start_MM   =   10\n"
           << "Start_DD   =   28\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2011\n"
           << "End_MM     =   10\n"
           << "End_DD     =   31\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n"
           << "InterpolationDistance=50.0\n";
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-29 06:00:00", "0000001000007000",  6, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-30 06:00:00", "0000001000007000",  6, bc->update(1));
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-31 06:00:00", "014000400000" END_FD "000",  2, bc->update(2));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, NoReallyGoodNeighbors)
{
    DataList data(3200, 110, 302);
    data.add("2011-10-28 06:00:00",    0.5, "0110000000001000", "")
        .add("2011-10-29 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-30 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-31 06:00:00",     14, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(1230)
        .add("2011-10-28 06:00:00",      1, "0110000000001000", "")
        .add("2011-10-29 06:00:00",      6, "0110000000001000", "")
        .add("2011-10-30 06:00:00", -32767, "0110000000001000", "")
        .add("2011-10-31 06:00:00",      2, "0110000000001000", "")
        .setStation(4040)
        .add("2011-10-28 06:00:00",      1, "0110000000001000", "")
        .add("2011-10-29 06:00:00",      6, "0110000000001000", "")
        .add("2011-10-30 06:00:00",      6, "0110000000001000", "")
        .add("2011-10-31 06:00:00",      2, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2011\n"
           << "Start_MM   =   10\n"
           << "Start_DD   =   28\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2011\n"
           << "End_MM     =   10\n"
           << "End_DD     =   31\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n"
           << "TypeIds=402\n"
           << "InterpolationDistance=50.0\n"
        // distance 3200 -- 1230 is 25 km, distance 3200 -- 4040 is 49 km; 1230 is missing on 2011-10-30
           << "warning_distance_closest_neighbor = 30.0\n";
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);
    ASSERT_EQ(4, logs->count(Message::INFO));
    EXPECT_EQ(0, logs->find("no really good neighbors at obstime=2011-10-30 06:00:00", Message::INFO));


    // run again without modifications and same message
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(1, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
    EXPECT_EQ(0, logs->find("no really good neighbors at obstime=2011-10-30 06:00:00"));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, MaxNeighbors)
{
    DataList data(3200, 110, 302);
    data.add("2011-10-28 06:00:00",    0.5, "0110000000001000", "")
        .add("2011-10-29 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-30 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-31 06:00:00",     14, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(1230)
        .add("2011-10-28 06:00:00",      1, "0110000000001000", "")
        .add("2011-10-29 06:00:00",      8, "0110000000001000", "")
        .add("2011-10-30 06:00:00",      4, "0110000000001000", "")
        .add("2011-10-31 06:00:00",      2, "0110000000001000", "")
        .setStation(4040)
        .add("2011-10-28 06:00:00",      1, "0110000000001000", "")
        .add("2011-10-29 06:00:00",      4, "0110000000001000", "")
        .add("2011-10-30 06:00:00",      8, "0110000000001000", "")
        .add("2011-10-31 06:00:00",      2, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2011\n"
           << "Start_MM   =   10\n"
           << "Start_DD   =   28\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2011\n"
           << "End_MM     =   10\n"
           << "End_DD     =   31\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n"
           << "TypeIds=402\n"
           << "InterpolationDistance=50.0\n"
        // distance 3200 -- 1230 is 25 km, distance 3200 -- 4040 is 49 km
           << "max_neighbors = 1\n";
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-29 06:00:00", "0000001000007000",  8, bc->update(0)) << "2011-10-29";
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-30 06:00:00", "0000001000007000",  4, bc->update(1)) << "2011-10-30";
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-31 06:00:00", "014000400000" END_FD "000",  2, bc->update(2)) << "2011-10-31";

    // run again without modifications or messages
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BoneDryAccumulation)
{
    // check correct redistribution of -1 accumulation -- independent of neighbors
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",     -1, "0140000000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000000000" END_FD "000", -1, bc->update(1));

    // run again without modifications or messages
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BoneDryAccumulationMissingRows)
{
    // check correct redistribution of -1 accumulation -- independent of neighbors
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        // 2011-10-13 is not in table
        .add("2011-10-14 06:00:00",     -1, "0140000000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000000000" END_FD "000", -1, bc->update(1));

    // run again without modifications or messages
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, ComplaintFmis0NotBoneDry)
{
    // check correct redistribution of -1 accumulation -- independent of neighbors
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-14 06:00:00",    0.0, "0140000000002000", "QC1-2-72.b12,QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(1, logs->count(Message::WARNING));
    ASSERT_EQ(0, logs->find("fmis=0 and original!=-1"));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, Bad1stRedisFixWet)
{
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, 0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, 1.0, "0000001000007000", "QC1-7-110,QC2-redist-fake")
        .add("2011-10-14 06:00:00",    0.6, 0.1, "0140004000007000", "QC1-2-72.b12,QC1-7-110,QC2-redist-fake")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    0.2, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.2, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    0.2, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.2, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 0.3, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000", 0.3, bc->update(1));

    ASSERT_EQ(3, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->find("redistributed sum"));

    // run again without modifications or messages
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, Bad1stRedisFixDry)
{
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, 0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00", -32767, 1.0, "0000001000007000", "QC1-7-110,QC2-redist-fake")
        .add("2011-10-14 06:00:00",    0.0, 0.1, "0140004000007000", "QC1-2-72.b12,QC1-7-110,QC2-redist-fake")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",     -1, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    0.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.0, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 11, 18);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 0.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "014000400000" END_FD "000", 0.0, bc->update(1));

    ASSERT_EQ(3, logs->count(Message::INFO));
    ASSERT_LE(0, logs->find("redistributed sum"));

    // run again without modifications or messages
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, LostPrecipitation)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(99500, 69.3718, 29.692,  55, 0, 'SKOGFOSS',    NULL, 99500, NULL, NULL, NULL, 10, 't', '1968-01-01 00:00:00');"
        << "INSERT INTO station VALUES(99340, 69.682,  29.1483, 85, 0, 'ØVRE NEIDEN', NULL, 99340, NULL, NULL, NULL,  0, 't', '2011-09-23 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    DataList data(99500, 110, 302);
    data.add("2012-01-07 06:00:00",     -1,  -1, "0110000000001000", "")
        .add("2012-01-08 06:00:00", -32767, 0.8, "0000001000007000", "QC1-7-110,QC2N_99340,QC2-redist")
        .add("2012-01-09 06:00:00",    1.5,  -1, "000000400000" END_FD "000", "")
        .setStation(99340)
        .add("2012-01-07 06:00:00", 1.4, "0110000000001000", "")
        .add("2012-01-08 06:00:00", 1.5, "0110000000001000", "")
        .add("2012-01-09 06:00:00",  -1, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   01\n"
           << "Start_DD   =   01\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   01\n"
           << "End_DD     =   10\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n";
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);

    // only 2012-01-08 is updated, 2012-01-09 is part of the
    // redistribution period but stays dry as set in previous run
    ASSERT_RUN(algo, bc, 1);
    EXPECT_STATION_OBS_CONTROL_CORR(99500, "2012-01-08 06:00:00", "0000001000007000", 1.5, bc->update(0));
    ASSERT_EQ(2, logs->count(Message::INFO));
    ASSERT_LE(0, logs->find("redistributed sum"));

    // run again without modifications or messages
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, LostPrecipitationOldQC2)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(99500, 69.3718, 29.692,  55, 0, 'SKOGFOSS',    NULL, 99500, NULL, NULL, NULL, 10, 't', '1968-01-01 00:00:00');"
        << "INSERT INTO station VALUES(99340, 69.682,  29.1483, 85, 0, 'ØVRE NEIDEN', NULL, 99340, NULL, NULL, NULL,  0, 't', '2011-09-23 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    DataList data(99500, 110, 302);
    data.add("2012-01-20 06:00:00",     -1, -1, "0110000000001000", "")
        .add("2012-01-21 06:00:00", -32767, -1, "0000001000007000", "QC1-7-110,QC2N_99340,QC2-redist")
        .add("2012-01-22 06:00:00", -32767, -1, "0000001000007000", "QC1-7-110,QC2N_99340,QC2-redist")
        .add("2012-01-23 06:00:00",    1.6, -1, "0000004000007000", "")
        .add("2012-01-24 06:00:00", -32767,  0, "0000001000002006", "QC1-7-110,watchRR")
        .setStation(99340)
        .add("2012-01-20 06:00:00", 0.2, "0110000000001000", "")
        .add("2012-01-21 06:00:00",  -1, "0110000000001000", "")
        .add("2012-01-22 06:00:00",  -1, "0110000000001000", "")
        .add("2012-01-23 06:00:00",  -1, "0110000000001000", "")
        .add("2012-01-24 06:00:00",  -1, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   01\n"
           << "Start_DD   =   15\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   01\n"
           << "End_DD     =   25\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n";
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(1, logs->count(Message::WARNING));
    ASSERT_LE(0, logs->find("redistributed sum"));

    // run again with no modification and same message
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(1, logs->count(Message::WARNING));
    ASSERT_LE(0, logs->find("redistributed sum"));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, BadRedistributedSumRounding)
{
    // this tests correct addition/rounding while calculating the sum
    // of distributed precipitation

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(48500, 59.991,  6.024,   51, 0, 'ROSENDAL',               NULL, 48500, NULL, NULL, NULL,  9, 't', '1930-01-01 00:00:00');"
        << "INSERT INTO station VALUES(48780, 60.1322, 6.3305,  33, 0, 'MAURANGER KRAFTSTASJON', NULL, 48780, NULL, NULL, NULL, 10, 't', '1977-01-01 00:00:00');"
        << "INSERT INTO station VALUES(49070, 60.2803, 6.3828, 342, 0, 'KVÅLE',                  NULL, 49070, NULL, NULL, NULL,  9, 't', '1966-07-01 00:00:00');"
        << "INSERT INTO station VALUES(50150, 60.0422, 5.9057,  45, 0, 'HATLESTRAND',            NULL, 50150, NULL, NULL, NULL,  9, 't', '1943-07-01 00:00:00');"
        << "INSERT INTO station VALUES(49351, 60.1222, 6.5561,  32, 0, 'TYSSEDAL IA',            NULL, 49351, NULL, NULL, NULL,  9, 't', '2000-09-27 00:00:00');"
        << "INSERT INTO station VALUES(47820, 59.8594, 6.2786, 178, 0, 'EIKEMO',                 NULL, 47820, NULL, NULL, NULL,  9, 't', '1961-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    // data for 48780 are from dev-vm101, neighbor data are modified to avoid update
    DataList data(48780, 110, 302);
    data.add("2012-02-15 06:00:00", 21.5, "0110000000001000", "")
        .add("2012-02-16 06:00:00", -32767,   7.2, "0000001000007000", "QC1-7-110,QC2N_49351_49070_50120_48500_50150,QC2-redist")
        .add("2012-02-17 06:00:00", -32767,  16.2, "0000001000007000", "QC1-7-110,QC2N_49351_49070_50120_48500_50150,QC2-redist")
        .add("2012-02-18 06:00:00", -32767,  33.8, "0000001000007000", "QC1-7-110,QC2N_49070_48500_50150_47820_48450,QC2-redist")
        .add("2012-02-19 06:00:00", -32767,  34.9, "0000001000007000", "QC1-7-110,QC2N_49070_50150_47820_48450_46450,QC2-redist")
        .add("2012-02-20 06:00:00",   93.1,     1, "011000400000" END_FD "000", "QC1-7-110,QC2N_49351_49070_48500_50150_47820,QC2-redist,QC2-redist-endpoint");
    const int neighbors[] = { 47820, 48500, 49070, 49351, 50150, -1 };
    for(int n=0; neighbors[n]>0; ++n) {
        data.setStation(neighbors[n])
            .add("2012-02-15 06:00:00",   18.7, "0110000000001000", "")
            .add("2012-02-16 06:00:00",    7.2, "0110000000001000", "")
            .add("2012-02-17 06:00:00",   16.2, "0110000000001000", "")
            .add("2012-02-18 06:00:00",   33.8, "0110000000001000", "")
            .add("2012-02-19 06:00:00",   34.9, "0110000000001000", "")
            .add("2012-02-20 06:00:00",      1, "0110000000001000", "");
    }
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   02\n"
           << "Start_DD   =   15\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   02\n"
           << "End_DD     =   25\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n";
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::WARNING));
    ASSERT_EQ(0, logs->count(Message::INFO));

    // now change the data and trigger a redistribution which may only
    // change back corrected 123->1
    data.setStation(48780).add("2012-02-20 06:00:00", 93.1, 123, "0110004000007000", "QC1-7-110,QC2N_49351_49070_48500_50150_47820,QC2-redist");
    ASSERT_NO_THROW(data.update(db));

    ASSERT_RUN(algo, bc, 1);
    EXPECT_STATION_OBS_CONTROL_CORR(48780, "2012-02-20 06:00:00", "011000400000" END_FD "000", 1, bc->update(0));
    ASSERT_EQ(2, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, SuspiciousRowsOnlyIfHQC0)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(35340, 58.718, 9.210, 36.0, 0.0, 'RISXR BRANNSTASJON', NULL, 35340, NULL, NULL, NULL, 9, 't', '1968-02-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    DataList data(35340, 110, 302);
    data.add("2012-04-28 06:00:00", -32767, -1, "0000001000002006", "QC1-7-110,QC2-redist-bonedry,hqc")
        .add("2012-04-29 06:00:00", -32767, -1, "0000001000002006", "QC1-7-110,QC2-redist-bonedry,hqc")
        .add("2012-04-30 06:00:00",     -1, -1, "0110004000002006", "QC1-7-110,QC2-redist-bonedry,hqc");
    // same again with fhqc=0
    data.add("2012-05-01 06:00:00", -32767, -1, "0000001000002000", "QC1-7-110,QC2-redist-bonedry")
        .add("2012-05-02 06:00:00", -32767, -1, "0000001000002000", "QC1-7-110,QC2-redist-bonedry")
        .add("2012-05-03 06:00:00",     -1, -1, "0110004000002000", "QC1-7-110,QC2-redist-bonedry");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   04\n"
           << "Start_DD   =   27\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   05\n"
           << "End_DD     =   03\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n";
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(1, logs->count(Message::WARNING));
    ASSERT_LE(0, logs->find("missing or bad data in accumulation period"));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, RowsWithoutCorrectedAndFHCQ)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(41200, 58.318, 7.594, 275.0, 0.0, 'FINSLAND', NULL, 41200, NULL, NULL, NULL, 9, 't', '1971-06-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    DataList data(41200, 110, 302);
    data.add("2012-05-26 06:00:00",     -1,     -1, "0110000000001000", "")
        .add("2012-05-27 06:00:00", -32767, -32767, "0000001000007000", "QC1-7-110,QC2-redist-bonedry")
        .add("2012-05-28 06:00:00",     -1,     -1, "0110004000002006", "QC1-7-110,QC2-redist-bonedry,hqc")
        .add("2012-05-29 06:00:00",     -1,     -1, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   05\n"
           << "Start_DD   =   26\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   05\n"
           << "End_DD     =   29\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n";
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    ASSERT_EQ(3, logs->count(Message::WARNING));
    ASSERT_LE(0, logs->find("has fd != 2 while endpoint has fd == 2"));
    ASSERT_LE(0, logs->find("fmis!=3 while fd=2"));
    ASSERT_LE(0, logs->find("fhqc!=0/4 for some rows"));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, AccumulationWithoutMissingRowsFHCQ)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(41480, 58.616, 7.407, 278.0, 0.0, '�SERAL', NULL, 41480, NULL, NULL, NULL, 9, 't', '1895-07-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    DataList data(41480, 110, 302);
    data.add("2012-02-14 06:00:00",   -1,  -1, "0110000000001000", "")
        .add("2012-02-15 06:00:00",    0,   0, "0110004000002006", "watchRR")
        .add("2012-02-16 06:00:00",   -1,  -1, "0110000000001000", "")
        .add("2012-02-17 06:00:00",  0.7, 0.7, "0110004000002000", "")
        .add("2012-02-18 06:00:00",  9.6, 9.6, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   02\n"
           << "Start_DD   =   14\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   02\n"
           << "End_DD     =   18\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n";
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(1, logs->count(Message::WARNING));
    ASSERT_LE(0, logs->find("2012-02-17 06:00:00.*accumulation without missing rows"));
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, RowsWithFd3)
{
    // may not redistribute if no accumulated value yet
    DataList data(83880, 110, 302);
    data.add("2011-10-12 06:00:00",    0.3, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-13 06:00:00",    6.5, "0140000000001000", "QC1-2-72.b12")
        .add("2011-10-14 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-15 06:00:00", -32767, "0000003000003000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00",   12.8, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-12 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    2.6, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    5.7, "0110000000001000", "")
        .add("2011-10-16 06:00:00",   54.2, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    8.2, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    4.5, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.1, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    0.2, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    6.4, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    2.4, "0110000000001000", "")
        .setStation(84070)
        .add("2011-10-12 06:00:00",     -1, "0110000000001000", "")
        .add("2011-10-13 06:00:00",    2.1, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    0.6, "0110000000001000", "")
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00", -32767, "0000003000002000", "QC1-7-110");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    data.setStation(83880)
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110");
    ASSERT_NO_THROW(data.update(db));

    ASSERT_RUN(algo, bc, 4);
    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(RedistributionTest, DifferentLevels)
{
    // may not mix different levels
    DataList data(3780, 110, 302);
    data.add("2013-07-29 06:00:00",   0, "0110000000001010", "")
        .add("2013-07-30 06:00:00",  -1, "0110000000001010", "")
        .add("2013-07-31 06:00:00", 6.2, "0110000000001010", "")
        .add("2013-08-01 06:00:00",  -1, "0110000000001010", "")
        .add("2013-08-02 06:00:00",  -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00",  -1, "0110000000001010", "")
        .add("2013-08-04 06:00:00", 4.8, "0110000000001010", "")
        .add("2013-08-05 06:00:00", 2.4, "0110000000001010", "")
        .add("2013-08-06 06:00:00",   0, "0110000000001010", "");
    data.setStation(17251)
        .add("2013-07-29 06:00:00",   0, "0110000000001010", "")
        .add("2013-07-30 06:00:00",  -1, "0110000000001010", "")
        .add("2013-07-31 06:00:00", 0.4, "0140000000001010", "QC1-2-72.b12")
        .add("2013-08-01 06:00:00",  -1, "0110000000001010", "")
        .add("2013-08-02 06:00:00",  -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00", 0.5, "0110000000000010", "")
        .add("2013-08-04 06:00:00",   7, "0110000000001010", "")
        .add("2013-08-05 06:00:00", 4.5, "0110000000001010", "")
        .add("2013-08-06 06:00:00", 0.3, "0110000000000010", "");
    data.setStation(17780)
        .add("2013-07-29 06:00:00", -32767,   0.2, "0000001000007000", "QC1-7-110,QC2N___,QC2-redist")
        .add("2013-07-30 06:00:00", -32767,    -1, "0000001000007000", "QC1-7-110,QC2N___,QC2-redist")
        .add("2013-07-31 06:00:00", -32767,   3.6, "0000001000007000", "QC1-7-110,QC2N___,QC2-redist")
        .add("2013-08-01 06:00:00",      8,   0.6, "0110004000008000", "QC1-7-110,QC2N___,QC2-redist-endpoint")
        .add("2013-08-02 06:00:00",     -1,    -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00", -32767,     0, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-04 06:00:00", -32767,  21.9, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-05 06:00:00",   24.5,   2.6, "0110004000008001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-06 06:00:00", -32767,   2.1, "0000001000007001", "QC1-7-110,QC2N___,hqc-m");
    data.setStation(18610)
        .add("2013-07-29 06:00:00",   1, "0140000000001010", "QC1-2-72.b12")
        .add("2013-07-30 06:00:00",  -1, "0110000000001010", "")
        .add("2013-07-31 06:00:00", 7.3, "0140000000001010", "QC1-2-72.b12")
        .add("2013-08-01 06:00:00",   1, "0140000000001010", "QC1-2-72.b12")
        .add("2013-08-02 06:00:00",  -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00",  -1, "0110000000001010", "")
        .add("2013-08-04 06:00:00", 7.5, "0110000000000010", "")
        .add("2013-08-05 06:00:00", 0.4, "0110000000001010", "")
        .add("2013-08-06 06:00:00",   4, "0140000000001011", "QC1-2-72.b12,hqc-m");
    data.setStation(18450)
        .add("2013-07-29 06:00:00",      1,  0.2, "0110004000008000", "QC1-7-110,QC2N___,QC2-redist-endpoint")
        .add("2013-07-30 06:00:00",     -1,   -1, "0110000000001010", "")
        .add("2013-07-31 06:00:00",      5,    5, "0140000000001010", "QC1-2-72.b12")
        .add("2013-08-01 06:00:00",    0.5,  0.5, "0140000000001010", "QC1-2-72.b12")
        .add("2013-08-02 06:00:00",     -1,   -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00", -32767,    0, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-04 06:00:00", -32767, 19.2, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-05 06:00:00",   20.5,  1.3, "0110004000008001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-06 06:00:00",    5.5,  5.5, "0140000000001011", "QC1-2-72.b12,hqc-m");
    data.setStation(19930)
        .add("2013-07-29 06:00:00",    1.5,  0.2, "0110004000008000", "QC1-7-110,QC2N___,QC2-redist-endpoint")
        .add("2013-07-30 06:00:00", -32767,  1.4, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-07-31 06:00:00", -32767,    4, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-01 06:00:00", -32767,  0.2, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-02 06:00:00", -32767,   -1, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-03 06:00:00", -32767,  2.1, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-04 06:00:00", -32767, 29.1, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-05 06:00:00",     45,  8.2, "0110004000008001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-06 06:00:00", -32767,  5.1, "0000001000001005", "QC1-7-110,QC2N___,hqc-m");
    data.setLevel(48)
        .add("2013-08-01 06:00:00", -32767,  0.2, "0000001000007001", "QC1-7-110,QC2N___,hqc-i") // ???
        .setLevel(0);
    data.setStation(20520)
        .add("2013-07-29 06:00:00",  -1, "0110000000001010", "")
        .add("2013-07-30 06:00:00",   1, "0110000000000010", "")
        .add("2013-07-31 06:00:00",   1, "0110000000001010", "")
        .add("2013-08-01 06:00:00", 0.3, "0110000000001010", "")
        .add("2013-08-02 06:00:00",  -1, "0110000000000010", "")
        .add("2013-08-03 06:00:00",  -1, "0110000000000010", "")
        .add("2013-08-04 06:00:00",  15, "0110000000001010", "")
        .add("2013-08-05 06:00:00", 1.3, "0110000000001010", "")
        .add("2013-08-06 06:00:00",   5, "0110000000001010", "");
    data.setStation(24600)
        .add("2013-07-29 06:00:00",  1.5, "0110000000001010", "")
        .add("2013-07-30 06:00:00",   -1, "0110000000001010", "")
        .add("2013-07-31 06:00:00",  1.5, "0110000000001010", "")
        .add("2013-08-01 06:00:00",   -1, "0110000000001010", "")
        .add("2013-08-02 06:00:00",   -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00",    3, "0110000000001010", "")
        .add("2013-08-04 06:00:00", 14.6, "0110000000001010", "")
        .add("2013-08-05 06:00:00",  2.3, "0110000000001010", "")
        .add("2013-08-06 06:00:00",    7, "0110000000001010", "");
    data.setStation(27301)
        .add("2013-07-29 06:00:00",  2.2, "0110000000001010", "")
        .add("2013-07-30 06:00:00",  1.4, "0110000000001010", "")
        .add("2013-07-31 06:00:00",  1.7, "0110000000001010", "")
        .add("2013-08-01 06:00:00",  0.2, "0110000000001010", "")
        .add("2013-08-02 06:00:00",   -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00",   -1, "0110000000001010", "")
        .add("2013-08-04 06:00:00",  8.5, "0110000000001010", "")
        .add("2013-08-05 06:00:00",    2, "0110000000001010", "")
        .add("2013-08-06 06:00:00",  4.2, "0110000000001010", "");
    data.setStation(30380)
        .add("2013-07-29 06:00:00",    4, "0110000000000010", "")
        .add("2013-07-30 06:00:00",  4.5, "0110000000001010", "")
        .add("2013-07-31 06:00:00",  4.2, "0110000000001010", "")
        .add("2013-08-01 06:00:00",   -1, "0110000000001010", "")
        .add("2013-08-02 06:00:00",   -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00",  0.1, "0110000000001010", "")
        .add("2013-08-04 06:00:00",   15, "0110000000001010", "")
        .add("2013-08-05 06:00:00",   16, "0110000000001010", "")
        .add("2013-08-06 06:00:00",   14, "0110000000001010", "");
    data.setStation(30530)
        .add("2013-07-29 06:00:00",  0.8, "0110004000008000", "QC1-7-110,QC2N___,QC2-redist-endpoint")
        .add("2013-07-30 06:00:00",   -1, "0110000000001010", "")
        .add("2013-07-31 06:00:00",  0.3, "0110000000001010", "")
        .add("2013-08-01 06:00:00",   -1, "0110000000001010", "")
        .add("2013-08-02 06:00:00",   -1, "0110000000001010", "")
        .add("2013-08-03 06:00:00", -32767, 5.5, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-04 06:00:00", -32767, 8.5, "0000001000007001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-05 06:00:00",   21.5, 7.5, "0110004000008001", "QC1-7-110,QC2N___,hqc-m")
        .add("2013-08-06 06:00:00",    6.8, 6.8, "0110000000001010", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2013\n"
           << "Start_MM   =   07\n"
           << "Start_DD   =   29\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2013\n"
           << "End_MM     =   08\n"
           << "End_DD     =   06\n"
           << "End_hh     =   06\n"
           << "ParamId=110\n"
           << "TypeIds=302\n";
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

#ifdef FD8
TEST_F(RedistributionTest, MixingFd7And8)
{
    DataList data(83880, 110, 302);
    data.add("2011-10-13 06:00:00",    4.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-15 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-16 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-17 06:00:00",   40.0, "0140004000002000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-13 06:00:00",    5.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    5.0, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    5.0, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    5.0, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    5.0, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-13 06:00:00",    6.0, "0110000000001000", "")
        .add("2011-10-14 06:00:00",    6.0, "0110000000001000", "")
        .add("2011-10-15 06:00:00",    6.0, "0110000000001000", "")
        .add("2011-10-16 06:00:00",    6.0, "0110000000001000", "")
        .add("2011-10-17 06:00:00",    6.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 13, 17);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 4);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0000001000007000", 10.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-15 06:00:00", "0000001000007000", 10.0, bc->update(1));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-16 06:00:00", "0000001000007000", 10.0, bc->update(2));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "0140004000008000", 10.0, bc->update(3));
    ASSERT_EQ(1, logs->count("QC2-redist-endpoint"));

    ASSERT_RUN(algo, bc, 0);

    // add another 4 days of data, now with new (flaggspec 9.11)
    // endpoint marker fd=4
    data.setStation(83880)
        .add("2011-10-18 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-19 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-20 06:00:00", -32767, "0000003000002000", "QC1-7-110")
        .add("2011-10-21 06:00:00",   44.0, "0140004000004000", "QC1-2-72.b12,QC1-7-110")
        .setStation(83520)
        .add("2011-10-18 06:00:00",    5.5, "0110000000001000", "")
        .add("2011-10-19 06:00:00",    5.5, "0110000000001000", "")
        .add("2011-10-20 06:00:00",    5.5, "0110000000001000", "")
        .add("2011-10-21 06:00:00",    5.5, "0110000000001000", "")
        .setStation(84190)
        .add("2011-10-18 06:00:00",    6.6, "0110000000001000", "")
        .add("2011-10-19 06:00:00",    6.6, "0110000000001000", "")
        .add("2011-10-20 06:00:00",    6.6, "0110000000001000", "")
        .add("2011-10-21 06:00:00",    6.6, "0110000000001000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params2;
    Configure(params2, 13, 22);

    ASSERT_CONFIGURE(algo, params2);
    ASSERT_RUN(algo, bc, 4);

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-18 06:00:00", "0000001000007000", 11.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-19 06:00:00", "0000001000007000", 11.0, bc->update(1));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-20 06:00:00", "0000001000007000", 11.0, bc->update(2));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-21 06:00:00", "0140004000008000", 11.0, bc->update(3));
    ASSERT_EQ(1, logs->count("QC2-redist-endpoint"));

    ASSERT_RUN(algo, bc, 0);
}
#endif

#if 0

there is a hole in the data between 2012-06-11 and 2012-06-21 because 06-11 has a late tbtime and the rows between were deleted by the cleanup job; qqc2 tries to fill up with new rows and then complains that the endpoint has fd=7 (i.e. after redistribution) while the "new rows" have fd=4 (before redistribution)

kvalobs=# select obstime,original,tbtime,corrected,controlinfo,cfailed from data where stationid=85440 AND obstime between '2012-06-01 06:00:00' and '2012-06-30 06:00:00' AND paramid=110 AND typeid=302 AND sensor='0' AND level=0 order by obstime;
       obstime       | original |           tbtime           | corrected |   controlinfo    |                         cfailed                         
---------------------+----------+----------------------------+-----------+------------------+---------------------------------------------------------
 2012-06-11 06:00:00 |        0 | 2012-06-25 12:56:06.430861 |         0 | 0110004000002000 | QC1-7-110
 2012-06-21 06:00:00 |   -32767 | 2012-06-22 00:30:11        |         8 | 0000001000007000 | QC1-7-110,QC2N_83300_83520_86950_82530_82840,QC2-redist
 2012-06-22 06:00:00 |   -32767 | 2012-06-23 00:30:10        |         4 | 0000001000007000 | QC1-7-110,QC2N_83300_83520_86950_82530_84070,QC2-redist
 2012-06-23 06:00:00 |   -32767 | 2012-06-24 00:30:17        |       0.6 | 0000001000007000 | QC1-7-110,QC2N_83300_83520_86950_82530_82840,QC2-redist
 2012-06-24 06:00:00 |   -32767 | 2012-06-25 00:30:19        |        -1 | 0000001000007000 | QC1-7-110,QC2N_83300_83520_86950_82530_82840,QC2-redist
 2012-06-25 06:00:00 |     13.3 | 2012-06-25 12:56:06.516496 |        -1 | 0110004000007000 | QC1-7-110,QC2N_83300_83520_86950_82530,QC2-redist
 2012-06-26 06:00:00 |   -32767 | 2012-06-27 00:30:17        |       0.7 | 0000001000007000 | QC1-7-110,QC2N_85180_83300_83520_86950_82530,QC2-redist
 2012-06-27 06:00:00 |   -32767 | 2012-06-28 00:30:15        |       1.3 | 0000001000007000 | QC1-7-110,QC2N_85180_83300_83520_86950_82530,QC2-redist
 2012-06-28 06:00:00 |   -32767 | 2012-06-29 00:30:15        |       0.3 | 0000001000007000 | QC1-7-110,QC2N_85180_83300_83520_86950_82530,QC2-redist
 2012-06-29 06:00:00 |   -32767 | 2012-06-30 00:30:14        |       0.2 | 0000001000007000 | QC1-7-110,QC2N_83300_83520_86950_82530,QC2-redist
 2012-06-30 06:00:00 |   -32767 | 2012-07-01 00:30:22        |         0 | 0000001000007000 | QC1-7-110,QC2N_83300_83520_86950_82530,QC2-redist
(11 rows)

kvalobs=# select stationid,obstime,original,tbtime,corrected,controlinfo,cfailed from data where stationid in (83300,83520,86950,82530,82840,85180) AND obstime between '2012-06-01 06:00:00' and '2012-06-30 06:00:00' AND paramid=110 AND typeid=302 AND sensor='0' AND level=0 order by stationid,obstime;
 stationid |       obstime       | original |           tbtime           | corrected |   controlinfo    |                         cfailed                         
-----------+---------------------+----------+----------------------------+-----------+------------------+---------------------------------------------------------
     82530 | 2012-06-22 06:00:00 |        9 | 2012-06-22 06:04:48.772196 |         9 | 0110000000001000 | 
     82530 | 2012-06-23 06:00:00 |       -1 | 2012-06-23 15:50:07.905163 |        -1 | 0110000000001000 | 
     82530 | 2012-06-24 06:00:00 |       -1 | 2012-06-24 06:44:50.714889 |        -1 | 0110000000001000 | 
     82530 | 2012-06-25 06:00:00 |       -1 | 2012-06-25 06:17:42.622375 |        -1 | 0110000000001000 | 
     82530 | 2012-06-26 06:00:00 |       -1 | 2012-06-26 05:59:09.338839 |        -1 | 0110000000001000 | 
     82530 | 2012-06-27 06:00:00 |        0 | 2012-06-27 06:10:29.951725 |         0 | 0110000000001000 | 
     82530 | 2012-06-28 06:00:00 |        0 | 2012-06-28 06:15:09.488469 |         0 | 0110000000001000 | 
     82530 | 2012-06-29 06:00:00 |       -1 | 2012-06-29 06:11:02.505477 |        -1 | 0110000000001000 | 
     82530 | 2012-06-30 06:00:00 |        0 | 2012-06-30 16:55:25.727084 |         0 | 0110000000001000 | 
     82840 | 2012-06-21 06:00:00 |      4.5 | 2012-06-22 11:30:30.801143 |       4.5 | 0110000000001000 | 
     82840 | 2012-06-22 06:00:00 |      7.8 | 2012-06-22 06:10:28.862029 |       7.8 | 0110000000000000 | 
     82840 | 2012-06-23 06:00:00 |      0.7 | 2012-06-23 07:17:04.387919 |       0.7 | 0110000000001000 | 
     82840 | 2012-06-24 06:00:00 |       -1 | 2012-06-24 05:55:03.376711 |        -1 | 0110000000001000 | 
     82840 | 2012-06-25 06:00:00 |   -32767 | 2012-06-26 00:30:17        |        -1 | 0000001000000005 | watchRR
     82840 | 2012-06-26 06:00:00 |      0.1 | 2012-06-26 07:09:59.915593 |       0.1 | 0110000000000000 | 
     82840 | 2012-06-27 06:00:00 |       -1 | 2012-06-27 06:08:08.106822 |        -1 | 0110000000001000 | 
     82840 | 2012-06-28 06:00:00 |      0.1 | 2012-06-28 05:58:11.479266 |       0.1 | 0110000000001000 | 
     82840 | 2012-06-29 06:00:00 |   -32767 | 2012-06-30 00:30:13        |         0 | 0000001000007000 | QC1-7-110,QC2N_82650_82530_83520_83300_81370,QC2-redist
     82840 | 2012-06-30 06:00:00 |   -32767 | 2012-07-01 00:30:22        |         0 | 0000001000007000 | QC1-7-110,QC2N_82650_82530_83520_83300_81770,QC2-redist
     83300 | 2012-06-22 06:00:00 |      4.3 | 2012-06-22 04:59:43.618229 |       4.3 | 0110000000001000 | 
     83300 | 2012-06-23 06:00:00 |      1.2 | 2012-06-23 05:05:34.320644 |       1.2 | 0110000000001000 | 
     83300 | 2012-06-24 06:00:00 |       -1 | 2012-06-24 05:03:16.937698 |        -1 | 0110000000001000 | 
     83300 | 2012-06-25 06:00:00 |       -1 | 2012-06-25 05:04:24.14553  |        -1 | 0110000000001000 | 
     83300 | 2012-06-26 06:00:00 |       -1 | 2012-06-26 05:08:56.210489 |        -1 | 0110000000001000 | 
     83300 | 2012-06-27 06:00:00 |      2.7 | 2012-06-27 05:04:22.599099 |       2.7 | 0110000000001000 | 
     83300 | 2012-06-28 06:00:00 |      0.1 | 2012-06-28 05:00:28.808342 |       0.1 | 0110000000001000 | 
     83300 | 2012-06-29 06:00:00 |       -1 | 2012-06-29 05:03:55.33518  |        -1 | 0110000000001000 | 
     83300 | 2012-06-30 06:00:00 |       -1 | 2012-06-30 05:04:35.142201 |        -1 | 0110000000001000 | 
     83520 | 2012-06-22 06:00:00 |      2.9 | 2012-06-22 06:04:01.383974 |       2.9 | 0110000000001000 | 
     83520 | 2012-06-23 06:00:00 |      0.6 | 2012-06-23 07:10:58.244453 |       0.6 | 0110000000001000 | 
     83520 | 2012-06-24 06:00:00 |       -1 | 2012-06-24 07:37:31.965539 |        -1 | 0110000000001000 | 
     83520 | 2012-06-25 06:00:00 |       -1 | 2012-06-25 06:47:13.972211 |        -1 | 0110000000001000 | 
     83520 | 2012-06-26 06:00:00 |       -1 | 2012-06-26 06:36:31.703648 |        -1 | 0110000000001000 | 
     83520 | 2012-06-27 06:00:00 |        1 | 2012-06-27 06:22:42.968701 |         1 | 0110000000001000 | 
     83520 | 2012-06-28 06:00:00 |      0.4 | 2012-06-28 06:15:04.372585 |       0.4 | 0110000000001000 | 
     83520 | 2012-06-29 06:00:00 |      0.5 | 2012-06-29 06:52:33.808387 |       0.5 | 0110000000001000 | 
     83520 | 2012-06-30 06:00:00 |      0.1 | 2012-06-30 06:44:05.503942 |       0.1 | 0110000000001000 | 
     85180 | 2012-06-21 06:00:00 |   -32767 | 2012-06-22 00:30:12        |        -1 | 0000001000007000 | QC1-7-110,QC2-redist-bonedry
     85180 | 2012-06-22 06:00:00 |   -32767 | 2012-06-23 00:30:10        |        -1 | 0000001000007000 | QC1-7-110,QC2-redist-bonedry
     85180 | 2012-06-23 06:00:00 |   -32767 | 2012-06-24 00:30:18        |        -1 | 0000001000007000 | QC1-7-110,QC2-redist-bonedry
     85180 | 2012-06-24 06:00:00 |       -1 | 2012-06-24 20:02:56.283567 |        -1 | 0110000000007000 | QC1-7-110,QC2-redist-bonedry
     85180 | 2012-06-25 06:00:00 |        0 | 2012-06-28 10:26:58.688823 |         0 | 0140000000001000 | QC1-2-72.b12
     85180 | 2012-06-26 06:00:00 |      1.5 | 2012-06-28 10:31:23.818454 |       1.5 | 0110000000001000 | 
     85180 | 2012-06-27 06:00:00 |       -1 | 2012-06-28 10:32:53.933431 |        -1 | 0110000000001000 | 
     85180 | 2012-06-28 06:00:00 |       -1 | 2012-06-28 10:32:53.959878 |        -1 | 0110000000001000 | 
     85180 | 2012-06-29 06:00:00 |        0 | 2012-07-02 12:25:50        |        -1 | 0140004000001007 | QC1-2-72.b12,watchRR
     85180 | 2012-06-30 06:00:00 |   -32767 | 2012-07-01 00:30:21        |        -1 | 0000001000002006 | QC1-7-110,watchRR
     86950 | 2012-06-22 06:00:00 |      1.5 | 2012-06-22 06:37:14.533522 |       1.5 | 0110000000001000 | 
     86950 | 2012-06-23 06:00:00 |       -1 | 2012-06-23 06:46:52.730221 |        -1 | 0110000000001000 | 
     86950 | 2012-06-24 06:00:00 |       -1 | 2012-06-24 08:36:11.375904 |        -1 | 0110000000001000 | 
     86950 | 2012-06-25 06:00:00 |       -1 | 2012-06-25 09:17:31.248009 |        -1 | 0110000000001000 | 
     86950 | 2012-06-26 06:00:00 |       -1 | 2012-06-26 05:34:24.223744 |        -1 | 0110000000001000 | 
     86950 | 2012-06-27 06:00:00 |      0.1 | 2012-06-27 05:10:05.261905 |       0.1 | 0110000000001000 | 
     86950 | 2012-06-28 06:00:00 |        1 | 2012-06-28 05:12:03.487515 |         1 | 0110000000001000 | 
     86950 | 2012-06-29 06:00:00 |       -1 | 2012-06-29 06:41:15.58571  |        -1 | 0110000000001000 | 
     86950 | 2012-06-30 06:00:00 |       -1 | 2012-06-30 04:42:09.269981 |        -1 | 0110000000001000 | 
(56 rows)
#endif
