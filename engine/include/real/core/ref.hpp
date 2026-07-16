#ifndef REALLIB_REF_HPP
#define REALLIB_REF_HPP

#include "real/core/types.hpp"
#include <type_traits>

namespace real {

template<typename T>
class Ref;

template<typename T>
class RefView;

class RefCounted {
public:
	RefCounted() = default;
	~RefCounted() = default;

	u32 get_reference_count() const { return count; }

private:
	u32 count {0};

	template<typename T>
	friend class Ref;
};

/**
 * signifies shared ownership of a pointer and its lifetime
 */
template<typename T>
class Ref {
public:
	static_assert(std::is_base_of_v<RefCounted, T>, "T must derive from ref counted to be a Ref");

	Ref(T *obj) 
		: object(obj) {
		
		obj->count++;
	}

	Ref(const Ref<T> &r) {
		object = (T*)r.object;
		object->count++;
	}

	~Ref() {
		object->count--;
		if(object->count == 0 && object != nullptr) {
			delete object;
		}
	}

	void operator =(const Ref<T> &r) {
		object->count--;
		if(object->count == 0 && object != nullptr) {
			delete object;
		}

		object = r.object;
		object->count++;
	}

	T *operator ->() { return object; }
	const T *operator ->() const { return object; }

private:
	T *object;

	template<typename Ts>
	friend class RefView;
};

}

#endif
