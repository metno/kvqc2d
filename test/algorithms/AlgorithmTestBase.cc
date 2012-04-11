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

#include "Helpers.h"
#include "Qc2Algorithm.h"

// ########################################################################

::testing::AssertionResult AssertObstime(const char* e_expr, const char* /*a_expr*/,
                                         const miutil::miTime& e, const kvalobs::kvData& a)
{
    if( e == a.obstime() )
        return ::testing::AssertionSuccess();
 
    ::testing::Message msg;
    msg << e_expr << " != " << e_expr << " (" << e.isoTime() << " and " << a.obstime().isoTime() << ")";
    return ::testing::AssertionFailure(msg);
}

::testing::AssertionResult AssertObsControlCfailed(const char* eo_expr, const char* eci_expr, const char* ecf_expr, const char* /*a_expr*/,
// ------------------------------------------------------------------------

                                                   const miutil::miTime& eo, const std::string& eci, const std::string& ecf, const kvalobs::kvData& a)
{
    bool failed = false;
    ::testing::Message msg;
    if( eo != a.obstime() ) {
        msg << "(obstime " << eo_expr << " != " << a.obstime().isoTime() << ")";
        failed = true;
    }
    if( eci != a.controlinfo().flagstring() ) {
        if( failed )
            msg << "; ";
        msg << "(controlinfo " << eci_expr << " != " << a.controlinfo().flagstring() << ")";
        failed = true;
    }
    if( a.cfailed().find(ecf) == std::string::npos ) {
        if( failed )
            msg << "; ";
        msg << "(cfailed " << ecf_expr << " not in " << a.cfailed() << ")";
        failed = true;
    }
    return failed ? ::testing::AssertionFailure(msg) : ::testing::AssertionSuccess();
}

// ------------------------------------------------------------------------

::testing::AssertionResult AssertStationObsControlCorrected(const char* es_expr, const char* eo_expr, const char* eci_expr, const char* eco_expr, const char* /*a_expr*/,
                                                            int es, const miutil::miTime& eo, const std::string& eci, float eco, const kvalobs::kvData& a)
{
    bool failed = false;
    ::testing::Message msg;
    if( es != a.stationID() ) {
        msg << "(station " << es_expr << " != " << a.stationID() << ")";
        failed = true;
    }
    if( eo != a.obstime() ) {
        if( failed )
            msg << "; ";
        msg << "(obstime " << eo_expr << " != " << a.obstime().isoTime() << ")";
        failed = true;
    }
    if( eci != a.controlinfo().flagstring() ) {
        if( failed )
            msg << "; ";
        msg << "(controlinfo " << eci_expr << " != " << a.controlinfo().flagstring() << ")";
        failed = true;
    }
    if( !Helpers::equal(a.corrected(), eco) ) {
        if( failed )
            msg << "; ";
        msg << "(corrected " << eco_expr << " != " << a.corrected() << ")";
        failed = true;
    }
    return failed ? ::testing::AssertionFailure(msg) : ::testing::AssertionSuccess();
}

// ########################################################################

AlgorithmTestBase::AlgorithmTestBase()
    : algo(0)
{
}

//------------------------------------------------------------------------

void AlgorithmTestBase::SetUp()
{
    logs = new TestNotifier();
    db = new SqliteTestDB();
    bc = new TestBroadcaster();
    if( algo != 0 ) {
        algo->setNotifier(logs);
        algo->setDatabase(db);
        algo->setBroadcaster(bc);
    }
}

// ------------------------------------------------------------------------

void AlgorithmTestBase::TearDown()
{
    delete algo;
    delete bc;
    delete db;
    delete logs;
}
