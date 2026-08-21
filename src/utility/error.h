#pragma once
#include <concepts>
#include <string_view>

template<typename E>
concept IErrorReporter = std::convertible_to<E, std::string_view> && requires(E error) {
    error.report();
};

template<typename E>
void reportError(E& error) requires IErrorReporter<E>
{
    error.report();
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
