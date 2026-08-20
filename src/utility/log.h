#pragma once
#include "types.h"
#include "string_id_map.h"
#include <format>
#include <memory>
#include <initializer_list>
#include <source_location>

#define LOG_BUFFER_SIZE 256

namespace Log
{
    using LogBuffer = std::array<char, LOG_BUFFER_SIZE>;

    enum class LogMessageType
    {
        Log,
        Error,
        Warning
    };

    struct LogMessageDetails
    {
        LogMessageType type;
        StringId channel = StringId::NONE;
        u32 verbosity = 0;
        std::source_location sourceLoc = std::source_location::current();
    };

    struct LoggingContext
    {
        u32 maxVerbosity;
        LogBuffer immediateBuffer;
        StringIdMap<std::string_view> channels { "log_channels" };

        LoggingContext(u32 maxVerbosity, std::initializer_list<std::string_view> channelNames);
    };

    class LoggerOutput
    {
    public:
        virtual void init() = 0;
        virtual void log(LogMessageDetails details, std::string_view message) = 0;
        virtual ~LoggerOutput() = default;
    };

    class ConsoleOutput : public LoggerOutput
    {
    public:
        virtual void init() override {}
        virtual void log(LogMessageDetails, std::string_view message) override;
    };

    class Logger
    {
    private:
        std::unique_ptr<Logger> forwardTo_ = nullptr;
        std::shared_ptr<LoggingContext> context_ = nullptr;
        std::unique_ptr<LoggerOutput> output_ = nullptr;
        u32 verbosity_ = 0;

    public:
        Logger() = default;
        Logger(std::shared_ptr<LoggingContext> context, std::unique_ptr<LoggerOutput> output);

        Logger& forwardTo(std::unique_ptr<Logger> other);
        Logger& setVerbosity(u32 verbosity);

        void log(LogMessageDetails details, std::string_view message);
        
        template <typename... Args>
        void log(LogMessageDetails details, std::format_string<Args...> format, Args&&... args)
        {
            assert(this->context_ != nullptr);
            
            auto fmtResult = std::format_to_n(
                this->context_->immediateBuffer.begin(),
                this->context_->immediateBuffer.size() - 1,
                std::forward<std::format_string<Args...>>(format),
                std::forward<Args>(args)...
            );
            this->context_->immediateBuffer[fmtResult.size] = '\0';
            std::string_view formattedMessage = std::string_view(this->context_->immediateBuffer.data(), fmtResult.size);
            
            this->log(std::move(details), std::move(formattedMessage));
        }
    };
}
