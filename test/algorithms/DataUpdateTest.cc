
#include <gtest/gtest.h>
#include "algorithms/DataUpdate.h"

TEST(DataUpdateTest, Text)
{
    // almost identical to HelpersTest::DataText

    const kvalobs::kvData d(18700, "2012-03-01 06:00:00", 12.0, 211, "2012-03-01 07:00:00", 302, 0, 0, 12.0,
                            kvalobs::kvControlInfo("0110000000001000"), kvalobs::kvUseInfo("0000000000000000"), "");
    const DataUpdate u(d);
    EXPECT_EQ("[stationid=18700 AND obstime='2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              u.text());
    EXPECT_EQ("[stationid=18700 AND obstime='2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              u.text(0));
    EXPECT_EQ("[stationid=18700 AND obstime BETWEEN '2012-02-29 06:00:00' AND '2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              u.text(24));
    EXPECT_EQ("[stationid=18700 AND obstime BETWEEN '2012-02-25 06:00:00' AND '2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              u.text("2012-02-25 06:00:00"));
    
    std::ostringstream msg;
    msg << u;
    EXPECT_EQ("[stationid=18700 AND obstime='2012-03-01 06:00:00' AND paramid=211 AND typeid=302 AND sensor='0' AND level=0; original=12.0 corr=12.0 controlinfo=0110000000001000 cfailed='']",
              msg.str());
}
