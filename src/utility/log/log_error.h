#pragma once
#include "log.h"
#include "log_utils.h"
#include "../rust_types.h"

namespace LogUtils
{
    class ErrorMessage
    {
    private:
        Log::Message message_ = Log::Message();

    public:
        ErrorMessage() = default;
        ErrorMessage(ErrorMessage&) = delete;
        ErrorMessage& operator=(ErrorMessage&) = delete;
        ErrorMessage(ErrorMessage&&) = default;
        ErrorMessage& operator=(ErrorMessage&&) = default;

        ErrorMessage(Log::LogMessageDetails details, std::string_view text);

        template <typename... Args>
        ErrorMessage(Log::LogMessageDetails details, std::format_string<Args...> format, Args&&... args)
        {
            details.type = Log::LogMessageType::Error;
            this->message_ = getGlobalLogger().context()
                .makeBufferedMessage(std::move(details))
                .setText(
                    std::forward<std::format_string<Args...>>(format),
                    std::forward<Args>(args)...
                );
        }

        // transfers message's contents to this
        ErrorMessage(Log::Message&& message);
    
        // logs and clears this's message contents
        void report();

        // transfers the contents of this into a `Result<T, ErrorMessage>`
        template<typename T>
        operator Result<T, ErrorMessage>()
        {
            Result<T, ErrorMessage> result = Err<ErrorMessage>(
                ErrorMessage(std::move(this->message_))
            );
            return result;
        }

        operator std::string_view() const { return this->message_.getText(); }
    };
}