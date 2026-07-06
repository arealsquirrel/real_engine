
/**
 * most of the code generated in this file is made by chatgpt
 * because I do not want to fucking write all these matrix tests
 */

#include "real/math/math.hpp"
#include "real/math/math_fwd.hpp"
#include "real/math/quaternion.hpp"
#include "real/math/vec3.hpp"
#include <real/math/mat3.hpp>
#include <real/math/mat4.hpp>
#include <gtest/gtest.h>

#include <gtest/gtest.h>

using namespace real;

class Mat3Test : public ::testing::Test {
protected:
	real::Mat3 A{
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

	real::Mat3 B{
        9, 8, 7,
        6, 5, 4,
        3, 2, 1
    };
};

class Mat4Test : public ::testing::Test
{
protected:
	real::Mat4 A{
        1,  2,  3,  4,
        5,  6,  7,  8,
        9, 10, 11, 12,
        13,14,15,16
	};

	real::Mat4 B{
        16,15,14,13,
        12,11,10, 9,
         8, 7, 6, 5,
         4, 3, 2, 1
	};
};

constexpr float EPSILON = 1e-5f;

class QuatTest : public ::testing::Test {
protected:
    Quaternion identity{1.0f, 0.0f, 0.0f, 0.0f};

    Vec3 xAxis{1.0f, 0.0f, 0.0f};
    Vec3 yAxis{0.0f, 1.0f, 0.0f};
    Vec3 zAxis{0.0f, 0.0f, 1.0f};
};


TEST(TestMath, Vectors) {

	EXPECT_EQ(Vec2(1.0f, 2.0f), Vec2(1.0f, 2.0f));
	EXPECT_EQ(Vec3(1.0f, 2.0f, 0.2f), Vec3(1.0f, 2.0f, 0.2f));
	EXPECT_EQ(Vec4(1.0f, 2.0f, 0.2f, -0.2), Vec4(1.0f, 2.0f, 0.2f, -0.2));

	EXPECT_EQ(5.0f, math::magnatude(Vec2(3,4)));
	EXPECT_EQ(Vec3(-2, 4, -2), math::cross(Vec3(1,2,3), Vec3(3,4,5)));
}

TEST(TestMath, Mat3) {
	using namespace real;
	Mat3 m (1,2,3, 4,5,6, 7,8,9);
	EXPECT_EQ(math::transpose(m).det(), m.det());
}

TEST_F(Mat3Test, MatrixAddition) {
    Mat3 expected{
        10, 10, 10,
        10, 10, 10,
        10, 10, 10
    };

    EXPECT_EQ(A + B, expected);
}

TEST_F(Mat3Test, ScalarAddition) {
    Mat3 expected{
        2, 3, 4,
        5, 6, 7,
        8, 9, 10
    };

    EXPECT_EQ(A + 1, expected);
}

TEST_F(Mat3Test, MatrixSubtraction) {
    Mat3 expected{
        -8, -6, -4,
        -2, 0, 2,
        4, 6, 8
    };

    EXPECT_EQ(A - B, expected);
}

TEST_F(Mat3Test, ScalarSubtraction) {
    Mat3 expected{
        0, 1, 2,
        3, 4, 5,
        6, 7, 8
    };

    EXPECT_EQ(A - 1, expected);
}

TEST_F(Mat3Test, MatrixMultiplication) {
    Mat3 expected{
        30, 24, 18,
        84, 69, 54,
        138,114, 90
    };

    EXPECT_EQ(A * B, expected);
}

TEST_F(Mat3Test, ScalarMultiplication) {
    Mat3 expected{
        2, 4, 6,
        8,10,12,
        14,16,18
    };

    EXPECT_EQ(A * 2, expected);
}

TEST_F(Mat4Test, MatrixAddition) {
    Mat4 expected{
        17,17,17,17,
        17,17,17,17,
        17,17,17,17,
        17,17,17,17
    };

    EXPECT_EQ(A + B, expected);
}

TEST_F(Mat4Test, ScalarAddition) {
    Mat4 expected{
        2,3,4,5,
        6,7,8,9,
        10,11,12,13,
        14,15,16,17
    };

    EXPECT_EQ(A + 1, expected);
}


TEST_F(Mat4Test, MatrixSubtraction) {
    Mat4 expected{
        -15,-13,-11,-9,
        -7,-5,-3,-1,
         1, 3, 5, 7,
         9,11,13,15
    };

    EXPECT_EQ(A - B, expected);
}


TEST_F(Mat4Test, ScalarSubtraction)
{
    Mat4 expected{
        0,1,2,3,
        4,5,6,7,
        8,9,10,11,
        12,13,14,15
    };

    EXPECT_EQ(A - 1, expected);
}

TEST_F(Mat4Test, MatrixMultiplication)
{
    Mat4 expected{
         80, 70, 60, 50,
        240,214,188,162,
        400,358,316,274,
        560,502,444,386
    };

    EXPECT_EQ(A * B, expected);
}

TEST_F(Mat4Test, ScalarMultiplication)
{
    Mat4 expected{
         2, 4, 6, 8,
        10,12,14,16,
        18,20,22,24,
        26,28,30,32
    };

    EXPECT_EQ(A * 2, expected);
}

TEST_F(Mat4Test, MatrixDivision)
{
    Mat4 C{
        16,32,48,64,
        80,96,112,128,
        144,160,176,192,
        208,224,240,256
    };

    Mat4 D{
        2,4,6,8,
        10,12,14,16,
        18,20,22,24,
        26,28,30,32
    };

    Mat4 expected{
        8,8,8,8,
        8,8,8,8,
        8,8,8,8,
        8,8,8,8
    };

    EXPECT_EQ(C / D, expected);
}

TEST_F(QuatTest, IdentityQuaternion) {
    EXPECT_FLOAT_EQ(math::qget_angle(identity), 0);

    Vec3 v(2.0f, 3.0f, 4.0f);
    Quaternion q = identity * v;
	EXPECT_EQ(q.v, v);
}

TEST_F(QuatTest, QuaternionMultiplication) {
    Quaternion qx = math::make_q_from_euler_angles(0, 90.0f, 0);
    Quaternion qy = math::make_q_from_euler_angles(0, 90.0f, 0);
    Quaternion result = qy * qx;

    EXPECT_NEAR(math::radians_to_degrees(math::qget_angle(result)), 180.0f, EPSILON);
}

TEST_F(QuatTest, GetAngle) {
    Quaternion q = math::make_q_from_euler_angles(0, 90.0f, 0);

    EXPECT_NEAR(math::radians_to_degrees(math::qget_angle(q)), 90.0f, 0.001f);
}

TEST_F(QuatTest, GetAxis) {
    Quaternion q = math::make_q_from_euler_angles(0, 90.0f, 0);
    Vec3 axis = math::qget_axis(q);

    EXPECT_NEAR(axis.x, 0.0f, 0.001f);
    EXPECT_NEAR(axis.y, 1.0f, 0.001f);
    EXPECT_NEAR(axis.z, 0.0f, 0.001f);
}

TEST_F(QuatTest, EulerRoundTrip) {
    Vec3 original(15.0f, -45.0f, 120.0f);

    Quaternion q = math::make_q_from_euler_angles(original.x, original.y, original.z);
    Vec3 converted = math::make_euler_angles_from_q(q);

    EXPECT_NEAR(converted.x, original.x, 0.001f);
    EXPECT_NEAR(converted.y, original.y, 0.001f);
    EXPECT_NEAR(converted.z, original.z, 0.001f);
}

