
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
