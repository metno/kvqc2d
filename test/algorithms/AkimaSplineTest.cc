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
#include "helpers/AkimaSpline.h"

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
