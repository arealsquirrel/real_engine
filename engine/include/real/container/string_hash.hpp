#ifndef REALLIB_STRING_HASH_HPP
#define REALLIB_STRING_HASH_HPP

#include "real/core/types.hpp"
#include <cstddef>
#include <cstring>
#include <string>

namespace real {

constexpr uint32_t REAL_FNV_PRIME = 0x01000193;

struct StringHash {
	StringHash() = default;

    StringHash(String str) {
		hash = 0xcbf29ce4;
        for (size_t c = 0; c < str.length(); c++) {
            hash ^= (u32)str[c];
            hash *= REAL_FNV_PRIME;
        }
    }

    ~StringHash() = default;

    u32 hash;

    operator u32() const { return hash; }
};

}

namespace std {
	template<>
	struct hash<real::StringHash> {
		std::size_t operator()(const real::StringHash &uuid) const {
			return uuid.hash;
		}
	};
}


#endif
