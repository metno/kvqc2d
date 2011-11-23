// -*- c++ -*-

#ifndef BROADCASTER_H
#define BROADCASTER_H 1

#include <kvalobs/kvData.h>

/**
 * Implementations will broadcast changes submitted to the kvalobs database.
 */
class Broadcaster {
public:
    virtual ~Broadcaster() { }
    virtual void queueChanged(const kvalobs::kvData&) = 0;
    virtual void sendChanges() = 0;
};

#endif
