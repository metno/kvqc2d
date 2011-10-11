
#include <gtest/gtest.h>
#include "algorithms/AkimaSpline.h"

TEST(AkimaSplineTest, testStraightLine)
{
    const int N = 6;
    const double xValues[N] = { 1, 2, 4, 6,  9, 10 };
    const double yValues[N] = { 2, 3, 5, 7, 10, 11 };
    AkimaSpline straight(std::vector<double>(xValues, xValues+N), std::vector<double>(yValues, yValues+N));

    for(double x=1.0; x < 10.0; x += 0.5) {
        ASSERT_EQ( x+1, straight.AkimaPoint(x) );
    }
}

// ------------------------------------------------------------------------

TEST(AkimaSplineTest, testParabola)
{
    const int N = 6;
    const double xValues[N] = { -3, -1, 0, 1, 2,  3 };
    double yValues[N];
    for(int i=0; i<N; ++i) {
        yValues[i] = xValues[i]*xValues[i]+1;
    }
    AkimaSpline parabola(std::vector<double>(xValues, xValues+N), std::vector<double>(yValues, yValues+N));

    for(double x=-3.0; x < 3.0; x += 0.5) {
        // allow for up to 0.2 deviation from actual parabola
        ASSERT_NEAR( (x*x)+1, parabola.AkimaPoint(x), 0.2 );
    }
}