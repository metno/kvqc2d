/* -*- c++ -*-
 Kvalobs - Free Quality Control Software for Meteorological Observations

 Copyright (C) 2011 met.no

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

#include "mathutil.h"

namespace Helpers {

bool equal(float a, float b)
{
    return fabs(a - b) < 4 * std::numeric_limits<float>::epsilon();
}

bool equal(double a, double b)
{
    return fabs(a - b) < 4 * std::numeric_limits<double>::epsilon();
}

float round1(float f)
{
    const float factor = 10;
    f *= factor;
    if (f < 0.0f)
        f -= 0.5;
    else
        f += 0.5;
    float ff = 0;
    modff(f, &ff);
    return ff / factor;
}

float round(float f, float factor)
{
    f *= factor;
    if (f < 0.0f)
        f -= 0.5;
    else
        f += 0.5;
    float ff = 0;
    modff(f, &ff);
    return ff / factor;
}

double randNormal()
{
    // very simple and approximate implementation, see https://en.wikipedia.org/wiki/Normal_distribution#Generating_values_from_normal_distribution
    int N = 12;
    double r = 0;
    for (int i = 0; i < N; ++i)
        r += drand48();
    r -= N / 2;
    return r;
}

double randUniform0()
{
    return 2*drand48()-1;
}

} // namespace Helpers
