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

#include "algorithms/AlgorithmTestBase.h"
#include "FlagMatch.h"

class FlagMatchTest : public AlgorithmTestBase {
};

using namespace kvQCFlagTypes;

TEST_F(FlagMatchTest, Match)
{
    FlagMatch fm;
    fm.require(f_fd, 2).require(f_fd, 3).require(f_fhqc, 0);

    EXPECT_TRUE(fm.matches(kvalobs::kvControlInfo("0000003000002000")));
    EXPECT_TRUE(fm.matches(kvalobs::kvControlInfo("0140004000002000")));

    EXPECT_FALSE(fm.matches(kvalobs::kvControlInfo("0140004000002001")));
}

TEST_F(FlagMatchTest, SQLtext)
{
    std::ostringstream sql;
    sql << "INSERT INTO station VALUES (180, 61.2944, 12.2719, 360, 0.0, 'TRYSIL VEGSTASJON', 1397, 180, '', '', '', 8, 1, '1993-11-10 00:00:00');";
    ASSERT_TRUE( db->exec(sql.str()) );

    EXPECT_EQ("", FlagMatch().sql("ci"));
    EXPECT_EQ("0=0", FlagMatch().sql("ci", true));
    EXPECT_EQ("substr(ci,13,1) IN ('2','3') AND substr(ci,16,1) IN ('0')",
            FlagMatch().require(f_fd, 2).require(f_fd, 3).require(f_fhqc, 0).sql("ci"));
    EXPECT_EQ("substr(controlinfo,7,1) IN ('0','1','2','3','5','6','7','8','9','A','B','C','D','E','F') AND substr(controlinfo,16,1) IN ('0')",
            FlagMatch().exclude(f_fmis, 4).require(f_fhqc, 0).sql("controlinfo"));
}

TEST_F(FlagMatchTest, SQLquery)
{
    std::ostringstream sql;
    sql << "INSERT INTO data VALUES (83880, '2011-10-10 06:00:00',   16.9, 110, '2011-10-10 09:01:31', 302, 0, 0,   16.9, '0140004000002001', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-11 06:00:00',     -1, 110, '2011-10-11 06:03:01', 302, 0, 0,     -1, '0110000000001000', '7000000000000000', '');"
        << "INSERT INTO data VALUES (83880, '2011-10-12 06:00:00',    0.3, 110, '2011-10-12 05:11:04', 302, 0, 0,    0.3, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-13 06:00:00',    6.5, 110, '2011-10-13 05:04:36', 302, 0, 0,    6.5, '0140000000001000', '7020400000000001', 'QC1-2-72.b12');"
        << "INSERT INTO data VALUES (83880, '2011-10-14 06:00:00', -32767, 110, '2011-10-15 00:35:29', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-15 06:00:00', -32767, 110, '2011-10-16 00:35:40', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-16 06:00:00', -32767, 110, '2011-10-17 00:30:56', 302, 0, 0, -32767, '0000003000002000', '7899900000000000', 'QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-17 06:00:00',   38.3, 110, '2011-10-17 09:11:19', 302, 0, 0,   38.3, '0140004000002000', '7330900000000001', 'QC1-2-72.b12,QC1-7-110');"
        << "INSERT INTO data VALUES (83880, '2011-10-18 06:00:00', -32767, 110, '2011-10-19 00:31:53', 302, 0, 0, -32767, '0000003000000000', '9999900000000000', '');"
        << "INSERT INTO data VALUES (83880, '2011-10-19 06:00:00',    0.6, 110, '2011-10-19 06:11:12', 302, 0, 0,    0.6, '0140000000000000', '7020400000000001', 'QC1-2-72.b12');";
    ASSERT_TRUE( db->exec(sql.str()) );

    std::list<kvalobs::kvData> series;
    ASSERT_TRUE( db->selectData(series, "WHERE " + FlagMatch().require(f_fd, 2).require(f_fd, 3).require(f_fhqc, 0).sql(true)) );
    EXPECT_EQ(4, series.size());

    ASSERT_TRUE( db->selectData(series, "WHERE " + FlagMatch().sql( true)) );
    EXPECT_EQ(10, series.size());

    ASSERT_TRUE( db->selectData(series, "WHERE " + FlagMatch().exclude(f_fcc, 4).sql(true)) );
    EXPECT_EQ(5, series.size());

    ASSERT_TRUE( db->selectData(series, "WHERE " + FlagMatch().require(f_fcc, 1).sql(true)) );
    EXPECT_EQ(1, series.size());
}
