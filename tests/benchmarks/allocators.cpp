
#include "real/core/allocator.hpp"
#include "real/container/ref.hpp"
#include <array>
#include <cstdlib>
#include <memory>
#include <picobench.h>
#include <string>

using namespace real;

struct A : real::RefCounted {
	std::array<std::string, 23> arr;
};

struct B : real::RefCounted {
	std::array<int, 10> arr;
};

static void system_allocator(picobench::state &s) {
	SystemAllocator sys;
	for (auto _ : s) {
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
    }
}

static void page_allocator(picobench::state &s) {
	PageAllocator sys(100, 10);
	for (auto _ : s) {
		char *mem1 = sys.allocate_mem();
		char *mem2 = sys.allocate_mem();
		char *mem3 = sys.allocate_mem();
		char *mem4 = sys.allocate_mem();
		char *mem5 = sys.allocate_mem();
		sys.free_mem(mem4);
		sys.free_mem(mem2);
		mem4 = sys.allocate_mem();
		mem2 = sys.allocate_mem();
		sys.free_mem(mem1);
		sys.free_mem(mem3);
		sys.free_mem(mem5);
		sys.free_mem(mem4);
		sys.free_mem(mem2);
    }
}

static void freelist_allocator(picobench::state &s) {
	LinkedListAllocator sys(10000);
	for (auto _ : s) {
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
    }
}

static void freelist_ref_allocator(picobench::state &s) {
	LinkedListAllocator sys(10000);
	for (auto _ : s) {
		Ref<A> mem1 = create_ref<A>(&sys);
		Ref<B> mem2 = create_ref<B>(&sys);
		Ref<A> mem3 = create_ref<A>(&sys);
		Ref<B> mem4 = create_ref<B>(&sys);
		Ref<A> mem5 = create_ref<A>(&sys);

		{
			Ref<A> rmem1(mem1);
			Ref<B> rmem2(mem2);
			Ref<A> rmem3(mem3);
			Ref<B> rmem4(mem4);
			Ref<A> rmem5(mem5);
		}
    }
}

static void system_shared_ptr(picobench::state &s) {
	for (auto _ : s) {
		std::shared_ptr<A> mem1 = std::make_shared<A>();
		std::shared_ptr<B> mem2 = std::make_shared<B>();
		std::shared_ptr<A> mem3 = std::make_shared<A>();
		std::shared_ptr<B> mem4 = std::make_shared<B>();
		std::shared_ptr<A> mem5 = std::make_shared<A>();

		{
			std::shared_ptr<A> rmem1(mem1);
			std::shared_ptr<B> rmem2(mem2);
			std::shared_ptr<A> rmem3(mem3);
			std::shared_ptr<B> rmem4(mem4);
			std::shared_ptr<A> rmem5(mem5);
		}
    }
}

PICOBENCH(freelist_ref_allocator);
PICOBENCH(system_shared_ptr);

