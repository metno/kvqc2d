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

AkimaSpline::AkimaSpline(std::vector<double> xt, std::vector<double> yt)
{

  npoints=xt.size();

  for (int j=0; j<xt.size();++j){
     tt[j]=xt[j];
     pp[j]=yt[j];
  }

  gsl_interp_accel *acc = gsl_interp_accel_alloc ();
  gsl_spline *spline = gsl_spline_alloc (gsl_interp_akima, npoints);
  gsl_spline_init (spline, tt, pp, npoints);

}

int AkimaSpline::AkimaPoint(double xp, double yp)
{

  //for (xi = tt[0]; xi <= tt[npoints-1]; xi += 1.0)  {
     //yi = gsl_spline_eval (spline, xi, acc);
  //}
  yp = gsl_spline_eval (spline, xp, acc);

return 0;
}

int AkimaSpline::AkimaPoints()
{

  for (xi = tt[0]; xi <= tt[npoints-1]; xi += 1.0)  {
     yi = gsl_spline_eval (spline, xi, acc);
	 std::cout << xi << " " << yi << std::endl; //This is just a test for now
  }

return 0;
}
