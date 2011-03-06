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
#include "ProcessImpl.h"
#include "BasicStatistics.h"
#include "Qc2App.h"
#include "Qc2Connection.h"
#include "Qc2D.h"
#include "ReadProgramOptions.h"
#include <sstream>
#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>
#include <memory>
#include <stdexcept>

#include "CheckedDataCommandBase.h"
#include "CheckedDataHelper.h"

#include <math.h>
//GNU Statistical library
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>


using namespace kvalobs;
using namespace std;
using namespace miutil;

int AkimaSpline(std::vector<double> tt, std::vector<double>pp, int npoints)
{

  int i, nseries;
  double xi, yi|;
  double tt[100], pp[100]; // only set up for time series; max 100 points, and trap to catch errors ...
  gsl_interp_accel *acc = gsl_interp_accel_alloc ();
  gsl_spline *spline = gsl_spline_alloc (gsl_interp_akima, npoints);
  gsl_spline_init (spline, tt, pp, npoints);
  for (xi = tt[0]; xi <= tt[nseries-1]; xi += 1.0)  {
     yi = gsl_spline_eval (spline, xi, acc);
  }
  gsl_spline_free (spline);
  gsl_interp_accel_free (acc);

  return 0;
}

