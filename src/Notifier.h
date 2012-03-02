// -*- c++ -*-

#ifndef Notifier_H
#define Notifier_H

#include <boost/shared_ptr.hpp>
#include <iosfwd>

class Notifier;

// #######################################################################

class Message {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR, FATAL };
    
    Message(Level level, Notifier* n);

    ~Message();

    void reset();

    template<class T>
    Message& operator<<(const T& t)
        { *mStream << t; return *this; }

private:
    boost::shared_ptr<std::ostringstream> mStream;
    Level mLevel;
    Notifier* mNotifier;
};

// #######################################################################

class Notifier
{
public:
    virtual ~Notifier() { }
    virtual void sendText(Message::Level level, const std::string& message) = 0;
};

#endif
