// -*- c++ -*-

#ifndef Qc2Algorithm_H
#define Qc2Algorithm_H

#include "ProcessImpl.h"
#include "ReadProgramOptions.h"

#include <kvalobs/kvStation.h>
#include <kvalobs/kvData.h>
#include <list>

class Broadcaster {
public:
    virtual ~Broadcaster() { }
    virtual void queueChanged(const kvalobs::kvData&) { }
    virtual void sendChanges() { }
};

/// Interface implemented by the different QC2 algorithms.

class Qc2Algorithm  
{
public:
    Qc2Algorithm(ProcessImpl* dispatcher);
    virtual ~Qc2Algorithm();

    virtual void run(const ReadProgramOptions& params) = 0;

    ProcessImpl* dispatcher() const
        { return mDispatcher; }

    Broadcaster* broadcaster() const
        { return mBroadcaster.get(); }

    void fillStationLists(std::list<kvalobs::kvStation> stations, std::list<int>& idList);
    void fillStationIDList(std::list<int>& idList);

private:
    ProcessImpl* mDispatcher;
    std::auto_ptr<Broadcaster> mBroadcaster;
};

#endif
