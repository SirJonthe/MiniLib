// Copyright (c) Jonathan Karlsson 2010
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_MATH_H_INCLUDED__
#define MML_MATH_H_INCLUDED__

#include <cmath>

#define mmlPI 3.14159265f

#define mmlDEG_MIN 0.0f
#define mmlDEG_MAX 360.0f
#define mmlDEG_QUAD (mmlDEG_MAX / 4.0f)

#define mmlRAD_MIN 0.0f
#define mmlRAD_MAX (mmlPI * 2.0f)
#define mmlRAD_QUAD (mmlRAD_MAX / 4.0f)

#define mmlUNIT_MIN 0.0f
#define mmlUNIT_MAX 1.0f
#define mmlUNIT_QUAD (mmlUNIT_MAX / 4.0f)

/* general mathematical functions
 ========================================*/
inline float     mmlDegToRad(float pDeg)         { return pDeg *((mmlRAD_QUAD*2.0f)/(mmlDEG_QUAD*2.0f)); }
inline float     mmlRadToDeg(float pRad)         { return pRad *((mmlDEG_QUAD*2.0f)/(mmlRAD_QUAD*2.0f)); }
inline float     mmlUnitToRad(float pUnit)       { return pUnit * mmlRAD_MAX; }
inline float     mmlRadToUnit(float pRad)        { return pRad / mmlRAD_MAX; }
inline float     mmlUnitToDeg(float pUnit)       { return pUnit * mmlDEG_MAX; }
inline float     mmlDegToUnit(float pDeg)        { return pDeg / mmlDEG_MAX; }

inline bool      mmlIsPow2(unsigned char x)      { return (x != 0) && !(x & (x - 1)); }
inline bool      mmlIsPow2(unsigned short x)     { return (x != 0) && !(x & (x - 1)); }
inline bool      mmlIsPow2(unsigned int x)       { return (x != 0) && !(x & (x - 1)); }
inline bool      mmlIsPow2(unsigned long long x) { return (x != 0) && !(x & (x - 1)); }

inline int       mmlRound(float x)               { return int(x + 0.5f); }
inline long long mmlRound(double x)              { return (long long)(x + 0.5); }
inline int       mmlFloor(float x)               { return int(x + float(1<<15)) - (1<<15); }
inline long long mmlFloor(double x)              { return (long long)(x + double(1<<31)) - (1<<31); }
inline int       mmlCeil(float x)                { return (1<<15) - int(float(1<<15) - x); }
inline long long mmlCeil(double x)               { return (1<<31) - (long long)(double(1<<31) - x); }

template < typename T > inline T mmlMin(const T &pA, const T &pB)                     { return (pA < pB) ? (pA) : (pB); }
template < typename T > inline T mmlMax(const T &pA, const T &pB)                     { return (pA > pB) ? (pA) : (pB); }
template < typename T > inline T mmlMin(const T &pA, const T &pB, const T &pC)        { return mmlMin(mmlMin(pA,pB),pC); }
template < typename T > inline T mmlMax(const T &pA, const T &pB, const T &pC)        { return mmlMax(mmlMax(pA,pB),pC); }
template < typename T > inline T mmlClamp(const T &min, const T &value, const T &max) { return mmlMax(mmlMin(value, max), min); }
template < typename T > inline T mmlAbs(const T &x)                                   { return mmlMax(x, -x); }

#define mmlAtLeast mmlMax
#define mmlAtMost  mmlMin

template < typename T > inline void mmlSwap(T &pA, T &pB) { T temp = pA; pA = pB; pB = temp; }

template < typename TA, typename TB > inline TA mmlLerp(const TA &a, const TA &b, const TB &x) { return a + (b - a) * x; }
template < typename TA, typename TB > inline TA mmlBilerp(const TA &i00, const TA &i10, const TA &i01, const TA &i11, const TB &x, const TB &y) { return mmlLerp(mmlLerp(i00, i10, x), mmlLerp(i01, i11, x), y); }
template < typename TA, typename TB > inline TA mmlTrilerp(const TA &i000, const TA &i100, const TA &i010, const TA &i110, const TA &i001, const TA &i101, const TA &i011, const TA &i111, const TB &x, const TB &y, const TB &z) { return mmlLerp(mmlBilerp(i000, i100, i010, i110, x, y), mmlBilerp(i001, i101, i011, i111, x, y), z); }
template < typename TA, typename TB > inline TA mmlQuerp(const TA &a, const TA &b, const TA &c, const TA &d, const TB &x) { return b + 0.5 * x * (c - a + x * (2 * a - 5 * b + 4 * c - d + x * (3 * (b - c) + d - a))); }
template < typename TA, typename TB > inline TA mmlBiquerp(const TA i[4][4], const TB &x, const TB &y) { return mmlQuerp(mmlQuerp(i[0], y), mmlQuerp(i[1], y), mmlQuerp(i[2], y), mmlQuerp(i[3], y), x); }
template < typename TA, typename TB > inline TA mmlTriquerp(const TA i[4][4][4], const TB &x, const TB &y, const TB &z) { return mmlQuerp(mmlBiquerp(i[0], y, z), mmlBiquerp(i[1], y, z), mmlBiquerp(i[2], y, z), mmlBiquerp(i[3], y, z), x); }

template < typename type_t >
inline type_t mmlSqrt(const type_t &x)
{
	return type_t(sqrt((double)x));
}

inline float mmlFastInvSqrt(float pX)
{
	union bits32
	{
		int		i;
		float	f;
	};
	volatile bits32 b;
	b.f = pX;
	b.i = 0x5f375a86 - (b.i >> 1);
	return b.f * (1.5f - 0.5f * pX * b.f * b.f);
}
inline float mmlFastSqrt(float pX) { return pX*mmlFastInvSqrt(pX); }
inline int mmlX86RoundCast(double x)
{
	union bits64
	{
		long long	i;
		double		f;
	};
	volatile bits64 b;
	b.f = x + 6755399441055744.0;
	return (int)b.i;
}
inline int mmlX86TruncCast(double x)
{
	union bits64
	{
		long long	i;
		double		f;
	};
	volatile bits64 b;
	b.f = x + 6755399441055743.5;
	return (int)b.i;
}

inline bool mmlIsNAN(float x)
{
	volatile float tmp = x;
	return tmp != x;
}

inline bool mmlIsInf(float x)
{
   volatile float tmp = x;
   return (tmp == x) && ((tmp - x) != 0.0f) && (x > 0.0f);
}

inline bool mmlIsNegInf(float x)
{
	volatile float tmp = x;
	return (tmp == x) && ((tmp - x) != 0.0f) && (x < 0.0f);
}

template < int n, typename type_t >
inline type_t mmlPi( void )
{
	// Nilakantha series
	type_t pi = type_t(3);
	const int iter = n * 4;
	for (int i = 0; i < iter; i += 4) {
		pi = pi + (type_t(4) / type_t((i+2) * (i+3) * (i+4))) - (type_t(4) / type_t((i+4) * (i+5) * (i+6)));
	}
	return pi;
}

template < typename type_t >
inline type_t mmlSin(type_t x)
{
	// based on: http://lab.polygonal.de/2007/07/18/fast-and-accurate-sinecosine-approximation/

	// Wrap input to valid range -pi ... pi
	// NOTE: Wrapping needs to be a function (mmlWrap) in order to make MPL work
	if (x < -mmlPI) {
		do {
			x += (mmlPI * type_t(2));
		} while (x < -mmlPI);
	} else if (x >  mmlPI) {
		do {
			x -= (mmlPI * type_t(2));
		} while (x >  mmlPI);
	}

	// Compute sine
	// 0.405284735 = 1.27323954 / PI
	type_t sin1 = x < type_t(0) ?
		type_t(1.27323954) * x + type_t(0.405284735) * x * x :
		type_t(1.27323954) * x - type_t(0.405284735) * x * x;
	type_t sin = sin1 < type_t(0) ?
		type_t(0.225) * (sin1 * -sin1 - sin1) + sin1 :
		type_t(0.225) * (sin1 *  sin1 - sin1) + sin1;
	return sin;
}

template < typename type_t >
inline type_t mmlCos(const type_t &x)
{
	return mmlSin(x + (mmlPI / type_t(2)));
}

/*inline bool mmlIsNAN(float x)
{
	union bits32
	{
		int		i;
		float	f;
		bits32(float x) : f(x) {}
	};
	static const bits32 nan = 0.0f / 0.0f;
	const bits32 f = x;
	return f.i == nan.i;
}

inline bool mmlIsInf(float x)
{
	union bits32
	{
		int		i;
		float	f;
		bits32(float x) : f(x) {}
	};
	static const bits32 inf = 1.0f / 0.0f;
	const bits32 f = x;
	return f.i == inf.i;
}

inline bool mmlIsInfNeg(float x)
{
	union bits32
	{
		int		i;
		float	f;
		bits32(float x) : f(x) {}
	};
	static const bits32 ninf = -1.0f / 0.0f;
	const bits32 f = x;
	return f.i == ninf.i;
}*/

#endif
