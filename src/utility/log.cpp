#include "log.h"

namespace Log
{
    LoggingContext::LoggingContext(u32 maxVerbosity, std::initializer_list<std::string_view> channelNames)
        : maxVerbosity(maxVerbosity)
    {
        this->channels.reserve(channelNames.size());
        for (std::string_view name : channelNames)
        {
            channels.add(StringId(name), name);
        }
    }

    void ConsoleOutput::log(LogMessageType type, Option<std::string_view> channel, std::string_view formattedMessage)
    {
        if (channel)
        {
            switch (type)
            {
                case LogMessageType::Log:
                    std::cout << "    (" << channel.value() << ") " << formattedMessage << "\n";
                    break;
                case LogMessageType::Error:
                    std::cerr << "\x1B[31m[!] (" << channel.value() << ") " << formattedMessage << "\x1B[0m\n";
                    break;
                case LogMessageType::Warning:
                    std::cerr << "\x1B[33m[?] (" << channel.value() << ") " << formattedMessage << "\x1B[0m\n";
                    break;
            }
        }
        else
        {
            switch (type)
            {
                case LogMessageType::Log:
                    std::cout << "    " << formattedMessage << "\n";
                    break;
                case LogMessageType::Error:
                    std::cout << "\x1B[31m[!] " << formattedMessage << "\x1B[0m\n";
                    break;
                case LogMessageType::Warning:
                    std::cout << "\x1B[33m[?] " << formattedMessage << "\x1B[0m\n";
                    break;
            }
        }
    }

    void Logger::logFormatted(LogMessageType type, Option<std::string_view> channel, u32 verbosity, std::string_view formattedMessage)
    {
        assert(this->output_ != nullptr);
        if (verbosity <= this->verbosity_)
            this->output_->log(type, channel, formattedMessage);
        if (this->forwardTo_)
            this->forwardTo_->logFormatted(type, channel, verbosity, formattedMessage);
    }

    Logger::Logger(std::shared_ptr<LoggingContext> context, std::unique_ptr<LoggerOutput> output)
        : context_(std::move(context)), output_(std::move(output))
    {
        this->output_->init();
        this->verbosity_ = this->context_->maxVerbosity;
    }

    Logger& Logger::forwardTo(Logger& other)
    {
        assert(other.context_ == this->context_);
        if (this->forwardTo_)
            this->forwardTo_->forwardTo(other);
        else
            this->forwardTo_ = &other;
        return *this;
    }

    Logger& Logger::setVerbosity(u32 verbosity)
    {
        assert(verbosity <= this->context_->maxVerbosity);
        this->verbosity_ = verbosity;
        return *this;
    }

    void Logger::logTyped(LogMessageType type, StringId channel, u32 verbosity, const std::string& msg)
    {
        if (channel.isNull())
            this->logFormatted(type, {}, verbosity, msg);
        else
            this->logFormatted(type, this->context_->channels.get(channel), verbosity, msg);
    }
}
