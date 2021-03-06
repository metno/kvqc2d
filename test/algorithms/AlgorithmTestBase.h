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

#ifndef ALGORITHMTESTBASE_H_
#define ALGORITHMTESTBASE_H_

#include <gtest/gtest.h>

#include "TestBroadcaster.h"
#include "TestData.h"
#include "TestDB.h"
#include "TestNotifier.h"

class Qc2Algorithm;

// ########################################################################

::testing::AssertionResult AssertObstime(const char* e_expr, const char* /*a_expr*/,
        const kvtime::time& e, const kvalobs::kvData& a);
inline ::testing::AssertionResult AssertObstime(const char* e_expr, const char* a_expr,
        const char* e, const kvalobs::kvData& a)
{ return AssertObstime(e_expr, a_expr, kvtime::maketime(e), a); }

#define EXPECT_OBSTIME(e, a) EXPECT_PRED_FORMAT2(AssertObstime, e, a)
#define ASSERT_OBSTIME(e, a) ASSERT_PRED_FORMAT2(AssertObstime, e, a)

#define EXPECT_CFAILED(e, a) EXPECT_PRED_FORMAT2(::testing::IsSubstring, e, a.cfailed())
#define ASSERT_CFAILED(e, a) EXPECT_PRED_FORMAT2(::testing::IsSubstring, e, a.cfailed())

::testing::AssertionResult AssertObsControlCfailed(const char* eo_expr, const char* eci_expr, const char* ecf_expr, const char* a_expr,
        const kvtime::time& eo, const std::string& eci, const std::string& ecf, const kvalobs::kvData& a);
inline ::testing::AssertionResult AssertObsControlCfailed(const char* eo_expr, const char* eci_expr, const char* ecf_expr, const char* a_expr,
        const char* eo, const std::string& eci, const std::string& ecf, const kvalobs::kvData& a)
{ return AssertObsControlCfailed(eo_expr, eci_expr, ecf_expr, a_expr, kvtime::maketime(eo), eci, ecf, a); }

#define EXPECT_OBS_CONTROL_CFAILED(eo, eci, ecf, a) EXPECT_PRED_FORMAT4(AssertObsControlCfailed, eo, eci, ecf, a)
#define ASSERT_OBS_CONTROL_CFAILED(eo, eci, ecf, a) ASSERT_PRED_FORMAT4(AssertObsControlCfailed, eo, eci, ecf, a)

::testing::AssertionResult AssertStationObsControlCorrected(const char* es_expr, const char* eo_expr, const char* eci_expr, const char* eco_expr, const char* a_expr,
        int es, const kvtime::time& eo, const std::string& eci, float eco, const kvalobs::kvData& a);
inline ::testing::AssertionResult AssertStationObsControlCorrected(const char* es_expr, const char* eo_expr, const char* eci_expr, const char* eco_expr, const char* a_expr,
                                                            int es, const char* eo, const std::string& eci, float eco, const kvalobs::kvData& a)
{ return AssertStationObsControlCorrected(es_expr, eo_expr, eci_expr, eco_expr, a_expr, es, kvtime::maketime(eo), eci, eco, a); }

#define EXPECT_STATION_OBS_CONTROL_CORR(es, eo, eci, eco, a) EXPECT_PRED_FORMAT5(AssertStationObsControlCorrected, es, eo, eci, eco, a)
#define ASSERT_STATION_OBS_CONTROL_CORR(es, eo, eci, eco, a) ASSERT_PRED_FORMAT5(AssertStationObsControlCorrected, es, eo, eci, eco, a)

#define ASSERT_PARSE_CONFIG(params, config)                                     \
    try {                                                                       \
        params.Parse(config);                                                   \
    } catch(ConfigException& e) {                                               \
        FAIL() << "Parse threw ConfigException: " << e.what();                  \
    } catch(ConvertException& e) {                                              \
        FAIL() << "Parse threw ConvertException: " << e.what(); \
    }

#define ASSERT_CONFIGURE(algo, params)                  \
    try {                                               \
        algo->configure(params);                        \
        if( !params.check() )                           \
            FAIL() << params.check().format("; ");      \
    } catch(ConfigException& ce) {                      \
        FAIL() << ce.what();                            \
    } catch(std::exception& e) {                        \
        FAIL() << e.what();                             \
    } catch(...) {                                      \
        FAIL() << "unknown exception";                  \
    }

#define ASSERT_RUN(ALGO, BROADCASTER, COUNT)                            \
    try {                                                               \
        logs->clear();                                                  \
        BROADCASTER->clear();                                           \
        ALGO->run();                                                    \
        if( BROADCASTER->count() != COUNT ) {                           \
            logs->dump();                                               \
            FAIL() << "Expected " << COUNT << ", but got " << BROADCASTER->count() << " updates."; \
        }                                                               \
    } catch(std::exception& e) {                                        \
        FAIL() << "Exception in run(): " << e.what();                   \
    } catch(...) {                                                      \
        FAIL() << "Unknown exception in run()";                         \
    }

// #######################################################################

void INSERT_STATION(std::ostream& sql, int stationid, const std::string& name, float lat, float lon, float height=0);

// #######################################################################

class AlgorithmTestBase: public ::testing::Test {
public:
    AlgorithmTestBase();
    void SetUp();
    void TearDown();

protected:
    Qc2Algorithm* algo;
    SqliteTestDB* db;
    TestBroadcaster* bc;
    TestNotifier* logs;
};

#endif /* ALGORITHMTESTBASE_H_ */
