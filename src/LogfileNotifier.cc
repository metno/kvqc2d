
#include "LogfileNotifier.h"

#include <milog/Logger.h>

void LogfileNotifier::sendText(Message::Level level, const std::string& message)
{
    const milog::LogLevel milogLevels[] = { milog::DEBUG, milog::INFO, milog::WARN, milog::ERROR, milog::FATAL };
    milog::Logger::logger().log(milogLevels[level], message);
}
