/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  Copyright (C) 2012 met.no

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
#include "AlgorithmHelpers.h"
#include "algorithms/GapInterpolationAlgorithm.h"
#include "DBConstraints.h"

namespace C = Constraint;

class GapInterpolationTest : public AlgorithmTestBase {
public:
    void SetUp();
};

void GapInterpolationTest::SetUp()
{
    algo = new GapInterpolationAlgorithm();
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES(69150, 63.488200, 10.879500, 40, 0, 'KVITHAMAR', 69150, NULL, NULL, NULL, NULL, 2, 't', '1987-05-12 00:00:00');";

    ASSERT_NO_THROW(db->exec(sql.str()));
}

TEST_F(GapInterpolationTest, test1)
{
    DataList data(68290, 211, 330);
    data.add("2012-03-25 18:00:00",      3.9,      3.9, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      3.6,      3.6, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      2.6,      2.6, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      3.0,      3.0, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      2.1,      2.1, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      0.8,      0.8, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      1.3,      1.3, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.1,      1.1, "0111000000100010", "");
    data.setStation(69100)
        .add("2012-03-25 18:00:00",      5.4,      5.4, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      5.9,      5.9, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      6.2,      6.2, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      5.9,      5.9, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      5.4,      5.4, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      5.2,      5.2, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      4.7,      4.7, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      4.1,      4.1, "0111000000100010", "");
    data.setStation(69150)
        .add("2012-03-25 18:00:00",      5.9,      5.9, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      5.7,      5.7, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      6.1,      6.1, "0111000000100010", "")
        .add("2012-03-25 21:00:00", -32767.0, -32767.0, "0000003000000000", "")
        .add("2012-03-25 22:00:00", -32767.0, -32767.0, "0000003000000000", "")
        .add("2012-03-25 23:00:00",      4.7,      4.7, "0110000000100010", "")
        .add("2012-03-26 00:00:00",      4.2,      4.2, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      3.9,      3.9, "0111000000100010", "");
    data.setStation(69380)
        .add("2012-03-25 18:00:00",      4.8,      4.8, "0111000000100010", "")
        .add("2012-03-25 19:00:00",      4.6,      4.6, "0111000000100010", "")
        .add("2012-03-25 20:00:00",      5.0,      5.0, "0111000000100010", "")
        .add("2012-03-25 21:00:00",      4.5,      4.5, "0111000000100010", "")
        .add("2012-03-25 22:00:00",      4.1,      4.1, "0111000000100010", "")
        .add("2012-03-25 23:00:00",      3.8,      3.8, "0111000000100010", "")
        .add("2012-03-26 00:00:00",      2.6,      2.6, "0111000000100010", "")
        .add("2012-03-26 01:00:00",      1.8,      1.8, "0111000000100010", "");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =   03\n"
           << "Start_DD   =   25\n"
           << "Start_hh   =   18\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =   03\n"
           << "End_DD     =   26\n"
           << "End_hh     =   01\n"
           << "TypeId     =  330\n"
           << "ParamId    =  211\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 2);
    logs->dump();

    ASSERT_RUN(algo, bc, 0);
}
