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

#include "ConfigParser.h"

#include <fstream>
#include "foreach.h"

std::string ErrorList::format(const std::string& separator) const
{
    std::ostringstream errors;
    //errors << '[' << mErrors.size() << "] ";
    for(unsigned int i=0; i<mErrors.size(); ++i) {
        errors << mErrors[i];
        if( i+1 < mErrors.size() )
            errors << separator;
    }
    return errors.str();
}

// ########################################################################

const std::string& ConfigParser::Item::value(unsigned int idx) const
{
    mRequested += 1;
    return mValues[idx];
}

// ------------------------------------------------------------------------

const std::string& ConfigParser::Item::value(unsigned int idx, const std::string& dflt) const
{
    if( idx >= mValues.size() )
        return dflt;
    else
        return value(idx);
}

// ########################################################################

ConfigParser::ConfigParser()
{
}

// ------------------------------------------------------------------------

ConfigParser::~ConfigParser()
{
}

// ------------------------------------------------------------------------

bool ConfigParser::load(std::istream& input)
{
    mItems.clear();
    mErrors.clear();
    const char* WS = " \t";

    int lineno = 0;
    std::string line;
    while( std::getline(input, line) ) {
        lineno += 1;
        if( line.empty() )
            continue;

        const size_t key_start = line.find_first_not_of(WS);
        if( key_start == std::string::npos || line[key_start] == '#' )
            continue;

        const size_t equal = line.find("=");
        if( equal == std::string::npos || equal <= key_start ) {
            mErrors.add() << "Line " << lineno << ": no/bad '='";
            continue;
        }

        size_t key_len = line.substr(key_start, equal-key_start).find_last_not_of(WS);
        if( key_len == std::string::npos )
            key_len = equal-key_start;
        else
            key_len += 1;
        if( key_len <= 0 ) {
            mErrors.add() << "Line " << lineno << ": empty key";
            continue;
        }
        const std::string key = line.substr(key_start, key_len);

        std::string value;
        size_t value_start = line.find_first_not_of(WS, equal+1);
        if( value_start != std::string::npos ) {
            size_t value_len = line.substr(value_start).find_last_not_of(WS);
            if( value_len != std::string::npos )
                value_len += 1;
            else
                value_len = line.size() - value_start;
            value = line.substr(value_start, value_len);
        }

        if( key.find("#") != std::string::npos ) {
            mErrors.add() << "Line " << lineno << ": key contains #";
            continue;
        }

        Item& item = mItems[key];
        item.mValues.push_back(value);
    }
    return mErrors.empty();
}

// ------------------------------------------------------------------------

bool ConfigParser::load(const std::string& filename)
{
    std::ifstream input(filename.c_str());
    return load(filename);
}

// ------------------------------------------------------------------------

const ConfigParser::Item& ConfigParser::get(const std::string& key) const
{
    static const Item empty_item;
    const mItems_t::const_iterator it = mItems.find(key);
    if( it != mItems.end() )
        return it->second;
    else
        return empty_item;
}

// ------------------------------------------------------------------------

ErrorList ConfigParser::checkUnrequested() const
{
    ErrorList errors;
    foreach(const mItems_t::value_type& ki, mItems) {
        if( ki.second.requestCount() == 0 )
            errors.add() << "unused option '" << ki.first << "'";
    }
    return errors;
}
