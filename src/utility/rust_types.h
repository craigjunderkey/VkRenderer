#pragma once
#include <expected>
#include <iostream>
#include <concepts>

// unit
struct Unit {};
inline Unit Ok()
{
    return Unit();
}


// control flow
inline void panic(std::string_view message)
{
    std::cerr << "\x1B[31mERROR: " << message << "\x1B[0m\n";
    std::exit(EXIT_FAILURE);
}

inline void panic()
{
    std::exit(EXIT_FAILURE);
}


// error
template<typename E>
concept IErrorReporter = requires(E error) {
    error.reportError();
    std::convertible_to<E, std::string_view>;
};

template<typename E>
void reportError(E& error) requires IErrorReporter<E>
{
    error.reportError();
}

template<typename E>
void reportError(E& error) requires std::convertible_to<E, std::string_view>
{
    std::cerr << "\x1B[31mERROR: " << static_cast<std::string_view>(error) << "\x1B[0m\n";
}

template<typename E>
void reportError(E& error)
{
    std::cerr << "\x1B[31mERROR (no additional info provided)\x1B[0m\n";
}

// result
template <typename T, typename E>
class Result
{
private:
    std::expected<T, E> val_;

    void useError()
    {
        assert(!this->val_.has_value());
        reportError<E>(this->val_.error());
        panic();
    }

public:
    
    // Result(std::expected<T, E> val) : val_(val) {}
    Result(std::expected<T, E>&& val) : val_(std::move(val)) {}

    // Result(std::unexpected<E> val) : val_(val) {}
    Result(std::unexpected<E>&& val) : val_(std::move(val)) {}

    // Result(E err) : val_(std::unexpected<E>(err)) {}
    Result(T&& val) : val_(std::expected<T, E>(std::move(val))) {}

    operator bool() const { return this->val_.has_value(); }
    operator std::expected<T, E>() { return std::move(this->val_); } // will this cause problems?

    T&& unwrap()
    {
        if (!this->val_)
            this->useError();

        return std::move(this->val_).value();
    }

    T&& expect(const char* message)
    {
        if (!this->val_)
            this->useError();
        
        return std::move(this->val_).value();
    }

    T&& take()
    {
        #ifdef NDEBUG
        if (!this->val_)
            this->useError();
        #endif

        return std::move(this->val_).value();
    }

    T& value()
    {
        assert(this->val_.has_value());
        return this->val_.value();
    }
    
    E& error()
    {
        assert(!this->val_.has_value());
        return this->val_.error();
    }

    inline bool isErr() const { return !this->val_.has_value(); }
    inline bool isOk() const { return this->val_.has_value(); }
};

template <typename E>
using Err = std::unexpected<E>;


// option
template <typename T>
using Option = std::optional<T>;

inline constexpr std::nullopt_t None()
{
    return std::nullopt;
}