#pragma once
#include "types.h"
#include "string_id_map.h"
#include <string>
#include <format>

#define ENABLE_STATIC_LOGGING

#ifdef ENABLE_STATIC_LOGGING
// add static shit here
#endif

template <typename... Args>
void log(StringId channel, u32 verbosity, std::format_string<Args...> format, Args&&... args)
{
    static std::array<char, 256> buffer;
    std::format_to_n(buffer.begin(), buffer.size(), std::move(format), args...);
    std::cout << static_cast<const char*>(buffer.data()) << "\n";
}

namespace Log
{
    struct LoggingContext
    {
        u32 numLevelsVerbosity;
        StringIdMap<std::string_view> channels;
    };

    class LoggerOutput
    {
    public:
        virtual void init() = 0;
        virtual void log(std::string_view channel, std::string_view formattedMessage) = 0;
        virtual void err(std::string_view channel, std::string_view formattedMessage) = 0;
        virtual void warn(std::string_view channel, std::string_view formattedMessage) = 0;
        virtual ~LoggerOutput() = default;
    };

    class Logger
    {
    private:
        const LoggingContext& context_;
        u32 verbosity_;
        LoggerOutput* output_;
        Logger* forwardTo_;
    
    public:
        Logger() = delete;
    };
}