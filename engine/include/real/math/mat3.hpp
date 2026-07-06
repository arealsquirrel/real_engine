
#include <cstddef>
#include <real/math/math_fwd.hpp>

namespace real {

struct Mat3 {
public:
	constexpr Mat3(real_t _e11, real_t _e12, real_t _e13,
		real_t _e21, real_t _e22, real_t _e23,
		real_t _e31, real_t _e32, real_t _e33)
		: e11(_e11), e12(_e12), e13(_e13),
		e21(_e21), e22(_e22), e23(_e23),
		e31(_e31), e32(_e32), e33(_e33) {}

	/**
	 * down the diagnal so basicly identity*scalar
	 */
	constexpr Mat3(real_t scalar) {
		e11 = scalar;
		e22 = scalar;
		e33 = scalar;
	}

	constexpr Mat3() = default;

	constexpr float det() const {
		return  e11*e22*e33 -
				e11*e32*e23 +
				e21*e32*e13 -
				e21*e12*e33 +
				e31*e12*e23 -
				e31*e22*e13;
	}

	constexpr inline void operator +=(const Mat3 m) {
		for(size_t i = 0; i < 9; i++)
			array[i] += m.array[i];
	}

	constexpr inline void operator +=(real_t s) {
		for(size_t i = 0; i < 9; i++)
			array[i] += s;
	}

	constexpr inline Mat3 &operator -=(const Mat3 m) {
		for(size_t i = 0; i < 9; i++)
			array[i] -= m.array[i];
		return *this;
	}

	constexpr inline Mat3 &operator -=(real_t s) {
		for(size_t i = 0; i < 9; i++)
			array[i] -= s;
		return *this;
	}

	constexpr inline Mat3 &operator *=(real_t s) {
		for(size_t i = 0; i < 9; i++)
			array[i] *= s;
		return *this;
	}

	constexpr inline Mat3 &operator *=(const Mat3 b) {
		Mat3 a = *this;
		for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
				array[i*3+j] = 0;
                for (int k = 0; k < 3; ++k) {
					array[i*3+j] += a.array[i*3+k]*b.array[k*3+j];
                }
            }
        }
		return *this;
	}

	constexpr inline Mat3 &operator /=(real_t s) {
		for(size_t i = 0; i < 9; i++)
			array[i] /= s;
		return *this;
	}

	constexpr inline Mat3 &operator /=(const Mat3 s) {
		for(size_t i = 0; i < 9; i++)
			array[i] /= s.array[i];
		return *this;
	}

	union {
		struct {
			real_t e11=0, e12=0, e13=0, e21=0, e22=0, e23=0, e31=0, e32=0, e33=0;
		};
	
		real_t array[9];
	};
};

constexpr inline Mat3 operator *(const Mat3 a, const Mat3 b) {
	Mat3 result = a;
	result *= b;
	return result;
}

constexpr inline Mat3 operator *(const Mat3 a, real_t s) {
	Mat3 res = a;
	res *= s;
	return res;
}

constexpr inline Mat3 operator +(const Mat3 a, const Mat3 b) {
	Mat3 res = a;
	res += b;
	return res;
}

constexpr inline Mat3 operator +(const Mat3 a, const real_t b) {
	Mat3 res = a;
	res += b;
	return res;
}

constexpr inline Mat3 operator /(const Mat3 a, real_t s) {
	Mat3 res = a;
	res /= s;
	return res;
}


constexpr inline Mat3 operator /(const Mat3 a, const Mat3 s) {
	Mat3 res = a;
	res /= s;
	return res;
}

constexpr inline Mat3 operator -(const Mat3 a, const Mat3 b) {
	Mat3 res = a;
	res -= b;
	return res;
}

constexpr inline Mat3 operator -(const Mat3 a, size_t b) {
	Mat3 res = a;
	res -= b;
	return res;
}

constexpr inline bool operator==(const Mat3 &a, const Mat3 &b) {
	for(size_t i = 0; i < 9; i++)
		if(a.array[i] != b.array[i]) return false;
	return true;
}

namespace math {

constexpr Mat3 transpose(const Mat3 m) {
	return Mat3(m.e11, m.e21, m.e31,  m.e12, m.e22, m.e32,  m.e13, m.e23, m.e33);
}

constexpr Mat3 inverse(const Mat3 m) {
	float d = m.det();
	if(d == 0) d = 1;
	return Mat3(
		 (m.e22*m.e33 - m.e23*m.e32)/d,
		-(m.e12*m.e33 - m.e13*m.e32)/d,
		 (m.e12*m.e23 - m.e13*m.e22)/d,
		-(m.e21*m.e33 - m.e23*m.e31)/d,
		 (m.e11*m.e23 - m.e13*m.e31)/d,
		-(m.e11*m.e23 - m.e13*m.e21)/d,
		 (m.e21*m.e32 - m.e22*m.e31)/d,
		-(m.e11*m.e32 - m.e12*m.e31)/d,
		 (m.e11*m.e22 - m.e12*m.e21)/d
	);
}

}

}

