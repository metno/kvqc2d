
#include <gtest/gtest.h>
#include "Helpers.h"

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

TEST(HelpersTest, Round)
{
    EXPECT_FLOAT_EQ(28.3f, Helpers::round(28.34f));
    EXPECT_FLOAT_EQ(28.3f, Helpers::round(28.25f));

    EXPECT_FLOAT_EQ(-8.3f, Helpers::round(-8.25f));
    EXPECT_FLOAT_EQ(-8.3f, Helpers::round(-8.31f));
}

TEST(HelpersTest, NormalisedDayOfYear)
{
    EXPECT_EQ(1, Helpers::normalisedDayOfYear("1900-01-01"));
    EXPECT_EQ(1, Helpers::normalisedDayOfYear("2000-01-01"));
    EXPECT_EQ(1, Helpers::normalisedDayOfYear("2011-01-01"));
    EXPECT_EQ(1, Helpers::normalisedDayOfYear("2012-01-01"));


    EXPECT_EQ(59, Helpers::normalisedDayOfYear("1900-02-28"));

    EXPECT_EQ(59, Helpers::normalisedDayOfYear("2000-02-28"));
    EXPECT_EQ(59, Helpers::normalisedDayOfYear("2000-02-29"));

    EXPECT_EQ(59, Helpers::normalisedDayOfYear("2011-02-28"));

    EXPECT_EQ(59, Helpers::normalisedDayOfYear("2012-02-28"));
    EXPECT_EQ(59, Helpers::normalisedDayOfYear("2012-02-29"));

    
    EXPECT_EQ(60, Helpers::normalisedDayOfYear("1900-03-01"));
    EXPECT_EQ(60, Helpers::normalisedDayOfYear("2000-03-01"));
    EXPECT_EQ(60, Helpers::normalisedDayOfYear("2011-03-01"));
    EXPECT_EQ(60, Helpers::normalisedDayOfYear("2012-03-01"));


    EXPECT_EQ(365, Helpers::normalisedDayOfYear("1900-12-31"));
    EXPECT_EQ(365, Helpers::normalisedDayOfYear("2000-12-31"));
    EXPECT_EQ(365, Helpers::normalisedDayOfYear("2011-12-31"));
    EXPECT_EQ(365, Helpers::normalisedDayOfYear("2012-12-31"));
}
