// -*- c++ -*-

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
        { return Message(Message::DEBUG, mNotifier); }

    Message info()
        { return Message(Message::INFO, mNotifier); }

    Message warning()
        { return Message(Message::WARNING, mNotifier); }

    Message error()
        { return Message(Message::ERROR, mNotifier); }

    void fillStationLists(DBInterface::StationList& stations, DBInterface::StationIDList& idList);
    void fillStationIDList(DBInterface::StationIDList& idList);

protected:
    void updateSingle(const kvalobs::kvData& update);
    void storeData(const DBInterface::DataList& toUpdate, const DBInterface::DataList& toInsert = DBInterface::DataList());

protected:
    miutil::miTime UT0, UT1;
    std::string CFAILED_STRING;
    float missing, rejected;

private:
    DBInterface* mDatabase;
    Broadcaster* mBroadcaster;
    Notifier* mNotifier;
    std::string mName;
};

#endif
