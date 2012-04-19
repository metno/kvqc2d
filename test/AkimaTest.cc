
#include <gtest/gtest.h>
#include "Akima.h"

TEST(AkimaTest, Linear)
{
    const double xs[6] = { 1, 2, 4, 6, 9, 10 };
    Akima straight;
    for(int i=0; i<6; ++i)
        straight.add(xs[i], xs[i]+1);

    for(double x=1.0; x < 10.0; x += 0.25) {
        const double y = x+1, a = straight.interpolate(x);
        ASSERT_NEAR(y, a, 0.0001) << x << ' ' << y << ' ' << a;
    }
}

TEST(AkimaTest, Parabola)
{
    const double xp[6] = { -3, -1, 0, 1, 2,  3 };
    Akima parabola;
    for(int i=0; i<6; ++i)
        parabola.add(xp[i], xp[i]*xp[i]+1);

    const double tx[12] = { -3, -2.5, -2, -1.5, -1, -0.5, 0, 0.5, 1, 1.5, 2, 2.5 };
    const double ta[12] = { 10, 7.40937, 5.175, 3.35312, 2, 1.2, 1, 1.275, 2, 3.25, 5, 7.25 };

    for(int i=0; i<12; ++i) {
        const double x = tx[i], y = x*x+1, a = parabola.interpolate(x), e = ta[i];
        ASSERT_NEAR(a, e, 0.0001) << "p x=" << x << " y=" << y << " akima=" << a;
    }
}

TEST(AkimaTest, Series)
{
    const double xp[6] = { 0, 1, 2, 4, 5, 6 };
    const double yp[6] = { -10.9, -10.4, -7.8, /* -8.75734, */ -8.0, -7.0, -7.5 };
    Akima akima;
    for(int i=0; i<6; ++i)
        akima.add(xp[i], yp[i]);

    ASSERT_NEAR(-7.84475, akima.interpolate(3), 0.0001);
}

TEST(AkimaTest, Distance)
{
    const double xp[6] = { 0, 1, 2, 4, 5, 6 };
    const double yp[6] = { -10.9, -10.4, -7.8, /* -8.75734, */ -8.0, -7.0, -7.5 };
    Akima akima;
    for(int i=0; i<6; ++i)
        akima.add(xp[i], yp[i]);

    ASSERT_NEAR(1.0, akima.distance(3.0), 0.0001);
    ASSERT_NEAR(0.5, akima.distance(2.5), 0.0001);
    ASSERT_NEAR(0.2, akima.distance(3.8), 0.0001);
}
