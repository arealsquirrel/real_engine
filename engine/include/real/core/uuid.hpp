
#ifndef SC_UUID_HPP
#define SC_UUID_HPP

#include <functional>
#include <real/core/types.hpp>

namespace real {

struct UUID {
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
		std::size_t operator()(const real::UUID &uuid) {
			return uuid.uuid;
		}
	};
}

#endif
