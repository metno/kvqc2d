
#include "Notifier.h"

#include <iomanip>
#include <iostream>
#include <sstream>

Message::Message(Level level, Notifier* n)
    : mStream(new std::ostringstream)
    , mLevel(level)
    , mNotifier(n)
{
}

namespace {
const char* levelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR", "FATAL" };
}

Message::~Message()
{
    std::string msg = mStream->str();
    if( !msg.empty() ) {
        if( mNotifier )
            mNotifier->sendText( mLevel, msg );
        else
            std::cerr << std::setw(7) << levelNames[mLevel] << ": " << msg << std::endl;
    }
}
