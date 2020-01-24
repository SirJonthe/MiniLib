// Copyright (c) Jonathan Karlsson 2012
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_QUATERNION_H_INCLUDED
#define MML_QUATERNION_H_INCLUDED

#include <cmath>
#include "mmlMatrix.h"
#include "mmlVector.h"

// REFERENCE!
// https://github.com/DanielChappuis/reactphysics3d/blob/master/src/mathematics/Quaternion.h
// https://github.com/DanielChappuis/reactphysics3d/blob/master/src/mathematics/Quaternion.cpp

class mmlQuaternion
{
public:
	float x, y, z, w;

public:
	mmlQuaternion( void )
	{
		// don't initialize
	}
	mmlQuaternion(float p_x, float p_y, float p_z, float p_w) : x(p_x), y(p_y), z(p_z), w(p_w) {}
	mmlQuaternion(const mmlQuaternion &p_quat) : x(p_quat.x), y(p_quat.y), z(p_quat.z), w(p_quat.w) {}
	mmlQuaternion &operator=(const mmlQuaternion &p_quat)
	{
		x = p_quat.x;
		y = p_quat.y;
		z = p_quat.z;
		w = p_quat.w;
		return *this;
	}
	mmlQuaternion(const mmlVector<3> &p_axis, float p_angle)
	{
		FromAxisAngle(p_axis, p_angle);
	}
	mmlQuaternion(float p_head, float p_pitch, float p_roll)
	{
		FromEulerAngles(p_head, p_pitch, p_roll);
	}
	explicit mmlQuaternion(const mmlMatrix<3,3> &p_matrix)
	{
		FromEulerMatrix(p_matrix);
	}
	explicit mmlQuaternion(const mmlMatrix<4,4> &p_matrix)
	{
		FromEulerMatrix(p_matrix);
	}
	explicit mmlQuaternion(const float * const p_vec)
	{
		x = p_vec[0];
		y = p_vec[1];
		z = p_vec[2];
		w = p_vec[3];
	}
public:
	operator float * const( void ) { return &x; }
	operator const float * const( void ) const { return &x; }
public:
	void FromAxisAngle(mmlVector<3> p_axis, float p_angle)
	{
		p_axis.NormalizeIf();
		p_angle *= 0.5f;
		const float sinAngle = sinf(p_angle);
		x = (p_axis[0] * sinAngle);
		y = (p_axis[1] * sinAngle);
		z = (p_axis[2] * sinAngle);
		w = cosf(p_angle);
	}
	void GetAxisAngle(mmlVector<3> &p_axis, float &p_angle) const
	{
		mmlQuaternion q = *this;
		if (q.w > 1.0f || q.w < 1.0f) { q.Normalize(); }

		p_angle = 2.0f * acosf(q.w);
		float d = sqrtf(1.0f - q.w * q.w);

		if (mmlIsApproxEqual(d, 0.0f) == true) {
			p_axis[0] = 1.0f;
			p_axis[1] = 0.0f;
			p_axis[2] = 0.0f;
		} else {
			p_axis[0] = q.x / d;
			p_axis[1] = q.y / d;
			p_axis[2] = q.z / d;
		}
	}
	void FromEulerAngles(float p_head, float p_pitch, float p_roll)
	{
		const float SINH = sinf(p_head);
		const float SINP = sinf(p_pitch);
		const float SINR = sinf(p_roll);

		const float COSH = cosf(p_head);
		const float COSP = cosf(p_pitch);
		const float COSR = cosf(p_roll);

		x = SINR * COSP * COSH - COSR * SINP * SINH;
		y = COSR * SINP * COSH + SINR * COSP * SINH;
		z = COSR * COSP * SINH - SINR * SINP * COSH;
		w = COSR * COSP * COSH + SINR * SINP * SINH;

		Normalize();
	}
	void FromEulerMatrix(const mmlMatrix<3,3> &p_matrix)
	{
		const float trace = mmlTrace(p_matrix) + 1.0f;

		if (trace > 0.0f) {
			const float scale = mmlSqrt(trace) * 2.0f;
			x = (p_matrix[1][2] - p_matrix[2][1]) / scale;
			y = (p_matrix[2][0] - p_matrix[0][2]) / scale;
			z = (p_matrix[0][1] - p_matrix[1][0]) / scale;
			w = 0.25f * scale;
		} else {
			if (p_matrix[0][0] > p_matrix[1][1] && p_matrix[0][0] > p_matrix[2][2]) {
				const float scale = mmlSqrt(1.0f + p_matrix[0][0] - p_matrix[1][1] - p_matrix[2][2]) * 2.0f;
				x = 0.25f * scale;
				y = (p_matrix[1][0] + p_matrix[0][1]) / scale;
				z = (p_matrix[0][2] + p_matrix[2][0]) / scale;
				w = (p_matrix[1][2] - p_matrix[2][1]) / scale;
			} else if (p_matrix[1][1] > p_matrix[2][2]) {
				const float scale = mmlSqrt(1.0f + p_matrix[1][1] - p_matrix[0][0] - p_matrix[2][2]) * 2.0f;
				x = (p_matrix[1][0] + p_matrix[0][1]) / scale;
				y = 0.25f * scale;
				z = (p_matrix[2][1] + p_matrix[1][2]) / scale;
				w = (p_matrix[2][0] - p_matrix[0][2]) / scale;
			} else {
				const float scale = mmlSqrt(1.0f + p_matrix[2][2] - p_matrix[0][0] - p_matrix[1][1]) * 2.0f;
				x = (p_matrix[2][0] + p_matrix[0][2]) / scale;
				y = (p_matrix[2][1] + p_matrix[1][2]) / scale;
				z = 0.25f * scale;
				w = (p_matrix[0][1] - p_matrix[1][0]) / scale;
			}
		}
		
		Normalize();
	}
	void FromEulerMatrix(const mmlMatrix<4,4> &p_matrix)
	{
		FromEulerMatrix(mmlMatrix<3,3>(p_matrix));
	}
	mmlMatrix<4,4> GetMatrix4( void ) const
	{
		mmlMatrix<4,4> matrix = mmlMatrix<4,4>(GetMatrix3());
		matrix[3][3] = 1.0f;
		return matrix;
	}
	mmlMatrix<3,3> GetMatrix3( void ) const
	{
		const float x2 = x * x;
		const float y2 = y * y;
		const float z2 = z * z;
		const float xy = x * y;
		const float xz = x * z;
		const float yz = y * z;
		const float wx = w * x;
		const float wy = w * y;
		const float wz = w * z;
		
		return mmlMatrix<3,3>(
			double(1.0f - 2.0f * (y2 + z2)), double(2.0f * (xy - wz)),        double(2.0f * (xz + wy)),
			double(2.0f * (xy + wz)),        double(1.0f - 2.0f * (x2 + z2)), double(2.0f * (yz - wx)),
			double(2.0f * (xz - wy)),        double(2.0f * (yz + wx)),        double(1.0f - 2.0f * (x2 + y2))
		);

		// Alternative method:
		/*float nQ = Scale();
		float s = 0.0f;

		if (nQ > 0.0f) {
			s = 2.0f / nQ;
		}

		float xs  = x * s;
		float ys  = y * s;
		float zs  = z * s;
		float wxs = w * xs;
		float wys = w * ys;
		float wzs = w * zs;
		float xxs = x * xs;
		float xys = x * ys;
		float xzs = x * zs;
		float yys = y * ys;
		float yzs = y * zs;
		float zzs = z * zs;

		// Create the matrix corresponding to the quaternion
		return mmlMatrix<3,3>(
			double(1.0f - yys - zzs), double(xys - wzs),        double(xzs + wys),
			double(xys + wzs),        double(1.0f - xxs - zzs), double(yzs - wxs),
			double(xzs - wys),        double(yzs + wxs),        double(1.0f - xxs - yys)
		);*/
	}
public:
	static mmlQuaternion &Cast(void *ptr)
	{
		return *reinterpret_cast<mmlQuaternion*>(ptr);
	}
	static const mmlQuaternion &Cast(const void *ptr)
	{
		return *reinterpret_cast<const mmlQuaternion*>(ptr);
	}
	void Normalize( void )
	{
		const float scale = 1.0f / Scale();
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
	}
	void Conj( void )
	{
		x = -x;
		y = -y;
		z = -z;
	}
	void Inv( void )
	{
		Normalize();
		Conj();
	}
	float Scale( void ) const
	{
		return mmlSqrt(x*x + y*y + z*z + w*w);
	}
	void RotateByAxisAngle(const mmlVector<3> &p_axis, float p_angle)
	{
		*this = mmlQuaternion(p_axis, p_angle) * (*this);
	}
	mmlQuaternion &operator+=(const mmlQuaternion &p_rhs)
	{
		x += p_rhs.x;
		y += p_rhs.y;
		z += p_rhs.z;
		w += p_rhs.w;
		return *this;
	}
	mmlQuaternion &operator-=(const mmlQuaternion &p_rhs)
	{
		x -= p_rhs.x;
		y -= p_rhs.y;
		z -= p_rhs.z;
		w -= p_rhs.w;
		return *this;
	}
	mmlQuaternion operator*(const mmlQuaternion &p_rhs) const
	{
		return mmlQuaternion(
			(w * p_rhs.x) + (x * p_rhs.w) + (y * p_rhs.z) - (z * p_rhs.y),
			(w * p_rhs.y) + (y * p_rhs.w) + (z * p_rhs.x) - (x * p_rhs.z),
			(w * p_rhs.z) + (z * p_rhs.w) + (x * p_rhs.y) - (y * p_rhs.x),
			(w * p_rhs.w) - (x * p_rhs.x) - (y * p_rhs.y) - (z * p_rhs.z)
			);
	}
	mmlQuaternion &operator*=(const mmlQuaternion &p_rhs)
	{
		*this = *this * p_rhs;
		return *this;
	}
	mmlQuaternion &operator*=(float r)
	{
		x *= r;
		y *= r;
		z *= r;
		w *= r;
		return *this;
	}
	bool IsNormalized(const float p_tolerance = 0.f) const
	{
		const float unit =
			x * x +
			y * y +
			z * z +
			w * w -
			1.0f;
		return (unit <= p_tolerance && unit >= -p_tolerance);
	}
	void SetIdentity( void )
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}
	static mmlQuaternion Identity( void )
	{
		return mmlQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
	}
};

inline mmlQuaternion operator+(mmlQuaternion l, const mmlQuaternion &r) { return l+=r; }
inline mmlQuaternion operator-(mmlQuaternion l, const mmlQuaternion &r) { return l-=r; }
inline mmlVector<3>  operator*(const mmlQuaternion &l, const mmlVector<3> &r) {
	mmlVector<3> uv, uuv;
	mmlVector<3> qvec = mmlVector<3>(double(l.x), double(l.y), double(l.z));
	uv = mmlCross(qvec, r);
	uuv = mmlCross(qvec, uv);
	uv *= (2.0f * l.w);
	uuv *= 2.0f;

	return r + uv + uuv;
}
inline mmlVector<3>  operator*(const mmlVector<3> &l, const mmlQuaternion &r) { return r * l; }
inline mmlQuaternion operator*(mmlQuaternion l, float r)                      { return l *= r; }
inline mmlQuaternion mmlConj(mmlQuaternion q)							      { q.Conj(); return q; }
inline mmlQuaternion mmlInv(mmlQuaternion q)                                  { q.Inv(); return q; }
inline mmlQuaternion mmlNormalize(mmlQuaternion q)                            { q.Normalize(); return q; }
inline float         mmlLen(const mmlQuaternion &q)                           { return q.Scale(); }

#endif
