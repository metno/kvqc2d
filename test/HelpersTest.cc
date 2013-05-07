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
#include "helpers/Helpers.h"
#include "helpers/mathutil.h"
#include "helpers/stringutil.h"
#include "helpers/timeutil.h"

TEST(HelpersTest, testMapFromList)
{
    std::map<int, float> map;
    ASSERT_TRUE(Helpers::fillMapFromList("1 2, 3 4", map) );
    ASSERT_EQ(2, map.size());
    ASSERT_FLOAT_EQ(2, map[1]);
    ASSERT_FLOAT_EQ(4, map[3]);

    map.clear();
    ASSERT_TRUE(Helpers::fillMapFromList("1 2; 3 4; 5 6", map, ';') );
    ASSERT_EQ(3, map.size());
    ASSERT_FLOAT_EQ(2, map[1]);
    ASSERT_FLOAT_EQ(4, map[3]);
    ASSERT_FLOAT_EQ(6, map[5]);

    map.clear();
    ASSERT_TRUE(Helpers::fillMapFromList("1 2", map, ':') );
    ASSERT_EQ(1, map.size());
    ASSERT_FLOAT_EQ(2, map[1]);

    map.clear();
    ASSERT_FALSE(Helpers::fillMapFromList("fish", map, ':') );
    ASSERT_TRUE(map.empty());

    map.clear();
    ASSERT_FALSE(Helpers::fillMapFromList("2 fish", map, ':') );
    ASSERT_TRUE(map.empty());

    map.clear();
    ASSERT_FALSE(Helpers::fillMapFromList("1 2 3", map, ':') );
    ASSERT_TRUE(map.empty());

    map.clear();
    ASSERT_FALSE(Helpers::fillMapFromList("1 2 : 3 4 :", map, ':') );
    ASSERT_TRUE(map.empty());

    map.clear();
    ASSERT_FALSE(Helpers::fillMapFromList("1 2 : 3 fish", map, ':') );
    ASSERT_TRUE(map.empty());
}

TEST(HelpersTest, Round1)
{
    EXPECT_FLOAT_EQ(28.3f, Helpers::round1(28.34f));
    EXPECT_FLOAT_EQ(28.3f, Helpers::round1(28.25f));

    EXPECT_FLOAT_EQ(-8.3f, Helpers::round1(-8.25f));
    EXPECT_FLOAT_EQ(-8.3f, Helpers::round1(-8.31f));
}

TEST(HelpersTest, Round)
{
    EXPECT_FLOAT_EQ(28.00f, Helpers::round(28.341f, 1));
    EXPECT_FLOAT_EQ(28.25f, Helpers::round(28.253f, 100));

    EXPECT_FLOAT_EQ(-8.16f, Helpers::round(-8.155f, 100));
    EXPECT_FLOAT_EQ(-8.30f, Helpers::round(-8.311f, 10));
}

TEST(HelpersTest, NormalisedDayOfYear)
{
    EXPECT_EQ(1, Helpers::normalisedDayOfYear(kvtime::makedate(1900, 01, 01)));
    EXPECT_EQ(1, Helpers::normalisedDayOfYear(kvtime::makedate(2000, 01, 01)));
    EXPECT_EQ(1, Helpers::normalisedDayOfYear(kvtime::makedate(2011, 01, 01)));
    EXPECT_EQ(1, Helpers::normalisedDayOfYear(kvtime::makedate(2012, 01, 01)));


    EXPECT_EQ(59, Helpers::normalisedDayOfYear(kvtime::makedate(1900, 02, 28)));

    EXPECT_EQ(59, Helpers::normalisedDayOfYear(kvtime::makedate(2000, 02, 28)));
    EXPECT_EQ(59, Helpers::normalisedDayOfYear(kvtime::makedate(2000, 02, 29)));

    EXPECT_EQ(59, Helpers::normalisedDayOfYear(kvtime::makedate(2011, 02, 28)));

    EXPECT_EQ(59, Helpers::normalisedDayOfYear(kvtime::makedate(2012, 02, 28)));
    EXPECT_EQ(59, Helpers::normalisedDayOfYear(kvtime::makedate(2012, 02, 29)));


    EXPECT_EQ(60, Helpers::normalisedDayOfYear(kvtime::makedate(1900, 03, 01)));
    EXPECT_EQ(60, Helpers::normalisedDayOfYear(kvtime::makedate(2000, 03, 01)));
    EXPECT_EQ(60, Helpers::normalisedDayOfYear(kvtime::makedate(2011, 03, 01)));
    EXPECT_EQ(60, Helpers::normalisedDayOfYear(kvtime::makedate(2012, 03, 01)));


    EXPECT_EQ(365, Helpers::normalisedDayOfYear(kvtime::makedate(1900, 12, 31)));
    EXPECT_EQ(365, Helpers::normalisedDayOfYear(kvtime::makedate(2000, 12, 31)));
    EXPECT_EQ(365, Helpers::normalisedDayOfYear(kvtime::makedate(2011, 12, 31)));
    EXPECT_EQ(365, Helpers::normalisedDayOfYear(kvtime::makedate(2012, 12, 31)));
}

TEST(HelpersTest, Median)
{
    int example7[7] = { 11,13,16,17,19,22,23 };
    EXPECT_DOUBLE_EQ(17, Helpers::median(example7+0, example7+7));
}

TEST(HelpersTest, Quartiles)
{
    double q1, q2, q3;

    int example7[7] = { 11,13,16,17,19,22,23 };
    Helpers::quartiles(example7, example7+7, q1, q2, q3);
    EXPECT_DOUBLE_EQ(13, q1);
    EXPECT_DOUBLE_EQ(17, q2);
    EXPECT_DOUBLE_EQ(22, q3);

    int example8[8] = { 11,14, 16,17, 19,22, 24,27 };
    Helpers::quartiles(example8, example8+8, q1, q2, q3);
    EXPECT_DOUBLE_EQ(15, q1);
    EXPECT_DOUBLE_EQ(18, q2);
    EXPECT_DOUBLE_EQ(23, q3);

    int example9[9] = { 11,14, 16,17,19,22,23, 27,28 };
    Helpers::quartiles(example9, example9+9, q1, q2, q3);
    EXPECT_DOUBLE_EQ(15, q1);
    EXPECT_DOUBLE_EQ(19, q2);
    EXPECT_DOUBLE_EQ(25, q3);

    int example10[10] = { 11,13,16,17,19, 21,23,27,28,30 };
    Helpers::quartiles(example10, example10+10, q1, q2, q3);
    EXPECT_DOUBLE_EQ(16, q1);
    EXPECT_DOUBLE_EQ(20, q2);
    EXPECT_DOUBLE_EQ(27, q3);
}

TEST(HelpersTest, DataText)
{
    const kvalobs::kvData d(18700, kvtime::maketime("2012-03-01 06:00:00"), 12.0, 211, kvtime::maketime("2012-03-01 07:00:00"), 302, 0, 0, 12.0,
                            kvalobs::kvControlInfo("0110000000001000"), kvalobs::kvUseInfo("0000000000000000"), "");
    EXPECT_EQ("[stationid=18700 AND obstime='2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              Helpers::datatext(d));
    EXPECT_EQ("[stationid=18700 AND obstime BETWEEN '2012-02-29 06:00:00' AND '2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              Helpers::datatext(d, 24));
    EXPECT_EQ("[stationid=18700 AND obstime BETWEEN '2012-02-25 06:00:00' AND '2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
            Helpers::datatext(d, kvtime::maketime("2012-02-25 06:00:00")));
}

TEST(HelpersTest, Split2)
{
    const Helpers::split2_t sp1 = Helpers::split2("1<2", "<");
    EXPECT_EQ("1", sp1.first );
    EXPECT_EQ("2", sp1.second);

    const Helpers::split2_t sp2 = Helpers::split2("17 != 16 ", "!=", true);
    EXPECT_EQ("17", sp2.first );
    EXPECT_EQ("16", sp2.second);

    ASSERT_THROW(Helpers::split2("17 != 16 ", "hehehe"), std::runtime_error);
}

TEST(HelpersTest, SplitN)
{
    {
        const Helpers::splitN_t sp = Helpers::splitN("1<2<3<4", "<");
        ASSERT_EQ(4, sp.size());
        EXPECT_EQ("1", sp[0] );
        EXPECT_EQ("2", sp[1] );
        EXPECT_EQ("3", sp[2] );
        EXPECT_EQ("4", sp[3] );
    }
    {
        const Helpers::splitN_t sp = Helpers::splitN("17 !=", "!=", true);
        ASSERT_EQ(2, sp.size());
        EXPECT_EQ("17", sp[0] );
        EXPECT_EQ("",   sp[1]);
    }
    {
        const Helpers::splitN_t sp = Helpers::splitN(" 17 !=    ", "!=", true);
        ASSERT_EQ(2, sp.size());
        EXPECT_EQ("17", sp[0] );
        EXPECT_EQ("",   sp[1]);
    }
    {
        const Helpers::splitN_t sp = Helpers::splitN(" 17 = ", "!=", true);
        ASSERT_EQ(1, sp.size());
        EXPECT_EQ("17 =", sp[0]);
    }
}

TEST(TimeTest, Iso)
{
    EXPECT_EQ("2012-02-26", kvtime::iso(kvtime::makedate(2012, 2, 26)));
    EXPECT_EQ("2013-01-12 05:58:32", kvtime::iso(kvtime::maketime(2013, 1, 12, 5, 58, 32)));
}
