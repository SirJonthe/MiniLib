// Copyright (c) Jonathan Karlsson 2010
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_VECTOR_H_INCLUDED
#define MML_VECTOR_H_INCLUDED

#define VAT typename mml::va_cast<type_t>::va_t

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
	explicit mmlVector(const VAT e0, ...) {
		va_list vl;
		va_start(vl, e0);
		e[0] = type_t(e0);
		for (int j = 1; j < n; ++j) {
			e[j] = type_t(va_arg(vl, VAT));
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
			for (; j < n; ++j) { e[j] = type_t(0.0f); }
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
	operator const type_t*( void ) const { return e; }
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
	static bool Compare(const mmlVector<n,type_t> &u, const mmlVector<n,type_t> &v)
	{
		for (int i = 0; i < n; ++i) {
			if (mmlIsApproxEqual(u[i], v[i]) == false) {
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
		return *this *= (type_t(1.0f) / r);
	}
	mmlVector<n,type_t> &operator/=(const mmlVector<n,type_t> &r)
	{
		for (int j = 0; j < n; ++j) { e[j] /= r.e[j]; }
		return *this;
	}
public:
	static mmlVector<n,type_t> &Cast(void *ptr)
	{
		return *reinterpret_cast<mmlVector<n,type_t>*>(ptr);
	}
	static const mmlVector<n,type_t> &Cast(const void *ptr)
	{
		return *reinterpret_cast<const mmlVector<n,type_t>*>(ptr);
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
			e[i] = floor(e[i] + type_t(0.5f));
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
			e[i] = type_t(0.0f);
		}
	}
	type_t Len2( void ) const
	{
		type_t l = type_t(0.0f);
		for (int j = 0; j < n; ++j) { l += e[j] * e[j]; }
		return l;
	}
	type_t Len( void ) const
	{
		return mmlSqrt(Len2());
	}
	bool IsNormalized( void ) const
	{
		return mmlIsApproxEqual(Len2(), type_t(1));
	}
	void Normalize( void )
	{
		const type_t invlen = type_t(1.0f) / Len();
		for (int j = 0; j < n; ++j) {
			e[j] *= invlen;
		}
	}
	void NormalizeIf( void )
	{
		const type_t l2 = Len2();
		if (mmlIsApproxEqual(l2, type_t(0)) == false && mmlIsApproxEqual(l2, type_t(1)) == false) {
			Normalize();
		}
	}
	void FixDenormals( void )
	{
		for (int i = 0; i < n; ++i) {
//			if (mmlAbs(e[i]) < type_t(1e-30f)) { e[i] = type_t(0.0f); }
			if (mmlIsApproxEqual(e[i], type_t(0)) == true) { e[i] = type_t(0); }
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
	static mmlVector<n,type_t> Fill(const type_t &val) {
		mmlVector<n> v;
		for (int i = 0; i < n; ++i) { v[i] = val; }
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
	type_t d = type_t(0.0f);
	for (int j = 0; j < n; ++j) { d += u[j] * v[j]; }
	return d;
}

//
// mmlDist
//
template < int n, typename type_t >
inline type_t mmlDist(const mmlVector<n,type_t> &u, const mmlVector<n,type_t> &v)
{
	return (u - v).Len();
}

//
// mmlAng
//
template < int n, typename type_t >
inline type_t mmlAng(const mmlVector<n,type_t> &u, const mmlVector<n,type_t> &v)
{
	return acos(mmlDot(u,v) / (u.Len() * v.Len()));
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
// mmlCross
//
template < typename type_t >
inline type_t mmlCross(const mmlVector<2,type_t> &v, const mmlVector<2,type_t> &w)
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
mmlVector<n,type_t> mmlSlerp(const mmlVector<n,type_t> &u, mmlVector<n,type_t> v, const type_t &d)
{
	type_t dot = mmlClamp(type_t(-1.0f), mmlDot(u, v), type_t(1.0f)); // clamp to remove floating point imprecision issues
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
inline mmlVector<n,type_t> mmlNormalize(mmlVector<n,type_t> v)
{
	v.Normalize();
	return v;
}

//
// mmlNormalizeIf
//
template < int n, typename type_t  >
inline mmlVector<n,type_t> mmlNormalizeIf(mmlVector<n,type_t> v)
{
	v.NormalizeIf();
	return v;
}

//
// mmlMin
//
template < int n, typename type_t  >
inline mmlVector<n,type_t> mmlMin(const mmlVector<n,type_t> &x, mmlVector<n,type_t> y)
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
inline mmlVector<n,type_t> mmlMax(const mmlVector<n,type_t> &x, mmlVector<n,type_t> y)
{
	for (int i = 0; i < n; ++i) {
		y[i] = mmlMax(x[i], y[i]);
	}
	return y;
}

//
// mmlMin
//
template < int n, typename type_t >
inline type_t mmlMin(const mmlVector<n,type_t> &v)
{
	type_t x = v[0];
	for (int i = 1; i < n; ++i) {
		x = mmlMin(x, v[i]);
	}
	return x;
}

//
// mmlMax
//
template < int n, typename type_t >
inline type_t mmlMax(const mmlVector<n,type_t> &v)
{
	type_t x = v[0];
	for (int i = 1; i < n; ++i) {
		x = mmlMax(x, v[i]);
	}
	return x;
}

//
// mmlReflect
//
template < int n, typename type_t  >
inline mmlVector<n,type_t> mmlReflect(const mmlVector<n,type_t> &incident, const mmlVector<n,type_t> &surfaceNormal)
{
	return incident - surfaceNormal * type_t(2.0f) * mmlDot(incident, surfaceNormal);
}

//
// mmlRefract
//
template < int n, typename type_t  >
inline bool mmlRefract(const mmlVector<n,type_t> &incident, const mmlVector<n,type_t> &surfaceNormal, const type_t &incidentRefractionIndex, const type_t &surfaceRefractionIndex, mmlVector<n,type_t> &outRefraction)
{
	const type_t rel = incidentRefractionIndex / surfaceRefractionIndex;
	const type_t cos = mmlDot(surfaceNormal, incident);
	const type_t sin = rel * rel * (type_t(1.0f) - cos * cos);
	if (sin > type_t(1.0f)) {
		return false;
	}
	outRefraction = rel * incident - (rel + sqrt(type_t(1.0f) - sin)) * surfaceNormal;
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

//
// mmlAnyTangent
//
template < typename type_t >
inline mmlVector<3,type_t> mmlAnyTangent(const mmlVector<3,type_t> &normal)
{
	const mmlVector<3,type_t> t1 = mmlCross(normal, mmlVector<3,type_t>(VAT(0), VAT(0), VAT(1)));
	const mmlVector<3,type_t> t2 = mmlCross(normal, mmlVector<3,type_t>(VAT(0), VAT(1), VAT(0)));
	return t1.Len2() > t2.Len2() ? t1 : t2;
}

#undef VAT

#endif
