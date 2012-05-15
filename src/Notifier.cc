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

#include "Notifier.h"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
const char* levelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR", "FATAL" };
}

// ========================================================================

Message::Message(Level level, Notifier* n, const std::string& category)
    : mStream(new std::ostringstream)
    , mLevel(level)
    , mNotifier(n)
    , mCategory(category)
{
}

// ------------------------------------------------------------------------

Message::~Message()
{
    std::string msg = mStream->str();
    if( !msg.empty() ) {
        msg = mCategory + ": " + msg;
        if( mNotifier )
            mNotifier->sendText( mLevel, msg );
        else
            std::cerr << std::setw(7) << levelNames[mLevel] << ": " << msg << std::endl;
    }
}

// ------------------------------------------------------------------------

void Message::reset()
{
    mStream->str("");
}

// ------------------------------------------------------------------------

#include "Notifier.icc"

#include "Instrument.h"
#include <kvalobs/kvData.h>
#include <puTools/miTime.h>

template Message& Message::operator<< <char>            (const char& t);
template Message& Message::operator<< <int>             (const int& t);
template Message& Message::operator<< <std::string>     (const std::string& t);
template Message& Message::operator<< <float>           (const float& t);
template Message& Message::operator<< <double>          (const double& t);
template Message& Message::operator<< <miutil::miTime>  (const miutil::miTime& t);
template Message& Message::operator<< <miutil::miDate>  (const miutil::miDate& t);
template Message& Message::operator<< <kvalobs::kvData> (const kvalobs::kvData& t);
template Message& Message::operator<< <Instrument>      (const Instrument& t);

Message& Message::operator<<(const char* t)
{
    *mStream << t; return *this;
}
