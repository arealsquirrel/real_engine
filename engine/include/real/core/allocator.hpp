#ifndef REALLIB_ALLOCATOR_HPP
#define REALLIB_ALLOCATOR_HPP

#include "real/core/core.hpp"
#include "real/container/span.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include <cstddef>
#include <iostream>

namespace real {

/*
 * this is the base class for all of our allocators
 *
 * the base class is just a wrapper for malloc and free
 * all other allocators will write their own implementation.
 *
 * please dont use this in it's abstact form, its mostly meant for storage
 */
class REALLIB_EXPORT Allocator {
public:
	/*
	 * give it the block thats its gonna manage
	 */
	Allocator(u32 _alloc_size);
	~Allocator();

public:
	virtual char *allocate_mem(u32 size) = 0;
	virtual void free_mem(char *mem, u32) = 0;

public:
	/* ------------- ALLOCATE ARRAY METHODS ------------- */

	/*
	 * calls the constructor for a list of T's with constructor
	 */
	template<class T, typename ...Args>
	[[nodiscard]]
	inline Span<T> allocate_array(u32 size, Args &&...args) {
		T *mem = reinterpret_cast<T*>(allocate_mem(sizeof(T) * size));
		for(u32 i = 0; i < size; i++)
			new (mem+i) T(std::forward<Args>(args)...);
		return Span<T>{size, mem};
	}

	/*
	 * calls the destructor on an array of T's
	 */
	template<class T>
	inline void free_array(Span<T> span) {
		for(u32 i = 0; i < span.elements; i++)
		 	(span.buffer+i)->~T();

		this->free_mem(RL_MEM_POINTER(span.buffer), span.elements*sizeof(T));
	}

	/* ------------- ALLOCATE OBJECT METHODS ------------- */

	/* allocates one T with constructor args Args... */
	template<class T, typename ...Args>
	[[nodiscard]]
	inline T *allocate_object(Args &&...args) {
		RL_LOG_INFO("Allocating object {}", typeid(T).name());
		T *mem = reinterpret_cast<T*>(allocate_mem(sizeof(T)));
		new (mem) T(std::forward<Args>(args)...);
		return mem;
	}

	/* calls the destructor on T */
	template<class T>
	inline void free_object(T *mem, u32 size=1) {
		RL_LOG_INFO("Freeing object {}", typeid(T).name());
		(mem)->~T();
		this->free_mem(RL_MEM_POINTER(mem), size*sizeof(T));
	}

public:
	u32 allocated_mem;
	const u32 alloc_size;
	char *buffer;
};

class REALLIB_EXPORT SystemAllocator : public Allocator {
public:
	SystemAllocator();
	~SystemAllocator();

	char *allocate_mem(u32 size) final override;
	void free_mem(char *mem, u32 size=0) final override;
};

class REALLIB_EXPORT StackAllocator : public Allocator {
public:
	StackAllocator(u32 size);
	~StackAllocator();

	char *allocate_mem(u32 size) final override;
	void free_mem(char *mem, u32 size) final override;

	void reset();

private:
	char *stack_pointer;
};

class REALLIB_EXPORT PageAllocator : public Allocator {
public:
	struct Header {
		Header *next;
	};

public:
	/**
	 * very important here, this input variable is not how many bytes are allocated,
	 * but how many T's can be stored in here
	 */
	PageAllocator(u32 _type_size, u32 amount_of_ts);
	~PageAllocator() = default;

	char *allocate_mem(u32 size=0);

	void free_mem(char *mem, u32 size=0);

	Header *alloc_list_start;

private:
	Header *free_list {nullptr};
	const size_t type_size;
};

class REALLIB_EXPORT LinkedListAllocator : public Allocator {
public:
	struct Header {
		Header *next;
		Header *back;
		u32 size;
		bool used;
	};

public:
	LinkedListAllocator(u32 size);
	~LinkedListAllocator();

	char *allocate_mem(u32 size=0);
	void free_mem(char *mem, u32 size=0);

	Header *list_begin;
	Header *list_end;

private:
	void compact(Header *iter);
	void print();

private:
	Header *last_alloc;
};

static Allocator *global_system_allocator() {
	static SystemAllocator alloc;
	return &alloc;
}

template<typename T, typename Alloc=Allocator>
class STDAllocator {
public:
	using value_type = T;

	STDAllocator(Alloc &alloc) noexcept
		: allocator(alloc) {};

    template <typename U, typename uAlloc>
    STDAllocator(const STDAllocator<U, uAlloc>&) noexcept {}

	T* allocate(std::size_t n) {
        if (n == 0) return nullptr;
        std::cout << "Allocating " << n << " element(s) of size " << sizeof(T) << "\n";
		Span<T> ptr = allocator.template allocate_array<T>(n);
		return ptr.buffer;
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t n) noexcept {
        std::cout << "Deallocating " << n << " element(s) of size " << sizeof(T) << "\n";
		Span<T> ptr = Span<T>{n, p};
		allocator.template free_array<T>(ptr);
    }

private:
	Alloc &allocator;
};

template<typename T>
using AllocSTDLinkedList = STDAllocator<T, LinkedListAllocator>;

template<typename T>
using AllocSTDPage = STDAllocator<T, PageAllocator>;

template<typename T>
using AllocSTDSystem = STDAllocator<T, SystemAllocator>;

template<typename T>
using AllocSTDStack = STDAllocator<T, StackAllocator>;

}

#endif
