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

#include "TestNotifier.h"

#include "foreach.h"
#include <boost/regex.hpp>
#include <iomanip>
#include <iostream>

const char* TestNotifier::levels[] = { "DEBUG", "INFO", "WARNING", "ERROR", "FATAL" };

// ------------------------------------------------------------------------

const std::string& TestNotifier::text(int idx) const
{
    return mMessages[idx].text;
}

// ------------------------------------------------------------------------

Message::Level TestNotifier::level(int idx) const
{
    return mMessages[idx].level;
}

// ------------------------------------------------------------------------

void TestNotifier::clear()
{
    mMessages.clear();
}

// ------------------------------------------------------------------------

void TestNotifier::sendText(Message::Level level, const std::string& message)
{
    mMessages.push_back(Record(level, message));
    //std::cout << std::setw(7) << levels[level] << " '" << message << "'" << std::endl;
}

// ------------------------------------------------------------------------

int TestNotifier::find(const std::string& needle, int level, int start) const
{
    const boost::regex e(needle);
    while(start>=0 && start < (int)mMessages.size()) {
        if( level<0 || mMessages[start].level==level) {
            const bool found = boost::regex_search(mMessages[start].text, e);// boost::algorithm::contains(mMessages[start].text, needle);
            if( found )
                return start;
        }
        start += 1;
    }
    return -1;
}

// ------------------------------------------------------------------------

int TestNotifier::count(int level) const
{
    if( level<0 )
        return mMessages.size();

    int c = 0;
    foreach(const Record& r, mMessages) {
        if( r.level == level )
            c += 1;
    }
    return c;
}

// ------------------------------------------------------------------------

int TestNotifier::next(Message::Level lvl, int idx) const
{
    for(; idx>=0 && idx<(int)mMessages.size(); ++idx)
        if( mMessages[idx].level == lvl )
            return idx;
    return -1;
}

// ------------------------------------------------------------------------

void TestNotifier::dump()
{
    dump(std::cout);
}

// ------------------------------------------------------------------------

void TestNotifier::dump(std::ostream& out)
{
    out << "========== log dump start ==========\n";
    for(unsigned int i=0; i<mMessages.size(); ++i)
        out << std::setw(3) << i << ' ' << std::setw(7) << levels[mMessages[i].level] << " \'" << mMessages[i].text << "\'\n";
    out << "=========== log dump end ===========\n";
}

