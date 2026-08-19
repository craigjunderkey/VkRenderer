#pragma once
#include "log.h"

namespace Log
{
    static Logger* logger;
}

template <typename... Args>
void log(StringId channel, u32 verbosity, std::format_string<Args...> format, Args&&... args)
{
    Log::logger->logTyped(Log::LogMessageType::Log, channel, verbosity, std::move(format), std::forward<Args>(args)...);
}

inline void log(StringId channel, u32 verbosity, const std::string& msg)
{
    Log::logger->logTyped(Log::LogMessageType::Log, channel, verbosity, msg);
}

template <typename... Args>
void dbglog(std::format_string<Args...> format, Args&&... args)
{
    Log::logger->logTyped(Log::LogMessageType::Log, {}, 0, std::move(format), std::forward<Args>(args)...);
}

inline void dbglog(const std::string& msg)
{
    Log::logger->logTyped(Log::LogMessageType::Log, {}, 0, msg);
}

template <typename... Args>
void err(StringId channel, u32 verbosity, std::format_string<Args...> format, Args&&... args)
{
    Log::logger->logTyped(Log::LogMessageType::Error, channel, verbosity, std::move(format), std::forward<Args>(args)...);
}

inline void err(StringId channel, u32 verbosity, const std::string& msg)
{
    Log::logger->logTyped(Log::LogMessageType::Error, channel, verbosity, msg);
}

template <typename... Args>
void dbgerr(std::format_string<Args...> format, Args&&... args)
{
    Log::logger->logTyped(Log::LogMessageType::Error, {}, 0, std::move(format), std::forward<Args>(args)...);
}

inline void dbgerr(const std::string& msg)
{
    Log::logger->logTyped(Log::LogMessageType::Error, {}, 0, msg);
}

template <typename... Args>
void warn(StringId channel, u32 verbosity, std::format_string<Args...> format, Args&&... args)
{
    Log::logger->logTyped(Log::LogMessageType::Warning, channel, verbosity, std::move(format), std::forward<Args>(args)...);
}

inline void warn(StringId channel, u32 verbosity, const std::string& msg)
{
    Log::logger->logTyped(Log::LogMessageType::Warning, channel, verbosity, msg);
}

template <typename... Args>
void dbgwarn(std::format_string<Args...> format, Args&&... args)
{
    Log::logger->logTyped(Log::LogMessageType::Warning, {}, 0, std::move(format), std::forward<Args>(args)...);
}

inline void dbgwarn(const std::string& msg)
{
    Log::logger->logTyped(Log::LogMessageType::Warning, {}, 0, msg);
}
