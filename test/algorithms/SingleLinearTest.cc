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
#include "algorithms/SingleLinear_v32.h"

class SingleLinearTest : public AlgorithmTestBase {
public:
    void SetUp();
    void TearDown();
protected:
    SingleLinearV32Algorithm* algo;
};

void SingleLinearTest::SetUp()
{
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES (180, 61.2944, 12.2719, 360, 0.0, 'TRYSIL VEGSTASJON', 1397, 180, '', '', '', 8, 1, '1993-11-10 00:00:00');" << std::endl;

    sql << "INSERT INTO station_param VALUES (180, 211, 0, 0,   1,  31, -1, 'QC1-4-211', 'highest;high;low;lowest\n16.50;15.50;-16.50;-17.50',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0,   1,  31, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;12.4;7.4;-41.8;-46.8;-55',  '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0,  32,  59, -1, 'QC1-4-211', 'highest;high;low;lowest\n16.50;15.50;-16.50;-17.50',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0,  32,  59, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;14.9;9.9;-40.9;-45.9;-55',  '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0,  60,  90, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;20.9;15.9;-35.6;-40.6;-55', '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0,  60,  90, -1, 'QC1-4-211', 'highest;high;low;lowest\n16.50;15.50;-16.50;-17.50',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0,  91, 120, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;27.7;22.7;-21.7;-26.7;-55', '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0,  91, 120, -1, 'QC1-4-211', 'highest;high;low;lowest\n15.04;13.76;-14.40;-15.68',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 121, 151, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;33;28;-10.7;-15.7;-55',     '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 121, 151, -1, 'QC1-4-211', 'highest;high;low;lowest\n14.00;13.50;-14.50;-15.50',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 152, 181, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;38.3;33.3;-5.9;-10.9;-55',  '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 152, 181, -1, 'QC1-4-211', 'highest;high;low;lowest\n14.00;13.50;-14.50;-15.50',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 182, 212, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;38.8;33.8;-3.4;-8.4;-55',   '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 182, 212, -1, 'QC1-4-211', 'highest;high;low;lowest\n14.00;12.81;-14.25;-15.48',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 213, 243, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;37.3;32.3;-3.9;-8.9;-55',   '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 213, 243, -1, 'QC1-4-211', 'highest;high;low;lowest\n11.61;10.58;-12.08;-13.11',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 244, 273, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;31.1;26.1;-12.7;-17.7;-55', '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 244, 273, -1, 'QC1-4-211', 'highest;high;low;lowest\n9.58;8.72;-10.20;-11.06',              '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 274, 304, -1, 'QC1-4-211', 'highest;high;low;lowest\n11.66;10.65;-11.57;-12.58',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 274, 304, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;23.1;18.1;-18.5;-23.5;-55', '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 305, 334, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;16.4;11.4;-29.7;-34.7;-55', '',    '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 305, 334, -1, 'QC1-4-211', 'highest;high;low;lowest\n14.00;13.50;-14.50;-15.50',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 335, 365, -1, 'QC1-4-211', 'highest;high;low;lowest\n16.50;15.50;-16.50;-17.50',            '\\N', '1500-01-01 00:00:00' );" << std::endl
        << "INSERT INTO station_param VALUES (180, 211, 0, 0, 335, 365, -1, 'QC1-1-211', 'max;highest;high;low;lowest;min\n50;13.2;8.2;-39.7;-44.7;-55',  '',    '1500-01-01 00:00:00' );" << std::endl;

    sql << "INSERT INTO data VALUES (180, '2011-10-01 09:00:00', 9.40,  211, '2011-10-01 08:55:41', 330, '0', 0, 9.40,  '0111100000100010', '7000000000000000','');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 10:00:00', 14.00, 211, '2011-10-01 09:55:40', 330, '0', 0, 14.90, '0111100000100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 11:00:00', 17.50, 211, '2011-10-01 10:55:39', 330, '0', 0, 17.50, '0111102100100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 12:00:00', 18.00, 211, '2011-10-01 11:55:57', 330, '0', 0, 18.60, '0211100000100012', '3000000000000051', 'QC1-1-211,hqc');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 13:00:00', 19.60, 211, '2011-10-01 12:55:42', 330, '0', 0, 19.60, '0211100000100012', '3000000000000051', 'QC1-1-211,hqc');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 14:00:00', 19.80, 211, '2011-10-01 13:55:55', 330, '0', 0, 19.80, '0211100000100012', '3000000000000051', 'QC1-1-211,hqc');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 15:00:00', 19.10, 211, '2011-10-01 14:55:40', 330, '0', 0, 19.10, '0211100000100012', '3000000000000051', 'QC1-1-211,hqc');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 16:00:00', 16.20, 211, '2011-10-01 15:55:34', 330, '0', 0, 16.20, '0111100000100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 17:00:00', 12.60, 211, '2011-10-01 16:55:34', 330, '0', 0, 12.60, '0111100000100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 18:00:00', 10.60, 211, '2011-10-01 17:55:42', 330, '0', 0, 10.60, '0111100000100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 19:00:00', 10.70, 211, '2011-10-01 18:55:42', 330, '0', 0, 10.70, '0111100000100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 20:00:00', 12.50, 211, '2011-10-01 19:55:35', 330, '0', 0, 12.50, '0111100000100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 21:00:00', 12.50, 211, '2011-10-01 20:55:41', 330, '0', 0, 12.50, '0111100000100010', '7000000000000000', '');" << std::endl;

    if( !db->exec(sql.str()) )
        throw std::runtime_error("could not populate db");

    algo = new SingleLinearV32Algorithm();
    algo->setDatabase(db);
    algo->setBroadcaster(bc);
}

void SingleLinearTest::TearDown()
{
    delete algo;
    AlgorithmTestBase::TearDown();
}

TEST_F(SingleLinearTest, testGetNorwegianFixedStations)
{
    std::list<kvalobs::kvStation> stations;
    Helpers::GetNorwegianFixedStations(db, stations);
    ASSERT_EQ(1, stations.size());
    ASSERT_EQ(180, stations.begin()->stationID());
}

TEST_F(SingleLinearTest, testContinuous)
{
    const kvalobs::kvData series[3] = {
            kvalobs::kvData(180, miutil::miTime("2011-10-01 21:00:00"), 12, 211, miutil::miTime::nowTime(), 330, 0, 0, -1, kvalobs::kvControlInfo("0000000000000000"), kvalobs::kvUseInfo("0000000000000000"), ""),
            kvalobs::kvData(180, miutil::miTime("2011-10-01 22:00:00"), 12, 211, miutil::miTime::nowTime(), 330, 0, 0, -1, kvalobs::kvControlInfo("0000000000000000"), kvalobs::kvUseInfo("0000000000000000"), ""),
            kvalobs::kvData(180, miutil::miTime("2011-10-01 23:00:00"), 12, 211, miutil::miTime::nowTime(), 330, 0, 0, -1, kvalobs::kvControlInfo("0000000000000000"), kvalobs::kvUseInfo("0000000000000000"), ""),
    };
    ASSERT_TRUE( Helpers::checkContinuousHourAndSameTypeID(std::vector<kvalobs::kvData>(series, series+3)) );
}

TEST_F(SingleLinearTest, test1)
{
    ReadProgramOptions params;
    params.UT0 = miutil::miTime("2011-10-01 21:00:00");
    params.UT1 = params.UT0;
    params.UT0.addDay(-3);
    params.pid = 211;
    params.Aflag[6].push_back('0');
    params.Uflag[0].push_back('3');
    params.Uflag[0].push_back('7');
    params.missing  = -12345;
    params.rejected = -23456;

    std::list<kvalobs::kvData> series;
    miutil::miTime t("2011-10-01 11:00:00"), tb=t, ta=t;
    tb.addHour(-1);
    ta.addHour(+1);
    ASSERT_TRUE( db->dataForStationParamTimerange(series, 180, params.pid, tb, tb) );
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(14.0, series.begin()->original());

    ASSERT_TRUE( db->dataForStationParamTimerange(series, 180, params.pid, ta, ta) );
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(18.0, series.begin()->original());

    algo->run(params);

    ASSERT_TRUE( db->dataForStationParamTimerange(series, 180, params.pid, t, t) );
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(16.0, series.begin()->corrected());
}
