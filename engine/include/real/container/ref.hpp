#ifndef REALLIB_REF_HPP
#define REALLIB_REF_HPP

#include "real/core/allocator.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include <type_traits>
#include <utility>

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
 * and must be created with an allocator :3
 */
template<typename T>
class Ref {
public:
	// static_assert(std::is_base_of_v<RefCounted, T>, "T must derive from ref counted to be a Ref");

	Ref() {
		allocator = nullptr;
		object = nullptr;
	}

	Ref(Allocator *_allocator, T *obj) 
		: object(obj), allocator(_allocator) {
		
		if(object != nullptr) {
			object->count++;
		}
	}

	Ref(const Ref<T> &r) 
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
			RL_LOG_TRACE("deleting {}", typeid(T).name());
			allocator->free_object(object);
		}
	}

	void operator =(const Ref<T> &r) {
		if(object != nullptr) {
			object->count--;
			if(object->count == 0) {
				RL_LOG_TRACE("deleting {}", typeid(T).name());
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

	// LOOK AWAY!!!
	/*
	void destroy() {
		allocator->free_object(object);
		allocator = nullptr;
		object = nullptr;
	}
	*/

	operator bool() { return (object == nullptr); }

private:
	T *object;
	Allocator *allocator;

	template<typename Ts>
	friend class RefView;
};

template<typename T>
class UniquePointer {
public:
	UniquePointer() {
		object = nullptr;
		allocator = nullptr;
	}

	UniquePointer(Allocator *_allocator, T *_object) 
		: allocator(_allocator) {
		object = _object;
	}

	~UniquePointer() {
		if(object != nullptr)
			allocator->free_object(object);
	}

	UniquePointer(UniquePointer &&other) {
		allocator = other.allocator;
		object = other.object;
		other.object = nullptr;
		other.allocator = nullptr;
	}

	UniquePointer(const UniquePointer<T>&) = delete;
	void operator =(const UniquePointer<T> &alloc) = delete;

	void operator =(UniquePointer<T> &&alloc) {
		object = alloc.object;
		allocator = alloc.allocator;
		alloc.object = nullptr;
		alloc.allocator = nullptr;
	}

	T *operator ->() { return object; }
	const T *operator ->() const { return object; }
	const T *get() const { return object; }
	T *get() { return object; }
	operator bool() { return (object == nullptr); }

	Ref<T> to_ref() { 
		auto r = Ref<T>(allocator, object);
		allocator = nullptr;
		object = nullptr;
		return r;
	}

	void destroy() {
		if(object != nullptr)
			allocator->free_object(object);

		allocator = nullptr;
		object = nullptr;
	}

	T *release() {
		auto *sobj = object;
		allocator = nullptr;
		object = nullptr;
		return sobj;
	}

private:
	T *object;
	Allocator *allocator;
};

template<typename T, typename ...Args>
inline UniquePointer<T> create_unique(Allocator *alloc, Args &&...args) { return UniquePointer<T>(alloc, alloc->allocate_object<T>(std::forward<Args>(args)...)); }

template<typename T, typename ...Args>
inline Ref<T> create_ref(Allocator *alloc, Args &&...args) { return Ref<T>(alloc, alloc->allocate_object<T>(std::forward<Args>(args)...)); }

}

#endif
