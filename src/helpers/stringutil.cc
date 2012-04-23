/* -*- c++ -*-
  Kvalobs - Free Quality Control Software for Meteorological Observations

  Copyright (C) 2011 met.no

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

#include "stringutil.h"

#include <boost/algorithm/string/trim.hpp>
#include <stdexcept>

namespace Helpers {

split2_t split2(const std::string& toSplit, const std::string& delimiter, bool trim)
{
    if( delimiter.empty() )
        throw std::runtime_error("empty delimiter");
    std::size_t pos = toSplit.find(delimiter);
    if( pos == std::string::npos )
        throw std::runtime_error("delimiter '" + delimiter + "' not found in '" + toSplit + "'");
    split2_t split(toSplit.substr(0, pos), toSplit.substr(pos+delimiter.size()));
    if( trim ) {
        boost::trim(split.first);
        boost::trim(split.second);
    }
    return split;
}

// ------------------------------------------------------------------------

splitN_t splitN(const std::string& toSplit, const std::string& delimiter, bool trim)
{
    if( delimiter.empty() )
        throw std::runtime_error("empty delimiter");
    splitN_t split;
    std::size_t begin = 0;
    while( begin < toSplit.size() ) {
        std::size_t end = toSplit.find(delimiter, begin);
        if( end == std::string::npos )
            end = toSplit.size();
        std::string part = toSplit.substr(begin, end-begin);
        if( trim )
            boost::trim(part);
        split.push_back(part);
        begin = end+delimiter.size();
    }
    if( begin == toSplit.size() )
        split.push_back("");
    return split;
}

} // namespace Helpers
