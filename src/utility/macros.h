#pragma once

#define CHECK(result) \
    { \
    auto _crazyJerry = std::move(result); \
    if (!_crazyJerry) return std::move(_crazyJerry.takeError()); }

#define CHECK_KEEP(result) if (!result) return std::move(result.takeError());

#define TRY_ASSIGN(var, result) \
    { \
    auto _crazyJerry = std::move(result); \
    if (!_crazyJerry) return std::move(_crazyJerry.takeError()); \
    var = _crazyJerry.take(); }

#define TO_STRING_LITERAL(x) #x

#define DECLARE_VK_RAII_WRAPPER_METHODS(wrappedType, field)             \
    wrappedType& operator*() { return field; }                  \
    const wrappedType& operator*() const { return field; }      \
    wrappedType* operator->() { return &field; }                \
    const wrappedType* operator->() const { return &field; }    \
    wrappedType&& take() { field = nullptr; return std::move(field); }
