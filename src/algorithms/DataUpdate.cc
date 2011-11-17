/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  Copyright (C) 2007-2011 met.no

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

#include "DataUpdate.h"

#include "AlgorithmHelpers.h"
#include <iostream>

DataUpdate::DataUpdate()
    : mNew(false)
    , mOrigControl("0000000000000000")
    , mOrigCorrected(-32767.0f)
    , mOrigCfailed("")
{
}

// ------------------------------------------------------------------------

DataUpdate::DataUpdate(const kvalobs::kvData& data)
    : mData(data)
    , mNew(false)
    , mOrigControl(mData.controlinfo())
    , mOrigCorrected(mData.corrected())
    , mOrigCfailed(mData.cfailed())
{
}

// ------------------------------------------------------------------------

DataUpdate::DataUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
                       float corrected, const std::string& controlinfo)
    : mData(templt.stationID(), obstime, -32767, templt.paramID(), tbtime, templt.typeID(), templt.sensor(),
            templt.level(), corrected, kvalobs::kvControlInfo(controlinfo), kvalobs::kvUseInfo(), "QC2-missing-row")
    , mNew(true)
    , mOrigControl(mData.controlinfo())
    , mOrigCorrected(mData.corrected())
    , mOrigCfailed(mData.cfailed())
{
    Helpers::updateUseInfo(mData);
}

// ------------------------------------------------------------------------

bool DataUpdate::isModified() const
{
    return mNew
        || mOrigCorrected != mData.corrected()
        || mOrigControl   != mData.controlinfo();
    // || mOrigCfailed   != mData.cfailed(); // this does not work because text is appended instead of overwritten
}

// ------------------------------------------------------------------------

DataUpdate& DataUpdate::controlinfo(const kvalobs::kvControlInfo& ci)
{
    mData.controlinfo(ci);
    Helpers::updateUseInfo(mData);
    return *this;
}

// ------------------------------------------------------------------------

DataUpdate& DataUpdate::cfailed(const std::string& cf, const std::string& extra)
{
    Helpers::updateCfailed(mData, cf, extra);
    return *this;
}

// ------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const DataUpdate& du)
{
    out << du.data() << "[cf='" << du.data().cfailed() << ']';
    if( du.isModified() ) {
        out << '{';
        if( du.isNew() )
            out << 'n';
        else
            out << 'm';
        out << '}';
    }
    return out;
}
