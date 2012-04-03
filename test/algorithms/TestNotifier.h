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

#ifndef TestNotifier_h
#define TestNotifier_h 1

#include "Notifier.h"

#include <string>
#include <vector>

class TestNotifier : public Notifier {
public:
    struct Record {
        Message::Level level;
        std::string text;
        Record(Message::Level l, const std::string& t)
            : level(l), text(t) { }
    };        

    int find(const std::string& needle, int level=-1, int start=0) const;

    int count(int level=-1) const;

    int next(Message::Level level, int startIdx=0) const;

    const std::string& text(int idx) const;

    Message::Level level(int idx) const;

    void clear();

    void sendText(Message::Level level, const std::string& message);

    void dump();
    void dump(std::ostream& out);

private:
    std::vector<Record> mMessages;

    static const char* levels[];
};

#endif /* TestNotifier_h */
