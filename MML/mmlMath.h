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
inline float mmlDegToRad(float pDeg)		{ return pDeg *((mmlRAD_QUAD*2.f)/(mmlDEG_QUAD*2.f)); }
inline float mmlRadToDeg(float pRad)		{ return pRad *((mmlDEG_QUAD*2.f)/(mmlRAD_QUAD*2.f)); }
inline float mmlUnitToRad(float pUnit)		{ return pUnit * mmlRAD_MAX; }
inline float mmlRadToUnit(float pRad)		{ return pRad / mmlRAD_MAX; }
inline float mmlUnitToDeg(float pUnit)		{ return pUnit * mmlDEG_MAX; }
inline float mmlDegToUnit(float pDeg)		{ return pDeg / mmlDEG_MAX; }
inline bool mmlIsPow2(unsigned char x)		{ return (x != 0) && !(x & (x - 1)); }
inline bool mmlIsPow2(unsigned short x)		{ return (x != 0) && !(x & (x - 1)); }
inline bool mmlIsPow2(unsigned int x)		{ return (x != 0) && !(x & (x - 1)); }
inline bool mmlIsPow2(unsigned long long x)	{ return (x != 0) && !(x & (x - 1)); }
template < typename T > inline T mmlMin2(const T &pA, const T &pB) { return (pA < pB) ? (pA) : (pB); }
template < typename T > inline T mmlMax2(const T &pA, const T &pB) { return (pA > pB) ? (pA) : (pB); }
template < typename T > inline T mmlMin3(const T &pA, const T &pB, const T &pC) { return mmlMin2(mmlMin2(pA,pB),pC); }
template < typename T > inline T mmlMax3(const T &pA, const T &pB, const T &pC) { return mmlMax2(mmlMax2(pA,pB),pC); }
template < typename T > inline void mmlSwap(T &pA, T&pB) { T temp = pA; pA = pB; pB = temp; }
template < typename T > inline T mmlClamp(const T &min, const T &value, const T &max) { return value < min ? min : (value > max ? max : value); }
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
	
	/*float xhalf = 0.5f*pX;
	int i = *(int*)&pX; // get bits for floating value
	i = 0x5f375a86 - (i>>1); // gives initial guess y0
	pX = *(float*)&i; // convert bits back to float
	pX = pX*(1.5f-xhalf*pX*pX); // Newton step, repeating increases accuracy

	// Due to a very good initial guess, no iterations are needed to recieve a really accurate result.
	// Initial guess: 0x5f375a86 (magic number)
	return pX;*/
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
