#include "log_error.h"

namespace LogUtils
{
    ErrorMessage::ErrorMessage(Log::LogMessageDetails details, std::string_view text)
    {
        details.type = Log::LogMessageType::Error;
        this->message_ = getGlobalLogger().context()
            .makeBufferedMessage(std::move(details))
            .setText(text);
    }

    ErrorMessage::ErrorMessage(Log::Message&& message)
        : message_(std::move(message))
    {
    }
    
    void ErrorMessage::report()
    {
        getGlobalLogger().log(std::move(this->message_));
    }
}