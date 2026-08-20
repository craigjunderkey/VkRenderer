#include "log_utils.h"

using namespace Log;
using namespace LogUtils::LogUtilsInternal;

Log::Logger* LogUtils::LogUtilsInternal::logger_ = nullptr;

namespace LogUtils
{
    void setGlobalLogger(Logger* logger)
    {
        assert(logger != nullptr);
        logger_ = logger;
    }

    void setGlobalLogger(Logger& logger)
    {
        logger_ = &logger;
    }

    Logger& getGlobalLogger()
    {
        assert(logger_ != nullptr);
        return *logger_;
    }

    void log(LogMessageDetails details, std::string_view message)
    {
        assert(logger_ != nullptr);
        logger_->log(std::move(details), std::move(message));
    }

    void warn(LogMessageDetails details, std::string_view message)
    {
        assert(logger_ != nullptr);
        details.type = LogMessageType::Warning;
        logger_->log(std::move(details), std::move(message));
    }

    void err(LogMessageDetails details, std::string_view message)
    {
        assert(logger_ != nullptr);
        details.type = LogMessageType::Error;
        logger_->log(std::move(details), std::move(message));
    }
}