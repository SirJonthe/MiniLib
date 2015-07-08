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
inline float     mmlDegToRad(float pDeg)         { return pDeg *((mmlRAD_QUAD*2.f)/(mmlDEG_QUAD*2.f)); }
inline float     mmlRadToDeg(float pRad)         { return pRad *((mmlDEG_QUAD*2.f)/(mmlRAD_QUAD*2.f)); }
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

template < typename T > inline T    mmlMin2(const T &pA, const T &pB)                    { return (pA < pB) ? (pA) : (pB); }
template < typename T > inline T    mmlMax2(const T &pA, const T &pB)                    { return (pA > pB) ? (pA) : (pB); }
template < typename T > inline T    mmlMin3(const T &pA, const T &pB, const T &pC)       { return mmlMin2(mmlMin2(pA,pB),pC); }
template < typename T > inline T    mmlMax3(const T &pA, const T &pB, const T &pC)       { return mmlMax2(mmlMax2(pA,pB),pC); }
template < typename T > inline T    mmlClamp(const T &min, const T &value, const T &max) { return value < min ? min : (value > max ? max : value); }

template < typename T > inline void mmlSwap(T &pA, T &pB)                                { T temp = pA; pA = pB; pB = temp; }

template < typename TA, typename TB > inline TA mmlLerp(TA a, TA b, TB x)                             { return a + (b - a) * x; }
template < typename TA, typename TB > inline TA mmlBilerp(TA i00, TA i10, TA i01, TA i11, TB x, TB y) { return mmlLerp(mmlLerp(i00, i10, x), mmlLerp(i01, i11, x), y); }
template < typename TA, typename TB > inline TA mmlTrilerp(TA i000, TA i100, TA i010, TA i110, TA i001, TA i101, TA i011, TA i111, TB x, TB y, TB z)
																									  { return mmlLerp(mmlBilerp(i000, i100, i010, i110, x, y), mmlBilerp(i001, i101, i011, i111, x, y), z); }
template < typename TA, typename TB > inline TA mmlQuerp(TA a, TA b, TA c, TA d, TB x)                { return b + 0.5 * x * (c - a + x * (2 * a - 5 * b + 4 * c - d + x * (3 * (b - c) + d - a))); }
template < typename TA, typename TB > inline TA mmlBiquerp(TA i[4][4], TB x, TB y)                    { return mmlQuerp(mmlQuerp(i[0], y), mmlQuerp(i[1], y), mmlQuerp(i[2], y), mmlQuerp(i[3], y), x); }
template < typename TA, typename TB > inline TA mmlTriquerp(TA i[4][4][4], TB x, TB y, TB z)          { return mmlQuerp(mmlBiquerp(i[0], y, z), mmlBiquerp(i[1], y, z), mmlBiquerp(i[2], y, z), mmlBiquerp(i[3], y, z), x); }

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
