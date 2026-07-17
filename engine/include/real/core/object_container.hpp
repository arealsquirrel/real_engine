#ifndef REALLIB_OBJECT_CONTAINER_HPP
#define REALLIB_OBJECT_CONTAINER_HPP

#include "real/container/ref.hpp"
#include "real/core/allocator.hpp"
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
	UniqueObjectSet(Allocator *_allocator)
		: allocator(_allocator) {}

	~UniqueObjectSet() = default;

	template<typename T, typename ...Args>
	std::pair<bool, Ref<T>> make_emplace(Args &&...args) {
		Ref<Base> base_obj(allocator, (Base*)allocator->allocate_object<T>(std::forward<Args>(args)...));
		Ref<T> poly_obj(allocator, (T*)base_obj.get());
		return std::make_pair(emplace(base_obj), poly_obj);
	}

	template<typename T>
	Ref<T> get() {
		static_assert(std::is_base_of_v<Base, T>, "must derive from object");
		u32 index = obj_set.at(T::object_typeinfo_static()->id);
		return Ref<T>(allocator, (T*)obj_array[index].get());
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

	bool emplace(Ref<Base> ptr) {
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
	std::vector<Ref<Base>> obj_array;
	std::map<UUID, u32> obj_set;
	u32 elements {0};
	Allocator *allocator;
};

template<typename T, typename Base=Object>
class REALLIB_EXPORT ObjectSet {
public:
	ObjectSet(Allocator *_allocator)
		: allocator(_allocator) {}

	~ObjectSet() = default;

	template<typename X, typename ...Args>
	void emplace(Args &&...args) {
		static_assert(std::is_base_of_v<Base, X>, "must derive from object");
		auto obj = create_ref<X>(allocator, std::forward<Args>(args)...);
		return std::make_pair(emplace(obj), obj);
	}

	bool emplace(Ref<Object> ptr) {	
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
	std::vector<Ref<Object>> obj_array;
	std::map<UUID, u32> obj_set;
	u32 elements {0};
	Allocator *allocator;
};

}

#endif
