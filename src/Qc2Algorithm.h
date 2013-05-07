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

#ifndef Qc2Algorithm_H
#define Qc2Algorithm_H

#include "AlgorithmConfig.h"
#include "DBInterface.h"
#include "Notifier.h"
#include <kvalobs/kvStation.h>
#include <kvalobs/kvData.h>
#include <list>

class Broadcaster;

// #######################################################################

/**
 * Interface to be implemented by the different QC2 algorithms.
 */
class Qc2Algorithm
{
public:
    Qc2Algorithm(const std::string& name);
    virtual ~Qc2Algorithm();

    virtual void configure(const AlgorithmConfig& params);

    virtual void run() = 0;

    void setBroadcaster(Broadcaster* b)
        { mBroadcaster = b; }

    Broadcaster* broadcaster() const
        { return mBroadcaster; }

    void setDatabase(DBInterface* db)
        { mDatabase = db; }

    DBInterface* database() const
        { return mDatabase; }

    const std::string& name() const
        { return mName; }

    void setNotifier(Notifier* n)
        { mNotifier = n; }

    Message debug()
        { return message(Message::DEBUG); }

    Message info()
        { return message(Message::INFO); }

    Message warning()
        { return message(Message::WARNING); }

    Message error()
        { return message(Message::ERROR); }

    void fillStationLists(DBInterface::StationList& stations, DBInterface::StationIDList& idList);
    void fillStationIDList(DBInterface::StationIDList& idList);

protected:
    void updateSingle(const kvalobs::kvData& update);
    void storeData(const DBInterface::DataList& toUpdate, const DBInterface::DataList& toInsert = DBInterface::DataList());

private:
    Message message(Message::Level level);

protected:
    kvtime::time UT0, UT1;
    std::string CFAILED_STRING;
    float missing, rejected;

private:
    DBInterface* mDatabase;
    Broadcaster* mBroadcaster;
    Notifier* mNotifier;
    std::string mName;
};

#endif
