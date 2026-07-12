#ifndef RL_UUID_HPP
#define RL_UUID_HPP

#include <functional>
#include <real/core/core.hpp>
#include <real/core/types.hpp>

namespace real {

struct REALLIB_EXPORT UUID {
public:
	UUID();
	UUID(const UUID &id);
	UUID(u64 id);
	~UUID();

	operator u64() const { return uuid; }
	
	bool operator ==(const UUID &uuid) { return (uuid.uuid == uuid); }

public:
	u64 uuid;
};

}

namespace std {
	template<>
	struct hash<real::UUID> {
		std::size_t operator()(const real::UUID &uuid) const {
			return uuid.uuid;
		}
	};
}

#endif
