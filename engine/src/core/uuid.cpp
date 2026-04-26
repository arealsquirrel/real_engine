
#include <real/core/uuid.hpp>
#include <limits>
#include <random>

namespace real {

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<u64> dis(
			std::numeric_limits<u64>::min(),
			std::numeric_limits<u64>::max()
		);

UUID::UUID() {
	uuid = dis(gen);
}

UUID::UUID(const UUID &id) 
	: uuid(id) {}

UUID::UUID(u64 id)
	: uuid(id) {}

UUID::~UUID() = default;

}

