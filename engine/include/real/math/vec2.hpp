#ifndef REALLIB_VEC2_HPP
#define REALLIB_VEC2_HPP

#include <real/math/math_fwd.hpp>

namespace real {

struct Vec2 {
	constexpr Vec2(real_t _x=0, real_t _y=0)
		: x(_x), y(_y) {}

	real_t x;
	real_t y;

	constexpr inline bool operator==(const Vec2 &vec) const {
		return (vec.x == x) && (vec.y == y);
	}

	constexpr inline void operator+=(const Vec2 vec) {
		x += vec.x;
		y += vec.y;
	}

	constexpr inline void operator-=(const Vec2 vec) {
		x -= vec.x;
		y -= vec.y;
	}

	constexpr inline void operator*=(real_t s) {
		x *= s;
		y *= s;
	}

	constexpr inline void operator/=(real_t s) {
		x /= s;
		y /= s;
	}
};

constexpr inline Vec2 operator+(const Vec2 a, const Vec2 b) {
	return Vec2(a.x+b.x, a.y+b.y);
}

constexpr inline Vec2 operator-(const Vec2 a, const Vec2 b) {
	return Vec2(a.x-b.x, a.y-b.y);
}

constexpr inline Vec2 operator*(const Vec2 a, float s) {
	return Vec2(a.x*s, a.y*s);	
}

constexpr inline Vec2 operator/(const Vec2 a, float s) {
	return Vec2(a.x/s, a.y/s);
}

}

#endif
