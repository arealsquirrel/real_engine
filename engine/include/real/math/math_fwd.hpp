#ifndef REALLIB_MATH_FWD_HPP
#define REALLIB_MATH_FWD_HPP

namespace real {

using real_t = float;
constexpr real_t real_tol = 0.00001f;
constexpr real_t pi = 3.14159265;

struct Vec2;
struct Vec3;
struct Vec4;

namespace math {

inline constexpr real_t degrees_to_radians(real_t deg) {
	return deg*pi / 180.0f;
}

inline constexpr real_t radians_to_degrees(real_t rad) {
	return rad*180.0f / pi;
}

}

}

#endif
