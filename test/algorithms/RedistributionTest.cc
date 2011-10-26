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

#define NEW_VERSION 1

#include "AlgorithmTestBase.h"
#ifdef NEW_VERSION
#include "algorithms/RedistributionAlgorithm.h"
#define ALGO_CLASS RedistributionAlgorithm2
#else
#include "algorithms/ProcessRedistribution.h"
#define ALGO_CLASS RedistributionAlgorithm
#endif
#include "AlgorithmHelpers.h"
#include "Helpers.h"
#include "foreach.h"

class RedistributionTest : public AlgorithmTestBase {
public:
    void SetUp();
    void TearDown();
protected:
    ALGO_CLASS* algo;
};

void RedistributionTest::SetUp()
{
    AlgorithmTestBase::SetUp();
    algo = new ALGO_CLASS();
    algo->setDatabase(db);
    algo->setBroadcaster(bc);
}

void RedistributionTest::TearDown()
{
    delete algo;
    AlgorithmTestBase::TearDown();
}

TEST_F(RedistributionTest, Station83880History2011117)
{
    std::ostringstream sql;
    sql << "INSERT INTO data VALUES (83880, '2011-10-10 06:00:00', 16.9, 110, '2011-10-10 09:01:31.322', 302, 0, 0, 16.9, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-11 06:00:00', -1, 110, '2011-10-11 06:03:01.324086', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83880, '2011-10-12 06:00:00', 0.3, 110, '2011-10-12 05:11:04.878004', 302, 0, 0, 0.3, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-13 06:00:00', 6.5, 110, '2011-10-13 05:04:36.37264', 302, 0, 0, 6.5, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-14 06:00:00', -32767, 110, '2011-10-15 00:35:29', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        //<< "INSERT INTO data VALUES (83880, '2011-10-14 06:00:00', -32767, 110, '2011-10-15 00:35:29', 302, 0, 0, 0.9, '0000001000007000', '5896900000000000', 'QC1-7-110,QC2N_84070_83520_84190,QC2-redist');"
        << "INSERT INTO data VALUES (83880, '2011-10-15 06:00:00', -32767, 110, '2011-10-16 00:35:40', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        //<< "INSERT INTO data VALUES (83880, '2011-10-15 06:00:00', -32767, 110, '2011-10-16 00:35:40', 302, 0, 0, 2.8, '0000001000007000', '5896900000000000', 'QC1-7-110,QC2N_83520_84190,QC2-redist');"
        << "INSERT INTO data VALUES (83880, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        //<< "INSERT INTO data VALUES (83880, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, 28.3, '0000001000007000', '5896900000000000', 'QC1-7-110,QC2N_83520_84190,QC2-redist');"
        << "INSERT INTO data VALUES (83880, '2011-10-17 06:00:00', 38.3, 110, '2011-10-17 09:11:19.382135', 302, 0, 0, 38.3, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        //<< "INSERT INTO data VALUES (83880, '2011-10-17 06:00:00', 38.3, 110, '2011-10-17 09:11:19', 302, 0, 0, 6.3, '0140004000007000', '5336900000000001', 'QC1-2-72.b12,QC1-7-110,QC2N_83520_84190,QC2-redist');"
        << "INSERT INTO data VALUES (83880, '2011-10-18 06:00:00', -32767, 110, '2011-10-19 00:31:53', 302, 0, 0, -32767, '0000003000000000', '9999900000000000', '');"
        << "INSERT INTO data VALUES (83880, '2011-10-19 06:00:00', 0.6, 110, '2011-10-19 06:11:12.788354', 302, 0, 0, 0.6, '0140000000000000', '7020400000000001', 'QC1-2-72.b12');"

        << "INSERT INTO data VALUES (83520, '2011-10-10 06:00:00', -1, 110, '2011-10-10 06:23:45.16182', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-11 06:00:00', -1, 110, '2011-10-11 07:24:36.110407', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-12 06:00:00', 0.1, 110, '2011-10-12 06:59:40.338528', 302, 0, 0, 0.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-13 06:00:00', 2.5, 110, '2011-10-13 07:48:30.943595', 302, 0, 0, 2.5, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-14 06:00:00', 2.6, 110, '2011-10-14 06:11:24.615894', 302, 0, 0, 2.6, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-15 06:00:00', 5.7, 110, '2011-10-15 07:16:28.315112', 302, 0, 0, 5.7, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-16 06:00:00', 54.2, 110, '2011-10-16 08:10:34.815529', 302, 0, 0, 54.2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-17 06:00:00', 11.4, 110, '2011-10-17 06:23:10.910783', 302, 0, 0, 11.4, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-18 06:00:00', 6.7, 110, '2011-10-18 06:44:43.25934', 302, 0, 0, 6.7, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-19 06:00:00', 0.7, 110, '2011-10-19 07:04:20.338402', 302, 0, 0, 0.7, '0110000000001000', '7000000000000000', '');"

        << "INSERT INTO data VALUES (84190, '2011-10-10 06:00:00', -1, 110, '2011-10-10 06:59:47.530607', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-11 06:00:00', -1, 110, '2011-10-11 05:56:23.216217', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-12 06:00:00', -1, 110, '2011-10-12 06:16:56.139462', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-13 06:00:00', 4.5, 110, '2011-10-13 06:23:40.804713', 302, 0, 0, 4.5, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-14 06:00:00', 0.1, 110, '2011-10-14 06:05:03.749075', 302, 0, 0, 0.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-15 06:00:00', 0.2, 110, '2011-10-16 15:13:23.870489', 302, 0, 0, 0.2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-16 06:00:00', 6.4, 110, '2011-10-16 15:13:23.723614', 302, 0, 0, 6.4, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-17 06:00:00', 2, 110, '2011-10-17 06:19:35.54819', 302, 0, 0, 2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-18 06:00:00', 0.3, 110, '2011-10-18 05:41:31.504729', 302, 0, 0, 0.3, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-19 06:00:00', 7.4, 110, '2011-10-19 06:36:28.514172', 302, 0, 0, 7.4, '0110000000001000', '7000000000000000', '');"

        /// << "INSERT INTO data VALUES (84070, '2011-10-10 06:00:00', 3.4, 110, '2011-10-10 12:41:17.514013', 302, 0, 0, 3.4, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        /// << "INSERT INTO data VALUES (84070, '2011-10-10 06:00:00', 3.4, 110, '2011-10-10 12:41:17', 302, 0, 0, -1, '0140004000007000', '5336900000000001', 'QC1-2-72.b12,QC1-7-110,QC2N_84190,QC2-redist');"
        << "INSERT INTO data VALUES (84070, '2011-10-11 06:00:00', -1, 110, '2011-10-11 05:25:53.667963', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-12 06:00:00', -1, 110, '2011-10-12 05:29:19.979603', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-13 06:00:00', 2.1, 110, '2011-10-14 05:34:52.530918', 302, 0, 0, 2.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-14 06:00:00', 0.6, 110, '2011-10-14 05:34:52.324945', 302, 0, 0, 0.6, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-15 06:00:00', -32767, 110, '2011-10-16 00:36:09', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-17 06:00:00', -32767, 110, '2011-10-18 00:39:35', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-18 06:00:00', 66.7, 110, '2011-10-19 06:43:04.224682', 302, 0, 0, 66.7, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-19 06:00:00', 1.4, 110, '2011-10-19 06:43:04.640316', 302, 0, 0, 1.4, '0110000000001000', '7000000000000000', '');";

    sql << "INSERT INTO station VALUES(83880, 68.0645, 16.663,   3, 0, 'SØRFJORD KRAFTVERK', NULL, 83880, NULL, NULL, NULL, 10, 't', '1985-01-01 00:00:00');"
        << "INSERT INTO station VALUES(83520, 67.8977, 15.8673, 70, 0, 'TØMMERNESET',        NULL, 83520, NULL, NULL, NULL,  9, 't', '1985-07-01 00:00:00');"
        << "INSERT INTO station VALUES(84070, 68.3302, 16.7883, 53, 0, 'BJØRKÅSEN',          NULL, 84070, NULL, NULL, NULL, 10, 't', '1964-01-01 00:00:00');"
        << "INSERT INTO station VALUES(84190, 68.2082, 17.5157, 29, 0, 'SKJOMEN - STIBERG',  NULL, 84190, NULL, NULL, NULL,  9, 't', '1987-09-01 00:00:00');";

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

    ASSERT_TRUE( db->exec(sql.str()) );

    std::stringstream config;
    config << "W_fhqc=0" << std::endl
            << "NibbleIndex = 15" << std::endl
            << "Start_YYYY = 2011" << std::endl
            << "Start_MM   =   10" << std::endl
            << "Start_DD   =   13" << std::endl
            << "Start_hh   =   06" << std::endl
            << "End_YYYY   = 2011" << std::endl
            << "End_MM     =   10" << std::endl
            << "End_DD     =   17" << std::endl
            << "End_hh     =   06" << std::endl
            << "U_2        =   0"  << std::endl
            << "InterpCode=2"  << std::endl
            << "Step_DD=1"  << std::endl
            << "ParamId=110"  << std::endl
            << "TypeIds=302"  << std::endl
            << "TypeIds=402"  << std::endl
            << "A_fd=2"  << std::endl
            << "S_fd=7"  << std::endl
            << "change_fmis=3->1"  << std::endl
            << "change_fmis=0->4"  << std::endl
            << "I_fd=1"  << std::endl
            << "InterpolationDistance=50.0"  << std::endl;
    ReadProgramOptions params;
    params.Parse(config);

    algo->run(params);
    ASSERT_EQ(4, bc->count());

    const float expected_corrected[4] = { 0.9, 2.8, 28.3, 6.3 };
    const char* expected_controlinfo[4] = { "0000001000007000", "0000001000007000", "0000001000007000", "0140004000007000" };
    for(int i=0; i<bc->count(); ++i) {
        const kvalobs::kvData& d = bc->updates()[i];
        EXPECT_EQ(83880, d.stationID()) << " at index " << i;
        EXPECT_FLOAT_EQ(expected_corrected[i], d.corrected()) << " at index " << i;
        EXPECT_EQ(expected_controlinfo[i], d.controlinfo().flagstring()) << " at index " << i;
    }

    bc->clear();
    algo->run(params);
    ASSERT_EQ(0, bc->count());
}

TEST_F(RedistributionTest, SeriesPossiblyIncomplete)
{
    std::ostringstream sql;
    sql // some data are fake for stationid=83880
        << "INSERT INTO data VALUES (83880, '2011-10-10 06:00:00', 16.9, 110, '2011-10-10 09:01:31.322', 302, 0, 0, 16.9, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-11 06:00:00', -1, 110, '2011-10-11 06:03:01.324086', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83880, '2011-10-12 06:00:00', 0.3, 110, '2011-10-12 05:11:04.878004', 302, 0, 0, 0.3, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-13 06:00:00', 6.5, 110, '2011-10-13 05:04:36.37264', 302, 0, 0, 6.5, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-14 06:00:00', 6.3, 110, '2011-10-14 05:00:00.00000', 302, 0, 0, 6.5, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-15 06:00:00', 6.7, 110, '2011-10-15 05:00:00.00000', 302, 0, 0, 6.5, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        //<< "INSERT INTO data VALUES (83880, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, 28.3, '0000001000007000', '5896900000000000', 'QC1-7-110,QC2N_83520_84190,QC2-redist');"
        << "INSERT INTO data VALUES (83880, '2011-10-17 06:00:00', 12.8, 110, '2011-10-17 09:11:19.382135', 302, 0, 0, 38.3, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        //<< "INSERT INTO data VALUES (83880, '2011-10-17 06:00:00', 12.8, 110, '2011-10-17 09:11:19', 302, 0, 0, 6.3, '0140004000007000', '5336900000000001', 'QC1-2-72.b12,QC1-7-110,QC2N_83520_84190,QC2-redist');"
        << "INSERT INTO data VALUES (83880, '2011-10-18 06:00:00', -32767, 110, '2011-10-19 00:31:53', 302, 0, 0, -32767, '0000003000000000', '9999900000000000', '');"
        << "INSERT INTO data VALUES (83880, '2011-10-19 06:00:00', 0.6, 110, '2011-10-19 06:11:12.788354', 302, 0, 0, 0.6, '0140000000000000', '7020400000000001', 'QC1-2-72.b12');"

        << "INSERT INTO data VALUES (83520, '2011-10-10 06:00:00', -1, 110, '2011-10-10 06:23:45.16182', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-11 06:00:00', -1, 110, '2011-10-11 07:24:36.110407', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-12 06:00:00', 0.1, 110, '2011-10-12 06:59:40.338528', 302, 0, 0, 0.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-13 06:00:00', 2.5, 110, '2011-10-13 07:48:30.943595', 302, 0, 0, 2.5, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-14 06:00:00', 2.6, 110, '2011-10-14 06:11:24.615894', 302, 0, 0, 2.6, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-15 06:00:00', 5.7, 110, '2011-10-15 07:16:28.315112', 302, 0, 0, 5.7, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-16 06:00:00', 54.2, 110, '2011-10-16 08:10:34.815529', 302, 0, 0, 54.2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-17 06:00:00', 11.4, 110, '2011-10-17 06:23:10.910783', 302, 0, 0, 11.4, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-18 06:00:00', 6.7, 110, '2011-10-18 06:44:43.25934', 302, 0, 0, 6.7, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-19 06:00:00', 0.7, 110, '2011-10-19 07:04:20.338402', 302, 0, 0, 0.7, '0110000000001000', '7000000000000000', '');"

        << "INSERT INTO data VALUES (84190, '2011-10-10 06:00:00', -1, 110, '2011-10-10 06:59:47.530607', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-11 06:00:00', -1, 110, '2011-10-11 05:56:23.216217', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-12 06:00:00', -1, 110, '2011-10-12 06:16:56.139462', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-13 06:00:00', 4.5, 110, '2011-10-13 06:23:40.804713', 302, 0, 0, 4.5, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-14 06:00:00', 0.1, 110, '2011-10-14 06:05:03.749075', 302, 0, 0, 0.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-15 06:00:00', 0.2, 110, '2011-10-16 15:13:23.870489', 302, 0, 0, 0.2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-16 06:00:00', 6.4, 110, '2011-10-16 15:13:23.723614', 302, 0, 0, 6.4, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-17 06:00:00', 2, 110, '2011-10-17 06:19:35.54819', 302, 0, 0, 2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-18 06:00:00', 0.3, 110, '2011-10-18 05:41:31.504729', 302, 0, 0, 0.3, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-19 06:00:00', 7.4, 110, '2011-10-19 06:36:28.514172', 302, 0, 0, 7.4, '0110000000001000', '7000000000000000', '');"

        /// << "INSERT INTO data VALUES (84070, '2011-10-10 06:00:00', 3.4, 110, '2011-10-10 12:41:17.514013', 302, 0, 0, 3.4, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        /// << "INSERT INTO data VALUES (84070, '2011-10-10 06:00:00', 3.4, 110, '2011-10-10 12:41:17', 302, 0, 0, -1, '0140004000007000', '5336900000000001', 'QC1-2-72.b12,QC1-7-110,QC2N_84190,QC2-redist');"
        << "INSERT INTO data VALUES (84070, '2011-10-11 06:00:00', -1, 110, '2011-10-11 05:25:53.667963', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-12 06:00:00', -1, 110, '2011-10-12 05:29:19.979603', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-13 06:00:00', 2.1, 110, '2011-10-14 05:34:52.530918', 302, 0, 0, 2.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-14 06:00:00', 0.6, 110, '2011-10-14 05:34:52.324945', 302, 0, 0, 0.6, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-15 06:00:00', -32767, 110, '2011-10-16 00:36:09', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-17 06:00:00', -32767, 110, '2011-10-18 00:39:35', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-18 06:00:00', 66.7, 110, '2011-10-19 06:43:04.224682', 302, 0, 0, 66.7, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-19 06:00:00', 1.4, 110, '2011-10-19 06:43:04.640316', 302, 0, 0, 1.4, '0110000000001000', '7000000000000000', '');";

    sql << "INSERT INTO station VALUES(83880, 68.0645, 16.663,   3, 0, 'SØRFJORD KRAFTVERK', NULL, 83880, NULL, NULL, NULL, 10, 't', '1985-01-01 00:00:00');"
        << "INSERT INTO station VALUES(83520, 67.8977, 15.8673, 70, 0, 'TØMMERNESET',        NULL, 83520, NULL, NULL, NULL,  9, 't', '1985-07-01 00:00:00');"
        << "INSERT INTO station VALUES(84070, 68.3302, 16.7883, 53, 0, 'BJØRKÅSEN',          NULL, 84070, NULL, NULL, NULL, 10, 't', '1964-01-01 00:00:00');"
        << "INSERT INTO station VALUES(84190, 68.2082, 17.5157, 29, 0, 'SKJOMEN - STIBERG',  NULL, 84190, NULL, NULL, NULL,  9, 't', '1987-09-01 00:00:00');";

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

    ASSERT_TRUE( db->exec(sql.str()) );

    std::stringstream config;
    config << "W_fhqc=0" << std::endl
            << "NibbleIndex = 15" << std::endl
            // start is a missing value, therefore no redistribution may be performed
            << "Start_YYYY = 2011" << std::endl
            << "Start_MM   =   10" << std::endl
            << "Start_DD   =   16" << std::endl
            << "Start_hh   =   06" << std::endl
            << "End_YYYY   = 2011" << std::endl
            << "End_MM     =   10" << std::endl
            << "End_DD     =   17" << std::endl
            << "End_hh     =   06" << std::endl
            << "U_2        =   0"  << std::endl
            << "InterpCode=2"  << std::endl
            << "Step_DD=1"  << std::endl
            << "ParamId=110"  << std::endl
            << "TypeIds=302"  << std::endl
            << "TypeIds=402"  << std::endl
            << "A_fd=2"  << std::endl
            << "S_fd=7"  << std::endl
            << "change_fmis=3->1"  << std::endl
            << "change_fmis=0->4"  << std::endl
            << "I_fd=1"  << std::endl
            << "InterpolationDistance=50.0"  << std::endl;
    ReadProgramOptions params;
    params.Parse(config);

    algo->run(params);
    ASSERT_EQ(0, bc->count());
}

TEST_F(RedistributionTest, StartOfDatabase)
{
    // may not redistribute if first missing value is start of database
    std::ostringstream sql;
    sql // some data are fake for stationid=83880
        << "INSERT INTO data VALUES (83880, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-17 06:00:00', 12.8, 110, '2011-10-17 09:11:19.382135', 302, 0, 0, 38.3, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-18 06:00:00', 0.6, 110, '2011-10-19 06:11:12.788354', 302, 0, 0, 0.6, '0140000000000000', '7020400000000001', 'QC1-2-72.b12');"

        << "INSERT INTO data VALUES (83520, '2011-10-16 06:00:00', 54.2, 110, '2011-10-16 08:10:34.815529', 302, 0, 0, 54.2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-17 06:00:00', 11.4, 110, '2011-10-17 06:23:10.910783', 302, 0, 0, 11.4, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-18 06:00:00', 6.7, 110, '2011-10-18 06:44:43.25934', 302, 0, 0, 6.7, '0110000000001000', '7000000000000000', '');"

        << "INSERT INTO data VALUES (84190, '2011-10-16 06:00:00', 6.4, 110, '2011-10-16 15:13:23.723614', 302, 0, 0, 6.4, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-17 06:00:00', 2, 110, '2011-10-17 06:19:35.54819', 302, 0, 0, 2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-18 06:00:00', 0.3, 110, '2011-10-18 05:41:31.504729', 302, 0, 0, 0.3, '0110000000001000', '7000000000000000', '');"

        << "INSERT INTO data VALUES (84070, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-17 06:00:00', -32767, 110, '2011-10-18 00:39:35', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-18 06:00:00', 66.7, 110, '2011-10-19 06:43:04.224682', 302, 0, 0, 66.7, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');";

    sql << "INSERT INTO station VALUES(83880, 68.0645, 16.663,   3, 0, 'SØRFJORD KRAFTVERK', NULL, 83880, NULL, NULL, NULL, 10, 't', '1985-01-01 00:00:00');"
        << "INSERT INTO station VALUES(83520, 67.8977, 15.8673, 70, 0, 'TØMMERNESET',        NULL, 83520, NULL, NULL, NULL,  9, 't', '1985-07-01 00:00:00');"
        << "INSERT INTO station VALUES(84070, 68.3302, 16.7883, 53, 0, 'BJØRKÅSEN',          NULL, 84070, NULL, NULL, NULL, 10, 't', '1964-01-01 00:00:00');"
        << "INSERT INTO station VALUES(84190, 68.2082, 17.5157, 29, 0, 'SKJOMEN - STIBERG',  NULL, 84190, NULL, NULL, NULL,  9, 't', '1987-09-01 00:00:00');";

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

    ASSERT_TRUE( db->exec(sql.str()) );

    std::stringstream config;
    config << "W_fhqc=0" << std::endl
            << "NibbleIndex = 15" << std::endl
            // start is a missing value, therefore no redistribution may be performed
            << "Start_YYYY = 2011" << std::endl
            << "Start_MM   =   10" << std::endl
            << "Start_DD   =   14" << std::endl
            << "Start_hh   =   06" << std::endl
            << "End_YYYY   = 2011" << std::endl
            << "End_MM     =   10" << std::endl
            << "End_DD     =   17" << std::endl
            << "End_hh     =   06" << std::endl
            << "U_2        =   0"  << std::endl
            << "InterpCode=2"  << std::endl
            << "Step_DD=1"  << std::endl
            << "ParamId=110"  << std::endl
            << "TypeIds=302"  << std::endl
            << "TypeIds=402"  << std::endl
            << "A_fd=2"  << std::endl
            << "S_fd=7"  << std::endl
            << "change_fmis=3->1"  << std::endl
            << "change_fmis=0->4"  << std::endl
            << "I_fd=1"  << std::endl
            << "InterpolationDistance=50.0"  << std::endl;
    ReadProgramOptions params;
    params.Parse(config);

    algo->run(params);
    ASSERT_EQ(0, bc->count());
}

TEST_F(RedistributionTest, TwoSeries)
{
    // redistribute twice if there are two accumulations
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, OneOfTwoTypeids)
{
    // may only redistribute among same typeid
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, NoGoodNeighbors)
{
    // may not redistribute if first missing value is start of database
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, BoneDry)
{
    // may not redistribute if first missing value is start of database
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, IncompleteSeries)
{
    // may not redistribute if no accumulated value yet
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, VerySmallValues)
{
    // may not redistribute if first missing value is start of database
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, Bugzilla1296)
{
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, Bugzilla1304)
{
    // by default assume dry
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, Bugzilla1322)
{
    // redistribution run starts in the middle of a sequence of missing values
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, Bugzilla1325)
{
    // consistency of accumulated value with sum of redistributed values
    FAIL() << "test not implemented";
}

TEST_F(RedistributionTest, Bugzilla1333)
{
    std::ostringstream sql;
    sql // some data are fake for stationid=83880
        << "INSERT INTO data VALUES (83880, '2011-10-10 06:00:00', 16.9, 110, '2011-10-10 09:01:31.322', 302, 0, 0, 16.9, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-11 06:00:00', -1, 110, '2011-10-11 06:03:01.324086', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83880, '2011-10-12 06:00:00', 0.3, 110, '2011-10-12 05:11:04.878004', 302, 0, 0, 0.3, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-13 06:00:00', 6.5, 110, '2011-10-13 05:04:36.37264', 302, 0, 0, 6.5, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-14 06:00:00', 6.3, 110, '2011-10-14 05:00:00.00000', 302, 0, 0, 6.5, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-15 06:00:00', 6.7, 110, '2011-10-15 05:00:00.00000', 302, 0, 0, 6.5, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        //<< "INSERT INTO data VALUES (83880, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, 28.3, '0000001000007000', '5896900000000000', 'QC1-7-110,QC2N_83520_84190,QC2-redist');"
        << "INSERT INTO data VALUES (83880, '2011-10-17 06:00:00', 12.8, 110, '2011-10-17 09:11:19.382135', 302, 0, 0, 38.3, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        //<< "INSERT INTO data VALUES (83880, '2011-10-17 06:00:00', 12.8, 110, '2011-10-17 09:11:19', 302, 0, 0, 6.3, '0140004000007000', '5336900000000001', 'QC1-2-72.b12,QC1-7-110,QC2N_83520_84190,QC2-redist');"
        << "INSERT INTO data VALUES (83880, '2011-10-18 06:00:00', -32767, 110, '2011-10-19 00:31:53', 302, 0, 0, -32767, '0000003000000000', '9999900000000000', '');"
        << "INSERT INTO data VALUES (83880, '2011-10-19 06:00:00', 0.6, 110, '2011-10-19 06:11:12.788354', 302, 0, 0, 0.6, '0140000000000000', '7020400000000001', 'QC1-2-72.b12');"

        << "INSERT INTO data VALUES (83520, '2011-10-10 06:00:00', -1, 110, '2011-10-10 06:23:45.16182', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-11 06:00:00', -1, 110, '2011-10-11 07:24:36.110407', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-12 06:00:00', 0.1, 110, '2011-10-12 06:59:40.338528', 302, 0, 0, 0.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-13 06:00:00', 2.5, 110, '2011-10-13 07:48:30.943595', 302, 0, 0, 2.5, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-14 06:00:00', 2.6, 110, '2011-10-14 06:11:24.615894', 302, 0, 0, 2.6, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-15 06:00:00', 5.7, 110, '2011-10-15 07:16:28.315112', 302, 0, 0, 5.7, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-16 06:00:00', 54.2, 110, '2011-10-16 08:10:34.815529', 302, 0, 0, 54.2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-17 06:00:00', 11.4, 110, '2011-10-17 06:23:10.910783', 302, 0, 0, 11.4, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-18 06:00:00', 6.7, 110, '2011-10-18 06:44:43.25934', 302, 0, 0, 6.7, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83520, '2011-10-19 06:00:00', 0.7, 110, '2011-10-19 07:04:20.338402', 302, 0, 0, 0.7, '0110000000001000', '7000000000000000', '');"

        << "INSERT INTO data VALUES (84190, '2011-10-10 06:00:00', -1, 110, '2011-10-10 06:59:47.530607', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-11 06:00:00', -1, 110, '2011-10-11 05:56:23.216217', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-12 06:00:00', -1, 110, '2011-10-12 06:16:56.139462', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-13 06:00:00', 4.5, 110, '2011-10-13 06:23:40.804713', 302, 0, 0, 4.5, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-14 06:00:00', 0.1, 110, '2011-10-14 06:05:03.749075', 302, 0, 0, 0.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-15 06:00:00', 0.2, 110, '2011-10-16 15:13:23.870489', 302, 0, 0, 0.2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-16 06:00:00', 6.4, 110, '2011-10-16 15:13:23.723614', 302, 0, 0, 6.4, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-17 06:00:00', 2, 110, '2011-10-17 06:19:35.54819', 302, 0, 0, 2, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-18 06:00:00', 0.3, 110, '2011-10-18 05:41:31.504729', 302, 0, 0, 0.3, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84190, '2011-10-19 06:00:00', 7.4, 110, '2011-10-19 06:36:28.514172', 302, 0, 0, 7.4, '0110000000001000', '7000000000000000', '');"

        /// << "INSERT INTO data VALUES (84070, '2011-10-10 06:00:00', 3.4, 110, '2011-10-10 12:41:17.514013', 302, 0, 0, 3.4, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        /// << "INSERT INTO data VALUES (84070, '2011-10-10 06:00:00', 3.4, 110, '2011-10-10 12:41:17', 302, 0, 0, -1, '0140004000007000', '5336900000000001', 'QC1-2-72.b12,QC1-7-110,QC2N_84190,QC2-redist');"
        << "INSERT INTO data VALUES (84070, '2011-10-11 06:00:00', -1, 110, '2011-10-11 05:25:53.667963', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-12 06:00:00', -1, 110, '2011-10-12 05:29:19.979603', 302, 0, 0, -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-13 06:00:00', 2.1, 110, '2011-10-14 05:34:52.530918', 302, 0, 0, 2.1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-14 06:00:00', 0.6, 110, '2011-10-14 05:34:52.324945', 302, 0, 0, 0.6, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (84070, '2011-10-15 06:00:00', -32767, 110, '2011-10-16 00:36:09', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-17 06:00:00', -32767, 110, '2011-10-18 00:39:35', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-18 06:00:00', 66.7, 110, '2011-10-19 06:43:04.224682', 302, 0, 0, 66.7, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (84070, '2011-10-19 06:00:00', 1.4, 110, '2011-10-19 06:43:04.640316', 302, 0, 0, 1.4, '0110000000001000', '7000000000000000', '');";

    sql << "INSERT INTO station VALUES(83880, 68.0645, 16.663,   3, 0, 'SØRFJORD KRAFTVERK', NULL, 83880, NULL, NULL, NULL, 10, 't', '1985-01-01 00:00:00');"
        << "INSERT INTO station VALUES(83520, 67.8977, 15.8673, 70, 0, 'TØMMERNESET',        NULL, 83520, NULL, NULL, NULL,  9, 't', '1985-07-01 00:00:00');"
        << "INSERT INTO station VALUES(84070, 68.3302, 16.7883, 53, 0, 'BJØRKÅSEN',          NULL, 84070, NULL, NULL, NULL, 10, 't', '1964-01-01 00:00:00');"
        << "INSERT INTO station VALUES(84190, 68.2082, 17.5157, 29, 0, 'SKJOMEN - STIBERG',  NULL, 84190, NULL, NULL, NULL,  9, 't', '1987-09-01 00:00:00');";

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

    ASSERT_TRUE( db->exec(sql.str()) );

    std::stringstream config;
    config << "W_fhqc=0" << std::endl
            << "NibbleIndex = 15" << std::endl
            << "Start_YYYY = 2011" << std::endl
            << "Start_MM   =   10" << std::endl
            << "Start_DD   =   13" << std::endl
            << "Start_hh   =   06" << std::endl
            << "End_YYYY   = 2011" << std::endl
            << "End_MM     =   10" << std::endl
            << "End_DD     =   17" << std::endl
            << "End_hh     =   06" << std::endl
            << "U_2        =   0"  << std::endl
            << "InterpCode=2"  << std::endl
            << "Step_DD=1"  << std::endl
            << "ParamId=110"  << std::endl
            << "TypeIds=302"  << std::endl
            << "TypeIds=402"  << std::endl
            << "A_fd=2"  << std::endl
            << "S_fd=7"  << std::endl
            << "change_fmis=3->1"  << std::endl
            << "change_fmis=0->4"  << std::endl
            << "I_fd=1"  << std::endl
            << "InterpolationDistance=50.0"  << std::endl;
    ReadProgramOptions params;
    params.Parse(config);

    algo->run(params);
    ASSERT_EQ(2, bc->count());

    const float expected_corrected[2] = { 10.5, 2.3 };
    const char* expected_controlinfo[2] = { "0000001000007000", "0140004000007000" };
    const char* expected_cfailed_end[2] = { ",QC2N_83520_84190,QC2-redist", ",QC2N_83520_84190,QC2-redist" };
    for(int i=0; i<bc->count(); ++i) {
        const kvalobs::kvData& d = bc->updates()[i];
        EXPECT_EQ(83880, d.stationID()) << " at index " << i;
        EXPECT_FLOAT_EQ(expected_corrected[i], d.corrected()) << " at index " << i;
        EXPECT_EQ(expected_controlinfo[i], d.controlinfo().flagstring()) << " at index " << i;
        EXPECT_TRUE(Helpers::endsWith(d.cfailed(), expected_cfailed_end[i])) << " at index " << i << " cfailed=" << d.cfailed();
    }

    std::list<kvalobs::kvData> cfailedWithQC2;
    ASSERT_TRUE( db->selectData(cfailedWithQC2, " WHERE cfailed LIKE '%QC2%'") );
    foreach(const kvalobs::kvData& d, cfailedWithQC2) {
        EXPECT_EQ(83880, d.stationID()) << " data=" << d;
    }

    bc->clear();
    algo->run(params);
    ASSERT_EQ(0, bc->count());
}
