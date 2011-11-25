
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
    if( mNotifier )
        mNotifier->sendText( mLevel, mStream->str() );
    else
        std::cerr << std::setw(7) << levelNames[mLevel] << ": " << mStream->str() << std::endl;
}
