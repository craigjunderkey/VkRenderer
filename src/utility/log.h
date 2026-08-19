#pragma once
#include "types.h"
#include "string_id_map.h"
#include <string>
#include <format>
#include <memory>
#include <initializer_list>

#define LOG_BUFFER_SIZE 256

namespace Log
{
    struct LoggingContext
    {
        u32 maxVerbosity;
        std::array<char, LOG_BUFFER_SIZE> buffer;
        StringIdMap<std::string_view> channels { "log_channels" };

        LoggingContext(u32 maxVerbosity, std::initializer_list<std::string_view> channelNames);
    };

    enum class LogMessageType
    {
        Log,
        Error,
        Warning
    };

    class LoggerOutput
    {
    public:
        virtual void init() = 0;
        virtual void log(LogMessageType type, Option<std::string_view> channel, std::string_view formattedMessage) = 0;
        virtual ~LoggerOutput() = default;
    };

    class ConsoleOutput : public LoggerOutput
    {
    public:
        virtual void init() override {}
        virtual void log(LogMessageType type, Option<std::string_view> channel, std::string_view formattedMessage) override;
    };

    class Logger
    {
    private:
        Logger* forwardTo_ = nullptr;
        std::shared_ptr<LoggingContext> context_ = nullptr;
        std::unique_ptr<LoggerOutput> output_ = nullptr;
        u32 verbosity_ = 0;

        void logFormatted(LogMessageType type, Option<std::string_view> channel, u32 verbosity, std::string_view formattedMessage);

    public:
        Logger() = default;
        Logger(std::shared_ptr<LoggingContext> context, std::unique_ptr<LoggerOutput> output);

        Logger& forwardTo(Logger& other);
        Logger& setVerbosity(u32 verbosity);

        template <typename... Args>
        void logTyped(LogMessageType type, StringId channel, u32 verbosity, std::format_string<Args...> format, Args&&... args)
        {
            assert(this->context_ != nullptr);
            
            auto fmtResult = std::format_to_n(
                this->context_->buffer.begin(),
                this->context_->buffer.size() - 1,
                std::move(format),
                std::forward<Args>(args)...
            );
            this->context_->buffer[fmtResult.size] = '\0';
            std::string_view formattedMessage = std::string_view(this->context_->buffer.data(), fmtResult.size);
            
            if (channel.isNull())
                this->logFormatted(type, {}, verbosity, std::move(formattedMessage));
            else
                this->logFormatted(type, this->context_->channels.get(channel), verbosity, std::move(formattedMessage));
        }

        template <typename... Args>
        void log(StringId channel, u32 verbosity, std::format_string<Args...> format, Args&&... args)
        {
            this->logTyped(LogMessageType::Log, channel, verbosity, std::move(format), std::forward<Args>(args)...);
        }

        template <typename... Args>
        void err(StringId channel, u32 verbosity, std::format_string<Args...> format, Args&&... args)
        {
            this->logTyped(LogMessageType::Error, channel, verbosity, std::move(format), std::forward<Args>(args)...);
        }

        template <typename... Args>
        void warn(StringId channel, u32 verbosity, std::format_string<Args...> format, Args&&... args)
        {
            this->logTyped(LogMessageType::Warning, channel, verbosity, std::move(format), std::forward<Args>(args)...);
        }

        void logTyped(LogMessageType type, StringId channel, u32 verbosity, const std::string& msg);
        
        inline void log(StringId channel, u32 verbosity, const std::string& msg)
        {
            logTyped(LogMessageType::Log, channel, verbosity, msg);
        }

        inline void err(StringId channel, u32 verbosity, const std::string& msg)
        {
            logTyped(LogMessageType::Error, channel, verbosity, msg);
        }

        inline void warn(StringId channel, u32 verbosity, const std::string& msg)
        {
            logTyped(LogMessageType::Warning, channel, verbosity, msg);
        }
    };
}
