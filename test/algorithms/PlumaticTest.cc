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
#include "algorithms/PlumaticAlgorithm.h"
#include "AlgorithmHelpers.h"
#include "foreach.h"

#include <algorithm>
#include <numeric>

class PlumaticTest : public AlgorithmTestBase {
public:
    void SetUp();
    void TearDown();
    void InsertRealData();
    void Configure(AlgorithmConfig& params, int bm=10, int bd=1, int bh=12, int em=11, int ed=10, int eh=12);
protected:
    PlumaticAlgorithm* algo;
};

// ========================================================================

void PlumaticTest::SetUp()
{
    AlgorithmTestBase::SetUp();
    algo = new PlumaticAlgorithm();
    algo->setDatabase(db);
    algo->setBroadcaster(bc);

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

void PlumaticTest::TearDown()
{
    delete algo;
    AlgorithmTestBase::TearDown();
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
        "highstart_flagchange       = fs=A\n"
        "highsingle_flagchange      = fs=B\n"
        "interruptedrain_flagchange = fs=C\n"
        "aggregation_flagchange     = fr=9\n"
        "discarded_cflags           = fr=9|fs=[ABC]\n"
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:01:00", "010B000000000000", "QC2h-1-highsingle", bc->updates()[0]);
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:39:00", "010B000000000000", "QC2h-1-highsingle", bc->updates()[1]);

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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
    
    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(1, bc->count());
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:00:00", "010B000000000000", "QC2h-1-highsingle", bc->updates()[0]);

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());

    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:43:00", "010A000000000000", "QC2h-1-highstart", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:01:00", "010A000000000000", "QC2h-1-highstart", bc->update(1));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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
    
    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(1, bc->count());
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:00:00", "010A000000000000", "QC2h-1-highstart", bc->update(0));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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
        .add("2011-10-02 00:16:00", 0.2, "0101000000000000", "")
        
        .add("2011-10-02 01:00:00", 0,   "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);
    
    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());

    std::list<kvalobs::kvData> series;
    ASSERT_NO_THROW(db->selectData(series, "WHERE obstime BETWEEN '2011-10-01 23:57:00' AND '2011-10-02 00:03:00';"));
    ASSERT_EQ(7, series.size());

    ASSERT_EQ(1+3+3+1, bc->count());
    // unusual time ordering due to update/insert (inserts first, then updates)
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:20:00", "000C000000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:21:00", "000C000000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(1));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 22:22:00", "000C000000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(2));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:59:00", "000C000000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(3));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-02 00:01:00", "000C000000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(4));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 21:59:00", "010A000000000000", "QC2h-1-highstart",                       bc->update(5));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-02 00:00:00", "010C000000000000", "QC2h-1-interruptedrain",                 bc->update(6));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-02 00:15:00", "010A000000000000", "QC2h-1-highstart",                       bc->update(7));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, RainInterruptStartEnd)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.3, "0101000000000000", "")
        // rain interruption at start, too short => high start at 12:00 and 12:03
        .add("2011-10-01 12:03:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 12:04:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "")
        
        .add("2011-10-01 13:58:00", 0.1, "0101000000000000", "")
        .add("2011-10-01 13:59:00", 0.4, "0101000000000000", "")
        .add("2011-10-01 14:00:00", 0.0, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params, 10, 1, 12, 11, 1, 14);
    
    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(2, bc->count());
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:00:00", "010B000000000000", "QC2h-1-highsingle", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 12:03:00", "010A000000000000", "QC2h-1-highstart",  bc->update(1));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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
        .add("2011-10-01 23:07:00", 0.4, "0101000000000000", "")
        
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(5, bc->count());

    // unusual time ordering due to update/insert (inserts first, then updates)
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:30:00", "000C000000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(0));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:31:00", "000C000000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(1));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:32:00", "000C000000000000", "QC2-missing-row,QC2h-1-interruptedrain", bc->update(2));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:07:00", "010B000000000000", "QC2h-1-highsingle",                      bc->update(3));
    EXPECT_OBS_CONTROL_CFAILED("2011-10-01 23:11:00", "010A000000000000", "QC2h-1-highstart",                       bc->update(4));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(3, bc->count());

    miutil::miTime t("2011-10-01 12:03:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1))
        EXPECT_OBS_CONTROL_CFAILED(t, "0901000000000000", "QC2h-1-aggregation-2", bc->update(i));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    EXPECT_EQ(3, bc->count());

    miutil::miTime t("2011-10-01 12:04:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1)) {
        const kvalobs::kvData& u = bc->updates()[i];
        EXPECT_EQ(t, u.obstime());
        EXPECT_EQ("0901000000000000", u.controlinfo().flagstring());
        EXPECT_FALSE(std::string::npos == u.cfailed().find("QC2h-1-aggregation-3"));
    }

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, Aggregation3Not2B)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        .add("2011-10-01 12:03:00", 0.1, "0101000000000000", "")
        // sliding sum of two three high for the next three
        .add("2011-10-01 12:04:00", 3.0, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 5.0, "0101000000000000", "")
        .add("2011-10-01 12:06:00", 3.0, "0101000000000000", "")

        .add("2011-10-01 12:07:00", 0.1, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    EXPECT_EQ(3, bc->count());

    miutil::miTime t("2011-10-01 12:04:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1))
        EXPECT_OBS_CONTROL_CFAILED(t, "0901000000000000", "QC2h-1-aggregation-3", bc->update(i));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(3, bc->count());

    miutil::miTime t("2011-10-01 12:04:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1))
        EXPECT_OBS_CONTROL_CFAILED(t, "0901000000000000", "QC2h-1-aggregation-2", bc->update(i));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(1, bc->count());
    ASSERT_EQ("2011-10-01 12:03:00", bc->updates()[0].obstime().isoTime());
    ASSERT_EQ("010A000000000000", bc->updates()[0].controlinfo().flagstring());
    ASSERT_TRUE(bc->updates()[0].cfailed().find("QC2h-1-highstart") != std::string::npos);

    // remove high start, and run again
    bc->clear();
    ASSERT_NO_THROW(data.add("2011-10-01 12:03:00", 0.2, "0101000000000000", "").update(db));

    ASSERT_NO_THROW(algo->run());
    EXPECT_EQ(2, bc->count());

    miutil::miTime t("2011-10-01 12:03:00");
    for(int i=0; i<bc->count(); ++i, t.addMin(1))
        EXPECT_OBS_CONTROL_CFAILED(t, "0901000000000000", "QC2h-1-aggregation-2", bc->update(i));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
}

// ------------------------------------------------------------------------

TEST_F(PlumaticTest, AggregationAfterHighStart)
{
    DataList data(27270, 105, 4);
    data.add("2011-10-01 12:00:00", 0.0, "0101000000000000", "")
        // next is high start
        .add("2011-10-01 12:03:00", 0.3, "0101000000000000", "")
        // sliding sum of two too high for the next, but may only mark the next two because of high start
        .add("2011-10-01 12:04:00", 8.2, "0101000000000000", "")
        .add("2011-10-01 12:05:00", 0.3, "0101000000000000", "")

        .add("2011-10-01 13:00:00", 0.1, "0101000000000000", "");
    ASSERT_NO_THROW(data.insert(db));

    AlgorithmConfig params;
    Configure(params);

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(3, bc->count());

    miutil::miTime t("2011-10-01 12:03:00");
    EXPECT_OBS_CONTROL_CFAILED(t, "010A000000000000", "QC2h-1-highstart", bc->update(0));
    t.addMin(1);
    for(int i=1; i<bc->count(); ++i, t.addMin(1))
        EXPECT_OBS_CONTROL_CFAILED(t, "0901000000000000", "QC2h-1-aggregation-2", bc->update(i));

    bc->clear();
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
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

    ASSERT_NO_THROW(algo->configure(params));
    ASSERT_TRUE(params.check()) << params.check().format("; ");
    ASSERT_NO_THROW(algo->run());
    ASSERT_EQ(0, bc->count());
}
