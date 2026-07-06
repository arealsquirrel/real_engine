#ifndef REALLIB_MAT4_HPP
#define REALLIB_MAT4_HPP

#include <cmath>
#include <cstddef>
#include <real/math/math_fwd.hpp>
#include <real/math/vec3.hpp>

namespace real {

struct Mat4 {
public:
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

	constexpr inline Mat4 &operator *=(const Mat4 b) {
		Mat4 a = *this;
		for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
				array[i*4+j] = 0;
                for (int k = 0; k < 4; ++k) {
					array[i*4+j] += a.array[i*4+k]*b.array[k*4+j];
                }
            }
        }
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
			real_t e11=0, e12=0, e13=0, e14=0, e21=0, e22=0, e23=0, e24=0, e31=0, e32=0, e33=0, e34=0, e41=0, e42=0, e43=0, e44=0;
		};
	
		real_t array[9];
	};
};

constexpr inline Mat4 operator *(const Mat4 a, const Mat4 b) {
	Mat4 result = a;
	result *= b;
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
	float range = near-far;
	float htan = std::tan(fov_radians/2);
	return Mat4(1.0f / (htan * aspect), 0, 0, 0,
				0, 1.0f/htan, 0, 0,
				0, 0, (-near - far) / range, 2.0f*far*near / range,
				0, 0, 1, 0);
}

}

#endif
