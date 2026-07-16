
#include "real/core/core.hpp"
#include "real/core/logging.hpp"
#include <cstdlib>
#include <iostream>
#include <real/core/allocator.hpp>

namespace real {

Allocator::Allocator(u32 _alloc_size)
	: alloc_size(_alloc_size) {

	buffer = (char*)malloc(alloc_size);
}

Allocator::~Allocator() {
	RL_LOG_TRACE("ALLOCATOR DESTROY -> m_allocatedMem {}", allocated_mem);
	free(buffer);
}

SystemAllocator::SystemAllocator() 
	: Allocator(0) {
	
	allocated_mem = 0;
}

SystemAllocator::~SystemAllocator() {
	if(allocated_mem != 0) {
		RL_LOG_ERROR("Memory leak detected of {} bytes", allocated_mem);
	}
}

char *SystemAllocator::allocate_mem(u32 size) {
	allocated_mem += size;
	return (char*)malloc(size);
}

void SystemAllocator::free_mem(char *mem, u32 size) {
	allocated_mem -= size;
	free(mem);
}

StackAllocator::StackAllocator(u32 size) 
	: Allocator(size) {
	
	stack_pointer = buffer;
	allocated_mem = 0;
}

StackAllocator::~StackAllocator() {
}

char *StackAllocator::allocate_mem(u32 size) {
	if(allocated_mem+size > alloc_size)
		return nullptr;
		
	char *alloc = stack_pointer;
	stack_pointer += size;
	allocated_mem += size;
	return alloc;
}

void StackAllocator::free_mem(char *mem, u32 size) {
	if(allocated_mem-size < 0)
		return;
	
	stack_pointer -= size;
	allocated_mem -= size;
}

void StackAllocator::reset() {
	stack_pointer = buffer;
	allocated_mem = 0;
}

PageAllocator::PageAllocator(u32 _type_size, u32 amount_of_ts) 
	: Allocator((_type_size+sizeof(Header))*amount_of_ts + sizeof(Header)), type_size(_type_size) {

	char *pointer = buffer;
	for (size_t i = 0; i < amount_of_ts; i++) {
		Header *h = (Header*)pointer;
		pointer += sizeof(Header);
		pointer += type_size;
		h->next = (Header*)(pointer);
	}

	((Header*)pointer)->next = nullptr;

	free_list = (Header*)buffer;
	alloc_list_start = (Header*)buffer;
}

char *PageAllocator::allocate_mem(u32) {
	std::cout << "giving mem from free list" << std::endl;
	Header *alloc = free_list;
	free_list = free_list->next;
	return (char*)alloc + sizeof(Header);
}

void PageAllocator::free_mem(char *mem, u32) {
	Header *header = (Header*)(mem-sizeof(Header));
	header->next = free_list;
	free_list = header->next;
}

LinkedListAllocator::LinkedListAllocator(u32 size) 
	: Allocator(size) {

	list_begin = (Header*)buffer;
	list_begin->back = nullptr;
	list_begin->next = nullptr;
	list_begin->used = false;
	list_begin->size = size-sizeof(Header);
}

LinkedListAllocator::~LinkedListAllocator() = default;

char *LinkedListAllocator::allocate_mem(u32 size) {

	// very simple first free block search
	Header *selected_block = nullptr;
	for(Header *iter = list_begin; iter != nullptr; iter = iter->next) {
		if(iter->used == false && iter->size >= size) {
			selected_block = iter;
			break;
		}
	}

	if(selected_block == nullptr) {
		return nullptr;
	}

	Header *alloc_header = (Header*)(((char*)selected_block) + sizeof(Header) + size);
	alloc_header->used = false;
	alloc_header->size = selected_block->size-sizeof(Header)-size;
	alloc_header->next = selected_block->next;
	alloc_header->back = selected_block;
	selected_block->next = alloc_header;
	selected_block->size = size;
	selected_block->used = true;

	return ((char*)selected_block)+sizeof(Header);
}

void LinkedListAllocator::free_mem(char *mem, u32 size) {
	Header *h = (Header*)(mem - sizeof(Header));
	h->used = false;
	compact();
}

void LinkedListAllocator::compact() {
	Header *selected_block = nullptr;
	for(Header *iter = list_begin; iter != nullptr; iter = iter->next) {
		if(iter->used)
			continue;

		if(iter->next != nullptr && iter->next->used == false) {
			iter->size += iter->next->size + sizeof(Header);
			if(iter->next->next != nullptr)
				iter->next->next->back = iter;
			iter->next = iter->next->next;
		}

		if(iter->back != nullptr && iter->back->used == false) {
			iter->size += iter->back->size + sizeof(Header);
			if(iter->back->back != nullptr)
				iter->back->back->next = iter;
			iter->back = iter->back->back;
		}
	}
}

}

