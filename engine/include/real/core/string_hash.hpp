#ifndef REALLIB_STRING_HASH_HPP
#define REALLIB_STRING_HASH_HPP

#include "real/core/types.hpp"
#include <cstddef>
#include <cstring>

namespace real {

constexpr uint32_t REAL_FNV_PRIME = 0x01000193;

struct StringHash {
    constexpr StringHash(String str) {
        for (size_t c = 0; c < str.length(); c++) {
            hash ^= (u32)c;
            hash *= REAL_FNV_PRIME;
        }
    }

    ~StringHash() = default;

    u32 hash;
};

}

#endif