#pragma once

#define CHECK(result) \
    { \
    auto _crazyJerry = std::move(result); \
    if (!_crazyJerry) return Err<std::string>(_crazyJerry.error()); }

#define CHECK_KEEP(result) if (!result) return Err<std::string>(result.error());

#define TRY_ASSIGN(var, result) \
    { \
    auto _crazyJerry = std::move(result); \
    if (!_crazyJerry) return Err<std::string>(_crazyJerry.error()); \
    var = _crazyJerry.take(); }

#define TO_STRING_LITERAL(x) #x

#define DECLARE_VK_RAII_WRAPPER_METHODS(wrappedType, field)             \
    wrappedType& operator*() { return field; }                  \
    const wrappedType& operator*() const { return field; }      \
    wrappedType* operator->() { return &field; }                \
    const wrappedType* operator->() const { return &field; }    \
    wrappedType&& take() { field = nullptr; return std::move(field); }
