#ifndef REALLIB_MATH_FWD_HPP
#define REALLIB_MATH_FWD_HPP

namespace real {

using real_t = float;
constexpr real_t real_tol = 0.00001f;

class Vec2;
class Vec3;
class Vec4;

namespace math {

template<typename Vec>
real_t magnatude(const Vec vec);

template<typename Vec>
static real_t dot(const Vec a, const Vec b);

template<typename Vec>
static Vec normalize(const Vec a);

}

}

#endif
