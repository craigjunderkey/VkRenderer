#include "log_output.h"
#include <chrono>

namespace Log
{
    std::ostream& displayChannel(std::ostream& out, StringId channel)
    {
        if (channel != StringId::NONE)
        {
            out << "(" << channel.string() << ") ";
        }
        return out;
    }

    void ConsoleOutput::log(LogMessageDetails details, std::string_view message)
    {
        switch (details.type)
        {
            case LogMessageType::Log:
                std::cout << "    ";
                displayChannel(std::cout, details.channel) << message << "\n";
                break;
            case LogMessageType::Error:
                std::cerr << "\x1B[31m[!] ";
                displayChannel(std::cerr, details.channel) << message
                    << "\n    in " << details.sourceLoc.file_name() << "\n    " << details.sourceLoc.function_name()
                    << " @ line " << details.sourceLoc.line() << "\x1B[0m\n";
                break;
            case LogMessageType::Warning:
                std::cerr << "\x1B[33m[?] ";
                displayChannel(std::cerr, details.channel) << message << "\x1B[0m\n";
                break;
        }
    }

    void Logger::log(LogMessageDetails details, std::string_view message)
    {
        assert(this->output_ != nullptr);
        assert(this->context_ != nullptr && this->context_->hasChannel(details.channel));
        if (details.verbosity <= this->verbosity && this->filter.containsChannel(*this->context_, details.channel))
        {
            this->output_->log(details, message);
        }
        if (this->forwardTo_)
        {
            this->forwardTo_->log(std::move(details), std::move(message));
        }
    }
    
    FileOutput::FileOutput(std::filesystem::path logDirectory)
        : logDirectory_(logDirectory)
    {
    }

    void FileOutput::init(LoggingContext& context)
    {
        // clear log directory
        std::filesystem::remove_all(this->logDirectory_);
        std::filesystem::create_directory(this->logDirectory_);

        // create log files
        this->globalLogFile_ = std::ofstream(this->logDirectory_ / "_global.log");
        this->globalLogFile_ << "all channels" << std::endl;
        for (u32 i = 0; i < context.numChannels(); i++)
        {
            StringId channel = context.getChannels()[i];
            std::string_view channelName = channel.string();
            std::filesystem::path fileName = channelName;
            fileName.replace_extension(".log");
            std::ofstream file = std::ofstream(this->logDirectory_ / fileName);
            file << "channel: " << channelName << std::endl;
            this->logFiles_.add(channel, std::move(file));
        }

        this->logDirectory_.clear();
    }

    void fileLog(std::ofstream& out, LogMessageDetails& details, std::string_view message,
        std::string_view time, bool channelDisplayEnabled)
    {
        switch (details.type)
        {
            case LogMessageType::Log:
                out << time << " - ";
                if (channelDisplayEnabled)
                    displayChannel(out, details.channel);
                out << message << std::endl;
                break;
            case LogMessageType::Error:
                out << time << " - [ERROR] ";
                if (channelDisplayEnabled)
                    displayChannel(out, details.channel);
                out << message
                    << "\n\tin " << details.sourceLoc.file_name() << "\n\t" << details.sourceLoc.function_name()
                    << " @ line " << details.sourceLoc.line() << std::endl;
                break;
            case LogMessageType::Warning:
                out << time << " - [WARNING] ";
                if (channelDisplayEnabled)
                    displayChannel(out, details.channel);
                out << message << std::endl;
                break;
        }
    }

    void FileOutput::log(LogMessageDetails details, std::string_view message)
    {
        auto timeFmtResult = std::format_to_n(
            this->timeBuffer_.begin(),
            this->timeBuffer_.size(),
            "{:%T}",
            std::chrono::system_clock::now()
        );
        
        std::string_view time = std::string_view(this->timeBuffer_.data(), this->timeBuffer_.size());
        
        fileLog(this->globalLogFile_, details, message, time, true);
        fileLog(this->logFiles_.get(details.channel), details, message, time, false);
    }
}