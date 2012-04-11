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

#include "config.h"
#include "FlagPatterns.h"
#include <gtest/gtest.h>

using namespace kvQCFlagTypes;

TEST(FlagPatternsTest, SingleMatch)
{
    FlagPatterns fs;
    fs.add(FlagPattern().permit(f_fd, 2).permit(f_fd, 3).permit(f_fhqc, 0));

    EXPECT_TRUE(fs.matches(kvalobs::kvControlInfo("0000003000002000")));
    EXPECT_TRUE(fs.matches(kvalobs::kvControlInfo("0140004000002000")));

    EXPECT_FALSE(fs.matches(kvalobs::kvControlInfo("0140004000002001")));
}

TEST(FlagPatternsTest, MultiMatch)
{
    FlagPatterns fs;
    ASSERT_TRUE (fs.parse("fcc=[234]|___._[012]_.___._)0123456789ABC(_.", FlagPattern::CONTROLINFO));
    ASSERT_TRUE (fs.matches(kvalobs::kvControlInfo("0020000000000A00")));
    ASSERT_FALSE(fs.matches(kvalobs::kvControlInfo("0010050000000A00")));
    ASSERT_TRUE (fs.matches(kvalobs::kvControlInfo("0010020000000F00")));

    fs.reset();
    ASSERT_TRUE (fs.parse("U2=0", FlagPattern::USEINFO));
    ASSERT_TRUE (fs.matches(kvalobs::kvControlInfo("AA0ABCDEFABCDEF8")));
    ASSERT_FALSE(fs.matches(kvalobs::kvControlInfo("AA1ABCDEFABCDEF8")));

    fs.reset();
    ASSERT_TRUE (fs.parse("", FlagPattern::CONTROLINFO));
    ASSERT_TRUE (fs.matches(kvalobs::kvControlInfo("AA1ABCDEFABCDEF8")));
}

TEST(FlagPatternsTest, SQLtext)
{
    EXPECT_EQ("", FlagPatterns().setDefaultIfEmpty(true).sql("ci"));
    EXPECT_EQ("0=1", FlagPatterns().setDefaultIfEmpty(false).sql("ci"));

    const std::string sql1 = FlagPatterns("__0.___.___.___.", FlagPattern::CONTROLINFO).sql("ci");
    const std::string sql2 = FlagPatterns("__[234].___.___.___.|___._[012]_.___._)0123456789ABC(_.", FlagPattern::CONTROLINFO).sql("ci");
#ifdef HAVE_SQL_WITH_WORKING_SUBSTR_IN
    EXPECT_EQ("substr(ci,3,1) IN ('0')", sql1);
    EXPECT_EQ("(substr(ci,3,1) IN ('2','3','4') OR (substr(ci,6,1) IN ('0','1','2') AND substr(ci,14,1) IN ('D','E','F')))", sql2);
#else /* !HAVE_SQL_WITH_WORKING_SUBSTR_IN */
    EXPECT_EQ("substr(ci,3,1)='0'", sql1);
    EXPECT_EQ("((substr(ci,3,1)='2' OR substr(ci,3,1)='3' OR substr(ci,3,1)='4') OR ((substr(ci,6,1)='0' OR substr(ci,6,1)='1' OR substr(ci,6,1)='2') AND (substr(ci,14,1)='D' OR substr(ci,14,1)='E' OR substr(ci,14,1)='F')))", sql2);
#endif /* !HAVE_SQL_WITH_WORKING_SUBSTR_IN */
}
