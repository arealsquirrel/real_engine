
#include "real/math/math.hpp"
#include "real/math/math_fwd.hpp"
#include "real/math/vec3.hpp"
#include <gtest/gtest.h>

TEST(TestMath, Vectors) {
	using namespace real;

	EXPECT_EQ(Vec2(1.0f, 2.0f), Vec2(1.0f, 2.0f));
	EXPECT_EQ(Vec3(1.0f, 2.0f, 0.2f), Vec3(1.0f, 2.0f, 0.2f));
	EXPECT_EQ(Vec4(1.0f, 2.0f, 0.2f, -0.2), Vec4(1.0f, 2.0f, 0.2f, -0.2));

	EXPECT_EQ(5.0f, math::magnatude(Vec2(3,4)));
	EXPECT_EQ(Vec3(-2, 4, -2), math::cross(Vec3(1,2,3), Vec3(3,4,5)));
}

