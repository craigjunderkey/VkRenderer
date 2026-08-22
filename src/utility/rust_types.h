#pragma once
#include <expected>
#include <iostream>
#include <concepts>
#include "error.h"

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
    std::cerr << "\x1B[31mCalled panic.\x1B[0m\n";
    std::exit(EXIT_FAILURE);
}


// result
template <typename T, typename E>
class Result
{
private:
    std::expected<T, E> val_;

    void useError()
    {
        E error = std::move(this->val_).error();
        reportError<E>(error);
        panic();
    }

public:
    
    Result(std::expected<T, E>&& val) : val_(std::move(val)) {}

    Result(std::unexpected<E>&& val) : val_(std::move(val)) {}

    Result(T&& val) : val_(std::expected<T, E>(std::move(val))) {}

    operator bool() const { return this->val_.has_value(); }
    operator std::expected<T, E>() { return std::move(this->val_); }

    Result(Result<T, E>&) = delete;
    Result& operator=(Result<T, E>&) = delete;
    Result(Result<T, E>&&) = default;
    Result& operator=(Result<T, E>&&) = default;
    
    T&& unwrap()
    {
        if (!this->val_)
            this->useError();
        return *std::move(this->val_);
    }

    T&& expect(const char* message)
    {
        if (!this->val_)
            this->useError();
        
        return *std::move(this->val_);
    }

    T&& take()
    {
        if (!this->val_)
            this->useError();

        return *std::move(this->val_);
    }

    T& value()
    {
        assert(this->val_.has_value());
        return *this->val_;
    }
    
    E& error()
    {
        assert(!this->val_.has_value());
        return this->val_.error();
    }

    E&& takeError()
    {
        assert(!this->val_.has_value());
        return std::move(this->val_).error();
    }

    inline bool isErr() const { return !this->val_.has_value(); }
    inline bool isOk() const { return this->val_.has_value(); }
};

template <typename E>
using Err = std::unexpected<E>;


// option
template <typename T>
using Option = std::optional<T>;
