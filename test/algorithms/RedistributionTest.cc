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
#include "algorithms/RedistributionAlgorithm.h"
#include "AlgorithmHelpers.h"
#include "foreach.h"

#include <boost/algorithm/string/predicate.hpp>
#include <algorithm>
#include <numeric>

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
    sql << "INSERT INTO station VALUES(83880, 68.0645, 16.663,    3, 0, 'SØRFJORD KRAFTVERK', NULL, 83880, NULL, NULL, NULL, 10, 't', '1985-01-01 00:00:00');"
        << "INSERT INTO station VALUES(83520, 67.8977, 15.8673,  70, 0, 'TØMMERNESET',        NULL, 83520, NULL, NULL, NULL,  9, 't', '1985-07-01 00:00:00');"
        << "INSERT INTO station VALUES(84070, 68.3302, 16.7883,  53, 0, 'BJØRKÅSEN',          NULL, 84070, NULL, NULL, NULL, 10, 't', '1964-01-01 00:00:00');"
        << "INSERT INTO station VALUES(84190, 68.2082, 17.5157,  29, 0, 'SKJOMEN - STIBERG',  NULL, 84190, NULL, NULL, NULL,  9, 't', '1987-09-01 00:00:00');"
        << "INSERT INTO station VALUES( 1230, 59.1223, 11.3865,   8, 0, 'HALDEN',             NULL,  1230, NULL, NULL, NULL,  9, 't', '1882-12-01 00:00:00');"
        << "INSERT INTO station VALUES( 3200, 59.3072, 11.1338,  31, 0, 'BATERØD',            NULL,  3200, NULL, NULL, NULL,  9, 't', '1942-01-01 00:00:00');"
        << "INSERT INTO station VALUES( 4040, 59.7512, 11.1532, 164, 0, 'ENEBAKK - BARBØL',   NULL,  4040, NULL, NULL, NULL,  9, 't', '1998-05-15 00:00:00');"
        << "INSERT INTO station VALUES(88100, 68.6457, 18.2455, 230, 0, 'BONES I BARDU',      NULL, 88100, NULL, NULL, NULL,  9, 't', '1907-05-01 00:00:00');"
        << "INSERT INTO station VALUES(89650, 68.6577, 18.8193, 314, 0, 'INNSET I BARDU',     NULL, 89650, NULL, NULL, NULL, 10, 't', '1907-05-01 00:00:00');";

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
    params.Parse(config);
}

// ------------------------------------------------------------------------

void RedistributionTest::RoundingTest(const float* values, const float* expected, const int N)
{
    const float rounded_acc = Helpers::round(std::accumulate(values, values+N, 0.0));

    float rounded_values[N];
    std::transform(values, values+N, rounded_values, Helpers::round);
    const float acc_of_rounded = Helpers::round(std::accumulate(rounded_values, rounded_values+N, 0.0)), scaling = rounded_acc / acc_of_rounded;

    float interpolated_values[N];
    for(int i=0; i<N; ++i)
        interpolated_values[i] = Helpers::round(rounded_values[i] * scaling);
    const float acc_of_interpolated = Helpers::round(std::accumulate(interpolated_values, interpolated_values+N, 0.0));
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
        const miutil::miTime t(2011, 10, 14 + i, 6, 0, 0);
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
        EXPECT_EQ(i==bc->count()-1 ? "0140004000007000" : "0000001000007000", d.controlinfo().flagstring());
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
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "0140004000007000",  6.3, bc->update(3));

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
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140004000007000",  3.4, bc->update(1));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-16 06:00:00", "0000001000007000", 10.3, bc->update(2));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "0140004000007000",  2.5, bc->update(3));
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
        EXPECT_STATION_OBS_CONTROL_CORR(83880, miutil::miTime(2011, 10, 13+i, 6, 0, 0), "0000001000007000", 2.0, bc->update(i));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "0140004000007000", 2.0, bc->update(4));
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
    ASSERT_NO_THROW(db->selectData(series, "WHERE stationid = 83880 AND obstime BETWEEN '2011-10-12 06:00:00' AND '2011-10-17 06:00:00';"));
    ASSERT_EQ(6, series.size());

    for(int i=0; i<bc->count()-1; ++i)
        EXPECT_STATION_OBS_CONTROL_CORR(83880, miutil::miTime(2011, 10, 13+i, 6, 0, 0), "0000001000007000", 2.0, bc->update(i));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "0140004000007000", 2.0, bc->update(4));

    std::stringstream config;
    config << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   10" << std::endl
           << "Start_DD   =   12" << std::endl
           << "Start_hh   =   06" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   10" << std::endl
           << "End_DD     =   17" << std::endl
           << "End_hh     =   06" << std::endl
           << "#InterpCode=2"  << std::endl
           << "#Step_DD=1"  << std::endl
           << "ParamId=110"  << std::endl
           << "TypeIds=302"  << std::endl
           << "TypeIds=402"  << std::endl
           << "endpoint_cflags     = fmis=4&fd=7" << std::endl //changed wrt. default configuration
           << "missingpoint_cflags = fmis=1&fd=7" << std::endl //changed wrt. default configuration
           << "InterpolationDistance=50.0"  << std::endl;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    ASSERT_NO_THROW(db->selectData(series, "WHERE stationid = 83880 AND obstime BETWEEN '2011-10-12 06:00:00' AND '2011-10-17 06:00:00';"));
    ASSERT_EQ(6, series.size());

    std::ostringstream sql;
    sql << "UPDATE data SET original = 3.0, corrected = 3.0 WHERE obstime = '2011-10-13 06:00:00' AND stationid IN (83520, 84190, 84070);"
        << "UPDATE data SET original = 1.0, corrected = 1.0 WHERE obstime = '2011-10-14 06:00:00' AND stationid IN (83520, 84190, 84070);";
    ASSERT_NO_THROW(db->exec(sql.str()));

    ASSERT_NO_THROW(db->selectData(series, "WHERE stationid = 83880 AND obstime BETWEEN '2011-10-12 06:00:00' AND '2011-10-17 06:00:00';"));
    ASSERT_EQ(6, series.size());

    ASSERT_RUN(algo, bc, 2);

    ASSERT_NO_THROW(db->selectData(series, "WHERE stationid = 83880 AND obstime BETWEEN '2011-10-12 06:00:00' AND '2011-10-17 06:00:00';"));
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
    EXPECT_STATION_OBS_CONTROL_CORR(84070, "2011-10-14 06:00:00", "0140004000007000", 0.2, bc->update(1));
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

    data.setStation(83520).add("2011-10-13 06:00:00", 1.0, "0110000000001000", "")
        .setStation(84190).add("2011-10-13 06:00:00", 1.0, "0110000000001000", "");
    ASSERT_NO_THROW(data.update(db));

    ASSERT_RUN(algo, bc, 2);
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
    sql << "UPDATE data SET original = 1.0, corrected = 1.0, controlinfo = '0110000000001000', useinfo='7000000000000000', cfailed='' WHERE stationid IN (83520, 84190, 84070) AND obstime = '2011-10-13 06:00:00';";
    ASSERT_NO_THROW(db->exec(sql.str()));

    ASSERT_RUN(algo, bc, 3);
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 0.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140004000007000", 0.1, bc->update(1));
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 0.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140004000007000", 0.0, bc->update(1));
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000",  -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140004000007000",  -1, bc->update(1));
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000",  -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140004000007000", 0.1, bc->update(1));
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", 0.0, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140004000007000", 0.1, bc->update(1));
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
            data.add(miutil::miTime(2011, 5, d, 6, 0, 0), 2.7, "0110000000001000", "");
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
    params.UT0 = "2011-05-05 06:00:00";
    params.UT1 = "2011-05-16 06:00:00";

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
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-17 06:00:00", "0140004000007000",  2.3, bc->update(1));

    EXPECT_CFAILED(",QC2N_83520_84190,QC2-redist", bc->update(0));
    EXPECT_CFAILED(",QC2N_83520_84190,QC2-redist", bc->update(1));

    std::list<kvalobs::kvData> cfailedWithQC2;
    ASSERT_NO_THROW(db->selectData(cfailedWithQC2, " WHERE cfailed LIKE '%QC2%' AND stationid != 83880"));
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
    ASSERT_EQ(0, logs->find("would be redistributed to zeros"));

    data.setStation(88100)
        .add("2011-12-21 06:00:00", 5, "0110000000001000")
        .add("2011-12-22 06:00:00", 5, "0110000000001000");
    ASSERT_NO_THROW(data.update(db));
    ASSERT_RUN(algo, bc, 2);
    EXPECT_STATION_OBS_CONTROL_CORR(89650, "2011-12-21 06:00:00", "0000001000007000", 5.6, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(89650, "2011-12-22 06:00:00", "0110004000007000", 5.6, bc->update(1));
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
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140004000007000",  -1, bc->update(1));
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
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140000000007000",  -1, bc->update(1));
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
           << "TypeIds=402\n"
           << "InterpolationDistance=50.0\n";
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-29 06:00:00", "0000001000007000",  6, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-30 06:00:00", "0000001000007000",  6, bc->update(1));
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-31 06:00:00", "0140004000007000",  2, bc->update(2));
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
    int idx = logs->next(Message::INFO, 0);
    ASSERT_LE(0, idx);
    ASSERT_TRUE(boost::algorithm::contains(logs->text(idx), "no really good neighbors at obstime=2011-10-30 06:00:00"
                                           " for accumulation ending in [stationid=3200 AND obstime='2011-10-31 06:00:00'"));
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
    EXPECT_STATION_OBS_CONTROL_CORR(3200, "2011-10-31 06:00:00", "0140004000007000",  2, bc->update(2)) << "2011-10-31";
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-13 06:00:00", "0000001000007000", -1, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(83880, "2011-10-14 06:00:00", "0140000000007000", -1, bc->update(1));
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
    ASSERT_EQ(1, logs->count(Message::WARNING));
    ASSERT_EQ(0, logs->find("fmis=0 and original!=-1"));
}

