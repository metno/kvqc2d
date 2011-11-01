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
#include "FlagSet.h"

class FlagSetTest : public AlgorithmTestBase {
};

using namespace kvQCFlagTypes;

TEST_F(FlagSetTest, SingleMatch)
{
    FlagSet fs;
    fs.add(FlagMatcher().permit(f_fd, 2).permit(f_fd, 3).permit(f_fhqc, 0));

    EXPECT_TRUE(fs.matches(kvalobs::kvControlInfo("0000003000002000")));
    EXPECT_TRUE(fs.matches(kvalobs::kvControlInfo("0140004000002000")));

    EXPECT_FALSE(fs.matches(kvalobs::kvControlInfo("0140004000002001")));
}

TEST_F(FlagSetTest, MultiMatch)
{
    FlagSet fs;
    ASSERT_TRUE(fs.parse("__[234].___.___.___.|___._[012]_.___._)0123456789ABC(_."));
    ASSERT_TRUE (fs.matches(kvalobs::kvControlInfo("0020000000000A00")));
    ASSERT_FALSE(fs.matches(kvalobs::kvControlInfo("0010050000000A00")));
    ASSERT_TRUE (fs.matches(kvalobs::kvControlInfo("0010020000000F00")));

    ASSERT_TRUE(fs.parse("__0.___.___.___."));
    ASSERT_TRUE (fs.matches(kvalobs::kvControlInfo("AA0ABCDEFABCDEF8")));
    ASSERT_FALSE(fs.matches(kvalobs::kvControlInfo("AA1ABCDEFABCDEF8")));

    ASSERT_TRUE(fs.parse(""));
    ASSERT_TRUE (fs.matches(kvalobs::kvControlInfo("AA1ABCDEFABCDEF8")));
}

TEST_F(FlagSetTest, SQLtext)
{
    EXPECT_EQ("", FlagSet().setDefaultIfEmpty(true).sql("ci"));
    EXPECT_EQ("0=1", FlagSet().setDefaultIfEmpty(false).sql("ci"));
    EXPECT_EQ("substr(ci,3,1) IN ('0')", FlagSet("__0.___.___.___.").sql("ci"));
    EXPECT_EQ("(substr(ci,3,1) IN ('2','3','4') OR (substr(ci,6,1) IN ('0','1','2') AND substr(ci,14,1) IN ('D','E','F')))",
              FlagSet("__[234].___.___.___.|___._[012]_.___._)0123456789ABC(_.").sql("ci"));
}
