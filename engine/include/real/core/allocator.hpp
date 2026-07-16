#ifndef REALLIB_ALLOCATOR_HPP
#define REALLIB_ALLOCATOR_HPP

#include "real/core/core.hpp"
#include "real/core/logging.hpp"
#include "real/core/span.hpp"
#include "real/core/types.hpp"
#include <cstddef>

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

	/* allocates one T with constructor args Arsg... */
	template<class T, typename ...Args>
	[[nodiscard]]
	inline T *allocate_object(Args &&...args) {
		T *mem = reinterpret_cast<T*>(allocate_mem(sizeof(T)));
		new (mem) T(std::forward<Args>(args)...);
		return mem;
	}

	/* calls the destructor on T */
	template<class T>
	inline void free_object(T *mem, u32 size=1) {
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

	inline char *allocate_mem(u32 size) final override;
	inline void free_mem(char *mem, u32 size) final override;
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
	PageAllocator(u32 _type_size, u32 amount_of_ts) 
		: Allocator((_type_size+sizeof(Header))*amount_of_ts + sizeof(Header)), type_size(_type_size) {
	
		char *pointer = buffer;
		for (size_t i = 0; i < amount_of_ts; i++) {
			Header *h = (Header*)pointer;
			pointer += sizeof(Header);
			pointer += type_size;
			h->next = (Header*)(pointer);
		}

		((Header*)pointer)->next = nullptr;

		alloc_list_start = (Header*)buffer;
		alloc_list_head = (Header*)buffer;
	}

	~PageAllocator() = default;

	char *allocate_mem(u32 size=0) final override {
		if(free_list == nullptr) {
			if(alloc_list_head->next == nullptr) {
				RL_LOG_WARN("Page Allocator out of memory");
				return nullptr;
			}

			Header *alloc = alloc_list_head;
			alloc_list_head = alloc_list_head->next;
			return (char*)alloc + sizeof(Header);
		}

		Header *alloc = free_list;
		free_list = free_list->next;
		return (char*)alloc + sizeof(Header);
	}

	void free_mem(char *mem, u32 size=0) final override {
		Header *header = (Header*)(mem-sizeof(Header));
		header->next = nullptr;
		free_list->next = header;
	}

	Header *alloc_list_start;

private:
	Header *alloc_list_head;
	Header *free_list {nullptr};
	const size_t type_size;
};

}

#endif
