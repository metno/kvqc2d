/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id$                                                       

  Copyright (C) 2007 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
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

#include "ProcessImpl.h"

#include "algorithms/SingleLinear_v32.h"
#include "algorithms/ProcessRedistribution.h"
#include "algorithms/DipTestAlgorithm.h"
#include "algorithms/GapInterpolationAlgorithm.h"
#include "algorithms/ProcessPlumatic.h"

#include "Qc2App.h"
#include "Qc2Connection.h"
#include "ReadProgramOptions.h"
#include "scone.h"

#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>
#include <puTools/miTime.h>

#include <boost/foreach.hpp>
#include <sstream>
#include <memory>
#include <stdexcept>

using namespace kvalobs;
using namespace std;
using namespace miutil;

class DummyAlgorithm : public Qc2Algorithm {
public:
    DummyAlgorithm(ProcessImpl* p)
        : Qc2Algorithm(p) { }

    virtual void run(const ReadProgramOptions&) {
        LOGINFO("Dummy algorithm.");
    }
};

// ########################################################################

ProcessImpl::ProcessImpl( Qc2App &app_, dnmi::db::Connection & con_ )
    : app( app_ ), con( con_ )
{
    mAlgorithms["SingleLinear"]   = new SingleLinearV32Algorithm(this);
    mAlgorithms["Redistribute"]   = new RedistributionAlgorithm(this);
    mAlgorithms["DipTest"]        = new DipTestAlgorithm(this);
    mAlgorithms["GapInterpolate"] = new GapInterpolationAlgorithm(this);
    mAlgorithms["Plumatic"]       = new PlumaticAlgorithm(this);
    mAlgorithms["Dummy"]          = new DummyAlgorithm(this);
}

ProcessImpl::~ProcessImpl()
{
    BOOST_FOREACH(algorithms_t::value_type algo, mAlgorithms) {
        delete algo.second;
    }
}

void ProcessImpl::GetStationList(std::list<kvalobs::kvStation>& StationList)
{
    kvalobs::kvDbGate dbGate( &con );
    std::list<kvalobs::kvStation> SL;

    try {
        //bool result = dbGate.select( StationList, kvQueries::selectAllStations("stationid"), "station" );
        bool result = dbGate.select( SL, kvQueries::selectAllStations("stationid"), "station" );
    } catch ( dnmi::db::SQLException & ex ) {
        IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
    } catch ( ... ) {
        IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
    }

    // Make Qc2 specific selection on the StationList here
    // Only use stations less than 100000 i.e. only Norwegian stations
    // Also remove stations that are moving, e.g. ships.
    for( std::list<kvalobs::kvStation>::const_iterator it = SL.begin(); it != SL.end(); ++it ) {
        if (it->stationID() < 100000  &&  it->maxspeed()==0.0) {
            StationList.push_back(*it);
        }
    }
    //for ( std::list<kvalobs::kvStation>::const_iterator iq = StationList.begin(); iq != StationList.end(); ++iq ) {
    //     std::cout << iq->stationID() << std::endl;
    //}

}

void ProcessImpl::GetStationList(std::list<kvalobs::kvStation>& StationList, miutil::miTime ProcessTime)
{
    kvalobs::kvDbGate dbGate( &con );
    try {
        bool result = dbGate.select( StationList, kvQueries::selectAllStations("stationid"), "station" );
    } catch ( dnmi::db::SQLException & ex ) {
        IDLOGERROR( "html", "Exception: " << ex.what() << std::endl );
    } catch ( ... ) {
        IDLOGERROR( "html", "Unknown exception: con->exec(ctbl) .....\n" );
    }
}

int ProcessImpl::select(const ReadProgramOptions& params)
{
    const std::string& algorithm = params.Algorithm;
    std::cout << "Algorithm setting is '" << algorithm << "'" << std::endl;
    algorithms_t::iterator a = mAlgorithms.find(algorithm);
    if( a != mAlgorithms.end() ) {
        LOGINFO("Case " + algorithm);
        a->second->run(params);
        LOGINFO(algorithm + " Completed");
    } else {
        std::cout << "Algorithm '" << algorithm << " not known." << std::endl;
        LOGINFO("Case ??: no valid algorithm specified");
    }
    return 0;
}
