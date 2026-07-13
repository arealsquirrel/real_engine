#ifndef REALLIB_TEMPLATES_HPP
#define REALLIB_TEMPLATES_HPP

#include <type_traits>
#include <vector>

namespace real {

template<typename Test, template<typename...> class Ref>
struct is_specialization : std::false_type {};
template<template<typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref>: std::true_type {};

struct not_std_vector {};
template<typename>
struct std_type_vector_v { using VectorType = not_std_vector; };
template<typename T, typename A>
struct std_type_vector_v<std::vector<T,A>> { using VectorType = T; };
template<typename T>
using std_vector_type = typename std_type_vector_v<T>::VectorType;
}

#endif
