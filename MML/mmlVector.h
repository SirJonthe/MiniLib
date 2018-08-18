// Copyright (c) Jonathan Karlsson 2010
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_VECTOR_H_INCLUDED__
#define MML_VECTOR_H_INCLUDED__

#include <stdarg.h>
#include <cmath>

#include "mmlMath.h"
#include "mmlInt.h"

//
// mmlVector
//
template < int n, typename type_t = float >
class mmlVector
{
public:
	static const int Dimension = n;

private:
	type_t e[n];

public:
	//
	// default
	//
	mmlVector( void ) {
		//for (int i = 0; i < n; ++i) { e[i] = type_t(0.0); }
		// do nothing, let initialize to garbage
	}
	//
	// copy
	//
	mmlVector(const mmlVector<n,type_t> &v) {
		for (int j = 0; j < n; ++j) { e[j] = v.e[j]; }
	}
	//
	// initializer list
	//
	explicit mmlVector(const typename mml::va_cast<type_t>::va_t e0, ...) {
		va_list vl;
		va_start(vl, e0);
		e[0] = type_t(e0);
		for (int j = 1; j < n; ++j) {
			e[j] = type_t(va_arg(vl, typename mml::va_cast<type_t>::va_t));
		}
		va_end(vl);
	}
	//
	// conversion
	//
	template < int m >
	explicit mmlVector(const mmlVector<m,type_t> &v) {
		if (n < m) {
			for (int j = 0; j < n; ++j) { e[j] = v[j]; }
		} else {
			int j = 0;
			for (; j < m; ++j) { e[j] = v[j]; }
			for (; j < n; ++j) { e[j] = type_t(0.0); }
		}
	}
	//
	// conversion
	//
	explicit mmlVector(const type_t * const v) {
		for (int j = 0; j < n; ++j) { e[j] = v[j]; }
	}
public:
	//
	// index and conversion
	//
	operator type_t*( void ) { return e; }
	operator const type_t* const ( void ) const { return e; }
public:
	//
	// comparison
	//
	bool operator==(const mmlVector<n,type_t> &r) const {
		for (int j = 0; j < n; ++j) {
			if (e[j] != r[j]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const mmlVector<n,type_t> &r) const {
		for (int j = 0; j < n; ++j) {
			if (e[j] == r[j]) {
				return false;
			}
		}
		return true;
	}
	static bool Compare(const mmlVector<n,type_t> &u, const mmlVector<n,type_t> &v, const type_t &p_tolerance)
	{
		for (int i = 0; i < n; ++i) {
			if (u[i] < v[i] - p_tolerance || u[i] > v[i] + p_tolerance) {
				return false;
			}
		}
		return true;
	}
public:
	//
	// assignment operators
	//
	mmlVector<n,type_t> &operator=(const mmlVector<n,type_t> &r)
	{
		for (int j = 0; j < n; ++j) { e[j] = r.e[j]; }
		return *this;
	}
	mmlVector<n,type_t> &operator+=(const mmlVector<n,type_t> &r)
	{
		for (int j = 0; j < n; ++j) { e[j] += r.e[j]; }
		return *this;
	}
	mmlVector<n,type_t> &operator-=(const mmlVector<n,type_t> &r)
	{
		for (int j = 0; j < n; ++j) { e[j] -= r.e[j]; }
		return *this;
	}
	mmlVector<n,type_t> &operator*=(const mmlVector<n,type_t> &r)
	{
		for (int j = 0; j < n; ++j) { e[j] *= r.e[j]; }
		return *this;
	}
	mmlVector<n,type_t> &operator*=(const type_t &r)
	{
		for (int j = 0; j < n; ++j) { e[j] *= r; }
		return *this;
	}
	mmlVector<n,type_t> &operator/=(const type_t &r)
	{
		return *this *= (type_t(1.0) / r);
	}
	mmlVector<n,type_t> &operator/=(const mmlVector<n,type_t> &r)
	{
		for (int j = 0; j < n; ++j) { e[j] /= r.e[j]; }
		return *this;
	}
public:
	static mmlVector<n,type_t> &Cast(void *ptr)
	{
		return *(mmlVector<n,type_t>*)ptr;
	}
	static const mmlVector<n,type_t> &Cast(const void *ptr)
	{
		return *(const mmlVector<n,type_t>*)ptr;
	}
	void Clamp(const mmlVector<n,type_t> &min, const mmlVector<n,type_t> &max)
	{
		for (int i = 0; i < n; ++i) {
			e[i] = mmlClamp(min[i], e[i], max[i]);
		}
	}
	void Round( void )
	{
		for (int i = 0; i < n; ++i) {
			e[i] = floor(e[i] + type_t(0.5));
		}
	}
	void Floor( void )
	{
		for (int i = 0; i < n; ++i) {
			e[i] = floor(e[i]);
		}
	}
	void Ceil( void )
	{
		for (int i = 0; i < n; ++i) {
			e[i] = ceil(e[i]);
		}
	}
	void SnapInt( void )
	{
		for (int i = 0; i < n; ++i) {
			e[i] = type_t(int(e[i]));
		}
	}
	void Zero( void )
	{
		for (int i = 0; i < n; ++i) {
			e[i] = type_t(0.0);
		}
	}
	type_t Len( void ) const
	{
		type_t l = type_t(0.0);
		for (int j = 0; j < n; ++j) { l += e[j] * e[j]; }
		return mmlSqrt(l);
	}
	void Normalize( void )
	{
		const type_t invlen = type_t(1.0) / Len();
		for (int j = 0; j < n; ++j) {
			e[j] *= invlen;
		}
	}
	bool IsNormalized(const type_t &p_tolerance = type_t(0.0)) const
	{
		type_t unit = type_t(0.0);
		for (int i = 0; i < n; ++i) {
			unit += e[i] * e[i];
		}
		unit -= type_t(1.0);
		return (unit <= p_tolerance && unit >= -p_tolerance);
	}
	void FixDenormals( void )
	{
		for (int i = 0; i < n; ++i) {
			if (mmlAbs(e[i]) < 1e-30f) { e[i] = type_t(0.0); }
		}
	}
	void Swap(int i, int j)
	{
		type_t t = e[i];
		e[i] = e[j];
		e[j] = t;
	}
	void Abs( void )
	{
		for (int j = 0; j < n; ++j) {
			e[j] = mmlAbs(e[j]);
		}
	}
	template < int m >
	mmlVector<n+m,type_t> Join(const mmlVector<m,type_t> &b) {
		mmlVector<m+n,type_t> v;
		for (int i = 0; i < n; ++i) { v[i] = e[i]; }
		for (int i = 0; i < m; ++i) { v[n + i] = b[i]; }
		return v;
	}
	mmlVector<n+1,type_t> Join(const type_t &b) {
		mmlVector<n+1,type_t> v;
		for (int i = 0; i < n; ++i) { v[i] = e[i]; }
		v[n] = b;
		return v;
	}
};

//
// arithmetic operators
//
template < int n, typename type_t > inline mmlVector<n,type_t> operator+(mmlVector<n,type_t> l, const mmlVector<n,type_t> &r) { return (l+=r); }
template < int n, typename type_t > inline mmlVector<n,type_t> operator-(mmlVector<n,type_t> l, const mmlVector<n,type_t> &r) { return (l-=r); }
template < int n, typename type_t > inline mmlVector<n,type_t> operator*(mmlVector<n,type_t> l, const mmlVector<n,type_t> &r) { return (l*=r); }
template < int n, typename type_t > inline mmlVector<n,type_t> operator*(mmlVector<n,type_t> l, const type_t &r) { return (l*=r); }
template < int n, typename type_t > inline mmlVector<n,type_t> operator*(const type_t &l, mmlVector<n,type_t> r) { return (r*=l); }
template < int n, typename type_t > inline mmlVector<n,type_t> operator/(mmlVector<n,type_t> l, const type_t &r) { return (l/=r); }
template < int n, typename type_t > inline mmlVector<n,type_t> operator/(mmlVector<n,type_t> l, const mmlVector<n,type_t> &r) { return (l/=r); }
template < int n, typename type_t > inline mmlVector<n,type_t> operator-(mmlVector<n,type_t> v) {
	for (int j = 0; j < n; ++j) { v[j] = -v[j]; }
	return v;
}

//
// mmlDot
//
template < int n, typename type_t >
inline type_t mmlDot(const mmlVector<n,type_t> &u, const mmlVector<n,type_t> &v)
{
	type_t d = type_t(0.0);
	for (int j = 0; j < n; ++j) { d += u[j] * v[j]; }
	return d;
}

//
// mmlDist
//
template < int n, typename type_t >
inline float mmlDist(const mmlVector<n,type_t> &u, const mmlVector<n,type_t> &v)
{
	return (u - v).Len();
}

//
// mmlAng
//
template < int n, typename type_t >
inline double mmlAng(const mmlVector<n,type_t> &u, const mmlVector<n,type_t> &v)
{
	return acos(double(mmlDot(u,v) / (u.Len() * v.Len())));
}

//
// mmlCross
//
template < typename type_t >
inline mmlVector<3,type_t> mmlCross(const mmlVector<3,type_t> &u, const mmlVector<3,type_t> &v)
{
	mmlVector<3,type_t> res;
	res[0] = u[1]*v[2] - u[2]*v[1];
	res[1] = u[2]*v[0] - u[0]*v[2];
	res[2] = u[0]*v[1] - u[1]*v[0];
	return res;
}

//
// mmlCross2
//
template < typename type_t >
inline float mmlCross2(const mmlVector<2,type_t> &v, const mmlVector<2,type_t> &w)
{
	return v[0]*w[1] - v[1]*w[0];
}

//
// mmlSurfaceNormal
//
template < typename type_t >
inline mmlVector<3> mmlSurfaceNormal(const mmlVector<3,type_t> &x, const mmlVector<3,type_t> &y, const mmlVector<3,type_t> &z) // this should be a specialization of mmlVector<3>
{
	mmlVector<3,type_t> normal = mmlCross(y-x, z-x);
	normal.Normalize();
	return normal;
}

//
// mmlSlerp
//
template < int n, typename type_t >
mmlVector<n> mmlSlerp(const mmlVector<n,type_t> &u, mmlVector<n,type_t> v, const type_t &d)
{
	type_t dot = mmlClamp(type_t(-1.0), mmlDot(u, v), type_t(1.0)); // clamp to remove floating point imprecision issues
	type_t theta = acos(dot) * d;
	v = v - u * dot;
	v.Normalize();
	return ((u * cos(theta)) + (v * sin(theta)));
}

//
// mmlNlerp
//
template < int n, typename type_t  >
mmlVector<n,type_t> mmlNlerp(const mmlVector<n,type_t> &u, const mmlVector<n,type_t> &v, const type_t &d)
{
	return mmlNormalize(mmlLerp(u, v, d));
}

//
// mmlQlerp
//
template < int n, typename type_t  >
mmlVector<n,type_t> mmlQlerp(const mmlVector<n,type_t> &a, const mmlVector<n,type_t> &b, const mmlVector<n,type_t> &c, const mmlVector<n,type_t> &d, const type_t &u, const type_t &v)
{
	mmlVector<n,type_t> v1 = mmlLerp(a, b, u);
	mmlVector<n,type_t> v2 = mmlLerp(d, c, u);
	return mmlLerp(v1, v2, v);
}

//
// mmlNormalize
//
template < int n, typename type_t  >
inline mmlVector<n> mmlNormalize(mmlVector<n,type_t> v)
{
	v.Normalize();
	return v;
}

//
// mmlMin
//
template < int n, typename type_t  >
inline mmlVector<n> mmlMin(const mmlVector<n> &x, mmlVector<n> y)
{
	for (int i = 0; i < n; ++i) {
		y[i] = mmlMin(x[i], y[i]);
	}
	return y;
}

//
// mmlMax
//
template < int n, typename type_t  >
inline mmlVector<n> mmlMax(const mmlVector<n> &x, mmlVector<n> y)
{
	for (int i = 0; i < n; ++i) {
		y[i] = mmlMax(x[i], y[i]);
	}
	return y;
}

//
// mmlReflect
//
template < int n, typename type_t  >
inline mmlVector<n,type_t> mmlReflect(const mmlVector<n,type_t> &incident, const mmlVector<n,type_t> &surfaceNormal)
{
	return incident - surfaceNormal * type_t(2.0) * mmlDot(incident, surfaceNormal);
}

//
// mmlRefract
//
template < int n, typename type_t  >
inline bool mmlRefract(const mmlVector<n,type_t> &incident, const mmlVector<n,type_t> &surfaceNormal, const type_t &incidentRefractionIndex, const type_t &surfaceRefractionIndex, mmlVector<n,type_t> &outRefraction)
{
	const type_t rel = incidentRefractionIndex / surfaceRefractionIndex;
	const type_t cos = mmlDot(surfaceNormal, incident);
	const type_t sin = rel * rel * (type_t(1.0) - cos * cos);
	if (sin > type_t(1.0)) {
		return false;
	}
	outRefraction = rel * incident - (rel + sqrt(type_t(1.0) - sin)) * surfaceNormal;
	return true;
}

//
// mmlTangent
//
template < typename type_t >
inline mmlVector<2,type_t> mmlTangent(const mmlVector<2,type_t> &normal)
{
	mmlVector<2,type_t> tan;
	tan[0] = -normal[1];
	tan[1] =  normal[0];
	return tan;
}

//
// mmlLineNormal
//
template < typename type_t >
inline mmlVector<2,type_t> mmlLineNormal(const mmlVector<2,type_t> &a, const mmlVector<2,type_t> &b)
{
	return mmlTangent(mmlNormalize(b - a));
}

//
// mmlAbs
//
template < int n, typename type_t >
inline mmlVector<n,type_t> mmlAbs(mmlVector<n,type_t> v)
{
	v.Abs();
	return v;
}

#endif
