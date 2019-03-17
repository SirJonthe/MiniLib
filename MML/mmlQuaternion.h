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
// https://svn.code.sf.net/p/irrlicht/code/trunk/include/quaternion.h

class mmlQuaternion
{
private:
	float x, y, z, w;
public:
	mmlQuaternion( void )
	{
		// don't initialize
	}
	mmlQuaternion(float p_x, float p_y, float p_z, float p_w) : x(p_x), y(p_y), z(p_z), w(p_w) {}
	mmlQuaternion(const mmlQuaternion &p_quat) : x(p_quat.x), y(p_quat.y), z(p_quat.z), w(p_quat.w) {}
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
//public:
//	operator float * const( void ) { return e; }
//	operator const float * const( void ) const { return e; }
public:
	void FromAxisAngle(mmlVector<3> p_axis, float p_angle)
	{
		p_axis.Normalize();
		p_angle *= 0.5f;
		const float sinAngle = mmlSin(p_angle);
		x = (p_axis[0] * sinAngle);
		y = (p_axis[1] * sinAngle);
		z = (p_axis[2] * sinAngle);
		w = mmlCos(p_angle);
	}
	void GetAxisAngle(mmlVector<3> &p_axis, float &p_angle) const
	{
		p_axis[0] = x;
		p_axis[1] = y;
		p_axis[2] = z;
		p_axis.Normalize();
		p_angle = acos(w) * 2.f;
	}
	void FromEulerAngles(float p_head, float p_pitch, float p_roll)
	{
		const float SINH = mmlSin(p_head);
		const float SINP = mmlSin(p_pitch);
		const float SINR = mmlSin(p_roll);

		const float COSH = mmlCos(p_head);
		const float COSP = mmlCos(p_pitch);
		const float COSR = mmlCos(p_roll);

		x = SINR * COSP * COSH - COSR * SINP * SINH;
		y = COSR * SINP * COSH + SINR * COSP * SINH;
		z = COSR * COSP * SINH - SINR * SINP * COSH;
		w = COSR * COSP * COSH + SINR * SINP * SINH;

		Normalize();
	}
	void FromEulerMatrix(const mmlMatrix<3,3> &p_matrix)
	{
		//  0  1  2  3
		//  4  5  6  7
		//  8  9 10 11
		// 12 13 14 15
		
		const float trace = mmlTrace(p_matrix) + 1.0f;
		
		if (trace > 0.0f) {
			const float scale = sqrt(trace) * 2.0f;
			x = (p_matrix[1][2] - p_matrix[2][1]) / scale;
			y = (p_matrix[2][0] - p_matrix[0][2]) / scale;
			z = (p_matrix[0][1] - p_matrix[1][0]) / scale;
			w = 0.25f * scale;
		} else {
			if (p_matrix[0][0] > p_matrix[1][1] && p_matrix[0][0] > p_matrix[2][2]) {
				const float scale = sqrt(1.0f + p_matrix[0][0] - p_matrix[1][1] - p_matrix[2][2]) * 2.0f;
				x = 0.25f * scale;
				y = (p_matrix[1][0] + p_matrix[0][1]) / scale;
				z = (p_matrix[0][2] + p_matrix[2][0]) / scale;
				w = (p_matrix[1][2] - p_matrix[2][1]) / scale;
			} else if (p_matrix[1][1] > p_matrix[2][2]) {
				const float scale = sqrt(1.0f + p_matrix[1][1] - p_matrix[0][0] - p_matrix[2][2]) * 2.0f;
				x = (p_matrix[1][0] + p_matrix[0][1]) / scale;
				y = 0.25f * scale;
				z = (p_matrix[2][1] + p_matrix[1][2]) / scale;
				w = (p_matrix[2][0] - p_matrix[0][2]) / scale;
			} else {
				const float scale = sqrt(1.0f + p_matrix[2][2] - p_matrix[0][0] - p_matrix[1][1]) * 2.0f;
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
	mmlMatrix<4,4> GetEulerMatrix4( void ) const
	{
		mmlMatrix<4,4> matrix = mmlMatrix<4,4>(GetEulerMatrix3());
		matrix[3][3] = 1.0f;
		return matrix;
		
		/*const float x2 = x * x;
		const float y2 = y * y;
		const float z2 = z * z;
		const float xy = x * y;
		const float xz = x * z;
		const float yz = y * z;
		const float wx = w * x;
		const float wy = w * y;
		const float wz = w * z;

		return mmlMatrix<4,4>(
			1.f - 2.f * (y2 + z2), 2.f * (xy - wz),       2.f * (xz + wy),       0.f,
			2.f * (xy + wz),       1.f - 2.f * (x2 + z2), 2.f * (yz - wx),       0.f,
			2.f * (xz - wy),       2.f * (yz + wx),       1.f - 2.f * (x2 + y2), 0.f,
			0.f,                   0.f,                   0.f,                   1.f
		);*/
	}
	mmlMatrix<3,3> GetEulerMatrix3( void ) const
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
			1.f - 2.f * (y2 + z2), 2.f * (xy - wz),       2.f * (xz + wy),
			2.f * (xy + wz),       1.f - 2.f * (x2 + z2), 2.f * (yz - wx),
			2.f * (xz - wy),       2.f * (yz + wx),       1.f - 2.f * (x2 + y2)
		);
	}
public:
	static mmlQuaternion &Cast(void *ptr)
	{
		return *(mmlQuaternion*)ptr;
	}
	static const mmlQuaternion &Cast(const void *ptr)
	{
		return *(mmlQuaternion*)ptr;
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
		return sqrt(x*x + y*y + z*z + w*w);
	}
	void RotateByAxisAngle(const mmlVector<3> &p_axis, float p_angle)
	{
		*this = mmlQuaternion(p_axis, p_angle) * (*this);
	}
	mmlQuaternion operator*(const mmlQuaternion &p_rhs) const
	{
		return mmlQuaternion(
			(p_rhs.w * x) + (p_rhs.x * w) + (p_rhs.y * z) - (p_rhs.z * y),
			(p_rhs.w * y) + (p_rhs.y * w) + (p_rhs.z * x) - (p_rhs.x * z),
			(p_rhs.w * z) + (p_rhs.z * w) + (p_rhs.x * y) - (p_rhs.y * x),
			(p_rhs.w * w) - (p_rhs.x * x) - (p_rhs.y * y) - (p_rhs.z * z)
			);
	}
	mmlVector<3> operator*(const mmlVector<3> &p_rhs) const
	{
		mmlVector<3> uv, uuv;
		mmlVector<3> qvec(x, y, z);
		uv = mmlCross(qvec, p_rhs);
		uuv = mmlCross(qvec, uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;
		
		return p_rhs + uv + uuv;
	}
	bool IsNormalized(const float p_tolerance = 0.f) const
	{
		const float unit =
			x * x +
			y * y +
			z * z +
			w * w -
			1.f;
		return (unit <= p_tolerance && unit >= -p_tolerance);
	}
	void SetIdentity( void )
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	/*void LookAt(const mmlVector<3> &p_point, mmlVector<3> &p_up)
	{
		mmlVector<3> forward = Normalize(p_point);
		Vector::OrthoNormalize(&up, &forward); // Keeps up the same, make forward orthogonal to up
		mmlVector<3> right = Cross(up, forward);

		Quaternion ret;
		w = sqrtf(1.0f + right[0] + up[1] + forward[2]) * 0.5f;
		float w4_recip = 1.0f / (4.0f * ret.w);
		x = (forward[1] - up[2]) * w4_recip;
		y = (right[2] - forward[0]) * w4_recip;
		z = (up[0] - right[1]) * w4_recip;

		return ret;
	}*/
};

#endif
