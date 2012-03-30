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
#include "algorithms/PlumaticAlgorithm.h"
#include "AlgorithmHelpers.h"
#include "DBConstraints.h"
#include "foreach.h"

#include <algorithm>
#include <numeric>

namespace C = Constraint;

class PlumaticTest : public AlgorithmTestBase {
public:
    void SetUp();
    void InsertRealData();
    void Configure(AlgorithmConfig& params, int bm=10, int bd=1, int bh=12, int em=11, int ed=10, int eh=12);
};

// ========================================================================

void PlumaticTest::SetUp()
{
    algo = new PlumaticAlgorithm();
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(27270, 59.278, 10.4312, 3, 0, 'TØNSBERG - KILEN', 1503, 27270, NULL, NULL, NULL, 9, 't', '2010-03-24 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 105, 0, 0, 1, 365, -1, 'QC1-1-105', 'max;highest;high;low;lowest;min\n5;4.0;3.0;-6.0;-6.0;-6.0', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0, 105, 0, 0, 0, 365, -1, 'QC1-3a-105', 'max\n3.0', NULL, '1500-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));
}

// ------------------------------------------------------------------------

void PlumaticTest::InsertRealData()
{
    DataList data(27270, 105, 4);
    data.add("2011-10-29 23:32:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 00:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 00:02:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 00:06:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 00:20:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 00:37:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 00:40:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 00:44:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 01:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 01:33:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 02:00:00",   0, -32766, "0000002000000700", "")
        .add("2011-10-30 02:50:00", 0.1,    0.1, "0100000000000000", "")
        .add("2011-10-30 02:53:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 02:57:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 03:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 03:01:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 03:02:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 03:07:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 03:10:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 03:21:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 03:39:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 03:50:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 04:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 05:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 05:49:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 05:55:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 06:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 06:04:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 06:13:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 06:16:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 06:25:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 06:30:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 06:51:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 07:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 07:13:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 07:19:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 07:30:00", 0.1,    0.1, "0101000000000000", "")
        .add("2011-10-30 08:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 09:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 10:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 11:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 12:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 13:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 14:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 15:00:00",   0,      0, "0101000000000000", "")
        .add("2011-10-30 16:00:00",   0,      0, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));
}

// ------------------------------------------------------------------------

void PlumaticTest::Configure(AlgorithmConfig& params, int bm, int bd, int bh, int em, int ed, int eh)
{
    std::stringstream config;
    config << "Start_YYYY = 2011\n"
        "Start_MM   =   " << bm << "\n"
        "Start_DD   =   " << bd << "\n"
        "Start_hh   =   " << bh << "\n"
        "End_YYYY   = 2011\n"
        "End_MM     =   " << em << "\n"
        "End_DD     =   " << ed << "\n"
        "End_hh     =   " << eh << "\n"
        "highstart_flagchange       = fs=A,fmis=2\n"
        "highsingle_flagchange      = fs=B,fmis=2\n"
        "interruptedrain_flagchange = fs=C,fmis=2\n"
        "aggregation_flagchange     = fr=9,fmis=2\n"
        "discarded_cflags           = fr=9|fs=[ABC]|fmis=)0(|fhqc=)0(\n"
        "stations = 0.1:27270;0.2:30270\n"
        "sliding_alarms = 2<8.4;3<10.2\n"
        "ParamId = 105\n";
    params.Parse(config);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, HighSingle)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 22:00:00", 0,   "0101000000000000", "")
        // next is high single
        .add("2011-10-01 22:01:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 23:00:00", 0,   "0101000000000000", "")
        .add("2011-10-01 23:29:00", 0.1, "0101000000000000", "")
        // next is high single
        .add("2011-10-01 23:39:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 23:43:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 23:44:00", 0.5, "0101000000000000", "")
        .add("2011-10-02 00:00:00", 0,   "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:01:00", "010B002000000000", "QC2h-1-highsingle", bc->updates()[0]);
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:39:00", "010B002000000000", "QC2h-1-highsingle", bc->updates()[1]);

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, HighSingleStartEnd)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.3, "0101000000000000", "")
        .add("2011-10-01 13:00:00", 0.0, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 1, 12, 11, 1, 15);
    
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);

    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:00:00", "010B002000000000", "QC2h-1-highsingle", bc->updates()[0]);

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, HighStart)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 22:00:00", 0,   "0101000000000000", "")
        
        .add("2011-10-01 22:43:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 22:44:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-01 23:00:00", 0,   "0101000000000000", "")
        .add("2011-10-01 23:01:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 23:02:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-02 00:00:00", 0,   "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:43:00", "010A002000000000", "QC2h-1-highstart", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:01:00", "010A002000000000", "QC2h-1-highstart", bc->update(1));

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, HighStartStartEnd)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 12:01:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 12:02:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 12:03:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 12:04:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));
    
    AlgorithmConfig params;
    Configure(params, 10, 1, 12, 11, 1, 13);
    
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);

    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:00:00", "010A002000000000", "QC2h-1-highstart", bc->update(0));

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, RainInterrupt)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 21:00:00", 0,   "0101000000000000", "")

        .add("2011-10-01 21:52:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 21:53:00", 0.4, "0101000000000000", "")
        // rain interruption, but too long => high start
        .add("2011-10-01 21:59:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 22:00:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-01 22:18:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 22:19:00", 0.5, "0101000000000000", "")
        // rain interruption
        .add("2011-10-01 22:23:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 22:24:00", 0.2, "0101000000000000", "")
        
        .add("2011-10-01 23:00:00", 0,   "0101000000000000", "")
        
        .add("2011-10-01 23:57:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 23:58:00", 0.5, "0101000000000000", "")
        // rain interruption at hour break
        .add("2011-10-02 00:00:00", 0,   "0101000000000000", "")
        .add("2011-10-02 00:02:00", 0.5, "0101000000000000", "")
        .add("2011-10-02 00:03:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-02 00:12:00", 0.1, "0101000000000000", "")
        // rain interruption, but not enough rain before => high start
        .add("2011-10-02 00:15:00", 0.5, "0101000000000000", "")
        .add("2011-10-02 00:16:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-02 01:00:00", 0,   "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);
    
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1+3+3+1);

    std::list<kvalobs::kvData> series;
    ASSERT_NO_THROW(db->selectData(series, "WHERE obstime BETWEEN '2011-10-01 23:57:00' AND '2011-10-02 00:03:00';"));
    ASSERT_EQ(7, series.size());

    // unusual time ordering due to update/insert (inserts first, then updates)
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:20:00", "000C002000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:21:00", "000C002000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(1));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:22:00", "000C002000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(2));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:59:00", "000C002000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(3));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-02 00:01:00", "000C002000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(4));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 21:59:00", "010A002000000000", "QC2h-1-highstart",                       bc->update(5));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-02 00:00:00", "010C002000000000", "QC2h-1-interruptedrain",                 bc->update(6));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-02 00:15:00", "010A002000000000", "QC2h-1-highstart",                       bc->update(7));

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, RainInterruptStartEnd)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.3, "0101000000000000", "")
        // rain interruption at start, too short => high single at 12:00 and high start at 12:03
        .add("2011-10-01 12:03:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 12:04:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-01 13:58:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 13:59:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 14:00:00", 0.0, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 1, 12, 11, 1, 14);
    
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);

    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:00:00", "010B002000000000", "QC2h-1-highsingle", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:03:00", "010A002000000000", "QC2h-1-highstart",  bc->update(1));

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, PluviometerResolution02)
{
    DataList data(30270, 105, 4);
    data.add("2011-10-01 22:00:00", 0,   "0101000000000000", "")

        // high start only for 0.1mm Pluviometer
        .add("2011-10-01 22:43:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 22:44:00", 0.2, "0101000000000000", "")
        
        .add("2011-10-01 23:00:00", 0,   "0101000000000000", "")
        // high start only for 0.1mm Pluviometer
        .add("2011-10-01 23:01:00", 0.2, "0101000000000000", "")
        .add("2011-10-01 23:02:00", 0.2, "0101000000000000", "")
        
        // high single for 0.2mm Pluviometer
        .add("2011-10-01 23:07:00", 0.6, "0101000000000000", "")
        
        // high start for 0.2mm Pluviometer
        .add("2011-10-01 23:11:00", 0.6, "0101000000000000", "")
        .add("2011-10-01 23:12:00", 0.2, "0101000000000000", "")
        
        .add("2011-10-01 23:18:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 23:19:00", 0.4, "0101000000000000", "")
        // rain interruption only for 0.1mm Pluviometer
        .add("2011-10-01 23:23:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 23:24:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-01 23:28:00", 0.2, "0101000000000000", "")
        .add("2011-10-01 23:29:00", 0.8, "0101000000000000", "")
        // rain interruption for 0.2mm Pluviometer
        .add("2011-10-01 23:33:00", 0.8, "0101000000000000", "")
        .add("2011-10-01 23:34:00", 0.2, "0101000000000000", "")

        .add("2011-10-02 00:00:00", 0,   "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 5);

    // unusual time ordering due to update/insert (inserts first, then updates)
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:30:00", "000C002000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:31:00", "000C002000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(1));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:32:00", "000C002000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(2));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:07:00", "010B002000000000", "QC2h-1-highsingle",                      bc->update(3));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:11:00", "010A002000000000", "QC2h-1-highstart",                       bc->update(4));

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, Aggregation2Not3)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        // sliding sum too high for the next three
        .add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 12:04:00", 9.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);

    miutil::miTime t("2011-10-01 12:03:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1)) {
        EXPECT_OBS_CONTROL_CFAILED(t, "0901002000000000", "QC2h-1-aggregation-2", bc->update(i));
        EXPECT_EQ("QC2h-1-aggregation-2", bc->update(i).cfailed());
    }

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, Aggregation3Not2A)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        // sliding sum of three too high for the next three
        .add("2011-10-01 12:04:00", 4.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 4.0, "0101000000000000", "")
        .add("2011-10-01 12:06:00", 4.0, "0101000000000000", "")
        .add("2011-10-01 12:07:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);

    miutil::miTime t("2011-10-01 12:04:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1)) {
        const kvalobs::kvData& u = bc->updates()[i];
        EXPECT_EQ(t, u.obstime());
        EXPECT_EQ("0901002000000000", u.controlinfo().flagstring());
        EXPECT_EQ("QC2h-1-aggregation-3", u.cfailed());
    }

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, Aggregation3Not2B)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        // sliding sum of three too high for the next three
        .add("2011-10-01 12:04:00", 3.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:06:00", 3.0, "0101000000000000", "")

        .add("2011-10-01 12:07:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);

    miutil::miTime t("2011-10-01 12:04:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1)) {
        EXPECT_OBS_CONTROL_CFAILED(t, "0901002000000000", "QC2h-1-aggregation-3", bc->update(i));
        EXPECT_EQ("QC2h-1-aggregation-3", bc->update(i).cfailed());
    }

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, Aggregation2And3)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        // sliding sum of two too high for the next three
        .add("2011-10-01 12:04:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:06:00", 5.0, "0101000000000000", "")

        .add("2011-10-01 12:07:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);

    miutil::miTime t("2011-10-01 12:04:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1)) {
        EXPECT_OBS_CONTROL_CFAILED(t, "0901002000000000", "QC2h-1-aggregation-2", bc->update(i));
        EXPECT_EQ("QC2h-1-aggregation-2", bc->update(i).cfailed());
    }

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, AggregationOverHighStart)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        // next is high start
        .add("2011-10-01 12:03:00", 0.3, "0101000000000000", "")
        // sliding sum of two too high for the next, but may not trigger because of high start
        .add("2011-10-01 12:04:00", 8.2, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);

    ASSERT_OBS_CONTROL_CFAILED("2011-10-01 12:03:00", "010A002000000000", "QC2h-1-highstart", bc->update(0));

    // remove high start and flags, and run again
    ASSERT_NO_THROW(data
                    .add("2011-10-01 12:03:00", 0.2, "0101000000000000", "")
                    .add("2011-10-01 12:04:00", 8.2, "0101000000000000", "")
                    .update(db));

    ASSERT_RUN(algo, bc, 2);

    miutil::miTime t("2011-10-01 12:03:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1)) {
        EXPECT_OBS_CONTROL_CFAILED(t, "0901002000000000", "QC2h-1-aggregation-2", bc->update(i));
        EXPECT_EQ("QC2h-1-aggregation-2", bc->update(i).cfailed());
    }

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, NoAggregation)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        // sliding sum of two too high for the next three; but no limits set
        .add("2011-10-01 12:04:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:06:00", 5.0, "0101000000000000", "")

        .add("2011-10-01 12:07:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2011\n"
        "Start_MM   =   10\n"
        "Start_DD   =    1\n"
        "Start_hh   =   12\n"
        "End_YYYY   = 2011\n"
        "End_MM     =   12\n"
        "End_DD     =    1\n"
        "End_hh     =   13\n"
        "stations = 0.1:27270\n"
        "sliding_alarms = \n"
        "ParamId = 105\n";
    params.Parse(config);
    algo->configure(params);
    ASSERT_RUN(algo, bc, 0);

    // re-configure with sliding_alarms
    std::stringstream config2;
    config2 << "Start_YYYY = 2011\n"
        "Start_MM   =   10\n"
        "Start_DD   =    1\n"
        "Start_hh   =   12\n"
        "End_YYYY   = 2011\n"
        "End_MM     =   12\n"
        "End_DD     =    1\n"
        "End_hh     =   13\n"
        "stations = 0.1:27270\n"
        "sliding_alarms = 2<8.4;3<10.2\n"
        "ParamId = 105\n";
    AlgorithmConfig params2;
    params2.Parse(config2);
    ASSERT_NO_THROW(algo->configure(params2));
    ASSERT_RUN(algo, bc, 3);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, Dry)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 13:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 14:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 15:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 16:00:00", 0.0, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, NoCheckOrigNotEqualCorrected)
{
    // checks that no flags are set when original != corrected
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        // sliding sum of two too high for the next three
        .add("2011-10-01 12:04:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 5.0, 4.0, "0101000000000000", "")
        .add("2011-10-01 12:06:00", 5.0, "0101000000000000", "")

        .add("2011-10-01 12:07:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "")
        // next two are high start
        .add("2011-10-01 13:43:00", 0.5, 0.2, "0101000000000000", "")
        .add("2011-10-01 13:44:00", 0.3, 0.2, "0101000000000000", "")
        
        // next two are high start
        .add("2011-10-01 13:53:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 13:54:00", 0.3, 0.2, "0101000000000000", "")

        .add("2011-10-01 14:00:00", 0,   "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);
    ASSERT_OBS_CONTROL_CFAILED("2011-10-01 13:53:00", "010A002000000000", "QC2h-1-highstart", bc->update(0));

    data.add("2011-10-01 12:05:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 13:43:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 13:44:00", 0.3, "0101000000000000", "")
        .add("2011-10-01 13:53:00", 0.5, "0101000000000000", "") // to reset flags from previous run
        .add("2011-10-01 13:54:00", 0.3, "0101000000000000", "");
    ASSERT_NO_THROW(data.update(db));

    ASSERT_RUN(algo, bc, 3+1+1);
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:04:00", "0901002000000000", "QC2h-1-aggregation-2", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:05:00", "0901002000000000", "QC2h-1-aggregation-2", bc->update(1));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:06:00", "0901002000000000", "QC2h-1-aggregation-2", bc->update(2));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 13:43:00", "010A002000000000", "QC2h-1-highstart", bc->update(3));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 13:53:00", "010A002000000000", "QC2h-1-highstart", bc->update(4));
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, NoCheckBadFlags)
{
    // checks that no flags are set when fhqc/fmis/... are set from before
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        // sliding sum of two too high for the next three
        .add("2011-10-01 12:04:00", 5.0, "0101002000000000", "")
        .add("2011-10-01 12:05:00", 5.0, "0101002000000000", "")
        .add("2011-10-01 12:06:00", 5.0, "0101002000000000", "")

        .add("2011-10-01 12:07:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "")
        // next two are high start
        .add("2011-10-01 13:43:00", 0.5, "0101000000000001", "")
        .add("2011-10-01 13:44:00", 0.3, "0101000000000001", "")
        
        .add("2011-10-01 14:00:00", 0,   "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    data.add("2011-10-01 12:04:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:06:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 13:43:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 13:44:00", 0.3, "0101000000000000", "");
    ASSERT_NO_THROW(data.update(db));

    ASSERT_RUN(algo, bc, 3+1);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, AggregationNoDoubleCfailed2)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(17980, 59.8435, 10.8243, 92, 0, 'OSLO - LJABRUVEIEN', NULL, 17980, NULL, NULL, NULL, 9, 't', '2000-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    // -- data are shifted by +10 years
    // select modificationtime, obstime, original, controlinfo, cfailed from data_history
    // where stationid = 17980 and paramid = 105
    // and obstime between '2020-07-12 22:19:00' and '2020-07-12 22:21:00'
    // order by obstime, modificationtime;

    DataList data(17980, 105, 4);
    data.add("2020-07-12 22:15:00", 0.1, "0101000000000000", "")
        .add("2020-07-12 22:17:00", 0.1, "0101000000000000", "")
        .add("2020-07-12 22:18:00", 0.3, "0101000000000000", "")
        .add("2020-07-12 22:19:00", 0.8, "0101000000000000", "")
        .add("2020-07-12 22:20:00", 3.6, "0201000000000000", "QC1-1-105:1")
        .add("2020-07-12 22:21:00", 2.1, "0101000000000000", "")
        .add("2020-07-12 22:22:00", 0.8, "0101000000000000", "")
        .add("2020-07-12 22:23:00", 0.5, "0101000000000000", "")
        .add("2020-07-12 22:24:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:25:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:26:00", 0.1, "0101000000000000", "")
        .add("2020-07-12 22:27:00", 0.1, "0101000000000000", "")
        .add("2020-07-12 22:28:00", 0.1, "0101000000000000", "")
        .add("2020-07-12 22:29:00", 0.1, "0101000000000000", "")
        .add("2020-07-12 22:30:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:31:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:32:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:33:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:34:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:35:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:36:00", 0.1, "0101000000000000", "")
        .add("2020-07-12 22:37:00", 0.2, "0101000000000000", "")
        .add("2020-07-12 22:38:00", 0.1, "0101000000000000", ""); // not the end of the observed shower, but enough for the test
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2020\n"
        "Start_MM   =   06\n"
        "Start_DD   =   01\n"
        "Start_hh   =   00\n"
        "End_YYYY   = 2021\n"
        "End_MM     =   09\n"
        "End_DD     =   01\n"
        "End_hh     =   00\n"
        "stations = 0.1:17980\n"
        "# unscaled sliding_alarms = 2<8.1;3<11.9;5<16.2;10<25.6;15<27.3;20<34.4;30<42.0;45<49.1;60<54.9;90<56.7;180<60.8;360<83.3;720<144.1;1440<159.7\n"
        "sliding_alarms = 2<4.05;3<5.95;5<8.10;10<12.80;15<13.65;20<17.20;30<21.00;45<24.55;60<27.45;90<28.35;180<30.40;360<41.65;720<72.05;1440<79.85\n"
        "ParamId = 105\n";
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);

    EXPECT_OBS_CONTROL_CFAILED("2020-07-12 22:19:00", "0901000000000000", "QC2h-1-aggregation-2", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2020-07-12 22:20:00", "0901000000000000", "QC2h-1-aggregation-2", bc->update(1));
    EXPECT_OBS_CONTROL_CFAILED("2020-07-12 22:21:00", "0901000000000000", "QC2h-1-aggregation-2", bc->update(2));
    ASSERT_EQ("QC1-1-105:1,QC2h-1-aggregation-2", bc->update(1).cfailed());

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, AggregationNoDoubleCfailed15)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(17980, 59.8435, 10.8243, 92, 0, 'OSLO - LJABRUVEIEN', NULL, 17980, NULL, NULL, NULL, 9, 't', '2000-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    // -- data are shifted by +10 years
    // select obstime, original, controlinfo, cfailed from data where stationid = 17980 and paramid = 105
    // and obstime between '2021-07-24 00:00:00' and '2021-07-24 01:00:00' order by obstime;

    DataList data(17980, 105, 4);
    data.add("2021-07-24 00:00:00", 0.0, "0101000000000000", "")
        .add("2021-07-24 00:18:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:20:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:21:00", 0.2, "0101000000000000", "")
        .add("2021-07-24 00:22:00", 0.5, "0101000000000000", "")
        .add("2021-07-24 00:23:00", 0.5, "0101000000000000", "")
        .add("2021-07-24 00:24:00", 0.7, "0101000000000000", "")
        .add("2021-07-24 00:25:00", 0.9, "0101000000000000", "")
        .add("2021-07-24 00:26:00", 1.1, "0101000000000000", "")
        .add("2021-07-24 00:27:00", 0.8, "0101000000000000", "")
        .add("2021-07-24 00:28:00", 1.1, "0101000000000000", "")
        .add("2021-07-24 00:29:00", 1.1, "0101000000000000", "")
        .add("2021-07-24 00:30:00", 1.5, "0101000000000000", "")
        .add("2021-07-24 00:31:00", 1.2, "0101000000000000", "")
        .add("2021-07-24 00:32:00", 1.0, "0101000000000000", "")
        .add("2021-07-24 00:33:00", 0.7, "0101000000000000", "")
        .add("2021-07-24 00:34:00", 0.8, "0101000000000000", "")
        .add("2021-07-24 00:35:00", 1.1, "0101000000000000", "")
        .add("2021-07-24 00:36:00", 0.9, "0101000000000000", "")
        .add("2021-07-24 00:37:00", 1.2, "0101000000000000", "")
        .add("2021-07-24 00:38:00", 1.1, "0101000000000000", "")
        .add("2021-07-24 00:39:00", 0.9, "0101000000000000", "")
        .add("2021-07-24 00:40:00", 0.5, "0101000000000000", "")
        .add("2021-07-24 00:41:00", 0.4, "0101000000000000", "")
        .add("2021-07-24 00:42:00", 0.3, "0101000000000000", "")
        .add("2021-07-24 00:43:00", 0.3, "0101000000000000", "")
        .add("2021-07-24 00:44:00", 0.2, "0101000000000000", "")
        .add("2021-07-24 00:45:00", 0.4, "0101000000000000", "")
        .add("2021-07-24 00:46:00", 0.3, "0101000000000000", "")
        .add("2021-07-24 00:47:00", 0.3, "0101000000000000", "")
        .add("2021-07-24 00:48:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:49:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:50:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:52:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:56:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:57:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:58:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 00:59:00", 0.1, "0101000000000000", "")
        .add("2021-07-24 01:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2020\n"
        "Start_MM   =   06\n"
        "Start_DD   =   01\n"
        "Start_hh   =   00\n"
        "End_YYYY   = 2021\n"
        "End_MM     =   09\n"
        "End_DD     =   01\n"
        "End_hh     =   00\n"
        "stations = 0.1:17980\n"
        "# unscaled sliding_alarms = 2<8.1;3<11.9;5<16.2;10<25.6;15<27.3;20<34.4;30<42.0;45<49.1;60<54.9;90<56.7;180<60.8;360<83.3;720<144.1;1440<159.7\n"
        "sliding_alarms = 2<4.05;3<5.95;5<8.10;10<12.80;15<13.65;20<17.20;30<21.00;45<24.55;60<27.45;90<28.35;180<30.40;360<41.65;720<72.05;1440<79.85\n"
        "ParamId = 105\n";
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 21);

    for(int i=0; i<bc->count(); ++i)
        EXPECT_EQ("QC2h-1-aggregation-15", bc->update(i).cfailed());

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, AggregationNoDoubleCfailed180)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(18269, 59.954, 10.905, 123, 0, 'OSLO - HAUGENSTUA', NULL, 18269, NULL, NULL, NULL, 9, 't', '2000-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));
    
    // -- data are shifted by +10 years
    // select obstime, original, controlinfo, cfailed from data where stationid = 18269 and paramid = 105
    // and obstime between '2021-06-07 03:00:00' and '2021-06-07 09:00:00' order by obstime;

    DataList data(18269, 105, 4);
    data.add("2021-06-07 03:00:00", 0.0, "0101000000000000", "")
        .add("2021-06-07 04:00:00", 0.0, "0101000000000000", "")
        .add("2021-06-07 05:00:00", 0.0, "0101000000000000", "")
        .add("2021-06-07 05:14:00", 0.1, "0100000000000000", "")
        .add("2021-06-07 05:16:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 05:17:00", 0.5, "0101000000000000", "")
        .add("2021-06-07 05:18:00", 0.6, "0101000000000000", "")
        .add("2021-06-07 05:19:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 05:20:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 05:21:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 05:22:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 05:23:00", 0.8, "0101000000000000", "")
        .add("2021-06-07 05:24:00", 1.0, "0101000000000000", "")
        .add("2021-06-07 05:25:00", 0.9, "0101000000000000", "")
        .add("2021-06-07 05:26:00", 0.9, "0101000000000000", "")
        .add("2021-06-07 05:27:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 05:28:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 05:29:00", 0.5, "0101000000000000", "")
        .add("2021-06-07 05:30:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 05:31:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 05:32:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 05:33:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 05:34:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 05:35:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 05:36:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 05:37:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 05:38:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 05:39:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 05:40:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 05:42:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 05:46:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 05:48:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 05:51:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:00:00", 0.0, "0101000000000000", "")
        .add("2021-06-07 06:05:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:08:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:10:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:11:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:12:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:13:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:14:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:16:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:17:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:18:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:19:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:20:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:21:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:22:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 06:23:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 06:24:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 06:25:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 06:26:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:27:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:28:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:29:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:30:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:31:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:32:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:33:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:34:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:35:00", 0.8, "0101000000000000", "")
        .add("2021-06-07 06:36:00", 0.9, "0101000000000000", "")
        .add("2021-06-07 06:37:00", 0.8, "0101000000000000", "")
        .add("2021-06-07 06:38:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 06:39:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:40:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:41:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:42:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 06:44:00", 0.2, "0101000000000000", "")
        .add("2021-06-07 06:45:00", 1.0, "0101000000000000", "")
        .add("2021-06-07 06:46:00", 0.9, "0101000000000000", "")
        .add("2021-06-07 06:47:00", 1.2, "0101000000000000", "")
        .add("2021-06-07 06:48:00", 1.2, "0101000000000000", "")
        .add("2021-06-07 06:49:00", 1.2, "0101000000000000", "")
        .add("2021-06-07 06:50:00", 1.0, "0101000000000000", "")
        .add("2021-06-07 06:51:00", 1.0, "0101000000000000", "")
        .add("2021-06-07 06:52:00", 1.0, "0101000000000000", "")
        .add("2021-06-07 06:53:00", 0.7, "0101000000000000", "")
        .add("2021-06-07 06:54:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:55:00", 0.3, "0101000000000000", "")
        .add("2021-06-07 06:56:00", 0.6, "0101000000000000", "")
        .add("2021-06-07 06:57:00", 1.0, "0101000000000000", "")
        .add("2021-06-07 06:58:00", 0.4, "0101000000000000", "")
        .add("2021-06-07 06:59:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 07:00:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 07:02:00", 0.1, "0100000000000000", "")
        .add("2021-06-07 07:04:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 07:08:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 07:11:00", 0.1, "0101000000000000", "")
        .add("2021-06-07 08:00:00", 0.0, "0101000000000000", "")
        .add("2021-06-07 09:00:00", 0.0, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2020\n"
        "Start_MM   =   06\n"
        "Start_DD   =   01\n"
        "Start_hh   =   00\n"
        "End_YYYY   = 2021\n"
        "End_MM     =   09\n"
        "End_DD     =   01\n"
        "End_hh     =   00\n"
        "stations = 0.1:18269\n"
        "# unscaled sliding_alarms = 2<8.1;3<11.9;5<16.2;10<25.6;15<27.3;20<34.4;30<42.0;45<49.1;60<54.9;90<56.7;180<60.8;360<83.3;720<144.1;1440<159.7\n"
        "sliding_alarms = 2<4.05;3<5.95;5<8.10;10<12.80;15<13.65;20<17.20;30<21.00;45<24.55;60<27.45;90<28.35;180<30.40;360<41.65;720<72.05;1440<79.85\n"
        "ParamId = 105\n";
    params.Parse(config);
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 85);

    for(int i=0; i<bc->count(); ++i) {
        EXPECT_EQ("QC2h-1-aggregation-180", bc->update(i).cfailed());
        EXPECT_LT(0, bc->update(i).original());
    }

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, NonOperationalMarked)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:03:00",  -5, "0101000000000000", "")
        // sliding sum of two too high for the next three
        .add("2011-10-01 12:04:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:06:00", 5.0, "0101000000000000", "")

        .add("2011-10-01 12:07:00",  -6, "0101000000000000", "")
        // next two are high start
        .add("2011-10-01 13:43:00", 0.5, "0101000000000000", "")
        .add("2011-10-01 13:44:00", 0.3, "0101000000000000", "")
        
        .add("2011-10-01 14:00:00", 0,   "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 1, 12, 10, 1, 14);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 1);

    ASSERT_EQ(5, logs->count());
    EXPECT_EQ(0, logs->find("-5 marker.*followed by.*12:04:00", Message::WARNING));
    EXPECT_EQ(1, logs->find("-5 marker.*followed by.*12:05:00", Message::WARNING));
    EXPECT_EQ(2, logs->find("-5 marker.*followed by.*12:06:00", Message::WARNING));
    EXPECT_EQ(3, logs->find("ignoring non-operational time for station 27270 between 2011-10-01 12:03:00 and 2011-10-01 12:07:00", Message::INFO));
    EXPECT_EQ(4, logs->find("UPDATE", Message::INFO));

    // now remove the markers for non-operational time and run again

    data.add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 12:07:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.update(db));

    std::list<kvalobs::kvData> series;
    miutil::miTime t0("2011-10-01 12:04:00"), t1("2011-10-01 12:06:00");
    ASSERT_NO_THROW(db->selectData(series, C::Station(27270) && C::Paramid(105) && C::Obstime(t0, t1)));
    ASSERT_EQ(3, series.size());

    ASSERT_RUN(algo, bc, 3);
    for(int i=0; i<bc->count(); ++i)
        EXPECT_EQ("QC2h-1-aggregation-2", bc->update(i).cfailed());

    ASSERT_RUN(algo, bc, 0);
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, NonOperationalZeroes)
{
    DataList data(27270, 105, 4);
    // operational
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-01 13:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 13:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-01 14:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 15:00:00", 0.0, "0101000000000000", "");

    // not operational 17:00 -- 17:59
    data.add("2011-10-01 16:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 16:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-01 18:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 18:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-01 19:00:00", 0.0, "0101000000000000", "");

    // not operational 22:00 -- 22:59
    data.add("2011-10-01 20:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 20:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-01 21:00:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 23:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 23:05:00", 0.2, "0101000000000000", "");

    // not operational 01:00 -- 02:59
    data.add("2011-10-02 00:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 00:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-02 03:00:00", 0.2, "0101000000000000", "")
        .add("2011-10-02 03:05:00", 0.2, "0101000000000000", "");

    // not operational 05:00 -- 05:59
    data.add("2011-10-02 04:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 04:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-02 06:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 07:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 07:05:00", 0.2, "0101000000000000", "");

    // not operational 09:00 -- 09:59
    data.add("2011-10-02 08:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 08:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-02 10:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 10:05:00", 0.2, "0101000000000000", "")
        .add("2011-10-02 11:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 11:05:00", 0.2, "0101000000000000", "");

    // operational
    data.add("2011-10-02 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 13:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 14:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 15:00:00", 0.0, "0101000000000000", "");

    // not operational 17:00 -- 18:59
    data.add("2011-10-02 16:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-02 17:00:00", 0.0, "0101000000000001", "") // fhqc != 0 -> discarded
        .add("2011-10-02 17:05:00", 0.1, "0101000000000001", "") // fhqc != 0 -> discarded
        .add("2011-10-02 18:00:00", 0.0, "0101000000000001", "") // fhqc != 0 -> discarded
        .add("2011-10-02 19:00:00", 0.0, "0101000000000000", "");

    data.add("2011-10-02 20:00:00", 0.2, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 1, 12, 10, 2, 20);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    ASSERT_EQ(6, logs->count());
    EXPECT_EQ(0, logs->find("ignoring non-operational .* 2011-10-01 17:00:00 and 2011-10-01 17:59:00"));
    EXPECT_EQ(1, logs->find("ignoring non-operational .* 2011-10-01 22:00:00 and 2011-10-01 22:59:00"));
    EXPECT_EQ(2, logs->find("ignoring non-operational .* 2011-10-02 01:00:00 and 2011-10-02 02:59:00"));
    EXPECT_EQ(3, logs->find("ignoring non-operational .* 2011-10-02 05:00:00 and 2011-10-02 05:59:00"));
    EXPECT_EQ(4, logs->find("ignoring non-operational .* 2011-10-02 09:00:00 and 2011-10-02 09:59:00"));
    EXPECT_EQ(5, logs->find("ignoring non-operational .* 2011-10-02 17:00:00 and 2011-10-02 18:59:00"));
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, Neighbors)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES( 3005, 59.1778, 11.2075,  40, 0, 'INGEDAL',             NULL,   3005, NULL,   NULL, NULL, 0, 't', '2010-01-21 00:00:00');"
        << "INSERT INTO station VALUES(17000, 59.1513, 10.8288,  10, 0, 'STROMTANGEN FYR',     1495,  17000, NULL,   NULL, NULL, 8, 't', '1994-05-09 00:00:00');"
        << "INSERT INTO station VALUES(17090, 59.3503,  10.897,  50, 0, 'RADE - KIRKEBO',      1511,  17090, NULL,   NULL, NULL, 0, 't', '2010-01-21 00:00:00');"
        << "INSERT INTO station VALUES(17150, 59.3786, 10.7752,  40, 0, 'RYGGE',               1494,  17150, 'ENRY', NULL, NULL, 8, 't', '1955-01-01 00:00:00');"
        << "INSERT INTO station VALUES(17280, 59.4352,  10.578,  14, 0, 'GULLHOLMEN',          1508,  17280, NULL,   NULL, NULL, 8, 't', '2010-07-01 00:00:00');"
        << "INSERT INTO station VALUES(17400, 59.4765,   10.79,  30, 0, 'KJESEBOTN',           NULL,  17400, NULL,   NULL, NULL, 0, 't', '2010-01-05 00:00:00');"
        << "INSERT INTO station VALUES(27045, 59.5867, 10.1917,  10, 0, 'SANDE - VALLE',       NULL,  27045, NULL,   NULL, NULL, 8, 't', '2001-02-01 00:00:00');"
        << "INSERT INTO station VALUES(27450,   59.23, 10.3483,  26, 0, 'MELSOM',              1481,  27450, NULL,   NULL, NULL, 8, 't', '2011-08-12 00:00:00');"
        << "INSERT INTO station VALUES(27470, 59.1845, 10.2553,  88, 0, 'TORP',                1483,  27470, 'ENTO', NULL, NULL, 8, 't', '1959-09-01 00:00:00');"
        << "INSERT INTO station VALUES(30420, 59.1833,  9.5667, 136, 0, 'SKIEN - GEITERYGGEN', 1475,  30420, 'ENSN', NULL, NULL, 8, 't', '1962-10-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    const int NDAYS = 4;
    DataList dataC(27270, 105, 4);
    for(int day=0; day<NDAYS; ++day) {
        for(int hour=0; hour<24; ++hour) {
            miutil::miTime tC(2011, 10, 1 + day, hour, 0, 0);
            dataC.add(tC, 0.0, "0101000000000000", "");
            if( (tC >= miutil::miTime(2011, 10, 1, 6, 0, 0) && tC <= miutil::miTime(2011, 10, 2, 5, 59, 0))
                || (tC >= miutil::miTime(2011, 10, 3, 6, 0, 0) && tC <= miutil::miTime(2011, 10, 4, 5, 59, 0)) )
            {
                tC.addMin(15);
                dataC.add(tC, 0.1, "0101000000000000", "");
                tC.addMin(1);
                dataC.add(tC, 0.5, "0101000000000000", "");
                tC.addMin(1);
                dataC.add(tC, 0.1, "0101000000000000", "");
            }
        }
    }
    ASSERT_NO_THROW(dataC.insert(db));

    const int neighborIDs[] = { 27450, 3005, 17000, 17090, 17150, 17280, 17400, 27045, 27470, 30420, -1 };
    DataList dataN(neighborIDs[0], 110, 302);
    for(int day=0; day<=NDAYS; ++day) {
        const miutil::miTime tN(2011, 10, 1 + day, 6, 0, 0);
        for(int i=0; neighborIDs[i]>0; ++i) {
            dataN.setStation(neighborIDs[i])
                .add(tN, (day == 1 || day==2) ? 5 : 0, "0101000000000000", "");
        }
    }
    ASSERT_NO_THROW(dataN.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 1, 0, 10, 1+NDAYS, 0);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    ASSERT_EQ(2, logs->count());
    EXPECT_EQ(0, logs->find("station 27270 is dry .* before 2011-10-03 06:00:00"));
    EXPECT_EQ(1, logs->find("station 27270 is wet .* before 2011-10-04 06:00:00"));
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, NeighborsLongNonOperationalPeriod)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(44640, 58.9572,  5.73,   72, 0, 'STAVANGER - VAALAND', 1416, 44640, NULL, NULL, NULL, 8, 't', '2008-07-23 00:00:00');"
        << "INSERT INTO station VALUES(44480, 58.6842, 5.9847, 263, 0, 'SAAYLAND I GJESDAL',  NULL, 44480, NULL, NULL, NULL, 9, 't', '1902-01-01 00:00:00');"
        << "INSERT INTO station VALUES(46300, 59.5887,  6.809, 333, 0, 'SULDALSVATN       ',  NULL, 46300, NULL, NULL, NULL, 9, 't', '1895-07-01 00:00:00');"
        << "INSERT INTO station VALUES(46850, 59.5558, 5.9955, 159, 0, 'HUNDSEID I VIKEDAL',  NULL, 46850, NULL, NULL, NULL, 9, 't', '1936-10-01 00:00:00');"
        << "INSERT INTO station VALUES(48090, 59.7927, 5.4318,  35, 0, 'LITLAB� - DALE    ',  NULL, 48090, NULL, NULL, NULL, 9, 't', '1971-08-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));

    {
        // fake Pluviometer data
        DataList data(44640, 105, 4);
        data.add("2010-08-08 00:00:00", 0, "0101000000000000", "")
            .add("2010-08-08 01:00:00", 0, "0101000000000000", "");
        // 5 days non-operational time here
        miutil::miTime t(2010, 8, 13, 4, 0, 0);
        for(int hour=0; hour<3; ++hour, t.addHour(1))
            data.add(t, 0, "0101000000000000", "");
        for(int hour=0; hour<24; ++hour, t.addHour(1)) {
            data.add(t, 0.1, "0101000000000000", "");
            t.addMin(1);
            data.add(t, 0.5, "0101000000000000", "");
            t.addMin(1);
            data.add(t, 0.1, "0101000000000000", "");
            t.addMin(-2);
        }
        for(int hour=0; hour<24; ++hour, t.addHour(1))
            data.add(t, 0, "0101000000000000", "");
        ASSERT_NO_THROW(data.insert(db));
    }
    {
        // fake neighbor data
        const int neighborIDs[] = { 44480, 46300, 48090, 46850, -1 };
        DataList data(neighborIDs[0], 110, 302);
        for(int day=0; day<8; ++day) {
            const miutil::miTime t(2010, 8, 8+day, 6, 0, 0);
            for(int i=0; neighborIDs[i]>0; ++i)
                data.setStation(neighborIDs[i]).add(t, 0, "0101000000000000", "");
        }
        ASSERT_NO_THROW(data.insert(db));
    }

    AlgorithmConfig params;
    std::stringstream config;
    config << "Start_YYYY = 2010\n"
        "Start_MM   =   08\n"
        "Start_DD   =   08\n"
        "Start_hh   =   00\n"
        "End_YYYY   = 2010\n"
        "End_MM     =   08\n"
        "End_DD     =   15\n"
        "End_hh     =   06\n"
        "stations = 0.1:44640\n"
        "sliding_alarms = 2<8.1;3<11.9;5<16.2;10<25.6;15<27.3;20<34.4;30<42.0;45<49.1;60<54.9;90<56.7;180<60.8;360<83.3;720<144.1;1440<159.7\n"
        "ParamId = 105\n";
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);

    ASSERT_EQ(2, logs->count());
    EXPECT_EQ(0, logs->find("ignoring non-operational time for station 44640 between 2010-08-08 02:00:00 and 2010-08-13 03:59:00"));
    EXPECT_EQ(1, logs->find("station 44640 is wet .* while .* neighbors \\([ ,0-9]+\\) are dry .highest=0. in 24h before 2010-08-14 06:00:00"));
}
