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
    , mForcedModified(false)
    , mOrigControl("0000000000000000")
    , mOrigCorrected(-32767.0f)
    , mOrigCfailed("")
{
}

// ------------------------------------------------------------------------

DataUpdate::DataUpdate(const kvalobs::kvData& data)
    : mData(data)
    , mNew(false)
    , mForcedModified(false)
    , mOrigControl(mData.controlinfo())
    , mOrigCorrected(mData.corrected())
    , mOrigCfailed(mData.cfailed())
{
}

// ------------------------------------------------------------------------

DataUpdate::DataUpdate(const kvalobs::kvData& templt, const miutil::miTime& obstime, const miutil::miTime& tbtime,
                       float original, float corrected, const std::string& controlinfo)
    : mData(templt.stationID(), obstime, original, templt.paramID(), tbtime, templt.typeID(), templt.sensor(),
            templt.level(), corrected, kvalobs::kvControlInfo(controlinfo), kvalobs::kvUseInfo(), "QC2-missing-row")
    , mNew(true)
    , mForcedModified(false)
    , mOrigControl(mData.controlinfo())
    , mOrigCorrected(mData.corrected())
    , mOrigCfailed(mData.cfailed())
{
    Helpers::updateUseInfo(mData);
}

// ------------------------------------------------------------------------

bool DataUpdate::isModified() const
{
    return mNew || mForcedModified
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

std::string DataUpdate::text(int daysBefore, bool modified) const
{
    miutil::miTime start = mData.obstime();
    if( daysBefore > 0 )
        start.addDay( -daysBefore );
    return text(start, modified);
}

// ------------------------------------------------------------------------

std::string DataUpdate::text(const miutil::miTime& start, bool modified) const
{
    std::ostringstream out;
    out << "[stationid=" << mData.stationID() << " AND ";
    if( start >= mData.obstime() ) {
        out << "obstime='" << mData.obstime().isoTime() << '\'';
    } else {
        out << "obstime BETWEEN '" << start.isoTime() << "' AND '" << mData.obstime().isoTime() << '\'';
    }
    out << " AND paramid="  << mData.paramID()
        << " AND typeid="   << mData.typeID()
        << " AND sensor='"   << mData.sensor() << "'"
        << " AND level="    << mData.level()
        << "; original=" << Helpers::digits1 << mData.original();
    if( modified ) {
        out << " corr=" << Helpers::digits1 << mData.corrected();
        if( mData.corrected() != mOrigCorrected )
            out << '(' << Helpers::digits1 << mOrigCorrected << ')';
        out << " controlinfo=" << mData.controlinfo().flagstring();
        if( mData.controlinfo() != mOrigControl )
            out << '(' << mOrigControl.flagstring() << ')';
        out << " cfailed='" << mData.cfailed() << '\'';
        if( mData.cfailed() != mOrigCfailed )
            out << '(' << mOrigCfailed << ')';
        if( isModified() ) {
            out << '{';
            if( isNew() )
                out << 'n';
            else
                out << 'm';
            out << '}';
        }
    } else {
        out << " corr="        << Helpers::digits1 << mOrigCorrected
            << " controlinfo=" << mOrigControl.flagstring()
            << " cfailed='"    << mOrigCfailed << "'";
    }
    out << ']';
    return out.str();
}

// ------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const DataUpdate& du)
{
    out << du.text(0, true);
    return out;
}
