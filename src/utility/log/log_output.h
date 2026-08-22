#pragma once
#include "log.h"
#include <filesystem>
#include <fstream>
#include "../string_id_map.h"

namespace Log
{
    class ConsoleOutput : public LoggerOutput
    {
    public:
        virtual void init(LoggingContext& context) override {}
        virtual void log(LogMessageDetails, std::string_view message) override;
    };

    class FileOutput : public LoggerOutput
    {
    private:
        std::filesystem::path logDirectory_;
        std::ofstream globalLogFile_;
        StringIdMap<std::ofstream> logFiles_ { "log_files" };
        std::array<char, 8> timeBuffer_;

    public:
        FileOutput(std::filesystem::path logDirectory);

        virtual void init(LoggingContext& context) override;
        virtual void log(LogMessageDetails, std::string_view message) override;
    };
}