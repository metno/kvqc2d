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

#include "FlagUpdate.h"
#include <gtest/gtest.h>

TEST(FlagUpdateTest, NoUpdate)
{
    EXPECT_EQ("0000003000002000", FlagUpdate("___.___.___.___.").apply(kvalobs::kvControlInfo("0000003000002000")).flagstring());
    EXPECT_EQ("77AABBFF00554488", FlagUpdate("___.___.___.___.").apply(kvalobs::kvControlInfo("77AABBFF00554488")).flagstring());
}

TEST(FlagUpdateTest, ParseAndUpdate)
{
    EXPECT_EQ("0000007000002000", FlagUpdate("___.__7.___.___.").apply(kvalobs::kvControlInfo("0000003000002000")).flagstring());
    EXPECT_EQ("77AABBFF00554455", FlagUpdate("___.___.___.__55").apply(kvalobs::kvControlInfo("77AABBFF00554488")).flagstring());

    EXPECT_EQ("0000007000002000", FlagUpdate("fmis=7").apply(kvalobs::kvControlInfo("0000003000002000")).flagstring());
    EXPECT_EQ("77AABBFF00554455", FlagUpdate("fcombi=5,fhqc=5").apply(kvalobs::kvControlInfo("77AABBFF00554488")).flagstring());
}

TEST(FlagUpdateTest, ParseNames)
{
    FlagUpdate fu;
    EXPECT_TRUE(fu.parse("fmis=7"));
    EXPECT_TRUE(fu.parse("fhqc=7,fmis=0"));

    EXPECT_FALSE(fu.parse("fhqc=x"));
    EXPECT_FALSE(fu.parse("hqc=7"));
    EXPECT_FALSE(fu.parse("fhqc=7fmis=[87]"));
}
