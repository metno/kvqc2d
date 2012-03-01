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

#include <boost/version.hpp>
#if BOOST_VERSION >= 104000

#include "AlgorithmTestBase.h"
#include "algorithms/StatisticalMean.h"
#include "foreach.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

class StatisticalMeanTest : public AlgorithmTestBase {
public:
    void SetUp();
};

// ========================================================================

void StatisticalMeanTest::SetUp()
{
    algo = new StatisticalMean();
    AlgorithmTestBase::SetUp();

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES( 7010, 61.1592, 11.4425, 240, 0, 'RENA - HAUGEDALEN', 1389,  7010, NULL, NULL, NULL, 8, 't', '1958-01-01 00:00:00');"
        << "INSERT INTO station VALUES(46910, 59.484,   5.7507,  64, 0, 'NEDRE VATS',        1417, 46910, NULL, NULL, NULL, 8, 't', '1969-01-01 00:00:00');"
        << "INSERT INTO station VALUES(70150, 63.7823, 11.6742,  81, 0, 'VERDAL - REPPE',    1278, 70150, NULL, NULL, NULL, 8, 't', '1992-12-01 00:00:00');"
        << "INSERT INTO station VALUES(76450, 65.702,  11.8572,   4, 0, 'VEGA - VALLSJOE',   1108, 76450, NULL, NULL, NULL, 8, 't', '1991-02-01 00:00:00');"
        << "INSERT INTO station VALUES(86500, 68.7003, 15.4168,   3, 0, 'SORTLAND',          1167, 86500, NULL, NULL, NULL, 8, 't', '1985-01-01 00:00:00');"
        << "INSERT INTO station VALUES(93700, 68.9968, 23.0335, 307, 0, 'KAUTOKEINO',        1047, 93700, NULL, NULL, NULL, 8, 't', '1996-07-08 00:00:00');"
        << "INSERT INTO station VALUES(96800, 70.3969, 28.1928,  10, 0, 'RUSTEFJELBMA',      1075, 96800, NULL, NULL, NULL, 8, 't', '1951-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));
}

// ------------------------------------------------------------------------

TEST_F(StatisticalMeanTest, MiniExample_PR)
{
    DataList data(7010, 178, 312);
    data.add("2012-02-28 00:00:00", 989.1, "0100000000000010")
        .add("2012-02-28 03:00:00", 988.7, "0100000000000010")
        .add("2012-02-28 06:00:00", 988.1, "0100000000000010")
        .add("2012-02-28 12:00:00", 987  , "0100000000000010")
        .add("2012-02-28 18:00:00", 987.7, "0100000000000010")
        .add("2012-02-29 00:00:00", 989.8, "0100000000000010")
        .add("2012-02-29 03:00:00", 990.4, "0100000000000010")
        .add("2012-02-29 06:00:00", 991.7, "0100000000000010")
        .add("2012-02-29 12:00:00", 993.6, "0100000000000010")
        .add("2012-02-29 18:00:00", 995.6, "0100000000000010")
        .add("2012-03-01 00:00:00", 996.6, "0100000000000010")
        .add("2012-03-01 03:00:00", 997  , "0100000000000010")
        .add("2012-03-01 06:00:00", 997  , "0100000000000010")
        .add("2012-03-01 12:00:00", 997.3, "0100000000000010")
        .add("2012-03-01 18:00:00", 995.4, "0100000000000010")
        .setStation(46910)
        .add("2012-02-28 06:00:00",  986.1, "0100000000000010")
        .add("2012-02-28 12:00:00",  985.7, "0100000000000010")
        .add("2012-02-28 18:00:00",  987.8, "0100000000000010")
        .add("2012-02-29 06:00:00",  991.6, "0100000000000010")
        .add("2012-02-29 12:00:00",  992.4, "0100000000000010")
        .add("2012-02-29 18:00:00",  995.7, "0100000000000010")
        .add("2012-03-01 06:00:00",  999.3, "0100000000000010")
        .add("2012-03-01 12:00:00", 1001.2, "0100000000000010")
        .add("2012-03-01 18:00:00", 1002.7, "0100000000000010")
        .setStation(70150)
        .add("2012-02-28 06:00:00", 985.2, "0100000000000010")
        .add("2012-02-28 09:00:00", 985.5, "0100000000000010")
        .add("2012-02-28 12:00:00", 985.4, "0100000000000010")
        .add("2012-02-28 18:00:00", 986.1, "0100000000000010")
        .add("2012-02-29 06:00:00", 989.9, "0100000000000010")
        .add("2012-02-29 09:00:00", 991.3, "0100000000000010")
        .add("2012-02-29 12:00:00", 992.9, "0100000000000010")
        .add("2012-02-29 18:00:00", 996  , "0100000000000010")
        .add("2012-03-01 06:00:00", 998.5, "0100000000000010")
        .add("2012-03-01 09:00:00", 999.1, "0100000000000010")
        .add("2012-03-01 12:00:00", 999  , "0100000000000010")
        .add("2012-03-01 18:00:00", 998.3, "0100000000000010")
        .setStation(76450)
        .add("2012-02-28 06:00:00", 981.5, "0100000000000010")
        .add("2012-02-28 12:00:00", 982.7, "0100000000000010")
        .add("2012-02-28 18:00:00", 984.3, "0100000000000010")
        .add("2012-02-29 06:00:00", 987.6, "0100000000000010")
        .add("2012-02-29 12:00:00", 990.8, "0100000000000010")
        .add("2012-02-29 18:00:00", 994.1, "0100000000000010")
        .add("2012-03-01 06:00:00", 997.6, "0100000000000010")
        .add("2012-03-01 12:00:00", 998.2, "0100000000000010")
        .add("2012-03-01 18:00:00", 997.6, "0100000000000010")
        .setStation(86500)
        .add("2012-02-28 00:00:00",  929.5, "0500000000000020", "QC1-1-178,QC1-9-178")
        .add("2012-02-28 03:00:00",  980.1, "0100000000000010")
        .add("2012-02-28 06:00:00",  980.1, "0100000000000010")
        .add("2012-02-28 09:00:00",  981.4, "0100000000000010")
        .add("2012-02-28 12:00:00",  982.9, "0100000000000010")
        .add("2012-02-28 15:00:00",  983.7, "0100000000000010")
        .add("2012-02-28 18:00:00",  985  , "0100000000000010")
        .add("2012-02-28 21:00:00",  986.1, "0100000000000010")
        .add("2012-02-29 00:00:00",  986.5, "0100000000000010")
        .add("2012-02-29 03:00:00",  987.3, "0100000000000010")
        .add("2012-02-29 06:00:00",  988.4, "0100000000000010")
        .add("2012-02-29 09:00:00",  990.4, "0100000000000010")
        .add("2012-02-29 12:00:00",  991.7, "0100000000000010")
        .add("2012-02-29 15:00:00",  993.2, "0100000000000010")
        .add("2012-02-29 18:00:00",  994.6, "0100000000000010")
        .add("2012-02-29 21:00:00",  996.1, "0100000000000010")
        .add("2012-03-01 00:00:00",  998  , "0100000000000010")
        .add("2012-03-01 03:00:00",  999  , "0100000000000010")
        .add("2012-03-01 06:00:00", 1000.2, "0100000000000010")
        .add("2012-03-01 09:00:00", 1001.6, "0100000000000010")
        .add("2012-03-01 12:00:00", 1001.6, "0100000000000010")
        .add("2012-03-01 15:00:00", 1001.4, "0100000000000010")
        .add("2012-03-01 18:00:00", 1001.7, "0100000000000010")
        .add("2012-03-01 21:00:00", 1000.8, "0100000000000010")
        .setStation(93700)
        .add("2012-02-28 00:00:00",  986.6, "0100000000000010")
        .add("2012-02-28 06:00:00",  985.8, "0100000000000010")
        .add("2012-02-28 09:00:00",  986.1, "0100000000000010")
        .add("2012-02-28 12:00:00",  986.4, "0100000000000010")
        .add("2012-02-28 15:00:00",  987.7, "0100000000000010")
        .add("2012-02-28 18:00:00",  989.6, "0100000000000010")
        .add("2012-02-29 00:00:00",  993.1, "0100000000000010")
        .add("2012-02-29 06:00:00",  993.8, "0100000000000010")
        .add("2012-02-29 12:00:00",  997.3, "0100000000000010")
        .add("2012-02-29 15:00:00",  998.1, "0100000000000010")
        .add("2012-02-29 18:00:00",  999.2, "0100000000000010")
        .add("2012-03-01 00:00:00", 1001.6, "0100000000000010")
        .add("2012-03-01 06:00:00", 1003.3, "0100000000000010")
        .add("2012-03-01 09:00:00", 1004.5, "0100000000000010")
        .add("2012-03-01 12:00:00", 1004.9, "0100000000000010")
        .add("2012-03-01 15:00:00", 1004.7, "0100000000000010")
        .add("2012-03-01 18:00:00", 1004.8, "0100000000000010")
        .setStation(96800)
        .add("2012-02-28 06:00:00",  986.4, "0110000000000010")
        .add("2012-02-28 12:00:00",  987.1, "0110000000000010") // useinfo=7100000400000000
        .add("2012-02-28 18:00:00",  990.2, "0110000000000010")
        .add("2012-02-29 06:00:00",  995.7, "0110000000000010")
        .add("2012-02-29 12:00:00",  996.6, "0110000000000010")
        .add("2012-02-29 18:00:00",  999.8, "0110000000000010")
        .add("2012-03-01 06:00:00", 1004.1, "0110000000000010")
        .add("2012-03-01 12:00:00", 1006.8, "0110000000000010") // useinfo=7100000400000000
        .add("2012-03-01 18:00:00", 1007.4, "0110000000000010");
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    2\n"
           << "Start_DD   =   28\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    3\n"
           << "End_DD     =    1\n"
           << "days       =    2\n"
           << "tolerance  =   10\n"
           << "ParamId    =  178\n"
           << "TypeIds    =  312\n"
           << "InterpolationDistance = 5000.0\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(0, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(StatisticalMeanTest, FakeManyMissing_PR)
{
    const int ctr = 7010;
    DataList data(ctr, 178, 312);
    miutil::miTime date("2012-01-01 06:00:00"), dateEnd("2012-02-29 06:00:00");
    for(; date <= dateEnd; date.addDay(1)) {
        const int m=date.month(), d=date.day();
        if( (m==1 && d>=15 && d<=18) )
            continue;
        data.setStation(ctr)
            .add(date,  970, "0100000000000010")
            .setStation(46910)
            .add(date, 1010, "0100000000000010")
            .setStation(70150)
            .add(date, 1011, "0100000000000010")
            .setStation(76450)
            .add(date, 1010, "0100000000000010");
    }
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    2\n"
           << "Start_DD   =    1\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    2\n"
           << "End_DD     =   29\n"
           << "days       =   30\n"
           << "tolerance  =   10\n"
           << "ParamId    =  178\n"
           << "TypeIds    =  22,311,312,330,342\n"
           << "InterpolationDistance = 5000.0\n";
    AlgorithmConfig params;
    params.Parse(config);

    int ndays = (dateEnd.date() - miutil::miDate("2012-01-17")) - int(30*0.9);
    
    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(ndays, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(StatisticalMeanTest, FakeDeviation_PR)
{
    const int ctr = 7010;
    DataList data(ctr, 178, 312);
    miutil::miTime date("2012-01-01 06:00:00"), dateEnd("2012-02-29 06:00:00");
    for(; date <= dateEnd; date.addDay(1)) {
        const int m=date.month(), d=date.day();
        if( (m==1 && d==15) || (m==2 && d==10) )
            continue;
        data.setStation(ctr)
            .add(date, 970 + ((m==1 && d==3) ? 29 : 0), "0100000000000010")
            .setStation(46910)
            .add(date, 1010, "0100000000000010")
            .setStation(70150)
            .add(date, 1011, "0100000000000010")
            .setStation(76450)
            .add(date, 1010, "0100000000000010");
    }
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    2\n"
           << "Start_DD   =    1\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    2\n"
           << "End_DD     =   29\n"
           << "days       =   30\n"
           << "tolerance  =   10\n"
           << "ParamId    =  178\n"
           << "TypeIds    =  22,311,312,330,342\n"
           << "InterpolationDistance = 5000.0\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(29, logs->count(Message::WARNING));

    for(int day=1, idx=0; day<=29; ++day) {
        idx = logs->next(Message::WARNING, idx);
        ASSERT_LE(0, idx);
        const std::string expect = (boost::format("station %1% for series ending at 2012-02-%2$02d") % ctr % day).str();
        EXPECT_TRUE(boost::algorithm::contains(logs->text(idx), expect))
            << "day=" << day;
        idx += 1;
    }
}

// ------------------------------------------------------------------------

TEST_F(StatisticalMeanTest, FakeDeviation_TA)
{
#include "StatisticalMean_n212.icc"

    const int ctr = 7010;
    DataList data(ctr, 211, 330);
    miutil::miTime date("2012-01-01 06:00:00"), dateEnd("2012-02-29 06:00:00");
    for(; date <= dateEnd; date.addDay(1)) {
        const int m=date.month(), d=date.day();
        if( (m==1 && d==15) || (m==2 && d==10) )
            continue;
        data.setStation(ctr);
        if( m==1 && d==7 )
            data.add(date, -32.1, "0100000000000010");
        else
            data.add(date, -11.2, "0100000000000010");
        data.setStation(46910)
            .add(date, 1.6, "0100000000000010")
            .setStation(70150)
            .add(date, -4.6, "0100000000000010")
            .setStation(76450)
            .add(date, -0.9, "0100000000000010");
    }
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    2\n"
           << "Start_DD   =    1\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    2\n"
           << "End_DD     =    7\n"
           << "days       =   30\n"
           << "tolerance  =   0.2\n"
           << "ParamId    =  211\n"
           << "TypeIds    =  330\n"
           << "InterpolationDistance = 5000.0\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(5, logs->count(Message::WARNING));

    for(int day=1, idx=0; day<=5; ++day) {
        idx = logs->next(Message::WARNING, idx);
        ASSERT_LE(0, idx);
        const std::string expect = (boost::format("station %1% for series ending at 2012-02-%2$02d") % ctr % day).str();
        EXPECT_TRUE(boost::algorithm::contains(logs->text(idx), expect))
            << "day=" << day << " expect='" << expect << "' but is '" << logs->text(idx) << "'";
        idx += 1;
    }
}

// ------------------------------------------------------------------------

TEST_F(StatisticalMeanTest, FakeDeviation_VV)
{
    // sight (synsvidde), completely arbitrary observation and
    // reference values -- made up to test the quartiles method
    
    const float ref_q1 = 100, ref_q2 = 200, ref_q3 = 300;
    const int paramid = 273, idtype = 330, stations[] = { 7010, 46910, 70150, 76450, -1 }, days = 28;
    std::ostringstream sql;
    for(int dOy=0; dOy<=365; ++dOy) {
        for(int s=0; stations[s]>0; ++s) {
            sql << "INSERT INTO statistical_reference_values VALUES("<<stations[s]<<','<<paramid<<','<<dOy<<",'ref_q1',"<<ref_q1<<");"
                << "INSERT INTO statistical_reference_values VALUES("<<stations[s]<<','<<paramid<<','<<dOy<<",'ref_q2',"<<ref_q2<<");"
                << "INSERT INTO statistical_reference_values VALUES("<<stations[s]<<','<<paramid<<','<<dOy<<",'ref_q3',"<<ref_q3<<");";
        }
    }
    ASSERT_NO_THROW(db->exec(sql.str()));
    sql.str("");

    DataList data(stations[0], paramid, idtype);
    miutil::miTime date("2012-01-01 06:00:00"), dateEnd("2012-03-31 06:00:00");
    for(int day=0; date <= dateEnd; date.addDay(1), day += 1) {
        for(int s=0; stations[s]>0; ++s) {
            const float obs = (s == 0) ? (50*(day%4))+125 : (100*(day%4))+50;
            data.setStation(stations[s])
                .add(date, obs, "0100000000000010");
        }
    }
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    2\n"
           << "Start_DD   =    1\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    2\n"
           << "End_DD     =    7\n"
           << "days       =   " << days << "\n"
           << "tolerance  =    1\n"
           << "ParamId    =  " << paramid << '\n'
           << "TypeIds    =  " << idtype << '\n'
           << "InterpolationDistance = 5000.0\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(7, logs->count(Message::WARNING));
}

// ------------------------------------------------------------------------

TEST_F(StatisticalMeanTest, FakeDeviation_RR24)
{
    // 24h-precipitation, completely arbitrary observation and
    // reference values -- made up to test the periodic sum method
    
    const int paramid = 110, idtype = 330, stations[] = { 7010, 46910, 70150, 76450, -1 };
    std::ostringstream sql;
    for(int dOy=0; dOy<=365; ++dOy) {
        for(int s=0; stations[s]>0; ++s)
            sql << "INSERT INTO statistical_reference_values VALUES(" << stations[s] << ',' << paramid << ',' << dOy << ",'ref_value', 30);";
    }
    ASSERT_NO_THROW(db->exec(sql.str()));
    sql.str("");

    DataList data(stations[0], paramid, idtype);
    miutil::miTime date("2012-01-01 06:00:00"), dateEnd("2012-03-31 06:00:00");
    for(int day=0; date <= dateEnd; date.addDay(1), day += 1) {
        for(int s=0; stations[s]>0; ++s) {
            const float obs = (s == 0) ? 2 : 1;
            data.setStation(stations[s])
                .add(date, obs, "0100000000000010");
        }
    }
    ASSERT_NO_THROW(data.insert(db));

    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    2\n"
           << "Start_DD   =    1\n"
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    2\n"
           << "End_DD     =    7\n"
           << "days       =   30\n"
           << "tolerance  =    1\n"
           << "ParamId    =  " << paramid << '\n'
           << "TypeIds    =  " << idtype << '\n'
           << "InterpolationDistance = 5000.0\n";
    AlgorithmConfig params;
    params.Parse(config);

    ASSERT_CONFIGURE(algo, params);
    ASSERT_RUN(algo, bc, 0);
    ASSERT_EQ(7, logs->count(Message::WARNING));
}

#endif /* BOOST_VERSION >= 104000 */

