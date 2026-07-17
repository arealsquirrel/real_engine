
#include "real/container/string_hash.hpp"
#include <gtest/gtest.h>
#include <real/real.hpp>

using namespace real;

// Tests factorial of 0.
TEST(CoreTest, StringHash) {
	StringHash h1("hello");
	StringHash h2("nope");
	StringHash h3("hello");
	EXPECT_EQ(h1.hash, h3.hash);
	EXPECT_NE(h1.hash, h2.hash);
}
