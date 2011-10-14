// -*- c++ -*-

#ifndef Qc2Algorithm_H
#define Qc2Algorithm_H

#include "DBInterface.h"
#include "ProcessControl.h"
#include "ProcessImpl.h"
#include "ReadProgramOptions.h"

#include <kvalobs/kvStation.h>
#include <kvalobs/kvData.h>
#include <list>

// #######################################################################

/**
 * Implementations will broadcast changes submitted to the kvalobs database.
 */
class Broadcaster {
public:
    virtual ~Broadcaster() { }
    virtual void queueChanged(const kvalobs::kvData&) = 0;
    virtual void sendChanges() = 0;
};

// #######################################################################

/**
 * Interface to be implemented by the different QC2 algorithms.
 */
class Qc2Algorithm  
{
public:
    Qc2Algorithm();
    virtual ~Qc2Algorithm();

    virtual void run(const ReadProgramOptions& params) = 0;

    void setBroadcaster(Broadcaster* b)
        { mBroadcaster = b; }

    Broadcaster* broadcaster() const
        { return mBroadcaster; }

    void setDatabase(DBInterface* db)
        { mDatabase = db; }

    DBInterface* database() const
        { return mDatabase; }

    ProcessControl checkFlags()
        { return mCheckFlags; }

    void fillStationLists(std::list<kvalobs::kvStation> stations, std::list<int>& idList);
    void fillStationIDList(std::list<int>& idList);

private:
    DBInterface* mDatabase;
    Broadcaster* mBroadcaster;
    ProcessControl mCheckFlags;
};

#endif
