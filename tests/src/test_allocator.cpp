
#include "real/core/allocator.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include <array>
#include <cstdio>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

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

	LinkedListAllocator alloc(500);

	std::string *str = alloc.allocate_object<std::string>();
	int *str1 = alloc.allocate_object<int>();
	auto *str2 = alloc.allocate_object<std::array<int, 7>>();
	std::string *str4 = alloc.allocate_object<std::string>();
	int *str11 = alloc.allocate_object<int>();
	auto *str22 = alloc.allocate_object<std::array<int, 7>>();

	alloc.free_object(str2);
	alloc.free_object(str4);
	alloc.free_object(str1);

	LinkedListAllocator::Header *selected_block = nullptr;
	u32 total_size = 0;
	for(auto *iter = alloc.list_begin; iter != nullptr; iter = iter->next) {
		total_size += iter->size + sizeof(LinkedListAllocator::Header);
		std::cout << iter->used << " " << iter->size << std::endl;
	}

	EXPECT_EQ(total_size, 500);
}

