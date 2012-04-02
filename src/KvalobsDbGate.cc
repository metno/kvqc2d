/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  Copyright (C) 2012 met.no

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

#include "KvalobsDbGate.h"

#include <time.h>
#include <sstream>
//#include <kvalobs/DataInsertTransaction.h>

#include <memory>
#include <boost/bind.hpp>

namespace {

template<class R, class F>
R retryWhileBusy(int busytimeout, const F& f)
{
    time_t now;
    time(&now);
    const time_t timeout = now + busytimeout;
    
    do {
        try {
            return f();
        } catch(dnmi::db::SQLBusy& bex) {
            time(&now);
            if( now > timeout )
                throw;
        }
    } while(true);
}

} // anonymous namespace

// ########################################################################

KvalobsDbExtract::~KvalobsDbExtract()
{
}

// ########################################################################

KvalobsDbGate::KvalobsDbGate()
    : mConnection(0)
    , mBusyTimeout(0)
{
}

// ------------------------------------------------------------------------

KvalobsDbGate::KvalobsDbGate(dnmi::db::Connection *c)
    : mConnection(c)
    , mBusyTimeout(0)
{
}

// ------------------------------------------------------------------------

KvalobsDbGate::~KvalobsDbGate()
{
}

// ------------------------------------------------------------------------

void KvalobsDbGate::beginTransaction()
{
    checkConnection();
    retryWhileBusy<void>(mBusyTimeout, boost::bind(&Connection_t::beginTransaction, mConnection));
}

// ------------------------------------------------------------------------

void KvalobsDbGate::endTransaction()
{
    checkConnection();
    retryWhileBusy<void>(mBusyTimeout, boost::bind(&Connection_t::endTransaction, mConnection));
}

// ------------------------------------------------------------------------

void KvalobsDbGate::rollBack()
{
    checkConnection();
    retryWhileBusy<void>(mBusyTimeout, boost::bind(&Connection_t::rollBack, mConnection));
}

// ------------------------------------------------------------------------

#if 0
bool KvalobsDbGate::insertImpl(const std::list<kvalobs::kvDbBase*> &elem,
                               const std::string &tblName,
                               InsertOption option)
{
    DataInsertTransaction::Action action( DataInsertTransaction::INSERTONLY );
    
    if( option == INSERT_OR_REPLACE )
        action = DataInsertTransaction::INSERT_OR_REPLACE;
    else if( option == INSERT_OR_UPDATE )
        action = DataInsertTransaction::INSERT_OR_UPDATE;
    
    DataInsertTransaction transaction( elem, action, tblName );
    
    con->perform( transaction, 3, dnmi::db::Connection::SERIALIZABLE );
}
#endif

// ------------------------------------------------------------------------

void KvalobsDbGate::select(KvalobsDbExtract* extract, const std::string& query)
{
    checkConnection();
    std::auto_ptr<dnmi::db::Result> results(retryWhileBusy<dnmi::db::Result*>(mBusyTimeout, boost::bind(&Connection_t::execQuery, mConnection, query)));
    if( extract != 0 ) {
        while (results->hasNext())
            extract->extractFromRow(retryWhileBusy<const dnmi::db::DRow&>(mBusyTimeout, boost::bind(&dnmi::db::Result::next, results.get())));
    }
}

// ------------------------------------------------------------------------

void KvalobsDbGate::update(const kvalobs::kvDbBase &elem, const std::string &tblName)
{
    std::ostringstream sql;
    sql << "UPDATE " << tblName << " " << elem.toUpdate();
    exec(sql.str());
}

// ------------------------------------------------------------------------

void KvalobsDbGate::exec(const std::string& sql)
{
    checkConnection();
    retryWhileBusy<void>(mBusyTimeout, boost::bind(&Connection_t::exec, mConnection, sql));
}

// ------------------------------------------------------------------------

void KvalobsDbGate::checkConnection()
{
    if( !mConnection )
        throw dnmi::db::SQLNotConnected("no connection");
}
