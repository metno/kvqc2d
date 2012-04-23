
#include <gtest/gtest.h>
#include "helpers/FormulaUU.h"

#define EXPECT_UU(TA, UU)                                       \
    EXPECT_FLOAT_EQ(UU, formulaUU(TA, formulaTD(TA, UU)))

#define EXPECT_TD(TA, TD)                                       \
    EXPECT_FLOAT_EQ(TD, formulaTD(TA, formulaUU(TA, TD)))

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
