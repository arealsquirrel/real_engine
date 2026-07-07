#ifndef REALLIB_QUATERNION_HPP
#define REALLIB_QUATERNION_HPP

#include "real/math/mat4.hpp"
#include "real/math/math.hpp"
#include <cmath>
#include <real/math/vec3.hpp>
#include <real/math/math_fwd.hpp>

namespace real {

struct Quaternion {
	real_t n;
	Vec3 v;

	constexpr Quaternion()
		: n(0), v(0,0,0) {}

	constexpr Quaternion(real_t e0, real_t e1, real_t e2, real_t e3)
		: n(e0), v(e1, e2, e3) {}

	inline constexpr Quaternion &operator+=(const Quaternion &quat) {
		n += quat.n;
		v += quat.v;
		return *this;
	}

	inline constexpr Quaternion &operator-=(const Quaternion &quat) {
		n -= quat.n;
		v -= quat.v;
		return *this;
	}

	inline constexpr Quaternion &operator*=(float s) {
		n *= s;
		v.x *= s;
		v.y *= s;
		v.z *= s;
		return *this;
	}

	inline constexpr Quaternion &operator/=(float s) {
		n /= s;
		v.x /= s;
		v.y /= s;
		v.z /= s;
		return *this;
	}

	inline constexpr Quaternion operator~() const {
		return Quaternion(n, -v.x, -v.y, -v.z);
	}
};

inline constexpr Quaternion operator+(const Quaternion q1, const Quaternion q2) {
	return Quaternion(q1.n+q2.n, q1.v.x+q2.v.x, q1.v.y+q2.v.y, q1.v.z+q2.v.z);
}

inline constexpr Quaternion operator-(const Quaternion q1, const Quaternion q2) {
	return Quaternion(q1.n-q2.n, q1.v.x-q2.v.x, q1.v.y-q2.v.y, q1.v.z-q2.v.z);
}

inline constexpr Quaternion operator*(const Quaternion q1, real_t s) {
	return Quaternion(q1.n*s, q1.v.x*s, q1.v.y*s, q1.v.z*s);
}

inline constexpr Quaternion operator/(const Quaternion q1, real_t s) {
	return Quaternion(q1.n/s, q1.v.x/s, q1.v.y/s, q1.v.z/s);
}

inline constexpr bool operator==(const Quaternion q1, const Quaternion q2) {
	return (q1.n == q2.n) && (q1.v.x == q2.v.x) && (q1.v.y == q2.v.y) && (q1.v.z == q2.v.z);	
}

inline constexpr Quaternion operator*(const Quaternion q1, const Quaternion q2) {
	return Quaternion(
			q1.n*q2.n - q1.v.x*q2.v.x - q1.v.y*q2.v.y - q1.v.z*q2.v.z,
			q1.n*q2.v.x + q1.v.x*q2.n + q1.v.y*q2.v.z - q1.v.z*q2.v.y,
			q1.n*q2.v.y + q1.v.y*q2.n + q1.v.z*q2.v.x - q1.v.x*q2.v.z,
			q1.n*q2.v.z + q1.v.z*q2.n + q1.v.x*q2.v.y - q1.v.y*q2.v.x);
}

inline constexpr Quaternion operator*(Quaternion q, Vec3 v) {
	return Quaternion(
			-(q.v.x*v.x + q.v.y*v.y + q.v.z*v.z),
			q.n*v.x + q.v.y*v.z - q.v.z*v.y,
			q.n*v.y + q.v.z*v.x - q.v.x*v.z,
			q.n*v.z + q.v.x*v.y - q.v.y*v.x);
}

inline constexpr Quaternion operator *(Vec3 v, Quaternion q) {
	return Quaternion(
			-(q.v.x*v.x + q.v.y*v.y + q.v.z*v.z),
			q.n*v.x + q.v.z*v.y - q.v.y*v.z,
			q.n*v.y + q.v.x*v.z - q.v.z*v.x,
			q.n*v.z + q.v.y*v.x - q.v.x*v.y);
}

namespace math {

inline constexpr real_t magnatude_quat(const Quaternion quat) {
	return magnatude_vec4(Vec4(quat.n, quat.v.x, quat.v.y, quat.v.z));
}

/* outputs radians */
inline constexpr float qget_angle(Quaternion q) {
	return (float)(2*std::acos(q.n));
}

inline constexpr Vec3 qget_axis(Quaternion q) {
	Vec3 v = q.v;
	float m = q.n;
	if(m <= real_tol)
			return Vec3(0,0,0);
	else
		return Vec3(v.x/m, v.y/m, v.z/m);
}

inline constexpr Quaternion qrotate(Quaternion q1, Quaternion q2) {
	return q1*q2*(~q1);
}

inline constexpr Vec3 qvrotate(Quaternion q, Vec3 v) {
	Quaternion t;
	t = q*v*(~q);
	return t.v;
}

/**
 * all the inputs are in degrees
 */
inline constexpr Quaternion make_q_from_euler_angles(
		real_t x, real_t y, real_t z) {

	Quaternion q;
	real_t roll = math::degrees_to_radians(x);
	real_t pitch = math::degrees_to_radians(y);
	real_t yaw = math::degrees_to_radians(z);

	real_t cyaw=0, cpitch=0, croll=0, syaw=0, spitch=0, sroll=0;
	real_t cyawcpitch=0, syawspitch=0, cyawspitch=0, syawcpitch=0;

	cyaw = std::cos(0.5f * yaw);
	cpitch = std::cos(0.5f * pitch);
	croll = std::cos(0.5f * roll);
	syaw = std::sin(0.5f * yaw);
	spitch = std::sin(0.5f * pitch);
	sroll = std::sin(0.5f * roll);

	cyawcpitch = cyaw*cpitch;
	syawspitch = syaw*spitch;
	cyawspitch = cyaw*spitch;
	syawcpitch = syaw*cpitch;

	q.n = cyawcpitch*croll + syawspitch*sroll;
	q.v.x = cyawcpitch*sroll - syawspitch*croll;
	q.v.y = cyawspitch*croll + syawcpitch*sroll;
	q.v.z = syawcpitch*croll - cyawspitch*sroll;

	return q;
}

/* outputs in degrees */
inline constexpr Vec3 make_euler_angles_from_q(Quaternion q) {
	real_t r11=0, r21=0, r31=0, r32=0, r33=0, r12=0, r13=0;
	real_t q00=0, q11=0, q22=0, q33=0;
	real_t tmp=0;
	Vec3 u;

	q00 = q.n * q.n;
	q11 = q.v.x * q.v.x;
	q22 = q.v.y * q.v.y;
	q33 = q.v.z * q.v.z;

	r11 = q00 + q11 - q22 - q33;
	r21 = 2 * (q.v.x*q.v.y + q.n*q.v.z);
	r31 = 2 * (q.v.x*q.v.z - q.n*q.v.y);
	r32 = 2 * (q.v.y*q.v.z + q.n*q.v.x);
	r33 = q00 - q11 - q22 + q33;

	tmp = std::fabs(r31);
	if(tmp > 0.999999) {
		r12 = 2 * (q.v.x*q.v.y - q.n*q.v.z);
		r13 = 2 * (q.v.x*q.v.z + q.n*q.v.y);
		u.x = radians_to_degrees(0.0f);
		u.y = radians_to_degrees(-(pi/2) * r31/tmp);
		u.z = radians_to_degrees(std::atan2(-r12, -r31*r13));
		return u;
	}

	u.x = radians_to_degrees(std::atan2(r32, r33));
	u.y = radians_to_degrees(std::asin(-r31));
	u.z = radians_to_degrees(std::atan2(r21, r11));
	return u;
}

inline constexpr Mat4 make_mat4_from_q(Quaternion q) {
    real_t xx = q.v.x * q.v.x;
    real_t yy = q.v.y * q.v.y;
    real_t zz = q.v.z * q.v.z;

	return Mat4(1.0f - 2.0f*(yy+zz), 2.0f*(q.v.x*q.v.y - q.n*q.v.z), 2.0f*(q.v.x*q.v.z+q.n*q.v.y), 0.0f,
				2.0f*(q.v.x*q.v.y+q.n*q.v.z), 1.0f - 2.0f*(xx+zz), 2.0f*(q.v.y*q.v.z - q.n*q.v.x), 0.0f,
				2.0f*(q.v.x*q.v.z - q.n*q.v.y), 2.0f*(q.v.y*q.v.z + q.n*q.v.x), 1.0f-2.0f*(xx+yy), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
}

}

}

#endif
