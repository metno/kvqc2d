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

#include "KvServicedBroadcaster.h"

#include "Qc2App.h"

#include <milog/milog.h>

KvServicedBroadcaster::KvServicedBroadcaster(Qc2App& app)
    : mApp(app)
{
}

void KvServicedBroadcaster::queueChanged(const kvalobs::kvData& d)
{
    mDataList.push_back(d);
}

void KvServicedBroadcaster::sendChanges()
{
    if (mDataList.empty())
        return;

    const int WAIT_MAX = 120;
    for (int i=0; i<WAIT_MAX; ++i) {
        bool busy = true;
        bool serviceOk = mApp.sendDataToKvService(mDataList, busy);
        if (!serviceOk) {
            LOGWARN("kvServiced problem, keeping " << mDataList.size() << " changes in memory.");
            return;
        }
        if (!busy) {
            mDataList.clear();
            return;
        }
        LOGINFO("kvServiced busy, waiting 1s ...");
        sleep(1);
    }
    LOGWARN("kvServiced busy for " << WAIT_MAX << "s, keeping " << mDataList.size() << " changes in memory.");
}
