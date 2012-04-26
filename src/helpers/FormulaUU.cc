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

#include "FormulaUU.h"

#include <cmath>

namespace Helpers {

// see email Gabriel Kielland, 2012-02-24 10:57
namespace {

const float C20 = 17.5043, C30 = 241.2;

}

const int UU_INVALID = -32767;

/** Calculate dew point from temperature TA and relative humidity UU. */
float formulaTD(float TA, float UU)
{
    if( UU <= 0 || UU >= 100 || TA == UU_INVALID )
        return UU_INVALID;
    float C2, C3;
    if( TA >= 0 ) {
        C2 = C20;
        C3 = C30;
    } else {
        C2 = 22.4433;
        C3 = 272.186;
    }
    const float lu = std::log(UU/100.0);
    const float a = (C20*TA)/(C30+TA);
    return C3*(a + lu)/(C2-a-lu);
}

// ------------------------------------------------------------------------

/** Calculate relative humidity from temperature TA and dew point TD. */
float formulaUU(float TA, float TD)
{
    if( TD == UU_INVALID || TA == UU_INVALID )
        return UU_INVALID;
    return 100*std::exp( (C20*TD)/(C30+TD) - (C20*TA)/(C30+TA) );
}

} // namespace Helpers
