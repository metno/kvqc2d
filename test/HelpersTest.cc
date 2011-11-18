
#include <gtest/gtest.h>
#include "Helpers.h"

TEST(HelpersTest, endsWith)
{
    ASSERT_TRUE( Helpers::endsWith("test.cfg", ".cfg") );
    ASSERT_TRUE( Helpers::endsWith(".cfg", ".cfg") );
    ASSERT_TRUE( Helpers::endsWith("falkjfhldakjfh alkdjfhalkhfkhf lkjahdsf  lkjh a.cfg.cfg", ".cfg") );

    ASSERT_FALSE( Helpers::endsWith("falkjfhldakjfh alkdjfhalkhfkhf lkjahdsf  lkjh a.cfg.config", ".cfg") );
    ASSERT_FALSE( Helpers::endsWith("test.config", ".cfg") );
    ASSERT_FALSE( Helpers::endsWith("cfg", ".cfg") );
    ASSERT_FALSE( Helpers::endsWith("no_cfg", ".cfg") );
    ASSERT_FALSE( Helpers::endsWith("bad.cfg ", ".cfg") );
    ASSERT_FALSE( Helpers::endsWith("c", ".cfg") );
}

TEST(HelpersTest, startsWith)
{
    ASSERT_TRUE( Helpers::startsWith("hallotest", "hallo") );
    ASSERT_TRUE( Helpers::startsWith("xzy", "xzy") );
    ASSERT_TRUE( Helpers::startsWith("falkjfhldakjfh alkdjfhalkhfkhf lkjahdsf  lkjh a", "falk") );

    ASSERT_FALSE( Helpers::startsWith("falkjfhldakjfh alkdjfhalkhfkhf lkjahdsf  lkjh a.cfg.config", "folk") );
    ASSERT_FALSE( Helpers::startsWith("lost: abcde", "list:") );
    ASSERT_FALSE( Helpers::startsWith("lis", "list") );
    ASSERT_FALSE( Helpers::startsWith("no_start", "start") );
    ASSERT_FALSE( Helpers::startsWith(" list: with space", "list:") );
    ASSERT_FALSE( Helpers::startsWith("l", "list:") );
}

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
