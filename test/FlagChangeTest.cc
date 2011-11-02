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

#include "FlagChange.h"
#include <gtest/gtest.h>


TEST(FlagChangeTest, NoMatchNoUpdate)
{
    EXPECT_EQ("0000003000002000", FlagChange("___.___.___.___.").apply(kvalobs::kvControlInfo("0000003000002000")).flagstring());
    EXPECT_EQ("77AABBFF00554488", FlagChange("___.___.___.___.").apply(kvalobs::kvControlInfo("77AABBFF00554488")).flagstring());
}

TEST(FlagChangeTest, NoMatchButUpdate)
{
    EXPECT_EQ("0000007000002000", FlagChange("___.__7.___.___.").apply(kvalobs::kvControlInfo("0000003000002000")).flagstring());
    EXPECT_EQ("77AABBFF00554455", FlagChange("___.___.___.__55").apply(kvalobs::kvControlInfo("77AABBFF00554488")).flagstring());
}

TEST(FlagChangeTest, MatchAndUpdate)
{
    EXPECT_EQ("0000007000002000", FlagChange("___.__3.___.___.->___.__7.___.___.").apply(kvalobs::kvControlInfo("0000003000002000")).flagstring());
    EXPECT_EQ("77AABBFF00554455", FlagChange("___.___.___.__88->___.___.___.__55").apply(kvalobs::kvControlInfo("77AABBFF00554488")).flagstring());
}

TEST(FlagChangeTest, MismatchNoUpdate)
{
    EXPECT_EQ("0000003000002000", FlagChange("___.__5.___.___.->___.__7.___.___.").apply(kvalobs::kvControlInfo("0000003000002000")).flagstring());
    EXPECT_EQ("77AABBFF00554488", FlagChange("___.___.___._5_8->___.___.___.__55").apply(kvalobs::kvControlInfo("77AABBFF00554488")).flagstring());
}

TEST(FlagChangeTest, Parse)
{
    FlagChange fc;

    EXPECT_TRUE(fc.reset().parse("___.__5.___.___.->___.__7.___.___."));
    EXPECT_TRUE(fc.reset().parse("->___.__7.___.___."));
    EXPECT_TRUE(fc.reset().parse("___.__7.___.___."));

    EXPECT_TRUE(fc.reset().parse("___.__5.___.___.->___.__7.___.___.;->___.___.___.___1"));
    EXPECT_TRUE(fc.reset().parse("___.__5.___.___.->___.__7.___.___.;___5___3___1___.->___.___.___.___1"));
    EXPECT_TRUE(fc.reset().parse("___.__5.___.___.->___.__7.___.___.;___.___.___.___1"));

    EXPECT_FALSE(fc.reset().parse("___.__5.___.___.->___.__7.___.___.->"));
    EXPECT_FALSE(fc.reset().parse("___.__5.___.___. ___.__7.___.___.->___.__7.___.___."));
}
