#pragma once

#define CHECK(result) \
    { \
    auto _crazyJerry = std::move(result); \
    if (!_crazyJerry) return Err<std::string>(std::move(_crazyJerry.error())); }

#define TRY_ASSIGN(var, result) \
    { \
    auto _crazyJerry = std::move(result); \
    if (!_crazyJerry) return Err<std::string>(std::move(_crazyJerry.error())); \
    var = _crazyJerry.value(); }