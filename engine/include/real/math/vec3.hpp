#ifndef REALLIB_VEC3_HPP
#define REALLIB_VEC3_HPP

#include "real/math/math_fwd.hpp"

namespace real {

struct Vec3 {
	constexpr Vec3(real_t _x=0, real_t _y=0, real_t _z=0)
		: x(_x), y(_y), z(_z) {}

	real_t x;
	real_t y;
	real_t z;

	constexpr inline bool operator==(const Vec3 &vec) const {
		return (vec.x == x) && (vec.y == y) && (vec.z == z);
	}

	constexpr inline void operator+=(const Vec3 vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
	}

	constexpr inline void operator-=(const Vec3 vec) {
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
	}

	constexpr inline void operator*=(real_t s) {
		x *= s;
		y *= s;
		z *= s;
	}

	constexpr inline void operator/=(real_t s) {
		x /= s;
		y /= s;
		z /= s;
	}
};

constexpr inline Vec3 operator+(const Vec3 a, const Vec3 b) {
	return Vec3(a.x+b.x, a.y+b.y, a.z+b.z);
}

constexpr inline Vec3 operator-(const Vec3 a, const Vec3 b) {
	return Vec3(a.x-b.x, a.y-b.y, a.z-b.z);
}

constexpr inline Vec3 operator*(const Vec3 a, float s) {
	return Vec3(a.x*s, a.y*s, a.z*s);	
}

constexpr inline Vec3 operator/(const Vec3 a, float s) {
	return Vec3(a.x/s, a.y/s, a.z/s);
}

namespace math {

/**
 * returns a vector purpendicular to the vectors a and b
 */
constexpr inline Vec3 cross(const Vec3 a, const Vec3 b) {
	return Vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

/**
 * returns the area of the parrellelagram bounded in 3d space by 
 * these 3 vectors
 */
constexpr inline real_t triple_scalar_product(Vec3 u, Vec3 v, Vec3 w) {
	return (u.x * (v.y*w.z - v.z*w.y)) +
		   (u.y * (-v.x*w.z + v.z*w.x)) +
		   (u.z * (v.x*w.y - v.y*w.x));
}

}

}

#endif
