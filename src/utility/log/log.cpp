#include "log.h"
#include <algorithm>

namespace Log
{
    LoggingContext::LoggingContext(u32 maxVerbosity, std::initializer_list<std::string_view> channelNames)
        : maxVerbosity(maxVerbosity)
    {
        this->channelIndices_.reserve(64);
        this->numChannels_ = channelNames.size();
        
        u32 i = 0;
        for (std::string_view name : channelNames)
        {
            StringId id = StringId::internString(name);
            this->channels_[i] = id;
            this->channelIndices_.add(id, i);
            i++;
        }
        std::sort(this->channels_.begin(), this->channels_.begin() + this->numChannels_);
    }

    bool LoggingContext::hasChannel(StringId channel) const
    {
        return this->getChannelIndex(channel).has_value();
    }

    Option<u32> LoggingContext::getChannelIndex(StringId channel) const
    {
        Option<const u32*> id = this->channelIndices_.tryGet(channel);
        if (id)
        {
            return *id.value();
        }
        else
        {
            return {};
        }
    }

    Logger::Logger(std::shared_ptr<LoggingContext> context, std::unique_ptr<LoggerOutput> output)
        : context_(std::move(context)), output_(std::move(output))
    {
        assert(this->context_ != nullptr);
        assert(this->output_ != nullptr);
        this->output_->init(*this->context_);
        this->verbosity = this->context_->maxVerbosity;
    }

    Logger* Logger::forwardTo(std::unique_ptr<Logger> other)
    {
        assert(other->context_ == this->context_);
        if (this->forwardTo_)
            this->forwardTo_->forwardTo(std::move(other));
        else
            this->forwardTo_ = std::move(other);
        return this;
    }

    Logger* Logger::setVerbosity(u32 verbosity)
    {
        assert(verbosity <= this->context_->maxVerbosity);
        this->verbosity = verbosity;
        return this;
    }

    Logger* Logger::setFilter(LogFilter filter)
    {
        this->filter = filter;
        return this;
    }

    Logger* Logger::setFilter(std::initializer_list<StringId> channels)
    {
        assert(this->context_ != nullptr);
        this->filter = LogFilter(*this->context_, std::move(channels));
        return this;
    }
    
    LogFilter::LogFilter(LoggingContext& context, std::initializer_list<StringId> ids)
    {
        u64 idsMask = 0;
        for (StringId id : ids)
        {
            Option<u32> index = context.getChannelIndex(id);
            if (!index)
                std::cerr << "ERROR: " << id << " not in logging context channels.\n";
            u64 channelMask = 0x1 << index.value();
            idsMask |= channelMask;
        }
        this->mask = idsMask;
    }

    bool LogFilter::containsChannel(LoggingContext& context, StringId id) const
    {
        Option<u32> index = context.getChannelIndex(id);
        if (!index)
            std::cerr << "ERROR: " << id << " not in logging context channels.\n";
        u64 channelMask = 0x1 << index.value();
        return static_cast<bool>(this->mask & channelMask);
    }

    BufferedLogMessage&& BufferedLogMessage::setText(std::string_view message)
    {
        memcpy(this->buffer_->data(), message.data(), message.size());
        this->text_ = std::string_view(this->buffer_->data(), message.size());
        return std::move(*this);
    }

    BufferedLogMessage LoggingContext::makeBufferedMessage(LogMessageDetails details)
    {
        BufferedLogMessage message = BufferedLogMessage(std::move(this->buffers.alloc()));
        message.details = std::move(details);
        return std::move(message);
    }

    void Logger::log(BufferedLogMessage& message)
    {
        this->log(message.details, message.getText());
    }

    void Logger::log(BufferedLogMessage&& message)
    {
        this->log(message.details, message.getText());
    }
}
