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
#include "algorithms/DipTestAlgorithm.h"
#include "AlgorithmHelpers.h"
#include "Helpers.h"

class DipTestTest : public AlgorithmTestBase {
public:
    void SetUp();
    void TearDown();
protected:
    DipTestAlgorithm* algo;
};

void DipTestTest::SetUp()
{
    AlgorithmTestBase::SetUp();
    algo = new DipTestAlgorithm();
    algo->setDatabase(db);
    algo->setBroadcaster(bc);
}

void DipTestTest::TearDown()
{
    delete algo;
    AlgorithmTestBase::TearDown();
}

TEST_F(DipTestTest, test1)
{
    std::ostringstream sql;
    sql << "INSERT INTO data VALUES(90800, '2011-08-13 12:00:00', 3.2, 90, '2011-08-13 11:56:01.916558', 330, 0, 0, 3.2, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 13:00:00', 3.0, 90, '2011-08-13 12:56:03.835825', 330, 0, 0, 3,   '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 14:00:00', 3.7, 90, '2011-08-13 13:56:04.3525',   330, 0, 0, 3.7, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 15:00:00', 4.1, 90, '2011-08-13 14:56:04.557816', 330, 0, 0, 4.1, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 16:00:00', 4.3, 90, '2011-08-13 15:56:00.973292', 330, 0, 0, 4.3, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 17:00:00', 64,  90, '2011-08-13 16:56:04.387755', 330, 0, 0, 64,  '0412000000000000', '7020300000000002','QC1-1-90,QC1-3a-90');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 18:00:00', 2,   90, '2011-08-13 17:56:02.55036',  330, 0, 0, 2,   '0112000000000000', '7010300000000001','QC1-3a-90');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 19:00:00', 2.9, 90, '2011-08-13 18:56:01.775408', 330, 0, 0, 2.9, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 20:00:00', 2,   90, '2011-08-13 19:55:59.784935', 330, 0, 0, 2,   '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 21:00:00', 3.3, 90, '2011-08-13 20:55:59.73048',  330, 0, 0, 3.3, '0111000000000000', '7000000000000000','');"
        << "INSERT INTO data VALUES(90800, '2011-08-13 22:00:00', 2.7, 90, '2011-08-13 21:56:01.502523', 330, 0, 0, 2.7, '0111000000000000', '7000000000000000','');";

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

    ASSERT_TRUE( db->exec(sql.str()) );

    std::stringstream config;
    config << "W_fhqc=0" << std::endl
            << "Start_YYYY = 2011" << std::endl
            << "Start_MM   =   08" << std::endl
            << "Start_DD   =   13" << std::endl
            << "Start_hh   =   12" << std::endl
            << "End_YYYY   = 2011" << std::endl
            << "End_MM     =   08" << std::endl
            << "End_DD     =   13" << std::endl
            << "End_hh     =   22" << std::endl
            << "ParValFilename = list: 1 9.0, 2 60.0, 81 10.0, 85 10.0, 86 12.5, 87 10.0, 88 12.5, 89 15.0, 90 12.5,"
               "91 12.5, 92 15.0, 93 15.0, 94 15.0, 104 100, 105 3.0, 113 15.0, 123 60.0, 172 12.0, 174 12.0, 175 12.0,"
               "176 12.0, 177 12.0, 197 1000.0, 198 500.0, 199 500.0, 200 1000.0, 211 7.5, 212 7.5, 213 7.5, 215 7.5,"
               " 221 15.0, 222 15.0" << std::endl;
    ReadProgramOptions params;
    params.Parse(config);

    algo->run(params);

    std::list<kvalobs::kvData> series;
    miutil::miTime t1("2011-08-13 17:00:00"), t2 = t1;
    t2.addHour(1);

    ASSERT_EQ(2, bc->count);

    ASSERT_TRUE( db->dataForStationParamTimerange(series, 90800, 90, t1, t2) );
    ASSERT_EQ(2, series.size());
    std::list<kvalobs::kvData>::const_iterator it = series.begin();
    EXPECT_EQ("0419000000000000", it->controlinfo().flagstring());
    EXPECT_TRUE(Helpers::endsWith(it->cfailed(), "QC2d-1-A") || Helpers::endsWith(it->cfailed(), "QC2d-1-L"));
    it++;
    EXPECT_EQ("0114000000000000", it->controlinfo().flagstring());
    EXPECT_TRUE(Helpers::endsWith(it->cfailed(), "QC2d-1"));
}
