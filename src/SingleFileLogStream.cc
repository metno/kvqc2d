/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 Copyright (C) 2016 met.no

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

#include "SingleFileLogStream.h"

#include <milog/StdLayout.h>

void SingleFileLogStream::write(const std::string &message)
{
    if (out_.is_open())
        out_ << message << std::flush;
}

SingleFileLogStream::SingleFileLogStream(const std::string& filename)
    : out_(filename.c_str(), std::ios_base::app)
{
    layout(new milog::StdLayout());
}

SingleFileLogStream::~SingleFileLogStream()
{
}
