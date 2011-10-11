/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id$                                                       

  Copyright (C) 2007 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
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

#include "AkimaSpline.h"

using namespace std;

// --------------------------------------------------------------------

AkimaSpline::AkimaSpline(const std::vector<double>& xt, const std::vector<double>& yt)
{
    const int npoints = xt.size();
    double xx[npoints], yy[npoints];
    std::copy(xt.begin(), xt.end(), xx);
    std::copy(yt.begin(), yt.end(), yy);
    acc = gsl_interp_accel_alloc();
    spline = gsl_spline_alloc(gsl_interp_akima, npoints);
    gsl_spline_init(spline, xx, yy, npoints);
}

// --------------------------------------------------------------------

AkimaSpline::~AkimaSpline()
{
    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);
}

// --------------------------------------------------------------------

double AkimaSpline::AkimaPoint(double xp) const
{
    return gsl_spline_eval(spline, xp, acc);
}

// --------------------------------------------------------------------

// int AkimaSpline::AkimaPoints()
// {
//     long nmax=npoints*10;  
// 
//     for (int i = 0; i < nmax +1; i++)  {
//         xi = tt[0] + (tt[npoints-1] - tt[0])*i/nmax; 
//         yi = AkimaPoint(xi);
//     }
// 
//     return 0;
// }
