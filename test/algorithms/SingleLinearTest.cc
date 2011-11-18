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
#include "algorithms/SingleLinearAlgorithm.h"

class SingleLinearTest : public AlgorithmTestBase {
public:
    void SetUp();
    void TearDown();
protected:
    SingleLinearAlgorithm* algo;
};

void SingleLinearTest::SetUp()
{
    AlgorithmTestBase::SetUp();
    algo = new SingleLinearAlgorithm();
    algo->setDatabase(db);
    algo->setBroadcaster(bc);
}

void SingleLinearTest::TearDown()
{
    delete algo;
    AlgorithmTestBase::TearDown();
}

TEST_F(SingleLinearTest, test1)
{
    std::ostringstream sql;
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
    ASSERT_NO_THROW(db->exec(sql.str()));

    const int pid = 211;
    std::stringstream config;
    config << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   10" << std::endl
           << "Start_hh   =   22" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   10" << std::endl
           << "End_DD     =    1" << std::endl
           << "End_hh     =   21" << std::endl
           << "neighbor_cflags    = ___.__0.___.___." << std::endl
           << "neighbor_uflags    = [37]_0.___.___.___." << std::endl
           << "missing_cflags     = ___.__[1234]____.___0" << std::endl
           << "update_flagchange  = ___.__3____.___.->___.__1____.___.;___.__[02]____.___.->___.__4____.___." << std::endl
           << "missing_flagchange = ___.__1____.___.->___.__3____.___.;___.__4____.___.->___.__2____.___." << std::endl
           << "ParamId=" << pid << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    std::list<kvalobs::kvData> series;
    miutil::miTime t("2011-10-01 11:00:00"), tb=t, ta=t;
    tb.addHour(-1);
    ta.addHour(+1);
    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 180, pid, tb, tb));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(14.0, series.begin()->original());

    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 180, pid, ta, ta));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(18.0, series.begin()->original());

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(1, bc->count());

    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 180, pid, t, t));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(16.0, series.begin()->corrected());
}

TEST_F(SingleLinearTest, test2)
{
    std::ostringstream sql;
    sql << "INSERT INTO data VALUES (180, '2011-10-01 10:00:00',  14.00, 211, '2011-10-01 09:55:40', 330, '0', 0, 14.90, '0111100000100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 11:00:00',  17.50, 211, '2011-10-01 10:55:39', 330, '0', 0, 17.50, '0111102100100010', '7000000000000000', '');" << std::endl
        << "INSERT INTO data VALUES (180, '2011-10-01 12:00:00', -32767, 211, '2011-10-01 11:55:57', 330, '0', 0, 18.60, '0211101000100012', '3000000000000051', '');" << std::endl;
    ASSERT_NO_THROW(db->exec(sql.str()));

    const int pid = 211;
    std::stringstream config;
    config << "Start_YYYY = 2011" << std::endl
           << "Start_MM   =   09" << std::endl
           << "Start_DD   =   10" << std::endl
           << "Start_hh   =   22" << std::endl
           << "End_YYYY   = 2011" << std::endl
           << "End_MM     =   10" << std::endl
           << "End_DD     =    1" << std::endl
           << "End_hh     =   21" << std::endl
           << "neighbor_cflags    = ___.__0.___.___." << std::endl
           << "neighbor_uflags    = [37]_0.___.___.___." << std::endl
           << "missing_cflags     = ___.__[1234]____.___0" << std::endl
           << "update_flagchange  = ___.__3____.___.->___.__1____.___.;___.__[02]____.___.->___.__4____.___." << std::endl
           << "missing_flagchange = ___.__1____.___.->___.__3____.___.;___.__4____.___.->___.__2____.___." << std::endl
           << "ParamId=" << pid << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(1, bc->count());

    std::list<kvalobs::kvData> series;
    miutil::miTime t("2011-10-01 11:00:00"), tb=t, ta=t;
    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 180, pid, t, t));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(params.rejected, series.begin()->corrected());
    ASSERT_EQ("0111102100100010", series.begin()->controlinfo().flagstring());
}


TEST_F(SingleLinearTest, testFromWiki)
{
    // see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:test:slv32

    std::ostringstream sql;
    sql << "INSERT INTO data VALUES(87120,'2025-09-16 06:00:00',11.3,211,'2010-09-16 06:03:18',330,'0',0,11.3,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 07:00:00',11.5,211,'2010-09-16 07:03:24',330,'0',0,11.5,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 08:00:00',10.7,211,'2010-09-16 08:03:19',330,'0',0,10.7,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 09:00:00',9.7,211,'2010-09-16 09:03:22',330,'0',0,9.7,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 10:00:00',7.9,211,'2010-09-16 10:03:21',330,'0',0,7.9,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 11:00:00',6.6,211,'2010-09-16 11:03:21',330,'0',0,6.6,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 12:00:00',-19.1,211,'2010-09-16 12:03:25',330,'0',0,-32766.0,'05120020000000A0','7038300000000002','QC1-1-211:1,QC1-3a-211:1,QC1-9-211:1');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 13:00:00',6.8,211,'2010-09-16 13:03:17',330,'0',0,6.8,'0110000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 14:00:00',6.4,211,'2010-09-16 14:03:19',330,'0',0,6.4,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 15:00:00',7.0,211,'2010-09-16 15:03:21',330,'0',0,7.0,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 16:00:00',-32767.0,211,'2010-09-16 16:51:24',330,'0',0,-32767.0,'0000003000000000','9899900000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 17:00:00',-32767.0,211,'2010-09-16 17:33:00',330,'0',0,-32767.0,'0000003000000000','9899900000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 18:00:00',8.3,211,'2010-09-16 18:04:25',330,'0',0,8.3,'0110000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 19:00:00',8.0,211,'2010-09-16 19:04:22',330,'0',0,8.0,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 20:00:00',7.8,211,'2010-09-16 20:04:23',330,'0',0,7.8,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 21:00:00',8.2,211,'2010-09-16 21:04:21',330,'0',0,8.2,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 22:00:00',8.1,211,'2010-09-16 22:04:25',330,'0',0,8.1,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-16 23:00:00',8.0,211,'2010-09-16 23:04:25',330,'0',0,8.0,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 00:00:00',7.7,211,'2010-09-17 00:04:20',330,'0',0,7.7,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 01:00:00',7.6,211,'2010-09-17 01:03:46',330,'0',0,7.6,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 02:00:00',7.5,211,'2010-09-17 02:03:44',330,'0',0,7.5,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 03:00:00',7.1,211,'2010-09-17 03:03:42',330,'0',0,7.1,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 04:00:00',7.3,211,'2010-09-17 04:03:46',330,'0',0,7.3,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 05:00:00',7.8,211,'2010-09-17 05:03:49',330,'0',0,7.8,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 06:00:00',7.4,211,'2010-09-17 06:03:46',330,'0',0,7.4,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 07:00:00',7.6,211,'2010-09-17 07:03:49',330,'0',0,7.6,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 08:00:00',8.5,211,'2010-09-17 08:03:43',330,'0',0,8.5,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 09:00:00',8.7,211,'2010-09-17 09:03:50',330,'0',0,8.7,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 10:00:00',-32767.0,211,'2010-09-17 10:32:33',330,'0',0,-32767.0,'0100003000000000','7899900000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 11:00:00',9.5,211,'2010-09-17 11:03:44',330,'0',0,9.5,'0110000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 12:00:00',9.5,211,'2010-09-17 12:04:47',330,'0',0,9.5,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 13:00:00',9.8,211,'2010-09-17 13:04:51',330,'0',0,9.8,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 14:00:00',9.3,211,'2010-09-17 14:04:48',330,'0',0,9.3,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 15:00:00',9.5,211,'2010-09-17 15:04:48',330,'0',0,9.5,'0111000000000010','7000000000000000','');"
        << "INSERT INTO data VALUES(87120,'2025-09-17 16:00:00',9.3,211,'2010-09-17 16:21:26',330,'0',0,9.3,'0111000000000010','7100000400000000','');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    const int pid = 211;
    std::stringstream config;
    config << "Start_YYYY=2025" << std::endl
           << "Start_MM=9" << std::endl
           << "Start_DD=16  " << std::endl
           << "Start_hh=6" << std::endl
           << "Start_mm=0" << std::endl
           << "Start_ss=0" << std::endl
           << "End_YYYY=2025" << std::endl
           << "End_MM=9" << std::endl
           << "End_DD=17 " << std::endl
           << "End_hh=16" << std::endl
           << "End_mm=0" << std::endl
           << "End_ss=0" << std::endl
           << "ParamId=" << pid << std::endl
           << "neighbor_cflags    = ___.__0.___.___." << std::endl
           << "neighbor_uflags    = [37]_0.___.___.___." << std::endl
           << "missing_cflags     = ___.__[1234]____.___0" << std::endl
           << "update_flagchange  = ___.___1___.___.;___.__3____.___.->___.__1____.___.;___.__[02]____.___.->___.__4____.___." << std::endl
           << "missing_flagchange = ___.__1____.___.->___.__3____.___.;___.__4____.___.->___.__2____.___." << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    // wiki step 3
    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    std::list<kvalobs::kvData> series;
    miutil::miTime t0("2025-09-16 12:00:00"), t1("2025-09-17 10:00:00");
    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 87120, pid, t0, t0));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(6.7, series.begin()->corrected());
    ASSERT_EQ("05120041000000A0", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC1-1-211:1,QC1-3a-211:1,QC1-9-211:1,QC2d-2", series.begin()->cfailed());

    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 87120, pid, t1, t1));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(9.1, series.begin()->corrected());
    ASSERT_EQ("0100001100000000", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC2d-2", series.begin()->cfailed());

    // wiki step 4, run again, no more updates allowed
    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());

    // wiki step 5
    sql.str("");
    sql << "UPDATE data SET corrected=5, original=5 WHERE stationid=87120 AND  obstime='2025-09-17 09:00:00' AND paramid=211;"
           "UPDATE data SET corrected=7.2, original=7.2 WHERE stationid=87120 AND  obstime='2025-09-16 11:00:00' AND paramid=211;";
    ASSERT_NO_THROW(db->exec(sql.str()));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 87120, pid, t0, t0));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(7.0, series.begin()->corrected());
    ASSERT_EQ("05120041000000A0", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC1-1-211:1,QC1-3a-211:1,QC1-9-211:1,QC2d-2,QC2d-2", series.begin()->cfailed());

    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 87120, pid, t1, t1));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(7.3, series.begin()->corrected());
    ASSERT_EQ("0100001100000000", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC2d-2,QC2d-2", series.begin()->cfailed());

    // wiki step 7
    sql.str("");
    sql << "UPDATE data SET useinfo='7010000000000000' WHERE stationid=87120 and obstime='2025-09-17 09:00:00' and paramid=211;"
           "UPDATE data SET useinfo='7010000000000000' WHERE stationid=87120 and obstime='2025-09-16 11:00:00' and paramid=211;";
    ASSERT_NO_THROW(db->exec(sql.str()));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 87120, pid, t0, t0));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(-32766, series.begin()->corrected());
    ASSERT_EQ("05120021000000A0", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC1-1-211:1,QC1-3a-211:1,QC1-9-211:1,QC2d-2,QC2d-2,QC2d-2", series.begin()->cfailed());

    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 87120, pid, t1, t1));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ( -32767, series.begin()->corrected());
    ASSERT_EQ("0100003100000000", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC2d-2,QC2d-2,QC2d-2", series.begin()->cfailed());

    // wiki step 8, run again, no more updates allowed
    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
}

TEST_F(SingleLinearTest, testFromKro)
{
    // \copy ( select stationid,obstime,original,paramid,tbtime,message_formatid,sensor,hlevel,corrected,controlinfo,useinfo,cfailed from kvalobs_data where stationid = 93000 and obstime between '2011-10-09 12:00' and '2011-10-10 20:00' and paramid = 211 ) to /tmp/ababa.out DELIMITER '|'
    std::ostringstream sql;
    sql << "INSERT INTO data VALUES(93000,'2011-10-09 12:00:00',-0.7,211,'2011-10-09 11:52:17',330,'0',0,-0.7,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 13:00:00',-0.9,211,'2011-10-09 12:52:23',330,'0',0,-0.9,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 14:00:00',-1.5,211,'2011-10-09 13:52:20',330,'0',0,-1.5,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 15:00:00',-1.5,211,'2011-10-09 14:52:19',330,'0',0,-1.5,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 16:00:00',-1.8,211,'2011-10-09 15:52:23',330,'0',0,-1.8,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 17:00:00',-32767.0,211,'2011-10-09 17:30:12',330,'0',0,0.2,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 18:00:00',-32767.0,211,'2011-10-09 18:38:30',330,'0',0,0.3,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 19:00:00',-32767.0,211,'2011-10-09 19:30:13',330,'0',0,0.2,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 20:00:00',-32767.0,211,'2011-10-09 20:30:12',330,'0',0,0.3,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 21:00:00',-32767.0,211,'2011-10-09 21:30:12',330,'0',0,0.4,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 22:00:00',-32767.0,211,'2011-10-09 22:30:13',330,'0',0,0.6,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-09 23:00:00',-32767.0,211,'2011-10-09 23:30:13',330,'0',0,0.9,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 00:00:00',-32767.0,211,'2011-10-10 00:37:48',330,'0',0,1.6,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 01:00:00',-32767.0,211,'2011-10-10 01:30:12',330,'0',0,2.0,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 02:00:00',-32767.0,211,'2011-10-10 02:30:10',330,'0',0,2.3,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 03:00:00',-32767.0,211,'2011-10-10 03:30:12',330,'0',0,2.4,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 04:00:00',-32767.0,211,'2011-10-10 04:30:46',330,'0',0,2.5,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 05:00:00',-32767.0,211,'2011-10-10 05:31:04',330,'0',0,2.6,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 06:00:00',-32767.0,211,'2011-10-10 08:21:47',330,'0',0,3.1,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 07:00:00',-32767.0,211,'2011-10-10 08:41:09',330,'0',0,3.4,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 08:00:00',-32767.0,211,'2011-10-10 08:48:08',330,'0',0,3.6,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 09:00:00',-32767.0,211,'2011-10-10 09:42:19',330,'0',0,3.8,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 10:00:00',-32767.0,211,'2011-10-10 10:30:43',330,'0',0,4.1,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 11:00:00',-32767.0,211,'2011-10-10 11:30:09',330,'0',0,4.2,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 12:00:00',-32767.0,211,'2011-10-10 12:32:30',330,'0',0,4.5,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 13:00:00',-32767.0,211,'2011-10-10 13:31:01',330,'0',0,4.6,'0000601000000000','7894700000000001','QC1-4-211');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 14:00:00',1.7,211,'2011-10-10 13:52:20',330,'0',0,1.7,'0110100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 15:00:00',1.6,211,'2011-10-10 14:52:19',330,'0',0,1.6,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 16:00:00',1.4,211,'2011-10-10 15:52:21',330,'0',0,1.4,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 17:00:00',1.4,211,'2011-10-10 16:52:20',330,'0',0,1.4,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 18:00:00',1.4,211,'2011-10-10 17:52:23',330,'0',0,1.4,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 19:00:00',1.6,211,'2011-10-10 18:52:24',330,'0',0,1.6,'0111100000100010','7000000000000000','');"
        << "INSERT INTO data VALUES(93000,'2011-10-10 20:00:00',1.3,211,'2011-10-10 19:52:18',330,'0',0,1.3,'0111100000100010','7000000000000000','');";

    ASSERT_NO_THROW(db->exec(sql.str()));

    const int pid = 211;
    std::stringstream config;
    config << "Start_YYYY=2011" << std::endl
           << "Start_MM=10" << std::endl
           << "Start_DD=09  " << std::endl
           << "Start_hh=6" << std::endl
           << "Start_mm=0" << std::endl
           << "Start_ss=0" << std::endl
           << "End_YYYY=2011" << std::endl
           << "End_MM=10" << std::endl
           << "End_DD=11 " << std::endl
           << "End_hh=6" << std::endl
           << "End_mm=0" << std::endl
           << "End_ss=0" << std::endl
           << "ParamId=" << pid << std::endl
           << "neighbor_cflags    = ___.__0.___.___." << std::endl
           << "neighbor_uflags    = [37]_0.___.___.___." << std::endl
           << "missing_cflags     = ___.__[1234]____.___0" << std::endl
           << "update_flagchange  = ___.___1___.___.;___.__3____.___.->___.__1____.___.;___.__[02]____.___.->___.__4____.___." << std::endl
           << "missing_flagchange = ___.__1____.___.->___.__3____.___.;___.__4____.___.->___.__2____.___." << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());

    sql.str("");
    sql << "UPDATE data SET useinfo='7000000000000000', controlinfo='0110100000100010', original=1.5, corrected=1.5 WHERE obstime='2011-10-10 12:00:00';";
    ASSERT_NO_THROW(db->exec(sql.str()));

    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(1, bc->count());

    std::list<kvalobs::kvData> series;
    miutil::miTime t0("2011-10-10 13:00:00");
    ASSERT_NO_THROW(db->dataForStationParamTimerange(series, 93000, pid, t0, t0));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(1.6, series.begin()->corrected());
}
