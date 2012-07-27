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

#ifndef Notifier_H
#define Notifier_H

#include <boost/shared_ptr.hpp>
#include <iosfwd>
#include <string>

class Notifier;

// #######################################################################

class Message {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR, FATAL };

    Message(Level level, Notifier* n, const std::string& category);

    ~Message();

    void reset();

    template<class T>
    Message& operator<<(const T& t);

    Message& operator<<(const char* t);

private:
    boost::shared_ptr<std::ostringstream> mStream;
    Level mLevel;
    Notifier* mNotifier;
    const std::string mCategory;
};

// #######################################################################

/**
 * \brief Interface for sending user messages from algorithms.
 *
 * Messages might be sent to a logfile (in operation, LogfileNotifier) or to
 * memory (for testing algorithms).
 */
class Notifier
{
public:
    virtual ~Notifier() { }
    virtual void sendText(Message::Level level, const std::string& message) = 0;
};

#endif
