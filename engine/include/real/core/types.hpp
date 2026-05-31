#ifndef REALLIB_TYPES_HPP
#define REALLIB_TYPES_HPP

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

// namespace real {

// type sizes as defined by this
// https://en.cppreference.com/w/cpp/language/types.html
// 32-bit = ILP32
// 64-bit = LLP64 for windows, LP64 for unix and linux
using u8 = unsigned char;
using i8 = char;

using u16 = unsigned short int;
using i16 = short int;

using u32 = unsigned int;
using i32 = int;

using u64 = unsigned long long int;
using i64 = long long int;

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T, typename ...Args>
Shared<T> make_shared(Args&& ...args) { return std::make_shared<T>(std::forward<Args>(args)...); }

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T, typename ...Args>
Unique<T> make_unique(Args&& ...args) { return std::make_unique<T>(std::forward<Args>(args)...); }

using String = std::string;
using StringView = std::string_view;
using Path = std::filesystem::path;

template<typename T>
using Optional = std::optional<T>;

// }

#endif
