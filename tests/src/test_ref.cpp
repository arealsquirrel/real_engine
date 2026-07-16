
#include "real/core/ref.hpp"
#include <gtest/gtest.h>

using namespace real;

struct A : real::RefCounted {
	A() = default;
	~A() = default;

	int x;
};

TEST(ref_test, count) {
	Ref<A> a(new A);
	a->x = 2;

	{
		Ref<A> ca(a);
		EXPECT_EQ(ca->x, 2);
		EXPECT_EQ(ca->get_reference_count(),2);
	}
}

