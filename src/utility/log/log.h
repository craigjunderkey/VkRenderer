#pragma once
#include "../types.h"
#include "../object_pool.h"
#include <unordered_map>
#include <format>
#include <memory>
#include <initializer_list>
#include <source_location>

#define LOG_BUFFER_SIZE 2048

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

    struct LoggingContext;

    struct LogFilter
    {
        u64 mask = 0xFFFFFFFFFFFFFFFF;

        constexpr LogFilter() = default;
        constexpr LogFilter(u64 mask) : mask(mask) {}
        LogFilter(LoggingContext& context, std::initializer_list<StringId> ids);

        bool containsChannel(LoggingContext& context, StringId id) const;

        inline constexpr LogFilter operator&(const LogFilter& other) const { return this->mask & other.mask; }
        inline constexpr LogFilter& operator&=(const LogFilter& other) { this->mask &= other.mask; return *this; }
        inline constexpr LogFilter operator|(const LogFilter& other) const { return this->mask | other.mask; }
        inline constexpr LogFilter& operator|=(const LogFilter& other) { this->mask |= other.mask; return *this;}
        inline constexpr LogFilter operator^(const LogFilter& other) const { return this->mask ^ other.mask; }
        inline constexpr LogFilter& operator^=(const LogFilter& other) { this->mask ^= other.mask; return *this; }
        inline constexpr LogFilter operator~() const { return ~this->mask; }
        inline constexpr LogFilter operator<<(u64 n) const { return this->mask << n; }
        inline constexpr LogFilter& operator<<=(u64 n) { this->mask <<= n; return *this; }
        inline constexpr LogFilter operator>>(u64 n) const { return this->mask >> n; }
        inline constexpr LogFilter& operator>>=(u64 n) { this->mask >>= n; return *this; }
        inline constexpr operator bool() const { return static_cast<bool>(this->mask); }
    };

    struct BufferedLogMessage
    {
    private:
        PoolPtr<LogBuffer> buffer_ = PoolPtr<LogBuffer>();

    public:
        LogMessageDetails details = {};
        
        BufferedLogMessage() = default;
        BufferedLogMessage(PoolPtr<LogBuffer>&& buffer) : buffer_(std::move(buffer)) {}
        BufferedLogMessage(BufferedLogMessage&) = delete;
        BufferedLogMessage& operator=(BufferedLogMessage&) = delete;
        BufferedLogMessage(BufferedLogMessage&&) = default;
        BufferedLogMessage& operator=(BufferedLogMessage&&) = default;

        // formats the string in this's buffer
        template <typename... Args>
        BufferedLogMessage&& setText(std::format_string<Args...> format, Args&&... args)
        {
            assert(this->buffer_ != nullptr);
            auto fmtResult = std::format_to_n(
                this->buffer_->begin(),
                this->buffer_->size() - 1,
                std::forward<std::format_string<Args...>>(format),
                std::forward<Args>(args)...
            );
            (*this->buffer_.get())[fmtResult.size] = '\0';
            return std::move(*this);
        }
        
        // copies contents of `text` into this's buffer
        BufferedLogMessage&& setText(std::string_view text);

        std::string_view getText() const;
        constexpr operator std::string_view() const { return this->getText(); }
    };

    using Message = BufferedLogMessage;

    struct LoggingContext
    {
    private:
        std::array<StringId, 64> channels_;
        std::unordered_map<StringId, u32> channelIndices_ = std::unordered_map<StringId, u32>();
        u32 numChannels_;

    public:
        u32 maxVerbosity;
        ObjectPool<LogBuffer> buffers = ObjectPool<LogBuffer>(7, true, "log_buffers");
        LogBuffer immediateBuffer;

        LoggingContext(u32 maxVerbosity, std::initializer_list<std::string_view> channelNames);

        constexpr const std::array<StringId, 64>& getChannels() const { return this->channels_; }
        constexpr u32 numChannels() const { return this->numChannels_; }
        bool hasChannel(StringId channel) const;
        Option<u32> getChannelIndex(StringId channel) const;
        
        BufferedLogMessage makeBufferedMessage(LogMessageDetails details);
    };

    class LoggerOutput
    {
    public:
        virtual void init(LoggingContext& context) = 0;
        virtual void log(LogMessageDetails details, std::string_view message) = 0;
        virtual ~LoggerOutput() = default;
    };

    class Logger
    {
    private:
        std::unique_ptr<Logger> forwardTo_ = nullptr;
        std::shared_ptr<LoggingContext> context_ = nullptr;
        std::unique_ptr<LoggerOutput> output_ = nullptr;

    public:
        u32 verbosity = 0;
        LogFilter filter = LogFilter();

        Logger() = default;
        Logger(std::shared_ptr<LoggingContext> context, std::unique_ptr<LoggerOutput> output);

        Logger* forwardTo(std::unique_ptr<Logger> other);
        Logger* setVerbosity(u32 verbosity);
        Logger* setFilter(LogFilter filter);
        Logger* setFilter(std::initializer_list<StringId> channels);
        
        LoggingContext& context()
        {
            assert(this->context_ != nullptr);
            return *this->context_;
        }

        LoggerOutput& output()
        {
            assert(this->output_ != nullptr);
            return *this->output_;
        }

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

        void log(BufferedLogMessage& message);
        void log(BufferedLogMessage&& message);
    };
}
