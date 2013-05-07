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

#include <gtest/gtest.h>
#include "algorithms/DataUpdate.h"

TEST(DataUpdateTest, Text)
{
    // almost identical to HelpersTest::DataText

    const kvalobs::kvData d(18700, kvtime::maketime("2012-03-01 06:00:00"), 12.0, 211, kvtime::maketime("2012-03-01 07:00:00"), 302, 0, 0, 12.0,
                            kvalobs::kvControlInfo("0110000000001000"), kvalobs::kvUseInfo("0000000000000000"), "");
    const DataUpdate u(d);
    EXPECT_EQ("[stationid=18700 AND obstime='2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              u.text());
    EXPECT_EQ("[stationid=18700 AND obstime='2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              u.text(0));
    EXPECT_EQ("[stationid=18700 AND obstime BETWEEN '2012-02-29 06:00:00' AND '2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              u.text(24));
    EXPECT_EQ("[stationid=18700 AND obstime BETWEEN '2012-02-25 06:00:00' AND '2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
            u.text(kvtime::maketime(2012, 02, 25, 6, 0, 0)));
    
    std::ostringstream msg;
    msg << u;
    EXPECT_EQ("[stationid=18700 AND obstime='2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              msg.str());
}
