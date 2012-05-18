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


#include "AlgorithmTestBase.h"
#include "algorithms/AggregatorLimits.h"
#include "helpers/AlgorithmHelpers.h"

#include <boost/algorithm/string/predicate.hpp>

// #define USE_STATION_PARAM 1

class AggregatorLimitsTest : public AlgorithmTestBase {
public:
    void SetUp();
};

void AggregatorLimitsTest::SetUp()
{
    algo = new AggregatorLimits();
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(18700, 59.942, 10.720,  94, 0, 'OSLO - BLINDERN', 1492, 18700, NULL, NULL, NULL, 8, 't', '1937-02-25 00:00:00');\n";

#ifdef USE_STATION_PARAM
    sql << "INSERT INTO station_param VALUES(0,     109, 0, 0, 1, 365, -1, 'QC1-1-109', 'max;highest;high;low;lowest;min\n160;120.0;100.0;-1.0;-1.0;-1', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(18700, 109, 0, 0, 1, 124, 18, 'QC1-1-109', 'max;highest;high;low;lowest;min\n140;120.0;100.0;-1.0;-1.0;-1', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(18700, 109, 0, 0, 1, 125, -1, 'QC1-1-109', 'max;highest;high;low;lowest;min\n150;120.0;100.0;-1.0;-1.0;-1', NULL, '1500-01-01 00:00:00');"
        << "INSERT INTO station_param VALUES(0,     110, 0, 0, 1, 365, -1, 'QC1-1-110', 'max;highest;high;low;lowest;min\n120;120.0;100.0;-1.0;-1.0;-1', NULL, '1500-01-01 00:00:00');";
#endif
    ASSERT_NO_THROW(db->exec(sql.str()));
}

TEST_F(AggregatorLimitsTest, FirstTest)
{
    DataList data(18700, 109, -330);
    data.add("2012-05-04 18:00:00", 145, "1000000000000000")
        .add("2012-05-05 06:00:00",  -2, "1000000000000000")
        .add("2012-05-05 18:00:00", 155, "1000000000000000")
        .add("2012-05-06 06:00:00",   1, "1000000000000000")
        .setType(330)
        .add("2012-05-05 18:00:00", 145, "1000000000000000");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   05\n"
           << "Start_DD   =   04\n"
           << "Start_hh   =   18\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   05\n"
           << "End_DD     =   06\n"
           << "End_hh     =   06\n";
#ifdef USE_STATION_PARAM
    config << "ParamID = 109\n"
           << "ParamID = 110\n";
#else
    config << "param_limits = 106<54.9;107<60.8;108<83.3;109<144.1;110<159.7\n";
#endif
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 3);

    ASSERT_OBS_CONTROL_CFAILED("2012-05-04 18:00:00", "1600000000000000", "QC2-agglim-max", bc->update(0));
    ASSERT_OBS_CONTROL_CFAILED("2012-05-05 06:00:00", "1600000000000000", "QC2-agglim-min", bc->update(1));
    ASSERT_OBS_CONTROL_CFAILED("2012-05-05 18:00:00", "1600000000000000", "QC2-agglim-max", bc->update(2));

    ASSERT_RUN(algo, bc, 0);
}
