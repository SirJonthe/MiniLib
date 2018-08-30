// Copyright (c) Jonathan Karlsson 2010
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_MATH_H_INCLUDED
#define MML_MATH_H_INCLUDED

#include <cmath>

#define mmlPI 3.14159265f

#define mmlDEG_MIN 0.0f
#define mmlDEG_MAX 360.0f
#define mmlDEG_QUAD (mmlDEG_MAX / 4.0f)

#define mmlRAD_MIN 0.0f
#define mmlRAD_MAX (mmlPI * 2.0f)
#define mmlRAD_QUAD (mmlRAD_MAX / 4.0f)

/* general mathematical functions
 ========================================*/
// inline float     mmlDegToRad(float pDeg)         { return pDeg *((mmlRAD_QUAD*2.0f)/(mmlDEG_QUAD*2.0f)); }
// inline float     mmlRadToDeg(float pRad)         { return pRad *((mmlDEG_QUAD*2.0f)/(mmlRAD_QUAD*2.0f)); }

template < typename real_t > inline real_t mmlDegToRad(const real_t &deg) { return deg * real_t(mmlRAD_QUAD / mmlDEG_QUAD); }
template < typename real_t > inline real_t mmlRadToDeg(const real_t &rad) { return rad * real_t(mmlDEG_QUAD / mmlRAD_QUAD); }

template < typename int_t > inline bool mmlIsPow2(const int_t &x) { return (x != 0) && !(x & (x - 1)); }

inline int       mmlRound(float x)               { return int(x + 0.5f); }
inline long long mmlRound(double x)              { return (long long)(x + 0.5); }
inline int       mmlFloor(float x)               { return int(x + float(1<<15)) - (1<<15); }
inline long long mmlFloor(double x)              { return (long long)(x + double(1<<31)) - (1<<31); }
inline int       mmlCeil(float x)                { return (1<<15) - int(float(1<<15) - x); }
inline long long mmlCeil(double x)               { return (1<<31) - (long long)(double(1<<31) - x); }

template < typename real_t > real_t mmlFmod(const real_t &a, const real_t &b) { return (a - b * mmlFloor(a / b)); }

template < typename num_t > inline num_t mmlMin(const num_t &pA, const num_t &pB)                         { return (pA < pB) ? (pA) : (pB); }
template < typename num_t > inline num_t mmlMax(const num_t &pA, const num_t &pB)                         { return (pA > pB) ? (pA) : (pB); }
template < typename num_t > inline num_t mmlMin(const num_t &pA, const num_t &pB, const num_t &pC)        { return mmlMin(mmlMin(pA, pB), pC); }
template < typename num_t > inline num_t mmlMax(const num_t &pA, const num_t &pB, const num_t &pC)        { return mmlMax(mmlMax(pA, pB), pC); }
template < typename num_t > inline num_t mmlClamp(const num_t &min, const num_t &value, const num_t &max) { return mmlMax(mmlMin(value, max), min); }
template < typename num_t > inline num_t mmlClamp01(const num_t &value)                                   { return mmlClamp((num_t)0, value, (num_t)1); }
template < typename num_t > inline num_t mmlAbs(const num_t &x)                                           { return mmlMax(x, -x); }
//template < typename T > inline T mmlWrap(T value, const T &max) {
//	if (value > max)    { value = value % (max + 1); }
//	else if (value < 0) { value = (max + 1) - (-value % max); }
//	return value;
//}
//inline double mmlWrap(double value, const double &max) {
//	if (value > max)      { value = mmlFmod(value, max); }
//	else if (value < 0.0) { value = max - (mmlFmod(-value, max)); }
//	return value;
//}
//inline float mmlWrap(float value, const float &max) {
//	if (value > max)       { value = mmlFmod(value, max); }
//	else if (value < 0.0f) { value = max - (mmlFmod(-value, max)); }
//	return value;
//}
template < typename real_t > inline real_t mmlWrap(const real_t &value, const real_t &max)
{
	real_t ratio = value / max;
	return max * (ratio - mmlFloor(ratio));
}
template < typename real_t > inline real_t mmlWrap(const real_t &min, const real_t & value, const real_t &max) { return mmlWrap(value - min, max - min) + min; }

#define mmlAtLeast mmlMax
#define mmlAtMost  mmlMin
#define mmlRepeat  mmlWrap

template < typename num_t > inline void mmlSwap(num_t &pA, num_t &pB) { num_t temp = pA; pA = pB; pB = temp; }

template < typename TA, typename TB > inline TA mmlLerp(const TA &a, const TA &b, const TB &x) { return a + (b - a) * x; }
template < typename TA, typename TB > inline TA mmlBilerp(const TA &i00, const TA &i10, const TA &i01, const TA &i11, const TB &x, const TB &y) { return mmlLerp(mmlLerp(i00, i10, x), mmlLerp(i01, i11, x), y); }
template < typename TA, typename TB > inline TA mmlTrilerp(const TA &i000, const TA &i100, const TA &i010, const TA &i110, const TA &i001, const TA &i101, const TA &i011, const TA &i111, const TB &x, const TB &y, const TB &z) { return mmlLerp(mmlBilerp(i000, i100, i010, i110, x, y), mmlBilerp(i001, i101, i011, i111, x, y), z); }
template < typename TA, typename TB > inline TA mmlQuerp(const TA &a, const TA &b, const TA &c, const TA &d, const TB &x) { return b + 0.5 * x * (c - a + x * (2 * a - 5 * b + 4 * c - d + x * (3 * (b - c) + d - a))); }
template < typename TA, typename TB > inline TA mmlBiquerp(const TA i[4][4], const TB &x, const TB &y) { return mmlQuerp(mmlQuerp(i[0], y), mmlQuerp(i[1], y), mmlQuerp(i[2], y), mmlQuerp(i[3], y), x); }
template < typename TA, typename TB > inline TA mmlTriquerp(const TA i[4][4][4], const TB &x, const TB &y, const TB &z) { return mmlQuerp(mmlBiquerp(i[0], y, z), mmlBiquerp(i[1], y, z), mmlBiquerp(i[2], y, z), mmlBiquerp(i[3], y, z), x); }

template < typename real_t > inline real_t mmlSqrt(const real_t &x) { return real_t(sqrt(double(x))); }

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

inline bool mmlIsNAN(float x)
{
	volatile float tmp = x;
	return tmp != x;
}

inline bool mmlIsNAN(double x)
{
	volatile double tmp = x;
	return tmp != x;
}

inline bool mmlIsInf(float x)
{
	volatile float tmp = x;
	return (tmp == x) && ((tmp - x) != 0.0f) && (x > 0.0f);
}

inline bool mmlIsInf(double x)
{
	volatile double tmp = x;
	return (tmp == x) && ((tmp - x) != 0.0) && (x > 0.0);
}

inline bool mmlIsNegInf(float x)
{
	volatile float tmp = x;
	return (tmp == x) && ((tmp - x) != 0.0f) && (x < 0.0f);
}

inline bool mmlIsNegInf(double x)
{
	volatile double tmp = x;
	return (tmp == x) && ((tmp - x) != 0.0) && (x < 0.0);
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

template < typename real_t >
inline real_t mmlSin(real_t x)
{
	// based on: http://lab.polygonal.de/2007/07/18/fast-and-accurate-sinecosine-approximation/

	// Wrap input to valid range -pi ... pi
	// NOTE: Wrapping needs to be a branchless function (mmlWrap) in order to make MPL work
//	if (x < -mmlPI) {
//		do {
//			x += (mmlPI * real_t(2));
//		} while (x < -mmlPI);
//	} else if (x >  mmlPI) {
//		do {
//			x -= (mmlPI * real_t(2));
//		} while (x >  mmlPI);
//	}
	x = mmlWrap(real_t(-mmlPI), x, real_t(mmlPI));

	// Compute sine
	// 0.405284735 = 1.27323954 / PI
	real_t sin1 = x < real_t(0) ?
		real_t(1.27323954) * x + real_t(0.405284735) * x * x :
		real_t(1.27323954) * x - real_t(0.405284735) * x * x;
	real_t sin = sin1 < real_t(0) ?
		real_t(0.225) * (sin1 * -sin1 - sin1) + sin1 :
		real_t(0.225) * (sin1 *  sin1 - sin1) + sin1;
	return sin;
}

template < typename real_t >
inline real_t mmlCos(const real_t &x)
{
	return mmlSin(x + (mmlPI / real_t(2)));
}

#endif
