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
#include "algorithms/DipTestAlgorithm.h"
#include "helpers/AlgorithmHelpers.h"

#include <boost/algorithm/string/predicate.hpp>

class DipTestTest : public AlgorithmTestBase {
public:
    void SetUp();
};

void DipTestTest::SetUp()
{
    algo = new DipTestAlgorithm();
    AlgorithmTestBase::SetUp();
}

TEST_F(DipTestTest, Bugzilla1327)
{
    DataList data(90800, 90, 330);
    data.add("2011-08-13 12:00:00", 3.2, "0111000000000000", "")
        .add("2011-08-13 13:00:00", 3.0, "0111000000000000", "")
        .add("2011-08-13 14:00:00", 3.7, "0111000000000000", "")
        .add("2011-08-13 15:00:00", 4.1, "0111000000000000", "")
        .add("2011-08-13 16:00:00", 4.3, "0111000000000000", "")
        .add("2011-08-13 17:00:00", 64,  "0412000000000000", "QC1-1-90,QC1-3a-90")
        .add("2011-08-13 18:00:00", 2,   "0112000000000000", "QC1-3a-90")
        .add("2011-08-13 19:00:00", 2.9, "0111000000000000", "")
        .add("2011-08-13 20:00:00", 2,   "0111000000000000", "")
        .add("2011-08-13 21:00:00", 3.3, "0111000000000000", "")
        .add("2011-08-13 22:00:00", 2.7, "0111000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(90800, 70.2457, 19.5005, 21, 0, 'TORSVÅG FYR', 1033, 90800, NULL, NULL, NULL, 8, 't', '1933-01-01 00:00:00');";

    sql << "INSERT INTO station_param VALUES(90800, 90, 0, 0,   1,  31, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;35.5;32.3;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0,  32,  59, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;37.6;34.2;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0,  60,  90, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;38.6;35.1;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0,  91, 120, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;27.3;24.8;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 121, 151, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;23.2;21.1;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 152, 181, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;30.9;28.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 182, 212, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;45.3;41.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 213, 243, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;21.6;19.7;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 244, 273, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;34.0;30.8;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 274, 304, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;39.1;35.5;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 305, 334, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;40.1;36.3;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 335, 365, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;37.0;33.7;0.0;0.0;0', NULL, '1500-01-01 00:00:00');";

    sql << "INSERT INTO station_param VALUES (0, 90, 0, 0, 0, 365, -1, 'QC1-3a-90', 'max\n12.5', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 90, 0, 0, 0, 365, -1, 'QC1-3b-90', 'no\n5', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 90, 0, 0, 1, 365, -1, 'QC1-1-90x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 90, 0, 0, 1, 365, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;93;93;0;0;0', 'DEFAULT MAX-MIN RANGE', '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   08" << std::endl
           << "Start_DD   =   13" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   08" << std::endl
           << "End_DD     =   13" << std::endl
           << "End_hh     =   22" << std::endl
           << "ParValFilename = list: 1 9.0, 2 60.0, 81 10.0, 85 10.0, 86 12.5, 87 10.0, 88 12.5, 89 15.0, 90 12.5,"
           <<     "91 12.5, 92 15.0, 93 15.0, 94 15.0, 104 100, 105 3.0, 113 15.0, 123 60.0, 172 12.0, 174 12.0, 175 12.0,"
           <<     "176 12.0, 177 12.0, 197 1000.0, 198 500.0, 199 500.0, 200 1000.0, 211 7.5, 212 7.5, 213 7.5, 215 7.5,"
           <<     " 221 15.0, 222 15.0" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    std::list<kvalobs::kvData> series;
    kvtime::time t1 = kvtime::maketime("2011-08-13 17:00:00"), t2 = t1;
    kvtime::addHours(t2, 1);

    ASSERT_NO_THROW(series = db->findDataOrderObstime(90800, 90, TimeRange(t1, t2)));
    ASSERT_EQ(2, series.size());
    std::list<kvalobs::kvData>::const_iterator it = series.begin();
    EXPECT_EQ("0419000000000000", it->controlinfo().flagstring());
    EXPECT_TRUE(boost::algorithm::ends_with(it->cfailed(), "QC2d-1-A"));
    EXPECT_GE(4, it->corrected());
    EXPECT_LE(2, it->corrected());
    it++;
    EXPECT_EQ("0114000000000000", it->controlinfo().flagstring());
    EXPECT_TRUE(boost::algorithm::ends_with(it->cfailed(), "QC2d-1"));
    EXPECT_FLOAT_EQ(2, it->corrected());

    ASSERT_RUN(algo, bc, 0);
}

TEST_F(DipTestTest, FromWikiSpecLinear)
{
    // see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:requirements:algorithms:diptest03 "Linear" example

    std::ostringstream sql;
    sql << "INSERT INTO data VALUES (18230, '2018-09-09 02:00:00',  4.3, 87, '2018-09-09 01:56:30', 330, 0, 0, 4.3, '1101000000000000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (18230, '2018-09-09 03:00:00',   18, 87, '2018-09-09 02:56:33', 330, 0, 0,  18, '1102000000000000', '5033300000000001', 'QC1-3a-87:1');"
        << "INSERT INTO data VALUES (18230, '2018-09-09 04:00:00',    4, 87, '2018-09-09 03:57:17', 330, 0, 0,   4, '1102000000000000', '5000000000000001', '');";

    sql << "INSERT INTO station VALUES (18230, 59.9228, 10.8342, 90, 0, 'ALNA', 1487, 18230, NULL, NULL, NULL, 8, 't', '2007-12-03 00:00:00');";

    sql << "INSERT INTO station_param VALUES (18230, 87, 0, 0,   1 , 31, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;20.4;15.4;0;0;0', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0,  60,  90, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;20.9;15.9;0;0;0', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0,  91, 120, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;22;17;0;0;0',     '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 152, 181, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;20.4;15.4;0;0;0', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 182, 212, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;20.9;15.9;0;0;0', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 213, 243, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;27.6;22.6;0;0;0', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 244, 273, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;23;18;0;0;0',     '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 305, 334, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;27.6;22.6;0;0;0', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 335, 365, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;24;19;0;0;0',     '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 274, 304, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;25.1;20.1;0;0;0', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 121, 151, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;19.4;14.4;0;0;0', '', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (18230, 87, 0, 0, 32, 59, -1, 'QC1-1-87', 'max;highest;high;low;lowest;min\n98;26.5;21.5;0;0;0',   '', '1500-01-01 00:00:00');";

    sql << "INSERT INTO station_param VALUES (0, 87, 0, 0, 0, 365, -1, 'QC1-3a-87', 'max\n10.0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 87, 0, 0, 0, 365, -1, 'QC1-3b-87', 'no\n5', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 87, 0, 0, 1, 365, -1, 'QC1-1-87x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 87, 0, 0, 1, 365, -1, 'QC1-1-87',  'max;highest;high;low;lowest;min\n93;93;93;0;0;0', 'DEFAULT MAX-MIN RANGE', '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2018" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   08" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2018" << std::endl
           << "End_MM     =   09" << std::endl
           << "End_DD     =   10" << std::endl
           << "End_hh     =   22" << std::endl
           << "ParValFilename = list: 1 9.0, 2 60.0, 81 10.0, 85 10.0, 86 12.5, 87 10.0, 88 12.5, 89 15.0, 90 12.5,"
           <<     "91 12.5, 92 15.0, 93 15.0, 94 15.0, 104 100, 105 3.0, 113 15.0, 123 60.0, 172 12.0, 174 12.0, 175 12.0,"
           <<     "176 12.0, 177 12.0, 197 1000.0, 198 500.0, 199 500.0, 200 1000.0, 211 7.5, 212 7.5, 213 7.5, 215 7.5,"
           <<     " 221 15.0, 222 15.0" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    std::list<kvalobs::kvData> series;
    kvtime::time t1 = kvtime::maketime("2018-09-09 03:00:00"), t2 = t1;
    kvtime::addHours(t2, 1);

    ASSERT_NO_THROW(series = db->findDataOrderObstime(18230, 87, TimeRange(t1, t2)));
    ASSERT_EQ(2, series.size());
    std::list<kvalobs::kvData>::const_iterator it = series.begin();
    EXPECT_EQ("1109000000000000", it->controlinfo().flagstring());
    EXPECT_TRUE(boost::algorithm::ends_with(it->cfailed(), "QC2d-1-L"));
    EXPECT_FLOAT_EQ(4.2, it->corrected());
    it++;
    EXPECT_EQ("1104000000000000", it->controlinfo().flagstring());
    EXPECT_TRUE(boost::algorithm::ends_with(it->cfailed(), "QC2d-1"));

    ASSERT_RUN(algo, bc, 0);
}

TEST_F(DipTestTest, FromWikiSpecAkima)
{
    // see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:requirements:algorithms:diptest03 "Akima" example

    std::ostringstream sql;

    sql // first line is fake, wiki example does not contain enough preceding data for Akima (needs 3 before, 2 after)
        << "INSERT INTO data VALUES (12320, '2018-09-25 17:00:00', 381.8, 104, '2018-09-25 16:00:00', 330, 0, 0, 381.8, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 18:00:00', 381.7, 104, '2018-09-25 17:54:15', 330, 0, 0, 381.7, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 19:00:00', 381.6, 104, '2018-09-25 18:53:50', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 20:00:00', 284.6, 104, '2018-09-25 19:54:35', 330, 0, 0, 284.6, '1102000000100100', '5033300000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 21:00:00', 381.6, 104, '2018-09-25 20:54:44', 330, 0, 0, 381.6, '1102000000100100', '5000000000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 22:00:00', 381.6, 104, '2018-09-25 21:53:48', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 23:00:00', 381.6, 104, '2018-09-25 22:53:46', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');";

    sql << "INSERT INTO station VALUES(12320, 60.818, 11.0696, 221, 0, 'HAMAR - STAVSBERG', 1385, 12320, NULL, NULL, NULL, 8, 't', '2005-10-21 00:00:00');";

    sql << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500.0;500.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3a-104', 'max\n30.0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3b-104', 'no\n48', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(12320, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500;500;50.0;0.0;-3.0', '', '2005-11-04 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2018" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   24" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2018" << std::endl
           << "End_MM     =   09" << std::endl
           << "End_DD     =   26" << std::endl
           << "End_hh     =   22" << std::endl
           << "ParValFilename = list: 1 9.0, 2 60.0, 81 10.0, 85 10.0, 86 12.5, 87 10.0, 88 12.5, 89 15.0, 90 12.5,"
           <<     "91 12.5, 92 15.0, 93 15.0, 94 15.0, 104 100, 105 3.0, 113 15.0, 123 60.0, 172 12.0, 174 12.0, 175 12.0,"
           <<     "176 12.0, 177 12.0, 197 1000.0, 198 500.0, 199 500.0, 200 1000.0, 211 7.5, 212 7.5, 213 7.5, 215 7.5,"
           <<     " 221 15.0, 222 15.0" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    std::list<kvalobs::kvData> series;
    kvtime::time t1 = kvtime::maketime("2018-09-25 20:00:00"), t2 = t1;
    kvtime::addHours(t2, 1);

    ASSERT_NO_THROW(series = db->findDataOrderObstime(12320, 104, TimeRange(t1, t2)));
    ASSERT_EQ(2, series.size());

    EXPECT_STATION_OBS_CONTROL_CORR(12320, "2018-09-25 20:00:00", "1109000000100100", 381.6, bc->update(0));
    EXPECT_CFAILED(",QC2d-1-A", bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(12320, "2018-09-25 21:00:00", "1104000000100100", 381.6, bc->update(1));

    ASSERT_RUN(algo, bc, 0);
}

TEST_F(DipTestTest, BadNeighboursForAkima)
{
    // in this test, no changes should be performed; neighbours required for akima, but not for linear artificially got useinfo(2) != 0

    std::ostringstream sql;

    sql << "INSERT INTO data VALUES (12320, '2018-09-25 17:00:00', 381.8, 104, '2018-09-25 16:00:00', 330, 0, 0, 381.8, '1101000000100100', '7020000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 18:00:00', 381.7, 104, '2018-09-25 17:54:15', 330, 0, 0, 381.7, '1101000000100100', '7020000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 19:00:00', 381.6, 104, '2018-09-25 18:53:50', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 20:00:00', 284.6, 104, '2018-09-25 19:54:35', 330, 0, 0, 284.6, '1102000000100100', '5033300000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 21:00:00', 381.6, 104, '2018-09-25 20:54:44', 330, 0, 0, 381.6, '1102000000100100', '5000000000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 22:00:00', 381.6, 104, '2018-09-25 21:53:48', 330, 0, 0, 381.6, '1101000000100100', '7020000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 23:00:00', 381.6, 104, '2018-09-25 22:53:46', 330, 0, 0, 381.6, '1101000000100100', '7020000000000000', '');";

    sql << "INSERT INTO station VALUES(12320, 60.818, 11.0696, 221, 0, 'HAMAR - STAVSBERG', 1385, 12320, NULL, NULL, NULL, 8, 't', '2005-10-21 00:00:00');";

    sql << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500.0;500.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3a-104', 'max\n30.0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3b-104', 'no\n48', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(12320, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500;500;50.0;0.0;-3.0', '', '2005-11-04 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2018" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   24" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2018" << std::endl
           << "End_MM     =   09" << std::endl
           << "End_DD     =   26" << std::endl
           << "End_hh     =   22" << std::endl
           << "ParValFilename = list: 104 100" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    std::list<kvalobs::kvData> series;
    kvtime::time t1 = kvtime::maketime("2018-09-25 20:00:00"), t2 = t1;
    kvtime::addHours(t2, 1);

    ASSERT_NO_THROW(series = db->findDataOrderObstime(12320, 104, TimeRange(t1, t2)));
    ASSERT_EQ(2, series.size());

    EXPECT_STATION_OBS_CONTROL_CORR(12320, "2018-09-25 20:00:00", "1109000000100100", 381.6, bc->update(0));
    EXPECT_CFAILED(",QC2d-1-L", bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(12320, "2018-09-25 21:00:00", "1104000000100100", 381.6, bc->update(1));

    ASSERT_RUN(algo, bc, 0);
}

#if 0
this test fails as there is no check for the size of the dip
TEST_F(DipTestTest, DipTooSmall)
{
    // in this test, no changes should be performed; dip is too small

    std::ostringstream sql;

    sql << "INSERT INTO data VALUES (12320, '2018-09-25 17:00:00', 381.8, 104, '2018-09-25 16:00:00', 330, 0, 0, 381.8, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 18:00:00', 381.7, 104, '2018-09-25 17:54:15', 330, 0, 0, 381.7, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 19:00:00', 381.6, 104, '2018-09-25 18:53:50', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 20:00:00', 371.6, 104, '2018-09-25 19:54:35', 330, 0, 0, 371.6, '1102000000100100', '5033300000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 21:00:00', 381.6, 104, '2018-09-25 20:54:44', 330, 0, 0, 381.6, '1102000000100100', '5000000000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 22:00:00', 381.6, 104, '2018-09-25 21:53:48', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 23:00:00', 381.6, 104, '2018-09-25 22:53:46', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');";

    sql << "INSERT INTO station VALUES(12320, 60.818, 11.0696, 221, 0, 'HAMAR - STAVSBERG', 1385, 12320, NULL, NULL, NULL, 8, 't', '2005-10-21 00:00:00');";

    sql << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500.0;500.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3a-104', 'max\n30.0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3b-104', 'no\n48', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(12320, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500;500;50.0;0.0;-3.0', '', '2005-11-04 00:00:00');";

    ASSERT_TRUE( ASSERT_NO_THROW(db->exec(sql.str()) ));

    std::stringstream config;
    config << "Start_YYYY = 2018" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   24" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2018" << std::endl
           << "End_MM     =   09" << std::endl
           << "End_DD     =   26" << std::endl
           << "End_hh     =   22" << std::endl
           << "ParValFilename = list: 104 100" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
}
#endif

TEST_F(DipTestTest, JumpTooMuch)
{
    // in this test, no changes should be performed; jump of neighbours is too big

    std::ostringstream sql;

    sql << "INSERT INTO data VALUES (12320, '2018-09-25 17:00:00', 381.8, 104, '2018-09-25 16:00:00', 330, 0, 0, 381.8, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 18:00:00', 381.7, 104, '2018-09-25 17:54:15', 330, 0, 0, 381.7, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 19:00:00', 381.6, 104, '2018-09-25 18:53:50', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 20:00:00', 351.6, 104, '2018-09-25 19:54:35', 330, 0, 0, 351.6, '1102000000100100', '5033300000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 21:00:00', 321.6, 104, '2018-09-25 20:54:44', 330, 0, 0, 321.6, '1102000000100100', '5000000000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 22:00:00', 321.6, 104, '2018-09-25 21:53:48', 330, 0, 0, 321.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 23:00:00', 321.6, 104, '2018-09-25 22:53:46', 330, 0, 0, 321.6, '1101000000100100', '7000000000000000', '');";

    sql << "INSERT INTO station VALUES(12320, 60.818, 11.0696, 221, 0, 'HAMAR - STAVSBERG', 1385, 12320, NULL, NULL, NULL, 8, 't', '2005-10-21 00:00:00');";

    sql << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500.0;500.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3a-104', 'max\n30.0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3b-104', 'no\n48', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(12320, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500;500;50.0;0.0;-3.0', '', '2005-11-04 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2018" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   24" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2018" << std::endl
           << "End_MM     =   09" << std::endl
           << "End_DD     =   26" << std::endl
           << "End_hh     =   22" << std::endl
           << "ParValFilename = list: 104 50" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
}

TEST_F(DipTestTest, AfterHQC)
{
    // in this test, no changes should be performed; neighbours artificially got controlinfo(15) != 0

    std::ostringstream sql;

    sql << "INSERT INTO data VALUES (12320, '2018-09-25 17:00:00', 381.8, 104, '2018-09-25 16:00:00', 330, 0, 0, 381.8, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 18:00:00', 381.7, 104, '2018-09-25 17:54:15', 330, 0, 0, 381.7, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 19:00:00', 381.6, 104, '2018-09-25 18:53:50', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 20:00:00', 371.6, 104, '2018-09-25 19:54:35', 330, 0, 0, 371.6, '1102000000100101', '5033300000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 21:00:00', 381.6, 104, '2018-09-25 20:54:44', 330, 0, 0, 381.6, '1102000000100100', '5000000000000001', 'QC1-3a-104:1');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 22:00:00', 381.6, 104, '2018-09-25 21:53:48', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');"
        << "INSERT INTO data VALUES (12320, '2018-09-25 23:00:00', 381.6, 104, '2018-09-25 22:53:46', 330, 0, 0, 381.6, '1101000000100100', '7000000000000000', '');";

    sql << "INSERT INTO station VALUES(12320, 60.818, 11.0696, 221, 0, 'HAMAR - STAVSBERG', 1385, 12320, NULL, NULL, NULL, 8, 't', '2005-10-21 00:00:00');";

    sql << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500.0;500.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3a-104', 'max\n30.0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 0, 365, -1, 'QC1-3b-104', 'no\n48', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 104, 0, 0, 1, 365, -1, 'QC1-1-104x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(12320, 104, 0, 0, 1, 365, -1, 'QC1-1-104', 'max;highest;high;low;lowest;min\n615;500;500;50.0;0.0;-3.0', '', '2005-11-04 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2018" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   24" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2018" << std::endl
           << "End_MM     =   09" << std::endl
           << "End_DD     =   26" << std::endl
           << "End_hh     =   22" << std::endl
           << "ParValFilename = list: 104 100" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    // after setting the HQC flag back to 0, the DipTest should perform updates
    sql.str("");
    sql << "UPDATE data SET controlinfo='1102000000100100' WHERE stationid = 12320 AND obstime = '2018-09-25 20:00:00';";
    ASSERT_NO_THROW(db->exec(sql.str()));

    ASSERT_RUN(algo, bc, 2);
}

TEST_F(DipTestTest, Bugzilla1320)
{
    // Akima interpolation for a series 1 0 X 0 1 should not go below 0 for things like wind which are physically restricted to >0
    std::ostringstream sql;
    sql // these data are fake
        << "INSERT INTO data VALUES(90800, '2011-08-13 14:00:00',  2, 90, '2011-08-13 13:56:04.3525',   330, 0, 0,  2, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 15:00:00',  1, 90, '2011-08-13 14:56:04.557816', 330, 0, 0,  1, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 16:00:00',  0, 90, '2011-08-13 15:56:00.973292', 330, 0, 0,  0, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 17:00:00', 64, 90, '2011-08-13 16:56:04.387755', 330, 0, 0, 64, '0412000000000000', '7020300000000002','QC1-1-90,QC1-3a-90');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 18:00:00',  0, 90, '2011-08-13 17:56:02.55036',  330, 0, 0,  0, '0112000000000000', '7010300000000001','QC1-3a-90');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 19:00:00',  1, 90, '2011-08-13 18:56:01.775408', 330, 0, 0,  1, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 20:00:00',  2, 90, '2011-08-13 19:55:59.784935', 330, 0, 0,  2, '0111000000000000', '7000000000000000','');";

    sql << "INSERT INTO station VALUES(90800, 70.2457, 19.5005, 21, 0, 'TORSVÅG FYR', 1033, 90800, NULL, NULL, NULL, 8, 't', '1933-01-01 00:00:00');";

    sql << "INSERT INTO station_param VALUES(90800, 90, 0, 0,   1,  31, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;35.5;32.3;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0,  32,  59, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;37.6;34.2;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0,  60,  90, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;38.6;35.1;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0,  91, 120, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;27.3;24.8;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 121, 151, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;23.2;21.1;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 152, 181, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;30.9;28.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 182, 212, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;45.3;41.0;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 213, 243, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;21.6;19.7;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 244, 273, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;34.0;30.8;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 274, 304, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;39.1;35.5;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 305, 334, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;40.1;36.3;0.0;0.0;0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(90800, 90, 0, 0, 335, 365, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;37.0;33.7;0.0;0.0;0', NULL, '1500-01-01 00:00:00');";

    sql << "INSERT INTO station_param VALUES (0, 90, 0, 0, 0, 365, -1, 'QC1-3a-90', 'max\n12.5', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 90, 0, 0, 0, 365, -1, 'QC1-3b-90', 'no\n5', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 90, 0, 0, 1, 365, -1, 'QC1-1-90x', '1;2;3;4;5;6\n-6999;-99.9;-99.8;999;6999;9999', '9999-VALUES', '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES (0, 90, 0, 0, 1, 365, -1, 'QC1-1-90', 'max;highest;high;low;lowest;min\n93;93;93;0;0;0', 'DEFAULT MAX-MIN RANGE', '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   08" << std::endl
           << "Start_DD   =   13" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   08" << std::endl
           << "End_DD     =   13" << std::endl
           << "End_hh     =   22" << std::endl
           << "ParValFilename = list: 90 12.5" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    std::list<kvalobs::kvData> series;
    kvtime::time t1 = kvtime::maketime("2011-08-13 17:00:00"), t2 = t1;
    kvtime::addHours(t2, 1);

    ASSERT_NO_THROW(series = db->findDataOrderObstime(90800, 90, TimeRange(t1, t2)));
    ASSERT_EQ(2, series.size());

    std::list<kvalobs::kvData>::const_iterator it = series.begin();
    EXPECT_EQ("0419000000000000", it->controlinfo().flagstring());
    EXPECT_TRUE(boost::algorithm::ends_with(it->cfailed(), "QC2d-1-L"));
    EXPECT_LE(0, it->corrected());
    it++;
    EXPECT_EQ("0114000000000000", it->controlinfo().flagstring());
    EXPECT_TRUE(boost::algorithm::ends_with(it->cfailed(), "QC2d-1"));

    ASSERT_RUN(algo, bc, 0);
}

TEST_F(DipTestTest, FaerderFyrNoDip)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(27500, 59.027, 10.524, 6.0, 0.0, 'F�RDER FYR', 1482, 27500, NULL, NULL, NULL, 8, 't', '1885-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    DataList data(27500, 211, 302);
    data.add("2012-02-28 23:00:00",   -36,    -36, "0511000000100020", "QC1-1-211,QC1-9-211")
        .add("2012-02-29 00:00:00", -44.8, -32766, "05120020001000A0", "QC1-1-211,QC1-3a-211,QC1-9-211")
        .add("2012-02-29 01:00:00", -47.5,  -47.5, "0510000000100020", "QC1-1-211,QC1-9-211")
        .add("2012-02-29 02:00:00", -46.6,  -46.6, "0511000000100020", "QC1-1-211,QC1-9-211");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012" << std::endl
           << "Start_MM   =   02" << std::endl
           << "Start_DD   =   28" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2012" << std::endl
           << "End_MM     =   02" << std::endl
           << "End_DD     =   29" << std::endl
           << "End_hh     =   12" << std::endl
           << "ParValFilename = list: 211 7.5" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::WARNING));
    ASSERT_EQ(0, logs->count(Message::INFO));

    data.add("2012-02-29 01:00:00", -30.0,  -30.0, "0512000000100020", "QC1-1-211,QC1-9-211") // set fs=2
        .add("2012-02-29 02:00:00", -30.0,  -30.0, "0511000000100020", "QC1-1-211,QC1-9-211");
    ASSERT_NO_THROW(data.update(db));
    ASSERT_RUN(algo, bc, 2);
    EXPECT_STATION_OBS_CONTROL_CORR(27500, "2012-02-29 00:00:00", "05190020001000A0", -33, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(27500, "2012-02-29 01:00:00", "0514000000100020", -30, bc->update(1));
    ASSERT_EQ(2, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

TEST_F(DipTestTest, BadFlagsBeforeAfter)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(27500, 59.027, 10.524, 6.0, 0.0, 'F�RDER FYR', 1482, 27500, NULL, NULL, NULL, 8, 't', '1885-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    DataList data(27500, 211, 302);
    data.add("2012-02-28 23:00:00", -36, "0111000000000000", "")
        .add("2012-02-29 00:00:00", -46, -32766, "0412000000000000", "???")
        .add("2012-02-29 01:00:00", -30, "0115000000000000", "???")
        .add("2012-02-29 02:00:00", -30, "0111000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012" << std::endl
           << "Start_MM   =   02" << std::endl
           << "Start_DD   =   28" << std::endl
           << "Start_hh   =   12" << std::endl
           << "End_YYYY   = 2012" << std::endl
           << "End_MM     =   02" << std::endl
           << "End_DD     =   29" << std::endl
           << "End_hh     =   12" << std::endl
           << "ParValFilename = list: 211 7.5" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::WARNING));
    ASSERT_EQ(1, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->find("flag pattern mismatch", Message::INFO));

    data.add("2012-02-29 01:00:00", -30, "0112000000000000", "???");
    ASSERT_NO_THROW(data.update(db));
    ASSERT_RUN(algo, bc, 2);
    EXPECT_STATION_OBS_CONTROL_CORR(27500, "2012-02-29 00:00:00", "0419000000000000", -33, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(27500, "2012-02-29 01:00:00", "0114000000000000", -30, bc->update(1));
    ASSERT_EQ(2, logs->count(Message::INFO));
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

TEST_F(DipTestTest, NoWARNIfDataMissingOutsideTimeRange)
{
    // very last row is a potential dip, and test time period is just
    // ending at this time: it is not possible to have data
    // afterwards, so the algorithm should not complain

    DataList data(27500, 213, 330);
    data.add("2012-03-20 00:00:00",  1.9, "0111000000000000", "");
    data.add("2012-03-20 01:00:00",  1.5, "0111000000000000", "");
    data.add("2012-03-20 02:00:00",  0.4, "0111000000000000", "");
    data.add("2012-03-20 03:00:00", -0.2, "0111000000000000", "");
    data.add("2012-03-20 04:00:00", -0.1, "0111000000000000", "");
    data.add("2012-03-20 05:00:00",  8.2, "0112000000000000", "QC1-3a-213");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012" << std::endl
           << "Start_MM   =   03" << std::endl
           << "Start_DD   =   20" << std::endl
           << "Start_hh   =   00" << std::endl
           << "End_YYYY   = 2012" << std::endl
           << "End_MM     =   03" << std::endl
           << "End_DD     =   20" << std::endl
           << "End_hh     =   05" << std::endl
           << "ParValFilename = list: 213 7.5" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    ASSERT_EQ(0, logs->count(Message::WARNING));
    ASSERT_EQ(0, logs->count(Message::INFO));
}

TEST_F(DipTestTest, DefaultStationParam)
{
    DataList data(93900, 177, 330);
    data.add("2012-07-16 00:00:00",       0.1,       0.1, "0111000000000010", "")
        .add("2012-07-16 01:00:00",       0.1,       0.1, "0111000000000010", "")
        .add("2012-07-16 02:00:00",      12.5,      12.5, "0112000000000010", "QC1-3a-177")
        .add("2012-07-16 03:00:00",       0.5,       0.5, "0112000000000010", "QC1-3a-177")
        .add("2012-07-16 04:00:00",       0.5,       0.5, "0111000000000010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(93900, 68.755, 23.539, 382, 0, 'SIHCCAJAVRI', 1199, 93900, NULL, NULL, NULL, 8, 't', '1912-01-01 00:00:00');";
    sql << "INSERT INTO station_param VALUES(0, 177, 0, 0, 1, 365, -1, 'QC1-1-177',"
        " 'max;highest;high;low;lowest;min\n16;16;16;-25;-25;-25', 'DEFAULT MAX-MIN RANGE', '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   07\n"
           << "Start_DD   =   16\n"
           << "Start_hh   =   00\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   07\n"
           << "End_DD     =   16\n"
           << "End_hh     =   04\n"
           << "ParValFilename = list: 177 12.0\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_STATION_OBS_CONTROL_CORR(93900, "2012-07-16 02:00:00", "0119000000000010",  0.3, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(93900, "2012-07-16 03:00:00", "0114000000000010",  0.5, bc->update(1));

    ASSERT_EQ(0, logs->count(Message::WARNING));
    ASSERT_EQ(2, logs->count(Message::INFO));
}

TEST_F(DipTestTest, Freq10Minutes)
{
    DataList data(76934, 178, 22);
    data.add("2014-09-19 07:00:00",    1017.5,    1017.5, "0101100000000010", "")
        .add("2014-09-19 07:10:00",    1020.6,    1020.6, "0101000000000010", "")
        .add("2014-09-19 07:20:00",    1020.7,    1020.7, "0101000000000010", "")
        .add("2014-09-19 07:30:00",    1020.8,    1020.8, "0101000000000010", "")
        .add("2014-09-19 07:40:00",    1017.7,    1017.7, "0101000000000010", "")
        .add("2014-09-19 07:50:00",    1017.7,    1017.7, "0101000000000010", "")
        .add("2014-09-19 08:00:00",    1017.6,    1017.6, "0101100000000010", "")
        .add("2014-09-19 08:10:00",    1021.3,    1021.3, "0101000000000010", "")
        .add("2014-09-19 08:20:00",    1017.5,    1017.5, "0101000000000010", "")
        .add("2014-09-19 08:30:00",    1017.4,    1017.4, "0101000000000010", "")
        .add("2014-09-19 08:40:00",    1021.7,    1021.7, "0102000000000010", "QC1-3a-178,QC1-3c-178")
        .add("2014-09-19 08:50:00",    1017.5,    1017.6, "0102000000000010", "QC1-3a-178")
        .add("2014-09-19 09:00:00",    1017.5,    1017.5, "0101100000000010", "")
        .add("2014-09-19 09:10:00",    1017.4,    1017.4, "0101000000000010", "")
        .add("2014-09-19 09:20:00",    1017.3,    1017.3, "0101000000000010", "")
        .add("2014-09-19 09:30:00",    1017.2,    1017.2, "0101000000000010", "")
        .add("2014-09-19 09:40:00",    1017.2,    1017.2, "0101000000000010", "")
        .add("2014-09-19 09:50:00",    1017.1,    1017.1, "0101000000000010", "")
        .add("2014-09-19 10:00:00",    1017.1,    1017.1, "0101100000000010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(76934, 61.2063, 1.8288, 142, 0, 'STATFJORD B', 1401, 76934, 'ENFB', 'LF6M', NULL, 0, 't', '1981-01-01 00:00:00');";
    sql << "INSERT INTO station_param VALUES(0, 178, 0, 0, 1, 366, -1, 'QC1-1-178',"
        " 'max;highest;high;low;lowest;min\n1069;1069;1069;851;851;851', 'DEFAULT MAX-MIN RANGE', '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2014" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   19" << std::endl
           << "Start_hh   =   07" << std::endl
           << "End_YYYY   = 2014" << std::endl
           << "End_MM     =   09" << std::endl
           << "End_DD     =   19" << std::endl
           << "End_hh     =   10" << std::endl
           << "ParValFilename = list: 178 2.0" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);
    EXPECT_STATION_OBS_CONTROL_CORR(76934, "2014-09-19 08:40:00", "0109000000000010", 1017.5, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(76934, "2014-09-19 08:50:00", "0104000000000010", 1017.6, bc->update(1));
    ASSERT_EQ(0, logs->count(Message::WARNING));
    ASSERT_EQ(2, logs->count(Message::INFO));
}

TEST_F(DipTestTest, Freq10Minutes2Sensors)
{
    DataList data(76934, 178, 22);
    data.add("2014-09-19 07:00:00",    1017.5,    1017.5, "0101100000000010", "")
        .add("2014-09-19 07:10:00",    1020.6,    1020.6, "0101000000000010", "")
        .add("2014-09-19 07:20:00",    1020.7,    1020.7, "0101000000000010", "")
        .add("2014-09-19 07:30:00",    1020.8,    1020.8, "0101000000000010", "")
        .add("2014-09-19 07:40:00",    1017.7,    1017.7, "0101000000000010", "")
        .add("2014-09-19 07:50:00",    1017.7,    1017.7, "0101000000000010", "")
        .add("2014-09-19 08:00:00",    1017.6,    1017.6, "0101100000000010", "")
        .add("2014-09-19 08:10:00",    1021.3,    1021.3, "0101000000000010", "")
        .add("2014-09-19 08:20:00",    1017.5,    1017.5, "0101000000000010", "")
        .add("2014-09-19 08:30:00",    1017.4,    1017.4, "0101000000000010", "")
        .add("2014-09-19 08:40:00",    1021.7,    1021.7, "0102000000000010", "QC1-3a-178,QC1-3c-178")
        .add("2014-09-19 08:50:00",    1017.5,    1017.6, "0102000000000010", "QC1-3a-178")
        .add("2014-09-19 09:00:00",    1017.5,    1017.5, "0101100000000010", "")
        .add("2014-09-19 09:10:00",    1017.4,    1017.4, "0101000000000010", "")
        .add("2014-09-19 09:20:00",    1017.3,    1017.3, "0101000000000010", "")
        .add("2014-09-19 09:30:00",    1017.2,    1017.2, "0101000000000010", "")
        .add("2014-09-19 09:40:00",    1017.2,    1017.2, "0101000000000010", "")
        .add("2014-09-19 09:50:00",    1017.1,    1017.1, "0101000000000010", "")
        .add("2014-09-19 10:00:00",    1017.1,    1017.1, "0101100000000010", "");
    data.setSensor(1)
        .add("2014-09-19 07:00:00",    1017.5,    1017.5, "0000000000000000", "")
        .add("2014-09-19 07:40:00",    1017.6,    1017.6, "0000000000000000", "")
        .add("2014-09-19 07:50:00",    1017.7,    1017.7, "0000000000000000", "")
        .add("2014-09-19 08:00:00",    1017.6,    1017.6, "0000000000000000", "")
        .add("2014-09-19 08:20:00",    1017.5,    1017.5, "0000000000000000", "")
        .add("2014-09-19 08:30:00",    1017.4,    1017.4, "0000000000000000", "")
        .add("2014-09-19 08:50:00",    1017.5,    1017.5, "0000000000000000", "")
        .add("2014-09-19 09:00:00",    1017.5,    1017.5, "0000000000000000", "")
        .add("2014-09-19 09:10:00",    1017.4,    1017.4, "0000000000000000", "")
        .add("2014-09-19 09:20:00",    1017.3,    1017.3, "0000000000000000", "")
        .add("2014-09-19 09:30:00",    1017.3,    1017.3, "0000000000000000", "")
        .add("2014-09-19 09:40:00",    1017.2,    1017.2, "0000000000000000", "")
        .add("2014-09-19 09:50:00",    1017.2,    1017.2, "0000000000000000", "")
        .add("2014-09-19 10:00:00",    1017.2,    1017.2, "0000000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(76934, 61.2063, 1.8288, 142, 0, 'STATFJORD B', 1401, 76934, 'ENFB', 'LF6M', NULL, 0, 't', '1981-01-01 00:00:00');";
    sql << "INSERT INTO station_param VALUES(0, 178, 0, 0, 1, 366, -1, 'QC1-1-178',"
        " 'max;highest;high;low;lowest;min\n1069;1069;1069;851;851;851', 'DEFAULT MAX-MIN RANGE', '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::stringstream config;
    config << "Start_YYYY = 2014" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   19" << std::endl
           << "Start_hh   =   07" << std::endl
           << "End_YYYY   = 2014" << std::endl
           << "End_MM     =   09" << std::endl
           << "End_DD     =   19" << std::endl
           << "End_hh     =   10" << std::endl
           << "ParValFilename = list: 178 2.0" << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);
    EXPECT_STATION_OBS_CONTROL_CORR(76934, "2014-09-19 08:40:00", "0109000000000010", 1017.5, bc->update(0));
    EXPECT_STATION_OBS_CONTROL_CORR(76934, "2014-09-19 08:50:00", "0104000000000010", 1017.6, bc->update(1));
    ASSERT_EQ(0, logs->count(Message::WARNING));
    ASSERT_EQ(2, logs->count(Message::INFO));
}
