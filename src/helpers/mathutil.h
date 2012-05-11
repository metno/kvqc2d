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

#ifndef HELPERS_MATHUTIL_H_
#define HELPERS_MATHUTIL_H_

#include <algorithm>
#include <cmath>
#include <limits>

namespace Helpers {

float round1(float f);
float round(float f, float factor);

bool equal(float a,  float b);
bool equal(double a, double b);

template<class RandAccessIter>
double median(RandAccessIter& begin, RandAccessIter& end)
{
    // improved version of http://stackoverflow.com/questions/1719070/what-is-the-right-approach-when-using-stl-container-for-median-calculation
    if( begin == end )
        return 0;

    std::size_t size = end - begin;
    std::size_t idxM = size/2;
    RandAccessIter itM = begin + idxM;
    std::nth_element(begin, itM, end);

    double median = *itM;
    if( size % 2 != 0 ) {
        // odd number of elements
        begin = itM;
        end = itM + 1;
    } else {
        // even number of elements
        RandAccessIter itM0 = itM-1;
        std::nth_element(begin, itM0, itM);
        median = ( median + *itM0)/2.0;
        begin = end = itM;
    }
    return median;
}

template<class RandAccessIter>
double median(const RandAccessIter& begin, const RandAccessIter& end)
{
    RandAccessIter b = begin, e = end;
    return median(b, e);
}

template<class RandAccessIter>
void quartiles(RandAccessIter begin, RandAccessIter end, double& q1, double& q2, double& q3)
{
    RandAccessIter b = begin, e = end;
    q2 = median(begin, end);
    q1 = median(b, begin);
    q3 = median(end, e);
}

double randNormal();

template<typename T>
void maximize(T& value, const T& by)
{ if( by > value ) value = by; }

template<typename T>
void minimize(T& value, const T& by)
{ if( by < value ) value = by; }

template<typename T>
T limited_value(const T& value, const T& mini, const T& maxi)
{ if( value > maxi ) return maxi; else if( value < mini ) return mini; else return value; }

template<typename T>
void limit_value(T& value, const T& mini, const T& maxi)
{ value = limited_value(value, mini, maxi); }

} // namespace Helpers

#endif /* HELPERS_MATHUTIL_H_ */
