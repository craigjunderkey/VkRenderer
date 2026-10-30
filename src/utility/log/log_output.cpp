#include "log_output.h"
#include <chrono>
#include "../file_handling.h"

namespace Log
{
    std::ostream& displayChannel(std::ostream& out, StringId channel)
    {
        if (channel)
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
    
    FileOutput::FileOutput(std::filesystem::path logDirectory)
        : logDirectory_(logDirectory)
    {
    }

    void FileOutput::init(LoggingContext& context)
    {
        // clear log directory
        FileHandling::deleteRecursive(this->logDirectory_).unwrap();
        FileHandling::createDirectory(this->logDirectory_).unwrap();

        // create log files
        this->globalLogFile_ = std::ofstream(this->logDirectory_ / "_all.log");
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
        // get time string
        auto timeFmtResult = std::format_to_n(
            this->timeBuffer_.begin(),
            this->timeBuffer_.size(),
            "{:%T}",
            std::chrono::system_clock::now()
        );
        std::string_view time = std::string_view(this->timeBuffer_.data(), this->timeBuffer_.size());
        
        // log the message with time included
        fileLog(this->globalLogFile_, details, message, time, true);
        if (details.channel)
        {
            fileLog(this->logFiles_.get(details.channel), details, message, time, false);
        }
    }
}