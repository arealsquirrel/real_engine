#ifndef REALLIB_MATH_HPP
#define REALLIB_MATH_HPP

#include <real/math/math_fwd.hpp>
#include "real/math/vec2.hpp"
#include "real/math/vec3.hpp"
#include <real/math/vec4.hpp>
#include <cmath>

namespace real {

namespace math {

template<>
real_t magnatude(const Vec2 vec) {
	return std::sqrt(vec.x*vec.x + vec.y*vec.y);
}

template<>
real_t magnatude(const Vec3 vec) {
	return std::sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

template<>
real_t magnatude(const Vec4 vec) {
	return std::sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + vec.w*vec.w);
}

template<>
Vec2 normalize(const Vec2 a) {
	real_t mag = magnatude(a);
	if(mag <= real_tol) mag = 1;
	Vec2 u(a.x / mag, a.y / mag);
	if(std::fabs(u.x) < real_tol) u.x = 0.0f;
	if(std::fabs(u.y) < real_tol) u.y = 0.0f;
	return u;
}

template<>
Vec3 normalize(const Vec3 a) {
	real_t mag = magnatude(a);
	if(mag <= real_tol) mag = 1;
	Vec3 u(a.x / mag, a.y / mag, a.z / mag);
	if(std::fabs(u.x) < real_tol) u.x = 0.0f;
	if(std::fabs(u.y) < real_tol) u.y = 0.0f;
	if(std::fabs(u.z) < real_tol) u.z = 0.0f;
	return u;
}

template<>
Vec4 normalize(const Vec4 a) {
	real_t mag = magnatude(a);
	if(mag <= real_tol) mag = 1;
	Vec4 u(a.x / mag, a.y / mag, a.z / mag, a.w / mag);
	if(std::fabs(u.x) < real_tol) u.x = 0.0f;
	if(std::fabs(u.y) < real_tol) u.y = 0.0f;
	if(std::fabs(u.z) < real_tol) u.z = 0.0f;
	if(std::fabs(u.w) < real_tol) u.w = 0.0f;
	return u;
}

}

}

#endif
