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

#include "AlgorithmTestBase.h"
#include "algorithms/StatisticalMean.h"
#include "foreach.h"

class MemoryNotifier : public Notifier {
public:
    struct Record {
        Message::Level level;
        std::string text;
        Record(Message::Level l, const std::string& t)
            : level(l), text(t) { }
    };        

    int find(const std::string& needle, int start=0) const;

    int size() const
        { return mMessages.size(); }

    int count(Message::Level level) const;

    const std::string& text(int idx) const
        { return mMessages[idx].text; }

    Message::Level level(int idx) const
        { return mMessages[idx].level; }

    void clear()
        { mMessages.clear(); }

    void sendText(Message::Level level, const std::string& message)
        { mMessages.push_back(Record(level, message)); }

    void dump(std::ostream& out);

private:
    std::vector<Record> mMessages;
};

// ------------------------------------------------------------------------

int MemoryNotifier::find(const std::string& needle, int start) const
{
    while(start < size()) {
        if( mMessages[start].text.find(needle) != std::string::npos )
            return start;
        start += 1;
    }
    return -1;
}

// ------------------------------------------------------------------------

int MemoryNotifier::count(Message::Level level) const
{
    int c = 0;
    foreach(const Record& r, mMessages) {
        if( r.level == level )
            c += 1;
    }
    return c;
}

// ------------------------------------------------------------------------

void MemoryNotifier::dump(std::ostream& out)
{
    const char* levels[] = {
        "DEBUG   ",
        "INFO    ",
        "WARNING ",
        "ERROR   ",
        "FATAL   "
    };
    for(int i=0; i<size(); ++i)
        out << levels[mMessages[i].level] << '\'' << mMessages[i].text << "\'\n";
}

// ========================================================================

class StatisticalMeanTest : public AlgorithmTestBase {
public:
    void SetUp();
    void TearDown();
    void Configure(AlgorithmConfig& params, int startDay, int endDay);
    void RoundingTest(const float* values, const float* expected, const int N);
protected:
    StatisticalMean* algo;
    MemoryNotifier* logs;
};

// ========================================================================

void StatisticalMeanTest::SetUp()
{
    AlgorithmTestBase::SetUp();
    algo = new StatisticalMean();
    logs = new MemoryNotifier();
    algo->setNotifier(logs);
    algo->setDatabase(db);
    algo->setBroadcaster(bc);

    std::ostringstream sql;
    sql << "INSERT INTO station VALUES( 7010, 61.1592, 11.4425, 240, 0, 'RENA - HAUGEDALEN', 1389,  7010, NULL, NULL, NULL, 8, 't', '1958-01-01 00:00:00');"
        << "INSERT INTO station VALUES(46910, 59.484,   5.7507,  64, 0, 'NEDRE VATS',        1417, 46910, NULL, NULL, NULL, 8, 't', '1969-01-01 00:00:00');"
        << "INSERT INTO station VALUES(70150, 63.7823, 11.6742,  81, 0, 'VERDAL - REPPE',    1278, 70150, NULL, NULL, NULL, 8, 't', '1992-12-01 00:00:00');"
        << "INSERT INTO station VALUES(76450, 65.702,  11.8572,   4, 0, 'VEGA - VALLSJXX',   1108, 76450, NULL, NULL, NULL, 8, 't', '1991-02-01 00:00:00');"
        << "INSERT INTO station VALUES(86500, 68.7003, 15.4168,   3, 0, 'SORTLAND',          1167, 86500, NULL, NULL, NULL, 8, 't', '1985-01-01 00:00:00');"
        << "INSERT INTO station VALUES(93700, 68.9968, 23.0335, 307, 0, 'KAUTOKEINO',        1047, 93700, NULL, NULL, NULL, 8, 't', '1996-07-08 00:00:00');"
        << "INSERT INTO station VALUES(96800, 70.3969, 28.1928,  10, 0, 'RUSTEFJELBMA',      1075, 96800, NULL, NULL, NULL, 8, 't', '1951-01-01 00:00:00');";
    ASSERT_NO_THROW(db->exec(sql.str()));
}

// ------------------------------------------------------------------------

void StatisticalMeanTest::TearDown()
{
    delete algo;
    delete logs;
    AlgorithmTestBase::TearDown();
}

// ------------------------------------------------------------------------
void StatisticalMeanTest::Configure(AlgorithmConfig& params, int startDay, int endDay)
{
    std::stringstream config;
    config << "Start_YYYY = 2012\n"
           << "Start_MM   =    2\n"
           << "Start_DD   =   " << startDay << '\n'
           << "Start_hh   =   06\n"
           << "End_YYYY   = 2012\n"
           << "End_MM     =    3\n"
           << "End_DD     =   " << endDay << '\n'
           << "days = 2\n"
           << "ParamId=178\n"
           << "TypeIds=312\n"
           << "InterpolationDistance=5000.0\n";
    params.Parse(config);
}

// ------------------------------------------------------------------------

TEST_F(StatisticalMeanTest, MiniExample)
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
