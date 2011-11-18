// -*- c++ -*-

#ifndef Qc2Algorithm_H
#define Qc2Algorithm_H

#include "DBInterface.h"
#include "AlgorithmConfig.h"

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

    void fillStationLists(std::list<kvalobs::kvStation>& stations, std::list<int>& idList);
    void fillStationIDList(std::list<int>& idList);

protected:
    void updateSingle(const kvalobs::kvData& update);
    void storeData(const std::list<kvalobs::kvData>& toUpdate, const std::list<kvalobs::kvData>& toInsert = std::list<kvalobs::kvData>());

protected:
    miutil::miTime UT0, UT1;
    std::string CFAILED_STRING;
    float missing, rejected;

private:
    DBInterface* mDatabase;
    Broadcaster* mBroadcaster;
    std::string mName;
};

#endif
