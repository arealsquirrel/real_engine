
#include "real/core/allocator.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include <array>
#include <cstdio>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

TEST(AllocatorTest, SystemAllocator) {
	using namespace real;
	SystemAllocator sys;
	auto *str = sys.allocate_object<std::string>("wazzup");
	EXPECT_EQ(str->compare("wazzup"), 0);
	sys.free_object<std::string>(str);
}

TEST(AllocatorTest, StackAllocator) {
	using namespace real;
	StackAllocator alloc(sizeof(std::string) * 10);
	
	{
		auto *str = alloc.allocate_object<std::string>("wazzup");
		EXPECT_EQ(str->compare("wazzup"), 0);
	}

	
	{
		auto *str = alloc.allocate_object<std::string>("wazzup");
		EXPECT_EQ(str->compare("wazzup"), 0);
	}

	alloc.free_mem(nullptr, sizeof(std::string));
	alloc.free_mem(nullptr, sizeof(std::string));
}

TEST(AllocatorTest, PageAllocator) {
	using namespace real;

	PageAllocator str_alloc(sizeof(std::string), 10);
	u32 header_count = 0;
	PageAllocator::Header *itr = str_alloc.alloc_list_start;
	while(itr != nullptr) {
		itr = itr->next;
		header_count++;
	}

	EXPECT_EQ(header_count, 11);

	std::string *str1 = str_alloc.allocate_object<std::string>();
	str1->assign("hello");

	std::string *str2 = str_alloc.allocate_object<std::string>();
	str2->assign("hello");

	str_alloc.free_mem((char*)str1);

	std::string *str3 = str_alloc.allocate_object<std::string>();
	str3->assign("hello");
}

TEST(AllocatorTest, LinkedAllocator) {
	using namespace real;

	LinkedListAllocator sys(10000);
	char *mem1 = sys.allocate_mem(rand() % 500);
	char *mem2 = sys.allocate_mem(rand() % 500);
	char *mem3 = sys.allocate_mem(rand() % 500);
	char *mem4 = sys.allocate_mem(rand() % 500);
	char *mem5 = sys.allocate_mem(rand() % 500);
	sys.free_mem(mem4);
	sys.free_mem(mem2);
	mem4 = sys.allocate_mem(rand() % 500);
	mem2 = sys.allocate_mem(rand() % 500);
	sys.free_mem(mem1);
	sys.free_mem(mem3);
	sys.free_mem(mem5);
	sys.free_mem(mem4);
	sys.free_mem(mem2);

	LinkedListAllocator::Header *selected_block = nullptr;
	u32 total_size = 0;
	for(auto *iter = sys.list_begin; iter != nullptr; iter = iter->next) {
		total_size += iter->size + sizeof(LinkedListAllocator::Header);
		std::cout << iter->used << " " << iter->size << "[" << iter << "][" << iter->back << "," << iter->next << "]" << std::endl;
	}

	EXPECT_EQ(total_size, 10000);
}

TEST(AllocatorTest, in_std) {
	using namespace real;
	LinkedListAllocator sys(10000);
	STDAllocator<int, LinkedListAllocator> vec_alloc(sys);
	std::vector<int, STDAllocator<int, LinkedListAllocator>> my_vec(vec_alloc);

	my_vec.push_back(3);
}

