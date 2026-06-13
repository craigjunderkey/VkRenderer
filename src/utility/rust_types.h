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
inline void panic(std::string message)
{
    std::cerr << "ERROR: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}

inline void print(const char* message)
{
    std::cout << message << std::endl;
}


// result
template <typename T, typename E>
class Result
{
private:
    std::expected<T, E> val_;

public:
    
    // Result(std::expected<T, E> val) : val_(val) {}
    Result(std::expected<T, E>&& val) : val_(std::move(val)) {}

    // Result(std::unexpected<E> val) : val_(val) {}
    Result(std::unexpected<E>&& val) : val_(std::move(val)) {}

    // Result(E err) : val_(std::unexpected<E>(err)) {}
    Result(T&& val) : val_(std::expected<T, E>(std::move(val))) {}

    operator bool() const { return this->val_.has_value(); }
    operator std::expected<T, E>() { return std::move(this->val_); } // will this cause problems?

    T&& unwrap() requires (not std::copy_constructible<T>)
    {
        if (!this->val_)
            panic(std::format("{}", this->val_.error()));

        return std::move(this->val_).value();
    }

    T unwrap() requires std::copy_constructible<T>
    {
        if (!this->val_)
            panic(std::format("{}", this->val_.error()));

        return this->val_.value();
    }

    T&& expect(const char* message) requires (not std::copy_constructible<T>)
    {
        if (!this->val_)
            panic(std::format("{}", this->val_.error()));
        
        return std::move(this->val_).value();
    }

    T expect(const char* message) requires std::copy_constructible<T>
    {
        if (!this->val_)
            panic(std::format("{}", this->val_.error()));
        
        return this->val_.value();
    }

    T&& take()
    {
        #ifdef NDEBUG
        if (!this->val_)
            panic(std::format("{}", this->val_.error()));
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