#ifndef REALLIB_OBJECT_CONTAINER_HPP
#define REALLIB_OBJECT_CONTAINER_HPP

#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include <memory>
#include <real/core/object.hpp>
#include <utility>
#include <map>
#include <vector>

namespace real {

/**
 * @brief stores a set of objects where the type of each object is unique
 */

template<typename Base>
class REALLIB_EXPORT UniqueObjectSet {
public:
	UniqueObjectSet() = default;
	~UniqueObjectSet() = default;

	template<typename T, typename ...Args>
	std::pair<bool, Shared<T>> make_emplace(Args &&...args) {
		auto obj = std::make_shared<T>(std::forward<Args>(args)...);
		return std::make_pair(emplace(obj), obj);
	}

	template<typename T>
	std::shared_ptr<T> get() {
		static_assert(std::is_base_of_v<Base, T>, "must derive from object");
		u32 index = obj_set.at(T::object_typeinfo_static()->id);
		return std::static_pointer_cast<T>(obj_array[index]);
	}

	template<typename T>
	bool has() {
		static_assert(std::is_base_of_v<Base, T>, "must derive from object");
		return has(T::object_type_info_static()->id);
	}
	
	template<typename T>
	bool erase() {
		static_assert(std::is_base_of_v<Base, T>, "must derive from object");
		return erase(T::object_typeinfo_static()->id);
	} 

	bool emplace(std::shared_ptr<Base> ptr) {
		auto resp = obj_set.emplace(ptr->object_typeinfo()->id, elements++);
		obj_array.push_back(ptr);
		return resp.second;
	}

	bool erase(UUID id) {
		auto f = obj_set.find(id);
		if(f == obj_set.end())
			return false;

		obj_array.erase(obj_array.begin() + f->second);
		obj_set.erase(f);
		return true;
	}

	bool has(UUID id) {
		return (obj_set.find(id) != obj_set.end());
	}

	auto begin() { return obj_array.begin(); }
	auto end() { return obj_array.end(); }

private:
	std::vector<Shared<Base>> obj_array;
	std::map<UUID, u32> obj_set;
	u32 elements {0};
};

template<typename T, typename Base=Object>
class REALLIB_EXPORT ObjectSet {
public:
	ObjectSet() = default;
	~ObjectSet() = default;

	template<typename X, typename ...Args>
	void emplace(Args &&...args) {
		static_assert(std::is_base_of_v<Base, X>, "must derive from object");
		auto obj = std::make_shared<X>(std::forward<X>(args)...);
		return std::make_pair(emplace(obj), obj);
	}

	bool emplace(std::shared_ptr<Object> ptr) {	
		auto resp = obj_set.emplace(ptr->get_instance_uuid(), elements++);
		obj_array.push_back(ptr);
		return resp.second;
	}

	bool erase(UUID id) {
		auto f = obj_set.find(id);
		if(f == obj_set.end())
			return false;
		obj_array.erase(obj_array.begin() + f->second);
		obj_set.erase(f);
		elements--;
		return true;
	}

	bool has(UUID id) {
		return (obj_set.find(id) != obj_set.end());
	}

	auto begin() { return obj_array.begin(); }
	auto end() { return obj_array.end(); }

private:
	std::vector<Shared<Object>> obj_array;
	std::map<UUID, u32> obj_set;
	u32 elements {0};
};

}

#endif
