
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

}

