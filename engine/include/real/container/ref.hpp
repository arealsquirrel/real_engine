#ifndef REALLIB_REF_HPP
#define REALLIB_REF_HPP

#include "real/core/allocator.hpp"
#include "real/core/types.hpp"
#include <type_traits>
#include <utility>

namespace real {

template<typename T, typename Alloc>
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

	template<typename T, typename Alloc>
	friend class Ref;
};

/**
 * signifies shared ownership of a pointer and its lifetime
 * and must be created with an allocator :3
 */
template<typename T, typename Alloc=Allocator>
class Ref {
public:
	static_assert(std::is_base_of_v<RefCounted, T>, "T must derive from ref counted to be a Ref");

	Ref() {
		allocator = nullptr;
		object = nullptr;
	}

	template<typename ...Args>
	Ref(Alloc *_allocator, Args &&...args) 
		: allocator(_allocator) {
		
		object = allocator->template allocate_object<T>(std::forward<Args>(args)...);
		object->count++;
	}

	Ref(Alloc *_allocator, T *obj) 
		: object(obj), allocator(_allocator) {
		
		if(object != nullptr) {
			object->count++;
		}
	}

	Ref(const Ref<T, Alloc> &r) 
		: allocator(r.allocator) {

		object = r.object;

		if(object != nullptr) {
			object->count++;
		}
	}

	~Ref() {
		if(object == nullptr)
			return;

		object->count--;
		if(object->count == 0 && object != nullptr) {
			allocator->free_object(object);
		}
	}

	void operator =(const Ref<T> &r) {
		if(object != nullptr) {
			object->count--;
			if(object->count == 0 && object != nullptr) {
				allocator->free_object(object);
			}
		}

		object = r.object;
		allocator = r.allocator;

		if(object != nullptr)
			object->count++;
	}

	T *operator ->() { return object; }
	const T *operator ->() const { return object; }
	const T *get() const { return object; }
	T *get() { return object; }

	void reset() {
		if(object == nullptr)
			return;

		object->count--;
		if(object->count == 0 && object != nullptr) {
			allocator->free_object(object);
		}
		object = nullptr;
		allocator = nullptr;
	}

	operator bool() { return (object == nullptr); }

private:
	T *object;
	Alloc *allocator;

	template<typename Ts>
	friend class RefView;
};

template<typename T, typename Alloc=Allocator>
class UniquePointer {
public:
	UniquePointer() {
		object = nullptr;
		allocator = nullptr;
	}

	template<typename ...Args>
	UniquePointer(Alloc *_allocator, Args &&...args) 
		: allocator(_allocator) {
		object = allocator->template allocate_object<T>(std::forward<Args>(args)...);
	}

	~UniquePointer() {
		allocator->free_object(object);
	}

	UniquePointer(const UniquePointer<T, Alloc>&) = delete;
	void operator =(const UniquePointer<T, Alloc> &alloc) {
		object = alloc.object;
		allocator = alloc.allocator;
	}

	T *operator ->() { return object; }
	const T *operator ->() const { return object; }
	const T *get() const { return object; }
	T *get() { return object; }
	operator bool() { return (object == nullptr); }

private:
	T *object;
	Alloc *allocator;
};

template<typename T>
class RefView {
public:

};

}

#endif
