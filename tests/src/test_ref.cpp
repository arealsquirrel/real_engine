
#include "real/core/allocator.hpp"
#include "real/container/ref.hpp"
#include <gtest/gtest.h>

using namespace real;

struct A : real::RefCounted {
	A() = default;
	~A() = default;

	int x;
};

TEST(ref_test, count) {
	LinkedListAllocator alloc(500);

	{
		Ref<A> a;
		a = create_ref<A>(&alloc);
		a->x = 2;

		{
			Ref<A> ca(a);
			EXPECT_EQ(ca->x, 2);
			EXPECT_EQ(ca->get_reference_count(), 2);
		}

		a.reset();
	}
}

TEST(ref_test, UniquePointer) {
	LinkedListAllocator alloc(500);

	{
		UniquePointer<A> a;
		a = create_unique<A>(&alloc);
	}
}

