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
  std::cout << "npoints " << npoints << std::endl;

  for (int j=0; j<xt.size();++j){
     tt[j]=xt[j];
     pp[j]=yt[j];
     std::cout << tt[j] << " " << pp[j] << std::endl;
  }

  //gsl_interp_accel *acc = gsl_interp_accel_alloc ();
  //gsl_spline *spline = gsl_spline_alloc (gsl_interp_akima, npoints);
  //gsl_spline_init (spline, tt, pp, npoints);

 //double xx,yy;

 //for (int i = 0; i < npoints; i++)  {
     //xx = tt[i]; 
     //yy = gsl_spline_eval(spline, xx, acc);
	 //std::cout << xx << " " << yy << " " << tt[i] << " " << pp[i] << std::endl; //This is just a test for now
  //}
     //xx=tt[npoints-1]+1.0;
     //yy = gsl_spline_eval(spline, xx, acc);
	 //std::cout << xx << " " << yy << std::endl; //This is just a test for now

}

double AkimaSpline::AkimaPoint(double xp)
{
  double yp;
  gsl_interp_accel *acc = gsl_interp_accel_alloc ();
  gsl_spline *spline = gsl_spline_alloc (gsl_interp_akima, npoints);
  gsl_spline_init (spline, tt, pp, npoints);

  yp = gsl_spline_eval(spline, xp, acc);

  gsl_spline_free (spline);
  gsl_interp_accel_free (acc);

return yp;
}

int AkimaSpline::AkimaPoints()
{
 double xi,yi;
 gsl_interp_accel *acc = gsl_interp_accel_alloc ();
 gsl_spline *spline = gsl_spline_alloc (gsl_interp_akima, npoints);
 gsl_spline_init (spline, tt, pp, npoints);

 for (int i = 0; i < npoints; i++)  {
     xi = tt[i]; 
     yi = gsl_spline_eval(spline, xi, acc);
	 std::cout << xi << " " << yi << " " << tt[i] << " " << pp[i] << std::endl; 
  }

  gsl_spline_free (spline);
  gsl_interp_accel_free (acc);

return 0;
}
