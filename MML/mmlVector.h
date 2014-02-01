// Copyright (c) Jonathan Karlsson 2010
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_VECTOR_H_INCLUDED__
#define MML_VECTOR_H_INCLUDED__

#include <stdarg.h>
#include <cmath>

#include "mmlMath.h"

#define mmlVectorCast(vec_ptr, n) *(mmlVector<n>*)(vec_ptr)

//
// mmlVector
//
template < int n >
class mmlVector
{
public:
	static const int Dimension = n;
private:
	float e[n];
public:
	//
	// default
	//
	mmlVector( void ) {
		for (int i = 0; i < n; ++i) { e[i] = 0.0f; }
		// do nothing, let initialize to garbage
	}
	//
	// copy
	//
	mmlVector(const mmlVector<n> &v) {
		for (int j = 0; j < n; ++j) e[j] = v.e[j];
	}
	//
	// initializer list
	//
	explicit mmlVector(float e0, ...) {
		va_list vl;
		va_start(vl, e0);
		e[0] = e0;
		for (int j = 1; j < n; ++j) {
			e[j] = (float)va_arg(vl, double);
		}
		va_end(vl);
	}
	//
	// conversion
	//
	template < int size2 >
	explicit mmlVector(const mmlVector<size2> &v) {
		if (n < size2) {
			for (int j = 0; j < n; ++j) e[j] = v[j];
		} else {
			int j = 0;
			for (; j < size2; ++j) e[j] = v[j];
			for (; j < n; ++j) e[j] = 0.f;
		}
	}
	//
	// conversion
	//
	explicit mmlVector(const float * const v) {
		for (int j = 0; j < n; ++j) e[j] = v[j];
	}
public:
	//
	// index and conversion
	//
	operator float*( void ) { return e; }
	operator const float* const ( void ) const { return e; }
public:
	//
	// comparison
	//
	bool operator==(const mmlVector<n> &r) const {
		for (int j = 0; j < n; ++j) {
			if (e[j] != r[j]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const mmlVector<n> &r) const {
		for (int j = 0; j < n; ++j) {
			if (e[j] == r[j]) {
				return false;
			}
		}
		return true;
	}
	static bool Compare(const mmlVector<n> &u, const mmlVector<n> &v, float p_tolerance)
	{
		for (int i = 0; i < n; ++i) {
			if (u[i] < v[i]-p_tolerance || u[i] > v[i]+p_tolerance) {
				return false;
			}
		}
		return true;
	}
public:
	//
	// assignment operators
	//
	mmlVector<n> &operator=(const mmlVector<n> &r)
	{
		for (int j = 0; j < n; ++j) e[j] = r.e[j];
		return *this;
	}
	mmlVector<n> &operator+=(const mmlVector<n> &r)
	{
		for (int j = 0; j < n; ++j) e[j] += r.e[j];
		return *this;
	}
	mmlVector<n> &operator-=(const mmlVector<n> &r)
	{
		for (int j = 0; j < n; ++j) e[j] -= r.e[j];
		return *this;
	}
	mmlVector<n> &operator*=(const mmlVector<n> &r)
	{
		for (int j = 0; j < n; ++j) e[j] *= r;
		return *this;
	}
	mmlVector<n> &operator*=(float r)
	{
		for (int j = 0; j < n; ++j) e[j] *= r;
		return *this;
	}
public:
	static mmlVector<n> &Cast(void *ptr)
	{
		return *(mmlVector<n>*)ptr;
	}
	static const mmlVector<n> &Cast(const void *ptr)
	{
		return *(const mmlVector<n>*)ptr;
	}
	void Clamp(const mmlVector<n> &min, const mmlVector<n> &max)
	{
		for (int i = 0; i < n; ++i) {
			if (e[i] > max[i]) {
				e[i] = max[i];
			} else if (e[i] < min[i]) {
				e[i] = min[i];
			}
		}
	}
	void Round( void )
	{
		for (int i = 0; i < n; ++i) {
			e[i] = floor(e[i] + 0.5f);
		}
	}
	void SnapInt( void )
	{
		for (int i = 0; i < n; ++i) {
			e[i] = float(int(e[i]));
		}
	}
	void Zero( void )
	{
		for (int i = 0; i < n; ++i) {
			e[i] = 0.f;
		}
	}
	float Len( void ) const
	{
		float l = 0.f;
		for (int j = 0; j < n; ++j) { l += e[j]*e[j]; }
		return sqrt(l);
	}
	float LenFast( void ) const
	{
		// pure voodoo
		float l = 0.f;
		for (int j = 0; j < n; ++j) { l += e[j]*e[j]; }
		float xhalf = 0.5f*l;
		int i = *(int*)&l;
		i = 0x5f375a86 - (i>>1);
		float x = *(float*)&i;
		x = x*(1.5f-xhalf*x*x);
		return l*x;
	}
	void Normalize( void )
	{
		const float invlen = 1.f/Len();
		for (int j = 0; j < n; ++j) {
			e[j] *= invlen;
		}
	}
	void NormalizeFast( void )
	{
		const float invlen = 1.f/LenFast();
		for (int j = 0; j < n; ++j) {
			e[j] *= invlen;
		}
	}
	bool IsNormalized(const float p_tolerance = 0.f) const
	{
		float unit = 0.f;
		for (int i = 0; i < n; ++i) {
			unit += e[i]*e[i];
		}
		unit -= 1.f;
		return (unit <= p_tolerance && unit >= -p_tolerance);
	}
	void FixDenormals( void )
	{
		for (int i = 0; i < n; ++i) {
			if (fabs(e[i]) < 1e-30f) { e[i] = 0.f; }
		}
	}
	void Swap(int i, int j)
	{
		float t = e[i];
		e[i] = e[j];
		e[j] = t;
	}
};

//
// arithmetic operators
//
template < int n > inline mmlVector<n> operator+(mmlVector<n> l, const mmlVector<n> &r) { return (l+=r); }
template < int n > inline mmlVector<n> operator-(mmlVector<n> l, const mmlVector<n> &r) { return (l-=r); }
template < int n > inline mmlVector<n> operator*(mmlVector<n> l, float r) { return (l*=r); }
template < int n > inline mmlVector<n> operator*(float l, mmlVector<n> r) { return (r*=l); }
template < int n > inline mmlVector<n> operator-(mmlVector<n> v) {
	for (int j = 0; j < n; ++j) { v[j] = -v[j]; }
	return v;
}

//
// mmlDot
//
template < int n >
inline float mmlDot(const mmlVector<n> &u, const mmlVector<n> &v)
{
	float d = 0.f;
	for (int j = 0; j < n; ++j) { d += u[j] * v[j]; }
	return d;
}

//
// mmlDist
//
template < int n >
inline float mmlDist(const mmlVector<n> &u, const mmlVector<n> &v)
{
	return (u - v).Len();
}

//
// mmlAng
//
template < int n >
inline float mmlAng(const mmlVector<n> &u, const mmlVector<n> &v)
{
	return acos( mmlDot(u,v) / (u.Len() * v.Len()) );
}

//
// mmlCross
//
inline mmlVector<3> mmlCross(const mmlVector<3> &u, const mmlVector<3> &v)
{
	mmlVector<3> res;
	res[0] = u[1]*v[2] - u[2]*v[1];
	res[1] = u[2]*v[0] - u[0]*v[2];
	res[2] = u[0]*v[1] - u[1]*v[0];
	return res;
}

//
// mmlNormal
//
inline mmlVector<3> mmlSurfaceNormal(const mmlVector<3> &x, const mmlVector<3> &y, const mmlVector<3> &z) // this should be a specialization of mmlVector<3>
{
	mmlVector<3> normal = mmlCross(y-x, z-x);
	normal.Normalize();
	return normal;
}

//
// mmlLerp
//
template < int n >
mmlVector<n> mmlLerp(const mmlVector<n> &u, const mmlVector<n> &v, float d)
{
	return u + d * (v - u);
}

//
// mmlLerp
//
template < int n >
mmlVector<n> mmlLerp(const mmlVector<n> &u, const mmlVector<n> &v, const mmlVector<n> &d)
{
	return u + (d * (v - u));
}

//
// mmlSlerp
//
template < int n >
mmlVector<n> mmlSlerp(const mmlVector<n> &u, mmlVector<n> v, float d)
{
	float dot = mmlClamp(-1.0f, mmlDot(u, v), 1.0f); // clamp to remove floating point imprecision issues
	float theta = acos(dot) * d;
	v = v - u * dot;
	v.Normalize();
	return ((u * cos(theta)) + (v * sin(theta)));
}

//
// mmlNlerp
//
template < int n >
mmlVector<n> mmlNlerp(const mmlVector<n> &u, const mmlVector<n> &v, float d)
{
	return mmlNormalize(mmlLerp(u, v, d));
}

//
// mmlQlerp
//
template < int n >
mmlVector<n> mmlQlerp(const mmlVector<n> &a, const mmlVector<n> &b, const mmlVector<n> &c, const mmlVector<n> &d, float u, float v)
{
	mmlVector<n> v1 = mmlLerp(a, b, u);
	mmlVector<n> v2 = mmlLerp(d, c, u);
	return mmlLerp(v1, v2, v);
}

//
// mmlNormalize
//
template < int n >
inline mmlVector<n> mmlNormalize(mmlVector<n> v)
{
	v.Normalize();
	return v;
}

//
// mmlNormalizeFast
//
template < int n >
inline mmlVector<n> mmlNormalizeFast(mmlVector<n> v)
{
	v.NormalizeFast();
	return v;
}

//
// mmlMin
//
template < int n >
inline mmlVector<n> mmlMin(const mmlVector<n> &x, mmlVector<n> y)
{
	for (int i = 0; i < n; ++i) {
		y[i] = (y[i] < x[i]) ? y[i] : x[i];
	}
	return y;
}

//
// mmlMax
//
template < int n >
inline mmlVector<n> mmlMax(const mmlVector<n> &x, mmlVector<n> y)
{
	for (int i = 0; i < n; ++i) {
		y[i] = (y[i] > x[i]) ? y[i] : x[i];
	}
	return y;
}

//
// mmlReflect
//
inline mmlVector<3> mmlReflect(const mmlVector<3> &incident, const mmlVector<3> &surfaceNormal)
{
	return incident - surfaceNormal * 2.0f * mmlDot(incident, surfaceNormal);
}

//
// mmlRefract
//
inline bool mmlRefract(const mmlVector<3> &incident, const mmlVector<3> &surfaceNormal, float incidentRefractionIndex, float surfaceRefractionIndex, mmlVector<3> &outRefraction)
{
	const float n = incidentRefractionIndex / surfaceRefractionIndex;
	const float cos = mmlDot(surfaceNormal, incident);
	const float sin = n * n * (1.0f - cos * cos);
	if (sin > 1.0f) {
		return false;
	}
	outRefraction = n * incident - (n + sqrt(1.0f - sin)) * surfaceNormal;
	return true;
}

#endif
