#ifndef MPL_INT4_H_INCLUDED__
#define MPL_INT4_H_INCLUDED__

#include "mplCommon.h"
#include "mplMask4.h"

namespace mpl
{

#if mplCompiler == mplMSVC
	#if defined(mplMSVC_SSE)
		__declspec(align(16))
	#elif defined (mplMSVC_NEON)
		__declspec(align(4))
	#endif
#endif
class int4
{
private:
	union {
#if defined(mplMSVC_SSE) || defined(mplGCC_SSE)
		__m128i reg;
#elif defined(mplGCC_NEON)
		int32x4_t reg;
#endif
		int e[4];
	} data;

public:
	int4( void ) {}
	inline int4(int a, int b, int c, int d);
	inline int4(int abcd);
	inline explicit int4(const char *v);
	inline explicit int4(const short *v);
	inline explicit int4(const int *v);
	inline int4(const char *v, int stride);
	inline int4(const short *v, int stride);
	inline int4(const int *v, int stride);
	inline int4(const int4 &r);
	inline int4 &operator=(const int4 &r);

	operator       int *( void )       { return data.e; }
	operator const int *( void ) const { return data.e; }

	inline int4 &operator +=(const int4 &r);
	inline int4 &operator -=(const int4 &r);
	inline int4 &operator *=(const int4 &r);
	inline int4 &operator /=(const int4 &r);
	inline int4 &operator *=(int r);
	inline int4 &operator /=(int r);

	inline int4 operator +(const int4 &r) const;
	inline int4 operator -(const int4 &r) const;
	inline int4 operator -( void ) const;
	inline int4 operator *(const int4 &r) const;
	inline int4 operator /(const int4 &r) const;
	inline int4 operator *(int r) const;
	inline int4 operator /(int r) const;

	inline int4 &operator &=(const int4 &r);
	inline int4 &operator |=(const int4 &r);
	inline int4 &operator ^=(const int4 &r);
	inline int4 &operator &=(const mask4 &r);
	inline int4 &operator |=(const mask4 &r);
	inline int4 &operator ^=(const mask4 &r);
	inline int4 &operator <<=(const int4 &r);
	inline int4 &operator >>=(const int4 &r);

	inline int4 operator &(const int4 &r) const;
	inline int4 operator |(const int4 &r) const;
	inline int4 operator ^(const int4 &r) const;
	inline int4 operator &(const mask4 &r);
	inline int4 operator |(const mask4 &r);
	inline int4 operator ^(const mask4 &r);
	inline int4 operator ~( void ) const;
	inline int4 operator <<(const int4 &r) const;
	inline int4 operator >>(const int4 &r) const;

	inline mask4 operator ==(const int4 &r) const;
	inline mask4 operator !=(const int4 &r) const;
	inline mask4 operator >=(const int4 &r) const;
	inline mask4 operator <=(const int4 &r) const;
	inline mask4 operator >(const int4 &r) const;
	inline mask4 operator <(const int4 &r) const;

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

inline int4 aligned_load(const int *v);
inline void unaligned_store(const int4 &i, int *out);
inline void aligned_store(const int4 &i, int *out);

}

mpl::int4::int4(const mpl::int4 &r)
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

mpl::int4::int4(int a, int b, int c, int d)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_setr_epi32(a, b, c, d);
//#elif defined(mplGCC_NEON)
#else
	data.e[0] = a;
	data.e[1] = b;
	data.e[2] = c;
	data.e[3] = d;
#endif
}

mpl::int4::int4(int abcd)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_set1_epi32(abcd);
#else
	data.e[0] = abcd;
	data.e[1] = abcd;
	data.e[2] = abcd;
	data.e[3] = abcd;
#endif
}

mpl::int4::int4(const char *v)
{
	data.e[0] = v[0];
	data.e[1] = v[1];
	data.e[2] = v[2];
	data.e[3] = v[3];
}

mpl::int4::int4(const short *v)
{
	data.e[0] = v[0];
	data.e[1] = v[1];
	data.e[2] = v[2];
	data.e[3] = v[3];
}

mpl::int4::int4(const int *v)
{
	// No SSE instructions for integer loads (maybe reinterpret cast to float?)
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_loadu_si128(*((const __m128i*)v));
#elif defined(mplGCC_NEON)
	data.reg = vld1q_s32(v);
#else
	data.e[0] = v[0];
	data.e[1] = v[1];
	data.e[2] = v[2];
	data.e[3] = v[3];
#endif
}

mpl::int4::int4(const char *v, int stride)
{
	data.e[0] = *v;
	v += stride;
	data.e[1] = *v;
	v += stride;
	data.e[2] = *v;
	v += stride;
	data.e[3] = *v;
	// NEON has instructions for this?
}

mpl::int4::int4(const short *v, int stride)
{
	data.e[0] = *v;
	v += stride;
	data.e[1] = *v;
	v += stride;
	data.e[2] = *v;
	v += stride;
	data.e[3] = *v;
	// NEON has instructions for this?
}

mpl::int4::int4(const int *v, int stride)
{
	data.e[0] = *v;
	v += stride;
	data.e[1] = *v;
	v += stride;
	data.e[2] = *v;
	v += stride;
	data.e[3] = *v;
	// NEON has instructions for this?
}

mpl::int4 &mpl::int4::operator =(const mpl::int4 &r)
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

mpl::int4 &mpl::int4::operator +=(const mpl::int4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_add_epi32(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = vaddq_s32(data.reg, r.data.reg);
#else
	data.e[0] += r.data.e[0];
	data.e[1] += r.data.e[1];
	data.e[2] += r.data.e[2];
	data.e[3] += r.data.e[3];
#endif
	return *this;
}

mpl::int4 &mpl::int4::operator -=(const mpl::int4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_sub_epi32(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = vsubq_s32(data.reg, r.data.reg);
#else
	data.e[0] -= r.data.e[0];
	data.e[1] -= r.data.e[1];
	data.e[2] -= r.data.e[2];
	data.e[3] -= r.data.e[3];
#endif
	return *this;
}

mpl::int4 &mpl::int4::operator *=(const mpl::int4 &r)
{
#if defined(mplGCC_NEON)
	data.reg = vmulq_s32(data.reg, r.data.reg);
#else
	data.e[0] *= r.data.e[0];
	data.e[1] *= r.data.e[1];
	data.e[2] *= r.data.e[2];
	data.e[3] *= r.data.e[3];
#endif
	return *this;
}

mpl::int4 &mpl::int4::operator /=(const mpl::int4 &r)
{
	// No SSE/NEON for this
	data.e[0] /= r.data.e[0];
	data.e[1] /= r.data.e[1];
	data.e[2] /= r.data.e[2];
	data.e[3] /= r.data.e[3];
	return *this;
}

mpl::int4 &mpl::int4::operator *=(int r)
{
#if defined(mplGCC_NEON)
	data.reg = vmulq_n_s32(data.reg, r);
#else
	data.e[0] *= r;
	data.e[1] *= r;
	data.e[2] *= r;
	data.e[3] *= r;
#endif
	return *this;
}

mpl::int4 &mpl::int4::operator /=(int r)
{
	// no SSE/NEON for this?
	data.e[0] /= r;
	data.e[1] /= r;
	data.e[2] /= r;
	data.e[3] /= r;
	return *this;
}

mpl::int4 mpl::int4::operator +(const mpl::int4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_add_epi32(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vaddq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] + r.data.e[0];
	i.data.e[1] = data.e[1] + r.data.e[1];
	i.data.e[2] = data.e[2] + r.data.e[2];
	i.data.e[3] = data.e[3] + r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator -(const mpl::int4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_sub_epi32(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vsubq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] - r.data.e[0];
	i.data.e[1] = data.e[1] - r.data.e[1];
	i.data.e[2] = data.e[2] - r.data.e[2];
	i.data.e[3] = data.e[3] - r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator -( void ) const
{
	return int4(0) - *this;
}

mpl::int4 mpl::int4::operator *(const mpl::int4 &r) const
{
	int4 i;
#if defined(mplGCC_NEON)
	i.data.reg = vmulq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] * r.data.e[0];
	i.data.e[1] = data.e[1] * r.data.e[1];
	i.data.e[2] = data.e[2] * r.data.e[2];
	i.data.e[3] = data.e[3] * r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator /(const mpl::int4 &r) const
{
	// No SSE/NEON for this?
	int4 i;
	i.data.e[0] = data.e[0] / r.data.e[0];
	i.data.e[1] = data.e[1] / r.data.e[1];
	i.data.e[2] = data.e[2] / r.data.e[2];
	i.data.e[3] = data.e[3] / r.data.e[3];
	return i;
}

mpl::int4 mpl::int4::operator *(int r) const
{
	int4 i;
#if defined(mplGCC_NEON)
	i.data.reg = vmulq_n_s32(data.reg, r);
#else
	i.data.e[0] = data.e[0] * r;
	i.data.e[1] = data.e[1] * r;
	i.data.e[2] = data.e[2] * r;
	i.data.e[3] = data.e[3] * r;
#endif
	return i;
}

mpl::int4 mpl::int4::operator /(int r) const
{
	// No SSE/NEON for this?
	int4 i;
	i.data.e[0] = data.e[0] / r;
	i.data.e[1] = data.e[1] / r;
	i.data.e[2] = data.e[2] / r;
	i.data.e[3] = data.e[3] / r;
	return i;
}

mpl::int4 &mpl::int4::operator &=(const mpl::int4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_and_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = vandq_s32(data.reg, r.data.reg);
#else
	data.e[0] &= r.data.e[0];
	data.e[1] &= r.data.e[1];
	data.e[2] &= r.data.e[2];
	data.e[3] &= r.data.e[3];
#endif
	return *this;
}

mpl::int4 &mpl::int4::operator |=(const mpl::int4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_or_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = vorrq_s32(data.reg, r.data.reg);
#else
	data.e[0] |= r.data.e[0];
	data.e[1] |= r.data.e[1];
	data.e[2] |= r.data.e[2];
	data.e[3] |= r.data.e[3];
#endif
	return *this;
}

mpl::int4 &mpl::int4::operator ^=(const mpl::int4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_xor_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	data.reg = veorq_s32(data.reg, r.data.reg);
#else
	data.e[0] ^= r.data.e[0];
	data.e[1] ^= r.data.e[1];
	data.e[2] ^= r.data.e[2];
	data.e[3] ^= r.data.e[3];
#endif
	return *this;
}

mpl::int4 &mpl::operator &=(const mpl::mask4 &r)
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_and_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = *(int32x4*)(&vandq_u32(*(uint32x4*)(&data.reg), r.data.reg));
#else
	i.data.e[0] &= r.data.e[0];
	i.data.e[1] &= r.data.e[1];
	i.data.e[2] &= r.data.e[2];
	i.data.e[3] &= r.data.e[3];
#endif
	return i;
}

mpl::int4 &mpl::operator |=(const mpl::mask4 &r)
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_or_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = *(int32x4)(&vorrq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	i.data.e[0] |= r.data.e[0];
	i.data.e[1] |= r.data.e[1];
	i.data.e[2] |= r.data.e[2];
	i.data.e[3] |= r.data.e[3];
#endif
	return i;
}

mpl::int4 &mpl::operator ^=(const mpl::mask4 &r)
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_xor_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = *(int32x4)(&veorq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	i.data.e[0] ^= r.data.e[0];
	i.data.e[1] ^= r.data.e[1];
	i.data.e[2] ^= r.data.e[2];
	i.data.e[3] ^= r.data.e[3];
#endif
	return i;
}

mpl::int4 &mpl::int4::operator <<=(const mpl::int4 &r)
{
	// Determine what shift mode to use
//#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
//#elif defined(mplGCC_NEON)
//#else
	data.e[0] <<= r.data.e[0];
	data.e[1] <<= r.data.e[1];
	data.e[2] <<= r.data.e[2];
	data.e[3] <<= r.data.e[3];
//#endif
	return *this;
}

mpl::int4 &mpl::int4::operator >>=(const mpl::int4 &r)
{
	// Determine what shift mode to use
//#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
//#elif defined(mplGCC_NEON)
//#else
	data.e[0] >>= r.data.e[0];
	data.e[1] >>= r.data.e[1];
	data.e[2] >>= r.data.e[2];
	data.e[3] >>= r.data.e[3];
//#endif
	return *this;
}

mpl::int4 mpl::int4::operator &(const mpl::int4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_and_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vandq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] & r.data.e[0];
	i.data.e[1] = data.e[1] & r.data.e[1];
	i.data.e[2] = data.e[2] & r.data.e[2];
	i.data.e[3] = data.e[3] & r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator |(const mpl::int4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_or_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vorrq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] | r.data.e[0];
	i.data.e[1] = data.e[1] | r.data.e[1];
	i.data.e[2] = data.e[2] | r.data.e[2];
	i.data.e[3] = data.e[3] | r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator ^(const mpl::int4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_xor_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = veorq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] ^ r.data.e[0];
	i.data.e[1] = data.e[1] ^ r.data.e[1];
	i.data.e[2] = data.e[2] ^ r.data.e[2];
	i.data.e[3] = data.e[3] ^ r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator &(const mpl::mask4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_and_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = *(int32x4)(&vandq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	i.data.e[0] = data.e[0] & r.data.e[0];
	i.data.e[1] = data.e[1] & r.data.e[1];
	i.data.e[2] = data.e[2] & r.data.e[2];
	i.data.e[3] = data.e[3] & r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator |(const mpl::mask4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_or_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = *(int32x4)(&vorrq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	i.data.e[0] = data.e[0] | r.data.e[0];
	i.data.e[1] = data.e[1] | r.data.e[1];
	i.data.e[2] = data.e[2] | r.data.e[2];
	i.data.e[3] = data.e[3] | r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator ^(const mpl::mask4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_xor_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = *(int32x4)(&veorq_u32(*(uint32x4)(&data.reg), r.data.reg));
#else
	i.data.e[0] = data.e[0] ^ r.data.e[0];
	i.data.e[1] = data.e[1] ^ r.data.e[1];
	i.data.e[2] = data.e[2] ^ r.data.e[2];
	i.data.e[3] = data.e[3] ^ r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator ~( void ) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_andnot_si128(data.reg, _mm_set1_epi32(-1));
#elif defined(mplGCC_NEON)
	i.data.reg = vmvnq_s32(data.reg);
#else
	i.data.e[0] = ~data.e[0];
	i.data.e[1] = ~data.e[1];
	i.data.e[2] = ~data.e[2];
	i.data.e[3] = ~data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator <<(const mpl::int4 &r) const
{
	// Determine what shift mode to use
	int4 i;
//#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
//#elif defined(mplGCC_NEON)
//#else
	i.data.e[0] = data.e[0] << r.data.e[0];
	i.data.e[1] = data.e[1] << r.data.e[1];
	i.data.e[2] = data.e[2] << r.data.e[2];
	i.data.e[3] = data.e[3] << r.data.e[3];
//#endif
	return i;
}

mpl::int4 mpl::int4::operator >>(const mpl::int4 &r) const
{
	// Determine what shift mode to use
	int4 i;
//#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
//#elif defined(mplGCC_NEON)
//#else
	i.data.e[0] = data.e[0] >> r.data.e[0];
	i.data.e[1] = data.e[1] >> r.data.e[1];
	i.data.e[2] = data.e[2] >> r.data.e[2];
	i.data.e[3] = data.e[3] >> r.data.e[3];
//#endif
	return i;
}

mpl::mask4 mpl::int4::operator ==(const mpl::int4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmpeq_epi32(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vceqq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] == r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] == r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] == r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] == r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::int4::operator !=(const mpl::int4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_andnot_si128(_mm_cmpeq_epi8(data.reg, r.data.reg), _mm_set1_epi32(-1));
#elif defined(mplGCC_NEON)
	i.data.reg = vmvnq_s32(vceqq_s32(data.reg, r.data.reg));
#else
	i.data.e[0] = data.e[0] != r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] != r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] != r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] != r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::int4::operator >=(const mpl::int4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_and_si128(_mm_cmpgt_epi32(data.reg, r.data.reg), _mm_set1_epi32(data.reg, r.data.reg));
#elif defined(mplGCC_NEON)
	i.data.reg = vcgeq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] >= r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] >= r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] >= r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] >= r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::int4::operator <=(const mpl::int4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_and_si128(_mm_cmplt_epi32(data.reg, r.data.reg), _mm_cmpeq_epi32(data.reg, r.data.reg));
#elif defined(mplGCC_NEON)
	i.data.reg = vcleq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] <= r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] <= r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] <= r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] <= r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::int4::operator >(const mpl::int4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmpgt_epi32(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vcgtq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] > r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] > r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] > r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] > r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::mask4 mpl::int4::operator >=(const mpl::int4 &r) const
{
	mask4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_cmplt_epi32(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vcltq_s32(data.reg, r.data.reg);
#else
	i.data.e[0] = data.e[0] < r.data.e[0] ? 0xffffffff : 0x0;
	i.data.e[1] = data.e[1] < r.data.e[1] ? 0xffffffff : 0x0;
	i.data.e[2] = data.e[2] < r.data.e[2] ? 0xffffffff : 0x0;
	i.data.e[3] = data.e[3] < r.data.e[3] ? 0xffffffff : 0x0;
#endif
	return i;
}

mpl::int4 mpl::aligned_load(const int *v)
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_load_si128((const __m128i*)v);
#elif defined(mplGCC_NEON)
	i.data.reg = vld1q_s32(v);
#else
	data.e[0] = v[0];
	data.e[1] = v[1];
	data.e[2] = v[2];
	data.e[3] = v[3];
#endif
	return i;
}

void mpl::unaligned_store(const mpl::int4 &i, int *out)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	_mm_storeu_si128((__m128i*)out, i.data.reg);
#elif defined(mplGCC_NEON)
	vst1q_s32(out, i.data.reg);
#else
	out[0] = i.data.e[0];
	out[1] = i.data.e[1];
	out[2] = i.data.e[2];
	out[3] = i.data.e[3];
#endif
}

void mpl::aligned_store(const mpl::int4 &i, int *out)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	_mm_store_si128((__m128i*)out, i.data.reg);
#elif defined(mplGCC_NEON)
	vst1q_s32(out, i.data.reg);
#else
	out[0] = i.data.e[0];
	out[1] = i.data.e[1];
	out[2] = i.data.e[2];
	out[3] = i.data.e[3];
#endif
}

#endif // MPL_INT4_H_INCLUDED__
