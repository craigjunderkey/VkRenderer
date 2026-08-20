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

    void ConsoleOutput::log(LogMessageDetails details, std::string_view message)
    {
        if (details.channel != StringId::NONE)
        {
            switch (details.type)
            {
                case LogMessageType::Log:
                    std::cout << "    (" << details.channel << ") " << message << "\n";
                    break;
                case LogMessageType::Error:
                    std::cerr << "\x1B[31m[!] (" << details.channel << ") " << message << "\x1B[0m\n";
                    break;
                case LogMessageType::Warning:
                    std::cerr << "\x1B[33m[?] (" << details.channel << ") " << message << "\x1B[0m\n";
                    break;
            }
        }
        else
        {
            switch (details.type)
            {
                case LogMessageType::Log:
                    std::cout << "    " << message << "\n";
                    break;
                case LogMessageType::Error:
                    std::cout << "\x1B[31m[!] " << message << "\x1B[0m\n";
                    break;
                case LogMessageType::Warning:
                    std::cout << "\x1B[33m[?] " << message << "\x1B[0m\n";
                    break;
            }
        }
    }

    void Logger::log(LogMessageDetails details, std::string_view message)
    {
        assert(this->output_ != nullptr);
        if (details.verbosity <= this->verbosity_)
        {
            this->output_->log(details, message);
        }
        if (this->forwardTo_)
        {
            this->forwardTo_->log(std::move(details), std::move(message));
        }
    }

    Logger::Logger(std::shared_ptr<LoggingContext> context, std::unique_ptr<LoggerOutput> output)
        : context_(std::move(context)), output_(std::move(output))
    {
        this->output_->init();
        this->verbosity_ = this->context_->maxVerbosity;
    }

    Logger& Logger::forwardTo(std::unique_ptr<Logger> other)
    {
        assert(other->context_ == this->context_);
        if (this->forwardTo_)
            this->forwardTo_->forwardTo(std::move(other));
        else
            this->forwardTo_ = std::move(other);
        return *this;
    }

    Logger& Logger::setVerbosity(u32 verbosity)
    {
        assert(verbosity <= this->context_->maxVerbosity);
        this->verbosity_ = verbosity;
        return *this;
    }
}
