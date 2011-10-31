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

#include "algorithms/SingleLinearAlgorithm.h"
#include "algorithms/RedistributionAlgorithm.h"
#include "algorithms/DipTestAlgorithm.h"
#include "algorithms/GapInterpolationAlgorithm.h"
#include "algorithms/ProcessPlumatic.h"

#include "Qc2App.h"
#include "Qc2Connection.h"
#include "ReadProgramOptions.h"

#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>

#include "foreach.h"

class DummyAlgorithm : public Qc2Algorithm {
public:
    DummyAlgorithm()
        : Qc2Algorithm() { }

    virtual void run(const ReadProgramOptions&) {
        LOGINFO("Dummy algorithm.");
    }
};

// ########################################################################

ProcessImpl::ProcessImpl( Qc2App &app_, dnmi::db::Connection & con_ )
    : app( app_ ), con( con_ )
{
    mCode2Name[1] = "SingleLinear";
    mCode2Name[2] = "Redistribute";
    mCode2Name[3] = "DipTest";
    mCode2Name[4] = "GapInterpolate";
    mCode2Name[5] = "Plumatic";
    mCode2Name[12] = "Dummy";

    mAlgorithms["SingleLinear"]   = new SingleLinearAlgorithm();
    mAlgorithms["Redistribute"]   = new RedistributionAlgorithm2();
    mAlgorithms["DipTest"]        = new DipTestAlgorithm();
    mAlgorithms["GapInterpolate"] = new GapInterpolationAlgorithm();
    mAlgorithms["Plumatic"]       = new PlumaticAlgorithm();
    mAlgorithms["Dummy"]          = new DummyAlgorithm();

}

ProcessImpl::~ProcessImpl()
{
    foreach(algorithms_t::value_type algo, mAlgorithms) {
        delete algo.second;
    }
}

int ProcessImpl::select(const ReadProgramOptions& params)
{
    // TODO this gives the Algorithm name higher priority than the AlgoCode -- okay?
    std::string algorithm = params.Algorithm;
    std::cout << "Algorithm setting is name='" << algorithm << "' code=" << params.AlgoCode << std::endl;
    if( algorithm == "NotSet" ) {
        std::map<int,std::string>::const_iterator it = mCode2Name.find(params.AlgoCode);
        if( it != mCode2Name.end() ) {
            algorithm = it->second;
        } else {
            LOGINFO("Algorithm name not set, and AlgoCode=" << params.AlgoCode << " unknown.");
            return 1;
        }
    }
    algorithms_t::iterator a = mAlgorithms.find(algorithm);
    if( a != mAlgorithms.end() ) {
        LOGINFO("Case " + algorithm);
        try {
            a->second->run(params);
            LOGINFO(algorithm + " Completed");
        } catch(DBException& dbe) {
            LOGERROR(algorithm + " Database exception: '" + dbe.what() + "'");
        }
    } else {
        std::cout << "Algorithm '" << algorithm << " not known." << std::endl;
        LOGINFO("Case ??: no valid algorithm specified");
    }
    return 0;
}
