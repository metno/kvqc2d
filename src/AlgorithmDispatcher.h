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

#ifndef __Qc2Process_h__
#define __Qc2Process_h__

#include <kvalobs/kvStation.h>
#include <map>
#include <string>

class AlgorithmConfig;
class Broadcaster;
class DBInterface;
class Notifier;
class Qc2App;
class Qc2Algorithm;

///Handles the interface to different processing algorithms.

class AlgorithmDispatcher
{
public:
    AlgorithmDispatcher();
    ~AlgorithmDispatcher();

    int select(const AlgorithmConfig& params);

    void setBroadcaster(Broadcaster* b);

    void setDatabase(DBInterface* db);

    void setNotifier(Notifier* n);

private:
    typedef std::map<std::string, Qc2Algorithm*> algorithms_t;
    algorithms_t mAlgorithms;

    Broadcaster* mBroadcaster;
    DBInterface* mDatabase;
    Notifier* mNotifier;
};

#endif
