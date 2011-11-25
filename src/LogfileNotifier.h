// -*- c++ -*-

#ifndef LogfileNotifier_H
#define LogfileNotifier_H

#include "Notifier.h"

class LogfileNotifier : public Notifier
{
public:
    virtual void sendText(Message::Level level, const std::string& message);
};

#endif
