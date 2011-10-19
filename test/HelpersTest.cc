
#include <gtest/gtest.h>
#include "Helpers.h"

TEST(HelpersTest, testEndsWith)
{
    ASSERT_TRUE( Helpers::string_endswith("test.cfg", ".cfg") );
    ASSERT_TRUE( Helpers::string_endswith(".cfg", ".cfg") );
    ASSERT_TRUE( Helpers::string_endswith("falkjfhldakjfh alkdjfhalkhfkhf lkjahdsf  lkjh a.cfg.cfg", ".cfg") );

    ASSERT_FALSE( Helpers::string_endswith("falkjfhldakjfh alkdjfhalkhfkhf lkjahdsf  lkjh a.cfg.config", ".cfg") );
    ASSERT_FALSE( Helpers::string_endswith("test.config", ".cfg") );
    ASSERT_FALSE( Helpers::string_endswith("cfg", ".cfg") );
    ASSERT_FALSE( Helpers::string_endswith("no_cfg", ".cfg") );
    ASSERT_FALSE( Helpers::string_endswith("bad.cfg ", ".cfg") );
    ASSERT_FALSE( Helpers::string_endswith("c", ".cfg") );
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
