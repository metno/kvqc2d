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

#ifndef KVALOBSPARAMETERINFO_H_
#define KVALOBSPARAMETERINFO_H_

#include <string>

struct ParameterInfo {
    ParameterInfo(const std::string& info);

    /** observed parameter id */
    int parameter;

    /** minimum physical value */
    float minValue;

    /** maximum physical value */
    float maxValue;

    /** maximum offset for neighbor correction */
    float maxOffset;

    /** maximum sigma for neighbors in neighbor correction */
    float maxSigma;

    /** return value forced inside physical value range */
    float constrained(float value) const
        { constrain(value); return value; }

    void constrain(float& value) const;

    bool hasNumerical(float value) const;

    float toNumerical(float value) const;

    float toStorage(float value) const;

    /** parameter id for minimum of 'parameter' between observations */
    int minParameter;

    /** parameter id for maximum of 'parameter' between observations */
    int maxParameter;

    /** approximate fluctuation of the parameter while measuring min
     * and max between two observations */
    float fluctuationLevel;
};

#endif /* KVALOBSPARAMETERINFO_H_ */
