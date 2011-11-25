// -*- c++ -*-

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
