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

#include <gtest/gtest.h>
#include "helpers/FormulaUU.h"

#define EXPECT_UU(TA, UU)                                       \
    EXPECT_FLOAT_EQ(UU, Helpers::formulaUU(TA, Helpers::formulaTD(TA, UU)))

#define EXPECT_TD(TA, TD)                                       \
    EXPECT_FLOAT_EQ(TD, Helpers::formulaTD(TA, Helpers::formulaUU(TA, TD)))

TEST(FormulaUUTest, TestUU)
{
    EXPECT_UU(20, 90);
    EXPECT_UU(20, 80);
    EXPECT_UU(20, 70);

    EXPECT_UU(10, 90);
    EXPECT_UU(10, 80);
    EXPECT_UU(10, 70);
}

TEST(FormulaUUTest, TestTD)
{
    EXPECT_TD(20, 10);
    EXPECT_TD(20, 11);
    EXPECT_TD(20, 12);

    EXPECT_TD(10, 7);
    EXPECT_TD(10, 8);
    EXPECT_TD(10, 9);
}
