#pragma once
#include "log.h"

namespace LogUtils
{
    namespace LogUtilsInternal
    {
        extern Log::Logger* logger_;
    }

    void setGlobalLogger(Log::Logger& logger);
    void setGlobalLogger(Log::Logger* logger);
    Log::Logger& getGlobalLogger();

    void log(Log::LogMessageDetails details, std::string_view message);
    void warn(Log::LogMessageDetails details, std::string_view message);
    void err(Log::LogMessageDetails details, std::string_view message);
    
    template<typename... Args>
    void log(Log::LogMessageDetails details, std::format_string<Args...> format, Args&&... args)
    {
        assert(LogUtilsInternal::logger_ != nullptr);
        LogUtilsInternal::logger_->log(
            std::move(details), std::forward<std::format_string<Args...>>(format), std::forward<Args>(args)...
        );
    }

    template<typename... Args>
    void warn(Log::LogMessageDetails details, std::format_string<Args...> format, Args&&... args)
    {
        assert(LogUtilsInternal::logger_ != nullptr);
        details.type = Log::LogMessageType::Warning;
        LogUtilsInternal::logger_->log(
            std::move(details), std::forward<std::format_string<Args...>>(format), std::forward<Args>(args)...
        );
    }

    template<typename... Args>
    void err(Log::LogMessageDetails details, std::format_string<Args...> format, Args&&... args)
    {
        assert(LogUtilsInternal::logger_ != nullptr);
        details.type = Log::LogMessageType::Error;
        LogUtilsInternal::logger_->log(
            std::move(details), std::forward<std::format_string<Args...>>(format), std::forward<Args>(args)...
        );
    }
}