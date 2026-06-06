#pragma once

// if error, return error; if result, just let it be the result

#define CHECK(result) if (!result) return Err<std::string>(std::move(result.error()));

#define TRY_ASSIGN(var, result) \
    { \
    auto _crazyJerry = std::move(result); \
    if (!_crazyJerry) return Err<std::string>(std::move(_crazyJerry.error())); \
    var = _crazyJerry.value(); }