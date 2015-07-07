#ifndef MPL_FLOAT4_H_INCLUDED__
#define MPL_FLOAT4_H_INCLUDED__

#include "mplCommon.h"
#include "mplMask4.h"
#include <cmath>

// MSVC ARM NEON looks *very* different from GCC ARM NEON

namespace mpl
{

#if mplCompiler == mplMSVC
	#if defined(mplMSVC_SSE)
		__declspec(align(16))
	#elif defined (mplMSVC_NEON)
		__declspec(align(4))
	#endif
#endif
class float4
{
private:
	union {
#if defined(mplMSVC_SSE) || defined(mplGCC_SSE)
		__m128 reg;
#elif defined(mplGCC_NEON)
		float32x4_t reg;
#endif
		float e[4];
	} data;

public:
	float4( void ) {}
	inline float4(float a, float b, float c, float d);
	inline float4(float abcd);
	inline explicit float4(const float *v);
	inline float4(const float *v, int stride);
	inline float4(const float4 &r);
	inline float4 &operator=(const float4 &r);

	operator       float *( void )       { return data.e; }
	operator const float *( void ) const { return data.e; }

	inline float4 &operator +=(const float4 &r);
	inline float4 &operator -=(const float4 &r);
	inline float4 &operator *=(const float4 &r);
	inline float4 &operator *=(float r);
	inline float4 &operator /=(const float4 &r);
	inline float4 &operator /=(float r);

	inline float4 operator +(const float4 &r) const;
	inline float4 operator -(const float4 &r) const;
	inline float4 operator -( void ) const;
	inline float4 operator *(const float4 &r) const;
	inline float4 operator *(float r) const;
	inline float4 operator /(const float4 &r) const;
	inline float4 operator /(float r) const;

	inline float4 &operator &=(const mask4 &r);
	inline float4 &operator |=(const mask4 &r);
	inline float4 &operator ^=(const mask4 &r);

	inline float4 operator &(const mask4 &r);
	inline float4 operator |(const mask4 &r);
	inline float4 operator ^(const mask4 &r);

	inline mask4 operator ==(const float4 &r) const;
	inline mask4 operator !=(const float4 &r) const;
	inline mask4 operator >=(const float4 &r) const;
	inline mask4 operator <=(const float4 &r) const;
	inline mask4 operator >(const float4 &r) const;
	inline mask4 operator <(const float4 &r) const;

	friend float4 sqrt(const float4 &f);
	friend float4 inv_sqrt(const float4 &f);
	friend float4 min(const float4 &a, const float4 &b);
	friend float4 max(const float4 &a, const float4 &b);
	friend float4 aligned_load(const float *v);
	friend void unaligned_store(const float4 &f, float *out);
	friend void aligned_store(const float4 &f, float *out);
}
#if mplCompiler == mplGCC
	#if defined(mplGCC_SSE)
		__attribute__((aligned(16)))
	#elif defined(mplGCC_NEON)
		__attribute__((aligned(4)))
	#endif
#endif
;

inline float4 sqrt(const float4 &f);
inline float4 inv_sqrt(const float4 &f);
inline float4 min(const float4 &a, const float4 &b);
inline float4 max(const float4 &a, const float4 &b);
inline float4 aligned_load(const float *v);
inline void unaligned_store(const float4 &f, float *v);
inline void aligned_store(const float4 &f, float *v);
inline float4 fabs(const float4 &a);

}

mpl::float4::float4(const mpl::float4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE) || defined(mplGCC_NEON)
	data.reg = r.data.reg;
#else
	data.e[0] = r.data.e[0];
	data.e[1] = r.data.e[1];
	data.e[2] = r.data.e[2];
	data.e[3] = r.data.e[3];
#endif
}

mpl::float4::float4(float a, float b, float c, float d)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_setr_ps(a, b, c, d);
#else
	data.e[0] = a;
	data.e[1] = b;
	data.e[2] = c;
	data.e[3] = d;
#endif
}

mpl::float4::float4(float abcd)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_set1_ps(abcd);
#else
	data.e[0] = abcd;
	data.e[1] = abcd;
	data.e[2] = abcd;
	data.e[3] = abcd;
#endif
}

mpl::float4::float4(const float *v)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_loadu_ps(v);
#else
	data.e[0] = v[0];
	data.e[1] = v[1];
	data.e[2] = v[2];
	data.e[3] = v[3];
#endif
}

mpl::float4::float4(const float *v, int stride)
{
	data.e[0] = *v;
	v+=stride;
	data.e[1] = *v;
	v+=stride;
	data.e[2] = *v;
	v+=stride;
	data.e[3] = *v;
	// NEON has instructions for this?
}

mpl::float4 &mpl::float4::operator =(const mpl::float4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE) || defined(mplGCC_NEON)
	data.reg = r.data.reg;
#else
	data.e[0] = r.data.e[0];
	data.e[1] = r.data.e[1];
	data.e[2] = r.data.e[2];
	data.e[3] = r.data.e[3];
#endif
	return *this;
}

mpl::float4 &mpl::float4::operator +=(const mpl::float4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_add_ps(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = vaddq_f32(data.reg, r.data.reg);
#else
	data.e[0] += r.data.e[0];
	data.e[1] += r.data.e[1];
	data.e[2] += r.data.e[2];
	data.e[3] += r.data.e[3];
#endif
	return *this;
}

mpl::float4 &mpl::float4::operator -=(const mpl::float4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_sub_ps(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = vsubq_f32(data.reg, r.data.reg);
#else
	data.e[0] -= r.data.e[0];
	data.e[1] -= r.data.e[1];
	data.e[2] -= r.data.e[2];
	data.e[3] -= r.data.e[3];
#endif
	return *this;
}

mpl::float4 &mpl::float4::operator *=(const mpl::float4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_mul_ps(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = vmulq_f32(data.reg, r.data.reg);
#else
	data.e[0] *= r.data.e[0];
	data.e[1] *= r.data.e[1];
	data.e[2] *= r.data.e[2];
	data.e[3] *= r.data.e[3];
#endif
	return *this;
}

mpl::float4 &mpl::float4::operator *=(float r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_mul_ps(data.reg, _mm_set1_ps(r));
#elif defined(mplGCC_NEON)
	data.reg = vmulq_n_f32(data.reg, r);
#else
	data.e[0] *= r;
	data.e[1] *= r;
	data.e[2] *= r;
	data.e[3] *= r;
#endif
	return *this;
}

mpl::float4 &mpl::float4::operator /=(const mpl::float4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_div_ps(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	 // Get inverse estimate
	float32x4_t inv;
	inv.data.reg = vrecpeq_f32(r.data.reg);
	// A few iterations of Newton-Raphson for accuracy
	inv = vmulq_f32(vrecpsq_f32(r.data.reg, inv), inv);
	inv = vmulq_f32(vrecpsq_f32(r.data.reg, inv), inv);
	// Multiply by 1/r
	data.reg = vmulq_f32(data.reg, inv);
#else
	data.e[0] /= r.data.e[0];
	data.e[1] /= r.data.e[1];
	data.e[2] /= r.data.e[2];
	data.e[3] /= r.data.e[3];
#endif
	return *this;
}

mpl::float4 &mpl::float4::operator /=(float r)
{
	return *this *= (1.0f / r);
}

mpl::float4 mpl::float4::operator +(const mpl::float4 &r) const
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_add_ps(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	f.data.reg = vaddq_f32(data.reg, r.data.reg);
#else
	f.data.e[0] = data.e[0] + r.data.e[0];
	f.data.e[1] = data.e[1] + r.data.e[1];
	f.data.e[2] = data.e[2] + r.data.e[2];
	f.data.e[3] = data.e[3] + r.data.e[3];
#endif
	return f;
}

mpl::float4 mpl::float4::operator -(const mpl::float4 &r) const
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_sub_ps(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	f.data.reg = vsubq_f32(data.reg, r.data.reg);
#else
	f.data.e[0] = data.e[0] - r.data.e[0];
	f.data.e[1] = data.e[1] - r.data.e[1];
	f.data.e[2] = data.e[2] - r.data.e[2];
	f.data.e[3] = data.e[3] - r.data.e[3];
#endif
	return f;
}

mpl::float4 mpl::float4::operator -( void ) const
{
	return float4(0.0f) - *this;
}

mpl::float4 mpl::float4::operator *(const mpl::float4 &r) const
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_mul_ps(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	f.data.reg = vmulq_f32(data.reg, r.data.reg);
#else
	f.data.e[0] = data.e[0] * r.data.e[0];
	f.data.e[1] = data.e[1] * r.data.e[1];
	f.data.e[2] = data.e[2] * r.data.e[2];
	f.data.e[3] = data.e[3] * r.data.e[3];
#endif
	return f;
}

mpl::float4 mpl::float4::operator *(float r) const
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_mul_ps(data.reg, _mm_set1_ps(r));
#elif defined(mplGCC_NEON)
	f.data.reg = vmulq_n_f32(data.reg, r);
#else
	f.data.e[0] = data.e[0] * r;
	f.data.e[1] = data.e[1] * r;
	f.data.e[2] = data.e[2] * r;
	f.data.e[3] = data.e[3] * r;
#endif
	return f;
}

mpl::float4 mpl::float4::operator /(const mpl::float4 &r) const
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_div_ps(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	// Get inverse estimate
	float32x4_t inv;
	inv.data.reg = vrecpeq_f32(r.data.reg);
	// A few iterations of Newton-Raphson for accuracy
	inv = vmulq_f32(vrecpsq_f32(r.data.reg, inv), inv);
	inv = vmulq_f32(vrecpsq_f32(r.data.reg, inv), inv);
	// Multiply by 1/r
	f.data.reg = vmulq_f32(data.reg, inv);
#else
	f.data.e[0] = data.e[0] / r.data.e[0];
	f.data.e[1] = data.e[1] / r.data.e[1];
	f.data.e[2] = data.e[2] / r.data.e[2];
	f.data.e[3] = data.e[3] / r.data.e[3];
#endif
	return f;
}

mpl::float4 mpl::float4::operator /(float r) const
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_div_ps(data.reg, _mm_set1_ps(r));
#elif defined(mplGCC_NEON)
	f.data.reg = vmulq_n_f32(data.reg, 1.0f / r);
#else
	f.data.e[0] = data.e[0] / r;
	f.data.e[1] = data.e[1] / r;
	f.data.e[2] = data.e[2] / r;
	f.data.e[3] = data.e[3] / r;
#endif
	return f;
}

mpl::float4 &mpl::float4::operator &=(const mpl::mask4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_and_si128(*(__m128i*)(&data.reg), r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = *(float32x4)(&vandq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	data.e[0] &= r.data.e[0];
	data.e[1] &= r.data.e[1];
	data.e[2] &= r.data.e[2];
	data.e[3] &= r.data.e[3];
#endif
	return *this;
}

mpl::float4 &mpl::float4::operator |=(const mpl::mask4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_or_si128(*(__m128i*)(&data.reg), r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = *(float32x4)(&vorrq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	data.e[0] |= r.data.e[0];
	data.e[1] |= r.data.e[1];
	data.e[2] |= r.data.e[2];
	data.e[3] |= r.data.e[3];
#endif
	return *this;
}

mpl::float4 &mpl::float4::operator ^=(const mpl::mask4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_xor_si128(*(__m128i*)(&data.reg), r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = *(float32x4)(&veorq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	data.e[0] ^= r.data.e[0];
	data.e[1] ^= r.data.e[1];
	data.e[2] ^= r.data.e[2];
	data.e[3] ^= r.data.e[3];
#endif
	return *this;
}

mpl::float4 mpl::float4::operator &(const mpl::mask4 &r)
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_and_si128(*(__m128i*)(&data.reg), r.data.reg);
#elif defined(mplGCC_NEON)
	f.data.reg = *(float32x4)(&vandq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	f.data.e[0] = data.e[0] & r.data.e[0];
	f.data.e[1] = data.e[1] & r.data.e[1];
	f.data.e[2] = data.e[2] & r.data.e[2];
	f.data.e[3] = data.e[3] & r.data.e[3];
#endif
	return f;
}

mpl::float4 mpl::float4::operator |(const mpl::mask4 &r)
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_or_si128(*(__m128i*)(&data.reg), r.data.reg);
#elif defined(mplGCC_NEON)
	f.data.reg = *(float32x4)(&vorrq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	f.data.e[0] = data.e[0] | r.data.e[0];
	f.data.e[1] = data.e[1] | r.data.e[1];
	f.data.e[2] = data.e[2] | r.data.e[2];
	f.data.e[3] = data.e[3] | r.data.e[3];
#endif
	return f;
}

mpl::float4 mpl::float4::operator ^(const mpl::mask4 &r)
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_xor_si128(*(__m128i*)(&data.reg), r.data.reg);
#elif defined(mplGCC_NEON)
	f.data.reg = *(float32x4)(&veorq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	f.data.e[0] = data.e[0] ^ r.data.e[0];
	f.data.e[1] = data.e[1] ^ r.data.e[1];
	f.data.e[2] = data.e[2] ^ r.data.e[2];
	f.data.e[3] = data.e[3] ^ r.data.e[3];
#endif
	return f;
}

mpl::mask4 mpl::float4::operator ==(const mpl::float4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmpeq_ps(data.reg, r.data.reg);
#elif defined(mglGCC_NEON)
	i.data.reg = vceqq_f32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] == r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] == r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] == r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] == r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::float4::operator !=(const mpl::float4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmpneq_ps(data.reg, r.data.reg);
#elif defined(mglGCC_NEON)
	i.data.reg = vmvnq_u32(vceqq_f32(data.reg, r.data.reg));
#else
	i.data.e[0] = data.e[0] != r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] != r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] != r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] != r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::float4::operator >=(const mpl::float4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmpge_ps(data.reg, r.data.reg);
#elif defined(mglGCC_NEON)
	i.data.reg = vcgeq_f32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] >= r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] >= r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] >= r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] >= r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::float4::operator <=(const mpl::float4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmple_ps(data.reg, r.data.reg);
#elif defined(mglGCC_NEON)
	i.data.reg = vcleq_f32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] <= r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] <= r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] <= r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] <= r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::float4::operator >(const mpl::float4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmpgt_ps(data.reg, r.data.reg);
#elif defined(mglGCC_NEON)
	i.data.reg = vcgtq_f32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] > r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] > r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] > r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] > r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::float4::operator <(const mpl::float4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmplt_ps(data.reg, r.data.reg);
#elif defined(mglGCC_NEON)
	i.data.reg = vcltq_f32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] < r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] < r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] < r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] < r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::float4 mpl::sqrt(const mpl::float4 &f)
{
	mpl::float4 s;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	s.data.reg = _mm_sqrt_ps(f.data.reg);
#elif defined(mplGCC_NEON)
	s.data.reg = vrsqrteq_f32(f.data.reg);
	s.data.reg = vrecpeq_f32(r.data.reg);
	s.data.reg = vmulq_f32(vrecpsq_f32(f.data.reg, s.data.reg), s.data.reg);
	s.data.reg = vmulq_f32(vrecpsq_f32(f.data.reg, s.data.reg), s.data.reg);
#else

	s = f / inv_sqrt(f);

#endif
	return s;
}

mpl::float4 mpl::inv_sqrt(const mpl::float4 &f)
{
	mpl::float4 s;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	s.data.reg = _mm_rsqrt_ps(f.data.reg);
#elif defined(mplGCC_NEON)
	s.data.reg = vrsqrteq_f32(f.data.reg);
	s.data.reg = vmulq_f32(vrecpsq_f32(f.data.reg, s.data.reg), s.data.reg);
	s.data.reg = vmulq_f32(vrecpsq_f32(f.data.reg, s.data.reg), s.data.reg);
#else

	// Two iterations of fast inverse square root.
	// I want performance and comparable accuracy to SIMD version.
	// Comparable accuracy means better cross platform compabibility.

	union float_bits {
		int i; float f;
	};

	for (int i = 0; i < 4; ++i) {

		float_bits b;

		b.f = f.data.e[i];
		float half = b.f * 0.5f;
		b.i = 0x5f3759df - (b.i >> 1);
		b.f = b.f * (1.5f - (half * b.f * b.f));
		s.data.e[i] = b.f * (1.5f - (half * b.f * b.f));

	}
#endif
	return s;
}

mpl::float4 mpl::min(const mpl::float4 &a, const mpl::float4 &b)
{
	mpl::float4 m;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	m.data.reg = _mm_min_ps(a.data.reg, b.data.reg);
#elif defined(mplGCC_NEON)
	m.data.reg = vminq_f32(a.data.reg, b.data.reg);
#else
	m[0] = a[0] < b[0] ? a[0] : b[0];
	m[1] = a[1] < b[1] ? a[1] : b[1];
	m[2] = a[2] < b[2] ? a[2] : b[2];
	m[3] = a[3] < b[3] ? a[3] : b[3];
#endif
	return m;
}

mpl::float4 mpl::max(const mpl::float4 &a, const mpl::float4 &b)
{
	mpl::float4 m;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	m.data.reg = _mm_max_ps(a.data.reg, b.data.reg);
#elif defined(mplGCC_NEON)
	m.data.reg = vmaxq_f32(a.data.reg, b.data.reg);
#else
	m[0] = a[0] > b[0] ? a[0] : b[0];
	m[1] = a[1] > b[1] ? a[1] : b[1];
	m[2] = a[2] > b[2] ? a[2] : b[2];
	m[3] = a[3] > b[3] ? a[3] : b[3];
#endif
	return m;
}

mpl::float4 mpl::aligned_load(const float *v)
{
	float4 f;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	f.data.reg = _mm_load_ps(v);
#elif defined(mplGCC_NEON)
	f.data.reg = vld1q_f32(v);
#else
	f.data.e[0] = v[0];
	f.data.e[1] = v[1];
	f.data.e[2] = v[2];
	f.data.e[3] = v[3];
#endif
	return f;
}

void mpl::aligned_store(const float4 &f, float *out)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	_mm_store_ps(out, f.data.reg);
#elif defined(mplGCC_NEON)
	vst1q_f32(out, f.data.reg);
#else
	out[0] = f.data.e[0];
	out[1] = f.data.e[1];
	out[2] = f.data.e[2];
	out[3] = f.data.e[3];
#endif
}

void mpl::unaligned_store(const float4 &f, float *out)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	_mm_storeu_ps(out, f.data.reg);
#elif defined(mplGCC_NEON)
	vst1q_f32(out, f.data.reg);
#else
	out[0] = f.data.e[0];
	out[1] = f.data.e[1];
	out[2] = f.data.e[2];
	out[3] = f.data.e[3];
#endif
}

mpl::float4 mpl::fabs(const mpl::float4 &a)
{
	return mpl::max(-a, a);
}

#endif
