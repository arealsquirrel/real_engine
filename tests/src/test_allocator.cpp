
#include "real/core/allocator.hpp"
#include "real/core/logging.hpp"
#include <gtest/gtest.h>
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
	RL_LOG_INFO("here");

	auto str1 = (std::string*)(str_alloc.allocate_mem());
	str1->assign("hello");


	RL_LOG_INFO("here");
	auto str2 = (std::string*)(str_alloc.allocate_mem());
	str2->assign("hello");


	RL_LOG_INFO("here");
	str_alloc.free_mem((char*)str1);

	RL_LOG_INFO("here");
	auto str3 = (std::string*)(str_alloc.allocate_mem());
	str3->assign("hello");
}

