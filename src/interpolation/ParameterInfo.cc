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

#include "ParameterInfo.h"

#include "helpers/stringutil.h"
#include "foreach.h"

#include <cstdlib>

namespace {
const float INVALID = -32767;
}

void BasicParameterInfo::constrain(float& value) const
{
    if( minValue != INVALID && value < minValue )
        value = minValue;
    if( maxValue != INVALID && value > maxValue )
        value = maxValue;
}

// ------------------------------------------------------------------------

BasicParameterInfo::BasicParameterInfo(const std::string& pi)
  : parameter(-1)
  , minValue(INVALID)
  , maxValue(INVALID)
{
    const Helpers::splitN_t items = Helpers::splitN(pi, ",", true);
    foreach(const std::string& item, items) {
        Helpers::split2_t kv = Helpers::split2(item, "=");
        if( kv.first == "par" ) {
            parameter = std::atoi(kv.second.c_str());
        } else if( kv.first == "minVal" ) {
            minValue = std::atof(kv.second.c_str());
        } else if( kv.first == "maxVal" ) {
            maxValue = std::atof(kv.second.c_str());
        }
    }
}

// ------------------------------------------------------------------------

ParameterInfo::ParameterInfo(const std::string& pi)
    : BasicParameterInfo(pi)
    , minParameter(-1)
    , maxParameter(-1)
{
    const Helpers::splitN_t items = Helpers::splitN(pi, ",", true);
    foreach(const std::string& item, items) {
        Helpers::split2_t kv = Helpers::split2(item, "=");
        if( kv.first == "minPar" ) {
            minParameter = std::atoi(kv.second.c_str());
        } else if( kv.first == "maxPar" ) {
            maxParameter = std::atoi(kv.second.c_str());
        } else if( kv.first == "fluctuationLevel" ) {
            fluctuationLevel = std::atof(kv.second.c_str());
        }
    }
}
