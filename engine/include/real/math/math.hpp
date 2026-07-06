#ifndef REALLIB_MATH_HPP
#define REALLIB_MATH_HPP

#include <real/math/math_fwd.hpp>
#include "real/math/vec2.hpp"
#include "real/math/vec3.hpp"
#include <real/math/vec4.hpp>
#include <real/math/mat4.hpp>
#include <cmath>

namespace real {

namespace math {

inline constexpr real_t magnatude_vec2(const Vec2 vec) {
	return std::sqrt(vec.x*vec.x + vec.y*vec.y);
}

inline constexpr real_t magnatude_vec3(const Vec3 vec) {
	return std::sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

inline constexpr real_t magnatude_vec4(const Vec4 vec) {
	return std::sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + vec.w*vec.w);
}

inline constexpr Vec2 normalize_vec2(const Vec2 a) {
	real_t mag = magnatude_vec2(a);
	if(mag <= real_tol) mag = 1;
	Vec2 u(a.x / mag, a.y / mag);
	if(std::fabs(u.x) < real_tol) u.x = 0.0f;
	if(std::fabs(u.y) < real_tol) u.y = 0.0f;
	return u;
}

inline constexpr Vec3 normalize_vec3(const Vec3 a) {
	real_t mag = magnatude_vec3(a);
	if(mag <= real_tol) mag = 1;
	Vec3 u(a.x / mag, a.y / mag, a.z / mag);
	if(std::fabs(u.x) < real_tol) u.x = 0.0f;
	if(std::fabs(u.y) < real_tol) u.y = 0.0f;
	if(std::fabs(u.z) < real_tol) u.z = 0.0f;
	return u;
}

inline constexpr Vec4 normalize_vec4(const Vec4 a) {
	real_t mag = magnatude_vec4(a);
	if(mag <= real_tol) mag = 1;
	Vec4 u(a.x / mag, a.y / mag, a.z / mag, a.w / mag);
	if(std::fabs(u.x) < real_tol) u.x = 0.0f;
	if(std::fabs(u.y) < real_tol) u.y = 0.0f;
	if(std::fabs(u.z) < real_tol) u.z = 0.0f;
	if(std::fabs(u.w) < real_tol) u.w = 0.0f;
	return u;
}

inline constexpr real_t dot_vec2(const Vec2 a, const Vec2 b) {
	return a.x*b.x + a.y*b.y;
}

inline constexpr real_t dot_vec3(const Vec3 a, const Vec3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline constexpr real_t dot_vec4(const Vec4 a, const Vec4 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

inline constexpr Mat4 look_at(Vec3 position, Vec3 target, Vec3 up) {
	Vec3 zaxis = normalize_vec3(position - target);
    Vec3 xaxis = normalize_vec3(cross(normalize_vec3(up), zaxis));
    Vec3 yaxis = cross(zaxis, xaxis);

	Mat4 translate(
			1,0,0, -position.x,
			0,1,0, -position.y,
			0,0,1, -position.z,
			0,0,0, 1);

	Mat4 rotate(xaxis.x, xaxis.y, xaxis.z, 0.0f,
				yaxis.x, yaxis.y, yaxis.z, 0.0f,
				zaxis.x, zaxis.y, zaxis.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

	return rotate * translate;
}

}

}

#endif
