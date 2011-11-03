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
#include "StandardBroadcaster.h"
#include "StandardDB.h"

#include <milog/milog.h>
#include <kvalobs/kvDbGate.h>

#include "foreach.h"

class DummyAlgorithm : public Qc2Algorithm {
public:
    DummyAlgorithm()
        : Qc2Algorithm("Dummy") { }

    virtual void run(const ReadProgramOptions&) {
        LOGINFO("Dummy algorithm.");
    }
};

// ########################################################################

ProcessImpl::ProcessImpl( Qc2App &app_, dnmi::db::Connection & con_ )
    : app( app_ ), con( con_ ), mDatabase(new StandardDB(&con)), mBroadcaster(new StandardBroadcaster(app))
{
    Qc2Algorithm* algorithms[] = {
        new SingleLinearAlgorithm(),
        new RedistributionAlgorithm2(),
        new DipTestAlgorithm(),
        new GapInterpolationAlgorithm(),
        new PlumaticAlgorithm(),
        new DummyAlgorithm()
    };
    const int N = sizeof(algorithms)/sizeof(algorithms[0]);
    for(int i=0; i<N; ++i) {
        Qc2Algorithm* a = algorithms[i];
        mAlgorithms[ a->name() ] = a;
        a->setBroadcaster(mBroadcaster);
        a->setDatabase(mDatabase);
    }
}

ProcessImpl::~ProcessImpl()
{
    foreach(algorithms_t::value_type algo, mAlgorithms) {
        delete algo.second;
    }
    delete mBroadcaster;
    delete mDatabase;
}

int ProcessImpl::select(const ReadProgramOptions& params)
{
    std::string algorithm = params.Algorithm;
    std::cout << "Algorithm setting is name='" << algorithm << "'" << std::endl;
    algorithms_t::iterator a = mAlgorithms.find(algorithm);
    if( a != mAlgorithms.end() ) {
        LOGINFO("Case '" + algorithm + "'");
        try {
            a->second->run(params);
            LOGINFO(algorithm + " Completed");
        } catch(DBException& dbe) {
            LOGERROR(algorithm + ": Database exception: '" + dbe.what() + "'");
        } catch(ConfigException& ce) {
            LOGERROR(algorithm + ": Configuration exception: '" + ce.what() + "'");
        } catch(...) {
            LOGERROR(algorithm + ": Exception -- please report bug in https://kvoss.bugs.met.no");
        }
    } else {
        std::cout << "Algorithm '" << algorithm << " not known." << std::endl;
        LOGINFO("Case ??: no valid algorithm specified");
    }
    return 0;
}
