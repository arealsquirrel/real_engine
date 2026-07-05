#ifndef REALLIB_VEC4_HPP
#define REALLIB_VEC4_HPP

#include "real/math/math_fwd.hpp"

namespace real {

struct Vec4 {
	constexpr Vec4(real_t _x=0, real_t _y=0, real_t _z=0, real_t _w=0)
		: x(_x), y(_y), z(_z), w(_w) {}

	real_t x;
	real_t y;
	real_t z;
	real_t w;

	constexpr inline bool operator==(const Vec4 &vec) const {
		return (vec.x == x) && (vec.y == y) && (vec.z == z) && (vec.w == w);
	}

	constexpr inline void operator+=(const Vec4 vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
	}

	constexpr inline void operator-=(const Vec4 vec) {
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
	}

	constexpr inline void operator*=(real_t s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	constexpr inline void operator/=(real_t s) {
		x /= s;
		y /= s;
		z /= s;
		w /= s;
	}
};

constexpr inline Vec4 operator+(const Vec4 a, const Vec4 b) {
	return Vec4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}

constexpr inline Vec4 operator-(const Vec4 a, const Vec4 b) {
	return Vec4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}

constexpr inline Vec4 operator*(const Vec4 a, float s) {
	return Vec4(a.x*s, a.y*s, a.z*s, a.w*s);	
}

constexpr inline Vec4 operator/(const Vec4 a, float s) {
	return Vec4(a.x/s, a.y/s, a.z/s, a.w/s);
}

}

#endif

