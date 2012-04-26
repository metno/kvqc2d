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

#include <kvskel/kvService.hh>
#include <iostream>
#include "foreach.h"

class DataReadyInput_i: public POA_CKvalObs::CService::DataReadyInput
{
public:
    DataReadyInput_i() { }
    virtual ~DataReadyInput_i() { }
    
    CORBA::Boolean dataReady(const CKvalObs::StationInfoList& infoList,
                             CKvalObs::CManager::CheckedInput_ptr callback,
                             CORBA::Boolean& busy);
};

CORBA::Boolean DataReadyInput_i::dataReady(const CKvalObs::StationInfoList& infoList,
                                           CKvalObs::CManager::CheckedInput_ptr /*callback*/,
                                           CORBA::Boolean& busy)
{
    std::cout << "update length=" << infoList.length() << std::endl;
    busy = false;
    for(unsigned int k=0; k<infoList.length(); ++k) {
        std::cout << " station="  << infoList[k].stationId
                  << " obstime='" << infoList[k].obstime << "'"
                  << " type="     << infoList[k].typeId_
                  << std::endl;
    }
    return true;
}

void update(CORBA::Object_ptr obj)
{
    CKvalObs::CService::DataReadyInput_var d = CKvalObs::CService::DataReadyInput::_narrow(obj);
    if (CORBA::is_nil(d)) {
        std::cerr << "cannot invoke on a nil object reference." << std::endl;
        return;
    }
    CKvalObs::StationInfoList sil;
    sil.length(2);
    for(unsigned int k=0; k<sil.length(); ++k) {
        sil[k].stationId = 12300 + k;
        sil[k].obstime   = CORBA::string_dup("2011-11-22 06:00:00");
        sil[k].typeId_   = 302;
    }

    CKvalObs::CManager::CheckedInput_var callback;
    CORBA::Boolean busy = true;
    CORBA::Boolean ok = d->dataReady(sil, callback, busy);
    if( !ok )
        std::cerr << "Not ok." << std::endl;
}

int main(int argc, char** argv)
{
    CORBA::ORB_ptr orb = CORBA::ORB_init(argc,argv,"omniORB4");
    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
    
    DataReadyInput_i *dummy = new DataReadyInput_i();
    PortableServer::ObjectId_var dummyid = poa->activate_object(dummy);
    CKvalObs::CService::DataReadyInput_var dummyref = dummy->_this();
    dummy->_remove_ref();

    PortableServer::POAManager_var pman = poa->the_POAManager();
    pman->activate();

    update(dummyref);

    orb->destroy();
    return 0;
}
