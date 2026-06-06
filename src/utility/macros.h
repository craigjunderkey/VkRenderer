#pragma once

// if error, return error; if result, just let it be the result

#define CHECK(result) if (!result) return Err<std::string>(std::move(result.error()));

#define TRY_ASSIGN(var, result) \
    if (!result) return Err<std::string>(std::move(result.error())); \
    var = result.value(); 
