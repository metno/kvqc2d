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
#include "FlagMatcher.h"
#include "config.h"

class FlagMatcherTest : public AlgorithmTestBase {
};

using namespace kvQCFlagTypes;

TEST_F(FlagMatcherTest, Match)
{
    FlagMatcher fm;
    fm.permit(f_fd, 2).permit(f_fd, 3).permit(f_fhqc, 0);

    EXPECT_TRUE(fm.matches(kvalobs::kvControlInfo("0000003000002000")));
    EXPECT_TRUE(fm.matches(kvalobs::kvControlInfo("0140004000002000")));

    EXPECT_FALSE(fm.matches(kvalobs::kvControlInfo("0140004000002001")));
}

TEST_F(FlagMatcherTest, Is)
{
    FlagMatcher fm;
    fm.permit(f_fd, 2).permit(f_fd, 3).permit(f_fhqc, 0).forbid(f_fhqc, 1);

    EXPECT_TRUE(fm.isPermitted(f_fhqc, 0));
    EXPECT_TRUE(fm.isForbidden(f_fhqc, 1));
    EXPECT_TRUE(fm.isAllowed(f_fhqc, 0));
    EXPECT_TRUE(fm.isPermitted(f_fd, 2));
    EXPECT_TRUE(fm.isPermitted(f_fd, 3));

    EXPECT_FALSE(fm.isAllowed(f_fd, 1));
    EXPECT_FALSE(fm.isForbidden(f_fd, 1));
    EXPECT_FALSE(fm.isPermitted(f_fd, 1));
}

TEST_F(FlagMatcherTest, Parse)
{
    FlagMatcher fm("___.___.___.[23]__0");

    EXPECT_TRUE(fm.isAllowed(f_fhqc, 0));
    EXPECT_TRUE(fm.isAllowed(f_fd, 3));

    EXPECT_FALSE(fm.isAllowed(f_fhqc, 1));
    EXPECT_FALSE(fm.isAllowed(f_fd, 1));
    EXPECT_FALSE(fm.isAllowed(f_fd, 4));
}

TEST_F(FlagMatcherTest, SQLtext)
{
    EXPECT_EQ("", FlagMatcher().sql("ci"));
    EXPECT_EQ("0=0", FlagMatcher().permit(f_fhqc, 0).reset().sql("ci", true));
    EXPECT_EQ("0=1", FlagMatcher().permit(f_fhqc, 0).permit(f_fmis, 0).forbid(f_fmis, 0).sql("ci", true));

    const std::string sql1 = FlagMatcher().permit(f_fd, 2).permit(f_fd, 3).permit(f_fhqc, 0).sql("ci");
    const std::string sql2 = FlagMatcher().forbid(f_fmis, 4).permit(f_fhqc, 0).sql("controlinfo");
    const std::string sql3 = FlagMatcher().permit(f_fd, 2).permit(f_fd, 3).sql("ci");
#ifdef HAVE_SQL_WITH_WORKING_SUBSTR_IN
    EXPECT_EQ("(substr(ci,13,1) IN ('2','3') AND substr(ci,16,1) IN ('0'))", sql1);
    EXPECT_EQ("(substr(controlinfo,7,1) NOT IN ('4') AND substr(controlinfo,16,1) IN ('0'))", sql2);
    EXPECT_EQ("substr(ci,13,1) IN ('2','3')", sql3);
#else
    EXPECT_EQ("((substr(ci,13,1)='2' OR substr(ci,13,1)='3') AND substr(ci,16,1)='0')", sql1);
    EXPECT_EQ("((substr(controlinfo,7,1)='0' OR substr(controlinfo,7,1)='1' OR substr(controlinfo,7,1)='2' OR substr(controlinfo,7,1)='3'"
              " OR substr(controlinfo,7,1)='5' OR substr(controlinfo,7,1)='6' OR substr(controlinfo,7,1)='7' OR substr(controlinfo,7,1)='8'"
              " OR substr(controlinfo,7,1)='9' OR substr(controlinfo,7,1)='A' OR substr(controlinfo,7,1)='B' OR substr(controlinfo,7,1)='C'"
              " OR substr(controlinfo,7,1)='D' OR substr(controlinfo,7,1)='E' OR substr(controlinfo,7,1)='F') AND substr(controlinfo,16,1)='0')", sql2);
    EXPECT_EQ("(substr(ci,13,1)='2' OR substr(ci,13,1)='3')", sql3);
#endif
}

TEST_F(FlagMatcherTest, SQLquery)
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
    ASSERT_NO_THROW(db->exec(sql.str()));

    std::list<kvalobs::kvData> series;

    const std::string sql1 = FlagMatcher().permit(f_fd, 2).permit(f_fd, 3).permit(f_fhqc, 0).sql(true);
    ASSERT_NO_THROW(db->selectData(series, "WHERE " + sql1)) << "sql='" << sql1 << "'";
    EXPECT_EQ(4, series.size()) << "sql='" << sql1 << "'";

    const std::string sql2 = FlagMatcher().sql(true);
    ASSERT_NO_THROW(db->selectData(series, "WHERE " + sql2)) << "sql='" << sql2 << "'";
    EXPECT_EQ(10, series.size()) << "sql='" << sql2 << "'";

    const std::string sql3 = FlagMatcher().forbid(f_fcc, 4).sql(true);
    ASSERT_NO_THROW(db->selectData(series, "WHERE " + sql3)) << "sql='" << sql3 << "'";
    EXPECT_EQ(5, series.size()) << "sql='" << sql3 << "'";

    const std::string sql4 = FlagMatcher().permit(f_fcc, 1).sql(true);
    ASSERT_NO_THROW(db->selectData(series, "WHERE " + sql4)) << "sql='" << sql4 << "'";
    EXPECT_EQ(1, series.size()) << "sql='" << sql4 << "'";
}
