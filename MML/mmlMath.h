// Copyright (c) Jonathan Karlsson 2010
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_MATH_H_INCLUDED
#define MML_MATH_H_INCLUDED

#include <cmath>
#include "mmlInt.h"

template < typename int_t > inline bool mmlIsPow2(const int_t &x) { return (x != 0) && !(x & (x - 1)); }

inline mml::sint32 mmlRound(float x)  { return mml::sint32(x + 0.5f); }
inline mml::sint64 mmlRound(double x) { return mml::sint64(x + 0.5); }
inline mml::sint32 mmlFloor(float x)  { return mml::sint32(x + float(1<<15)) - (1<<15); }
inline mml::sint64 mmlFloor(double x) { return mml::sint64(x + double(1<<31)) - (1<<31); }
inline mml::sint32 mmlCeil(float x)   { return (1<<15) - mml::sint32(float(1<<15) - x); }
inline mml::sint64 mmlCeil(double x)  { return (1<<31) - mml::sint64(double(1<<31) - x); }

//template < typename real_t > inline real_t mmlFract(const real_t &x) {  }
//template < typename real_t > inline real_t mmlFloor(const real_t &x) { return x - mmlFract(x); }
//template < typename real_t > inline real_t mmlCeil(const real_t &x)  { return x + (real_t(1) - mmlFract(x)); } // NOPE!!
//template < typename real_t > inline real_t mmlRound(const real_t &x) { return mmlFloor(x + real_t(0.5)); }
//template < typename real_t > inline real_t mmlTrunc(const real_t &x) { return ; }

template < typename num_t >  inline num_t  mmlMin(const num_t &pA, const num_t &pB)                            { return (pA < pB) ? (pA) : (pB); }
template < typename num_t >  inline num_t  mmlMax(const num_t &pA, const num_t &pB)                            { return (pA > pB) ? (pA) : (pB); }
template < typename num_t >  inline num_t  mmlMin(const num_t &pA, const num_t &pB, const num_t &pC)           { return mmlMin(mmlMin(pA, pB), pC); }
template < typename num_t >  inline num_t  mmlMax(const num_t &pA, const num_t &pB, const num_t &pC)           { return mmlMax(mmlMax(pA, pB), pC); }
template < typename num_t >  inline num_t  mmlClamp(const num_t &min, const num_t &value, const num_t &max)    { return mmlMax(mmlMin(value, max), min); }
template < typename num_t >  inline num_t  mmlClamp(const num_t &value)                                        { return mmlClamp(num_t(0), value, num_t(1)); }
template < typename num_t >  inline num_t  mmlAbs(const num_t &x)                                              { return mmlMax(x, -x); }
template < typename real_t > inline real_t mmlWrap(const real_t &value)                                        { return value - mmlFloor(value); }
template < typename real_t > inline real_t mmlWrap(const real_t &value, const real_t &max)                     { real_t ratio = value / max; return max * mmlWrap(ratio); }
template < typename real_t > inline real_t mmlWrap(const real_t &min, const real_t & value, const real_t &max) { return mmlWrap(value - min, max - min) + min; }
template < typename int_t >  inline type_t mmlIWrap(const type_t &min, const type_t &x, const type_t &max)         { type_t span = max - min; return min + (x + mmlAbs(x) * span - min) % span; }
template < typename num_t >  inline void   mmlSwap(num_t &pA, num_t &pB)                                       { num_t temp = pA; pA = pB; pB = temp; }

#define mmlAtLeast mmlMax
#define mmlAtMost  mmlMin
#define mmlRepeat  mmlWrap

template < typename TA, typename TB > inline TA mmlLerp(const TA &a, const TA &b, const TB &x) { return a + (b - a) * x; }
template < typename TA, typename TB > inline TA mmlBilerp(const TA &i00, const TA &i10, const TA &i01, const TA &i11, const TB &x, const TB &y) { return mmlLerp(mmlLerp(i00, i10, x), mmlLerp(i01, i11, x), y); }
template < typename TA, typename TB > inline TA mmlTrilerp(const TA &i000, const TA &i100, const TA &i010, const TA &i110, const TA &i001, const TA &i101, const TA &i011, const TA &i111, const TB &x, const TB &y, const TB &z) { return mmlLerp(mmlBilerp(i000, i100, i010, i110, x, y), mmlBilerp(i001, i101, i011, i111, x, y), z); }
template < typename TA, typename TB > inline TA mmlQuerp(const TA &a, const TA &b, const TA &c, const TA &d, const TB &x) { return b + 0.5 * x * (c - a + x * (2 * a - 5 * b + 4 * c - d + x * (3 * (b - c) + d - a))); }
template < typename TA, typename TB > inline TA mmlBiquerp(const TA i[4][4], const TB &x, const TB &y) { return mmlQuerp(mmlQuerp(i[0], y), mmlQuerp(i[1], y), mmlQuerp(i[2], y), mmlQuerp(i[3], y), x); }
template < typename TA, typename TB > inline TA mmlTriquerp(const TA i[4][4][4], const TB &x, const TB &y, const TB &z) { return mmlQuerp(mmlBiquerp(i[0], y, z), mmlBiquerp(i[1], y, z), mmlBiquerp(i[2], y, z), mmlBiquerp(i[3], y, z), x); }

template < typename real_t > inline real_t mmlSqrt(const real_t &x)                  { return real_t(sqrt(double(x))); }
template < typename real_t > inline real_t mmlFmod(const real_t &a, const real_t &b) { return (a - b * mmlFloor(a / b)); }
template < typename num_t >  inline num_t  mmlSign(const num_t &x)                   { return num_t((x >= num_t(0)) - (x < num_t(0))); }
//template < typename num_t >  inline num_t  mmlPow(const num_t &x, const num_t &y)    { return mmlExp(y * mmlLog(x)); }

//inline float mmlFastInvSqrt(float pX)
//{
//	union bits32
//	{
//		int		i;
//		float	f;
//	};
//	volatile bits32 b;
//	b.f = pX;
//	b.i = 0x5f375a86 - (b.i >> 1);
//	return b.f * (1.5f - 0.5f * pX * b.f * b.f);
//}
//inline float mmlFastSqrt(float pX) { return pX*mmlFastInvSqrt(pX); }
//inline int mmlX86RoundCast(double x)
//{
//	union bits64
//	{
//		long long	i;
//		double		f;
//	};
//	volatile bits64 b;
//	b.f = x + 6755399441055744.0;
//	return (int)b.i;
//}
//inline int mmlX86TruncCast(double x)
//{
//	union bits64
//	{
//		long long	i;
//		double		f;
//	};
//	volatile bits64 b;
//	b.f = x + 6755399441055743.5;
//	return (int)b.i;
//}

template < typename float_t >
inline bool mmlIsNAN(const float_t &x)
{
	volatile float_t tmp = x;
	return tmp != x;
}

template < typename float_t >
inline bool mmlIsInf(const float_t &x)
{
	volatile float_t tmp = x;
	return (tmp == x) && ((tmp - x) != float_t(0)) && (x > float_t(0));
}

template < typename float_t >
inline bool mmlIsNegInf(const float_t &x)
{
	volatile float_t tmp = x;
	return (tmp == x) && ((tmp - x) != float_t(0)) && (x < float_t(0));
}

template < int n, typename real_t >
inline real_t mmlPi( void )
{
	// Nilakantha series
	real_t pi = real_t(3);
	const int iter = n * 4;
	for (int i = 0; i < iter; i += 4) {
		pi = pi + (real_t(4) / real_t((i+2) * (i+3) * (i+4))) - (real_t(4) / real_t((i+4) * (i+5) * (i+6)));
	}
	return pi;
}

#define mmlPI 3.14159265f

#define mmlDEG_MIN 0.0f
#define mmlDEG_MAX 360.0f
#define mmlDEG_QUAD (mmlDEG_MAX / 4.0f)

#define mmlRAD_MIN 0.0f
#define mmlRAD_MAX (mmlPI * 2.0f)
#define mmlRAD_QUAD (mmlRAD_MAX / 4.0f)

template < typename real_t > inline real_t mmlDegToRad(const real_t &deg) { return deg * real_t(mmlRAD_QUAD / mmlDEG_QUAD); }
template < typename real_t > inline real_t mmlRadToDeg(const real_t &rad) { return rad * real_t(mmlDEG_QUAD / mmlRAD_QUAD); }

template < typename real_t >
inline real_t mmlSin(real_t x)
{
	// based on: http://lab.polygonal.de/2007/07/18/fast-and-accurate-sinecosine-approximation/

	// Wrap input to valid range -pi ... pi
//	if (x < -mmlPI) {
//		do {
//			x += (mmlPI * real_t(2));
//		} while (x < -mmlPI);
//	} else if (x >  mmlPI) {
//		do {
//			x -= (mmlPI * real_t(2));
//		} while (x >  mmlPI);
//	}
	// Compute sine
	// 1.27323954 = 4 / PI
	// 0.405284735 = 1.27323954 / PI
//	real_t sin1 = x < real_t(0) ?
//		real_t(1.27323954) * x + real_t(0.405284735) * x * x :
//		real_t(1.27323954) * x - real_t(0.405284735) * x * x;
//	real_t sin = sin1 < real_t(0) ?
//		real_t(0.225) * (sin1 * -sin1 - sin1) + sin1 :
//		real_t(0.225) * (sin1 *  sin1 - sin1) + sin1;
//	return sin;

	// Wrap input to valid range -pi ... pi
	x = mmlWrap(real_t(-mmlPI), x, real_t(mmlPI));
	// Compute sine
	// 1.27323954 = 4 / PI
	// 0.405284735 = 1.27323954 / PI
	const real_t magic_val1 = real_t(4) / real_t(mmlPI);
	const real_t magic_val2 = magic_val1 / real_t(mmlPI);
	const real_t sin1 = magic_val1 * x + -mmlSign(x) * magic_val2 * x * x;
	const real_t sin  = real_t(0.225) * (sin1 * mmlAbs(sin1) - sin1) + sin1;
	return sin;
}

//template < typename real_t >
//inline real_t mmlAsin(real_t x)
//{
//	x = mmlWrap(real_t(-1), x, real_t(1));
//
//	// work backwards from here
//	x = 0.255 * (s * abs(s) - s) + s;
//	s = 1.27323954 * a + sign(a) * 0.405284735 * a * a;
//
//	return a;
//}

template < typename real_t >
inline real_t mmlCos(const real_t &x)
{
	return mmlSin(x + (mmlPI / real_t(2)));
}

#endif
