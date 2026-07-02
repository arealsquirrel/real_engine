
#include <memory>
#include <real/core/object_container.hpp>

namespace real {

UniqueObjectSet::UniqueObjectSet() {
	elements = 0;
}

UniqueObjectSet::~UniqueObjectSet() = default;

bool UniqueObjectSet::emplace(std::shared_ptr<Object> ptr) {
	auto resp = obj_set.emplace(ptr->object_typeinfo()->id, elements++);
	obj_array.push_back(ptr);
	return resp.second;
}

bool UniqueObjectSet::erase(UUID id) {
	auto f = obj_set.find(id);
	if(f == obj_set.end())
		return false;

	obj_array.erase(obj_array.begin() + f->second);
	obj_set.erase(f);
	return true;
}

bool UniqueObjectSet::has(UUID id) {
	return (obj_set.find(id) != obj_set.end());
}

}
