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
};

void SingleLinearTest::SetUp()
{
    algo = new SingleLinearAlgorithm();
    AlgorithmTestBase::SetUp();
}

TEST_F(SingleLinearTest, test1)
{
    DataList data(180, 211, 330);
    data.add("2011-10-01 09:00:00", 9.40,  "0111100000100010", "")
        .add("2011-10-01 10:00:00", 14.00, "0111100000100010", "")
        .add("2011-10-01 11:00:00", 17.50, "0111104100100010", "")
        .add("2011-10-01 12:00:00", 18.00, "0211100000100012", "QC1-1-211,hqc")
        .add("2011-10-01 13:00:00", 19.60, "0211100000100012", "QC1-1-211,hqc")
        .add("2011-10-01 14:00:00", 19.80, "0211100000100012", "QC1-1-211,hqc")
        .add("2011-10-01 15:00:00", 19.10, "0211100000100012", "QC1-1-211,hqc")
        .add("2011-10-01 16:00:00", 16.20, "0111100000100010", "")
        .add("2011-10-01 17:00:00", 12.60, "0111100000100010", "")
        .add("2011-10-01 18:00:00", 10.60, "0111100000100010", "")
        .add("2011-10-01 19:00:00", 10.70, "0111100000100010", "")
        .add("2011-10-01 20:00:00", 12.50, "0111100000100010", "")
        .add("2011-10-01 21:00:00", 12.50, "0111100000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    const int pid = 211;
    std::stringstream config;
    config << "Start_YYYY = 2011\n"
           << "Start_MM   =   09\n"
           << "Start_DD   =   10\n"
           << "Start_hh   =   22\n"
           << "End_YYYY   = 2011\n"
           << "End_MM     =   10\n"
           << "End_DD     =    1\n"
           << "End_hh     =   21\n"
           << "ParamId=" << pid << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    std::list<kvalobs::kvData> series;
    miutil::miTime t("2011-10-01 11:00:00"), tb=t, ta=t;
    tb.addHour(-1);
    ta.addHour(+1);
    ASSERT_NO_THROW(series = db->findDataOrderObstime(180, pid, TimeRange(tb, tb)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(14.0, series.begin()->original());

    ASSERT_NO_THROW(series = db->findDataOrderObstime(180, pid, TimeRange(ta, ta)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(18.0, series.begin()->original());

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);

    ASSERT_NO_THROW(series = db->findDataOrderObstime(180, pid, TimeRange(t, t)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(16.0, series.begin()->corrected());

    ASSERT_RUN(algo, bc, 0);
}

TEST_F(SingleLinearTest, test2)
{
    DataList data(180, 211, 330);
    data.add("2011-10-01 10:00:00",  14.00, 14.90, "0111100000100010", "")
        .add("2011-10-01 11:00:00",  17.50, 17.50, "0111104100100010", "")
        .add("2011-10-01 12:00:00", -32767, 18.60, "0211101000100012", "");
    ASSERT_NO_THROW(data.insert(db));

    const int pid = 211;
    std::stringstream config;
    config << "Start_YYYY = 2011\n"
           << "Start_MM   =   09\n"
           << "Start_DD   =   10\n"
           << "Start_hh   =   22\n"
           << "End_YYYY   = 2011\n"
           << "End_MM     =   10\n"
           << "End_DD     =    1\n"
           << "End_hh     =   21\n"
           << "ParamId=" << pid << std::endl;
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);

    std::list<kvalobs::kvData> series;
    miutil::miTime t("2011-10-01 11:00:00"), tb=t, ta=t;
    ASSERT_NO_THROW(series = db->findDataOrderObstime(180, pid, TimeRange(t, t)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(params.rejected, series.begin()->corrected());
    ASSERT_EQ("0111102100100010", series.begin()->controlinfo().flagstring());

    ASSERT_RUN(algo, bc, 0);
}


TEST_F(SingleLinearTest, testFromWiki)
{
    // see https://kvalobs.wiki.met.no/doku.php?id=kvoss:system:qc2:test:slv32
    DataList data(87120, 211, 330);
    data.add("2025-09-16 06:00:00",    11.3,    11.3,"0111000000000010", "")
        .add("2025-09-16 07:00:00",    11.5,    11.5,"0111000000000010", "")
        .add("2025-09-16 08:00:00",    10.7,    10.7,"0111000000000010", "")
        .add("2025-09-16 09:00:00",     9.7,     9.7,"0111000000000010", "")
        .add("2025-09-16 10:00:00",     7.9,     7.9,"0111000000000010", "")
        .add("2025-09-16 11:00:00",     6.6,     6.6,"0111000000000010", "")
        .add("2025-09-16 12:00:00",   -19.1,-32766.0,"05120020000000A0", "QC1-1-211:1,QC1-3a-211:1,QC1-9-211:1")
        .add("2025-09-16 13:00:00",     6.8,     6.8,"0110000000000010", "")
        .add("2025-09-16 14:00:00",     6.4,     6.4,"0111000000000010", "")
        .add("2025-09-16 15:00:00",     7.0,     7.0,"0111000000000010", "")
        .add("2025-09-16 16:00:00",-32767.0,-32767.0,"0000003000000000", "")
        .add("2025-09-16 17:00:00",-32767.0,-32767.0,"0000003000000000", "")
        .add("2025-09-16 18:00:00",     8.3,     8.3,"0110000000000010", "")
        .add("2025-09-16 19:00:00",     8.0,     8.0,"0111000000000010", "")
        .add("2025-09-16 20:00:00",     7.8,     7.8,"0111000000000010", "")
        .add("2025-09-16 21:00:00",     8.2,     8.2,"0111000000000010", "")
        .add("2025-09-16 22:00:00",     8.1,     8.1,"0111000000000010", "")
        .add("2025-09-16 23:00:00",     8.0,     8.0,"0111000000000010", "")
        .add("2025-09-17 00:00:00",     7.7,     7.7,"0111000000000010", "")
        .add("2025-09-17 01:00:00",     7.6,     7.6,"0111000000000010", "")
        .add("2025-09-17 02:00:00",     7.5,     7.5,"0111000000000010", "")
        .add("2025-09-17 03:00:00",     7.1,     7.1,"0111000000000010", "")
        .add("2025-09-17 04:00:00",     7.3,     7.3,"0111000000000010", "")
        .add("2025-09-17 05:00:00",     7.8,     7.8,"0111000000000010", "")
        .add("2025-09-17 06:00:00",     7.4,     7.4,"0111000000000010", "")
        .add("2025-09-17 07:00:00",     7.6,     7.6,"0111000000000010", "")
        .add("2025-09-17 08:00:00",     8.5,     8.5,"0111000000000010", "")
        .add("2025-09-17 09:00:00",     8.7,     8.7,"0111000000000010", "")
        .add("2025-09-17 10:00:00",-32767.0,-32767.0,"0100003000000000", "")
        .add("2025-09-17 11:00:00",     9.5,     9.5,"0110000000000010", "")
        .add("2025-09-17 12:00:00",     9.5,     9.5,"0111000000000010", "")
        .add("2025-09-17 13:00:00",     9.8,     9.8,"0111000000000010", "")
        .add("2025-09-17 14:00:00",     9.3,     9.3,"0111000000000010", "")
        .add("2025-09-17 15:00:00",     9.5,     9.5,"0111000000000010", "")
        .add("2025-09-17 16:00:00",     9.3,     9.3,"0111000000000010", "");
    ASSERT_NO_THROW(data.insert(db));

    const int pid = 211;
    std::stringstream config;
    config << "Start_YYYY=2025\n"
           << "Start_MM=9\n"
           << "Start_DD=16  \n"
           << "Start_hh=6\n"
           << "Start_mm=0\n"
           << "Start_ss=0\n"
           << "End_YYYY=2025\n"
           << "End_MM=9\n"
           << "End_DD=17 \n"
           << "End_hh=16\n"
           << "End_mm=0\n"
           << "End_ss=0\n"
           << "ParamId=" << pid << "\n";
    AlgorithmConfig params;
    params.Parse(config);

    // wiki step 3
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    std::list<kvalobs::kvData> series;
    miutil::miTime t0("2025-09-16 12:00:00"), t1("2025-09-17 10:00:00");
    ASSERT_NO_THROW(series = db->findDataOrderObstime(87120, pid, TimeRange(t0, t0)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(6.7, series.begin()->corrected());
    ASSERT_EQ("05120041000000A0", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC1-1-211:1,QC1-3a-211:1,QC1-9-211:1,QC2d-2", series.begin()->cfailed());

    ASSERT_NO_THROW(series = db->findDataOrderObstime(87120, pid, TimeRange(t1, t1)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(9.1, series.begin()->corrected());
    ASSERT_EQ("0100001100000000", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC2d-2", series.begin()->cfailed());

    // wiki step 4, run again, no more updates allowed
    ASSERT_RUN(algo, bc, 0);

    // wiki step 5
    data.add("2025-09-17 09:00:00", 5.0, "0111000000000010", "")
        .add("2025-09-16 11:00:00", 7.2, "0111000000000010", "");
    ASSERT_NO_THROW(data.update(db));

    ASSERT_RUN(algo, bc, 2);

    ASSERT_NO_THROW(series = db->findDataOrderObstime(87120, pid, TimeRange(t0, t0)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(7.0, series.begin()->corrected());
    ASSERT_EQ("05120041000000A0", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC1-1-211:1,QC1-3a-211:1,QC1-9-211:1,QC2d-2,QC2d-2", series.begin()->cfailed());

    ASSERT_NO_THROW(series = db->findDataOrderObstime(871200, pid, TimeRange(t1, t1)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(7.3, series.begin()->corrected());
    ASSERT_EQ("0100001100000000", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC2d-2,QC2d-2", series.begin()->cfailed());

    // wiki step 7
    std::ostringstream sql;
    sql << "UPDATE data SET useinfo='7010000000000000' WHERE stationid=87120 and obstime='2025-09-17 09:00:00' and paramid=211;"
           "UPDATE data SET useinfo='7010000000000000' WHERE stationid=87120 and obstime='2025-09-16 11:00:00' and paramid=211;";
    ASSERT_NO_THROW(db->exec(sql.str()));

    ASSERT_RUN(algo, bc, 2);

    ASSERT_NO_THROW(series = db->findDataOrderObstime(87120, pid, TimeRange(t0, t0)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(-32766, series.begin()->corrected());
    ASSERT_EQ("05120021000000A0", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC1-1-211:1,QC1-3a-211:1,QC1-9-211:1,QC2d-2,QC2d-2,QC2d-2", series.begin()->cfailed());

    ASSERT_NO_THROW(series = db->findDataOrderObstime(87120, pid, TimeRange(t1, t1)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ( -32767, series.begin()->corrected());
    ASSERT_EQ("0100003100000000", series.begin()->controlinfo().flagstring());
    ASSERT_EQ("QC2d-2,QC2d-2,QC2d-2", series.begin()->cfailed());

    // wiki step 8, run again, no more updates allowed
    ASSERT_RUN(algo, bc, 0);
}

TEST_F(SingleLinearTest, testFromKro)
{
    // \copy ( select stationid,obstime,original,paramid,tbtime,message_formatid,sensor,hlevel,corrected,controlinfo,useinfo,cfailed from kvalobs_data where stationid = 93000 and obstime between '2011-10-09 12:00' and '2011-10-10 20:00' and paramid = 211 ) to /tmp/ababa.out DELIMITER '|'
    DataList data(93000, 211, 330);
    data.add("2011-10-09 12:00:00",     -0.7, -0.7, "0111100000100010", "")
        .add("2011-10-09 13:00:00",     -0.9, -0.9, "0111100000100010", "")
        .add("2011-10-09 14:00:00",     -1.5, -1.5, "0111100000100010", "")
        .add("2011-10-09 15:00:00",     -1.5, -1.5, "0111100000100010", "")
        .add("2011-10-09 16:00:00",     -1.8, -1.8, "0111100000100010", "")
        .add("2011-10-09 17:00:00", -32767.0,  0.2, "0000601000000000", "QC1-4-211")
        .add("2011-10-09 18:00:00", -32767.0,  0.3, "0000601000000000", "QC1-4-211")
        .add("2011-10-09 19:00:00", -32767.0,  0.2, "0000601000000000", "QC1-4-211")
        .add("2011-10-09 20:00:00", -32767.0,  0.3, "0000601000000000", "QC1-4-211")
        .add("2011-10-09 21:00:00", -32767.0,  0.4, "0000601000000000", "QC1-4-211")
        .add("2011-10-09 22:00:00", -32767.0,  0.6, "0000601000000000", "QC1-4-211")
        .add("2011-10-09 23:00:00", -32767.0,  0.9, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 00:00:00", -32767.0,  1.6, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 01:00:00", -32767.0,  2.0, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 02:00:00", -32767.0,  2.3, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 03:00:00", -32767.0,  2.4, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 04:00:00", -32767.0,  2.5, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 05:00:00", -32767.0,  2.6, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 06:00:00", -32767.0,  3.1, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 07:00:00", -32767.0,  3.4, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 08:00:00", -32767.0,  3.6, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 09:00:00", -32767.0,  3.8, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 10:00:00", -32767.0,  4.1, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 11:00:00", -32767.0,  4.2, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 12:00:00", -32767.0,  4.5, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 13:00:00", -32767.0,  4.6, "0000601000000000", "QC1-4-211")
        .add("2011-10-10 14:00:00",      1.7,  1.7, "0110100000100010", "")
        .add("2011-10-10 15:00:00",      1.6,  1.6, "0111100000100010", "")
        .add("2011-10-10 16:00:00",      1.4,  1.4, "0111100000100010", "")
        .add("2011-10-10 17:00:00",      1.4,  1.4, "0111100000100010", "")
        .add("2011-10-10 18:00:00",      1.4,  1.4, "0111100000100010", "")
        .add("2011-10-10 19:00:00",      1.6,  1.6, "0111100000100010", "")
        .add("2011-10-10 20:00:00",      1.3,  1.3, "0111100000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    const int pid = 211;
    std::stringstream config;
    config << "Start_YYYY=2011\n"
           << "Start_MM=10\n"
           << "Start_DD=09  \n"
           << "Start_hh=6\n"
           << "Start_mm=0\n"
           << "Start_ss=0\n"
           << "End_YYYY=2011\n"
           << "End_MM=10\n"
           << "End_DD=11 \n"
           << "End_hh=6\n"
           << "End_mm=0\n"
           << "End_ss=0\n"
           << "ParamId=" << pid << "\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    data.add("2011-10-10 12:00:00", 1.5, "0110100000100010", "");
    ASSERT_NO_THROW(data.update(db));

    ASSERT_RUN(algo, bc, 1);

    std::list<kvalobs::kvData> series;
    miutil::miTime t0("2011-10-10 13:00:00");
    ASSERT_NO_THROW(series = db->findDataOrderObstime(93000, pid, TimeRange(t0, t0)));
    ASSERT_EQ(1, series.size());
    ASSERT_FLOAT_EQ(1.6, series.begin()->corrected());
}

TEST_F(SingleLinearTest, ParamOrdering)
{
    DataList data(180, 211, 330);
    data.add("2025-09-17 06:00:00",      7.4, "0111000000000010", "")
        .add("2025-09-17 07:00:00",      7.6, "0111000000000010", "")
        .add("2025-09-17 08:00:00",      8.5, "0111000000000010", "")
        .add("2025-09-17 09:00:00",      8.7, "0111000000000010", "")
        .add("2025-09-17 10:00:00", -32767.0, "0100003000000000", "")
        .add("2025-09-17 11:00:00",      9.5, "0110000000000010", "")
        .add("2025-09-17 12:00:00",      9.5, "0111000000000010", "")
        .add("2025-09-17 13:00:00",      9.8, "0111000000000010", "")
        .setParam(212) // not a reasonable, just a different parameter id
        .add("2025-09-17 06:00:00",      4.2, "0111000000000010", "")
        .add("2025-09-17 07:00:00",      4.1, "0111000000000010", "")
        .add("2025-09-17 08:00:00",      4.0, "0111000000000010", "")
        .add("2025-09-17 09:00:00", -32767.0, "0100003000000000", "")
        .add("2025-09-17 10:00:00",      3.8, "0110000000000010", "")
        .add("2025-09-17 11:00:00",      3.7, "0110000000000010", "")
        .add("2025-09-17 12:00:00",      3.6, "0111000000000010", "")
        .add("2025-09-17 13:00:00",      3.5, "0111000000000010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2025\n"
           << "Start_MM   =   09\n"
           << "Start_DD   =   17\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2025\n"
           << "End_MM     =   09\n"
           << "End_DD     =   17\n"
           << "End_hh     =   13\n"
           << "ParamId=212\n"
           << "ParamId=211\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    ASSERT_OBSTIME("2025-09-17 09:00:00", bc->update(0));
    ASSERT_EQ(212, bc->update(0).paramID());
    
    ASSERT_OBSTIME("2025-09-17 10:00:00", bc->update(1));
    ASSERT_EQ(211, bc->update(1).paramID());

    ASSERT_RUN(algo, bc, 0);
}
