#ifndef REALLIB_MAT4_HPP
#define REALLIB_MAT4_HPP

// #include "real/math/math.hpp"
#include <cmath>
#include <cstddef>
#include <real/math/math_fwd.hpp>
#include <real/math/vec3.hpp>

namespace real {

/**
 * ts is stored column major because of glm
 */
struct Mat4 {
public:
	/**
	 * it looks just how you would expect <3, but stored column major
	 * so input row major, stored column major. I hate these stupid standards
	 */
	constexpr Mat4(
		real_t _e11, real_t _e12, real_t _e13, real_t _e14,
		real_t _e21, real_t _e22, real_t _e23, real_t _e24,
		real_t _e31, real_t _e32, real_t _e33, real_t _e34,
		real_t _e41, real_t _e42, real_t _e43, real_t _e44)
		: e11(_e11), e12(_e12), e13(_e13), e14(_e14),
		  e21(_e21), e22(_e22), e23(_e23), e24(_e24),
		  e31(_e31), e32(_e32), e33(_e33), e34(_e34),
		  e41(_e41), e42(_e42), e43(_e43), e44(_e44) {}

	/**
	 * down the diagnal so basicly identity*scalar
	 */
	constexpr Mat4(real_t scalar) {
		e11 = scalar;
		e22 = scalar;
		e33 = scalar;
		e44 = scalar;
	}

	constexpr Mat4() = default;

	constexpr inline void operator +=(const Mat4 m) {
		for(size_t i = 0; i < 16; i++)
			array[i] += m.array[i];
	}

	constexpr inline void operator +=(real_t s) {
		for(size_t i = 0; i < 16; i++)
			array[i] += s;
	}

	constexpr inline Mat4 &operator -=(const Mat4 m) {
		for(size_t i = 0; i < 16; i++)
			array[i] -= m.array[i];
		return *this;
	}

	constexpr inline Mat4 &operator -=(real_t s) {
		for(size_t i = 0; i < 16; i++)
			array[i] -= s;
		return *this;
	}

	constexpr inline Mat4 &operator *=(real_t s) {
		for(size_t i = 0; i < 16; i++)
			array[i] *= s;
		return *this;
	}

	constexpr inline Mat4 &operator /=(real_t s) {
		for(size_t i = 0; i < 16; i++)
			array[i] /= s;
		return *this;
	}

	constexpr inline Mat4 &operator /=(const Mat4 s) {
		for(size_t i = 0; i < 16; i++)
			array[i] /= s.array[i];
		return *this;
	}

	union {
		struct {
			// col, row
			real_t e11=0, e21=0, e31=0, e41=0,
				   e12=0, e22=0, e32=0, e42=0,
				   e13=0, e23=0, e33=0, e43=0,
				   e14=0, e24=0, e34=0, e44=0;
		};
	
		real_t array[16];
	};
};

constexpr inline Mat4 operator *(const Mat4 a, const Mat4 b) {
	Mat4 result = Mat4(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	for (size_t col = 0; col < 4; ++col) {
        for (size_t inner = 0; inner < 4; ++inner) {
            real_t b_val = b.array[col * 4 + inner];
            for (int row = 0; row < 4; ++row) {
                result.array[col * 4 + row] += a.array[inner * 4 + row] * b_val;
            }
        }
    }
	return result;
}

constexpr inline Mat4 operator *(const Mat4 a, real_t s) {
	Mat4 res = a;
	res *= s;
	return res;
}

constexpr inline Mat4 operator +(const Mat4 a, const Mat4 b) {
	Mat4 res = a;
	res += b;
	return res;
}

constexpr inline Mat4 operator +(const Mat4 a, const real_t b) {
	Mat4 res = a;
	res += b;
	return res;
}

constexpr inline Mat4 operator /(const Mat4 a, real_t s) {
	Mat4 res = a;
	res /= s;
	return res;
}


constexpr inline Mat4 operator /(const Mat4 a, const Mat4 s) {
	Mat4 res = a;
	res /= s;
	return res;
}

constexpr inline Mat4 operator -(const Mat4 a, const Mat4 b) {
	Mat4 res = a;
	res -= b;
	return res;
}

constexpr inline Mat4 operator -(const Mat4 a, size_t b) {
	Mat4 res = a;
	res -= b;
	return res;
}

constexpr inline bool operator==(const Mat4 &a, const Mat4 &b) {
	for(size_t i = 0; i < 16; i++)
		if(a.array[i] != b.array[i]) return false;
	return true;
}

namespace math {

constexpr inline Mat4 translate(Vec3 vec) {
	return Mat4(1, 0, 0, vec.x,
				0, 1, 0, vec.y,
				0, 0, 1, vec.z,
				0, 0, 0, 1);
}

constexpr inline Mat4 scale(Vec3 vec) {
	return Mat4(vec.x, 0, 0, 0,
				0, vec.y, 0, 0,
				0, 0, vec.z, 0,
				0, 0, 0, 1);
}

constexpr inline Mat4 perspective(float aspect, float fov_radians, float near, float far) {
	float hw = std::tan(fov_radians/2.0f)*near;
	return Mat4(near/hw, 0, 0, 0,
				0, near/(hw/aspect), 0, 0,
				0, 0, far/(near-far), far*near/(near-far),
				0, 0, -1, 0);
}

/**
 * gracefully stolen from stack overflow
 * https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
 */
constexpr inline Mat4 inverse(const Mat4 mat) {
	double inv[16]={}, det=0;
	auto &m = mat.array;
    int i=0;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

	Mat4 invOut;
    for (i = 0; i < 16; i++)
        invOut.array[i] = inv[i] * det;
	return invOut;
}

}

}

#endif
