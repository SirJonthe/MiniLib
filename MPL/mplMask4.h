#ifndef MPL_MASK4_H_INCLUDED__
#define MPL_MASK4_H_INCLUDED__

#include "mplCommon.h"

namespace mpl
{

	class int4;
	class float4;

#if mplCompiler == mplMSVC
	#if defined(mplMSVC_SSE)
		__declspec(align(16))
	#elif defined (mplMSVC_NEON)
		__declspec(align(4))
	#endif
#endif
class mask4
{
	friend class mpl::int4;
	friend class mpl::float4;

private:
	union {
#if defined(mplMSVC_SSE) || defined(mplGCC_SSE)
		__m128i reg;
#elif defined(mplGCC_NEON)
		uint32x4_t reg;
#endif
		int e[4];
	} data;

public:
	mask4( void ) {}
	inline mask4(unsigned int a, unsigned int b, unsigned int c, unsigned int d);
	inline mask4(unsigned int abcd);
	inline explicit mask4(const unsigned int *v);
	inline mask4(const unsigned int *v, int stride);
	inline mask4(const mask4 &r);
	inline mask4 &operator=(const mask4 &r);

	operator       unsigned int *( void )       { return data.e; }
	operator const unsigned int *( void ) const { return data.e; }

	inline mask4 &operator &=(const mask4 &r);
	inline mask4 &operator |=(const mask4 &r);
	inline mask4 &operator ^=(const mask4 &r);
	inline mask4 &operator <<=(const mask4 &r);
	inline mask4 &operator >>=(const mask4 &r);

	inline mask4 operator &(const mask4 &r) const;
	inline mask4 operator |(const mask4 &r) const;
	inline mask4 operator ^(const mask4 &r) const;
	inline mask4 operator ~( void ) const;
	inline mask4 operator <<(const mask4 &r) const;
	inline mask4 operator >>(const mask4 &r) const;
}
#if mplCompiler == mplGCC
	#if defined(mplGCC_SSE)
		__attribute__((aligned(16)))
	#elif defined(mplGCC_NEON)
		__attribute__((aligned(4)))
	#endif
#endif
;

}

mpl::mask4::mask4(const mpl::mask4 &r)
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

mpl::mask4::mask4(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_setr_epi32(*(int*)&a, *(int*)&b, *(int*)&c, *(int*)&d);
#else
	data.e[0] = a;
	data.e[1] = b;
	data.e[2] = c;
	data.e[3] = d;
#endif
}

mpl::uint4::uint4(unsigned int abcd)
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

mpl::uint4::uint4(const unsigned int *v)
{
	// No SSE instructions for integer loads (maybe reinterpret cast to float?)
#if defined(mplGCC_NEON)
	data.reg = vld1q_u32(v);
#else
	data.e[0] = v[0];
	data.e[1] = v[1];
	data.e[2] = v[2];
	data.e[3] = v[3];
#endif
}

mpl::uint4::uint4(const unsigned int *v, int stride)
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

mpl::uint4 &mpl::uint4::operator =(const mpl::uint4 &r)
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

mpl::uint4 &mpl::uint4::operator +=(const mpl::uint4 &r)
{
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	data.reg = _mm_add_epu32(data.reg, r.data.reg);
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

mpl::int4 &mpl::int4::operator &=(const int4 &r)
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

mpl::int4 &mpl::int4::operator |=(const int4 &r)
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

mpl::int4 &mpl::int4::operator ^=(const int4 &r)
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

mpl::int4 &mpl::int4::operator <<=(const int4 &r)
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

mpl::int4 &mpl::int4::operator >>=(const int4 &r)
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

mpl::int4 mpl::int4::operator &(const int4 &r) const
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

mpl::int4 mpl::int4::operator |(const int4 &r) const
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

mpl::int4 mpl::int4::operator ^(const int4 &r) const
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

mpl::int4 mpl::int4::operator &(const uint4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_and_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vandq_s32(data.reg, *(int32x4)(&r.data.reg));
#else
	i.data.e[0] = data.e[0] & r.data.e[0];
	i.data.e[1] = data.e[1] & r.data.e[1];
	i.data.e[2] = data.e[2] & r.data.e[2];
	i.data.e[3] = data.e[3] & r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator |(const uint4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_or_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = vorrq_s32(data.reg, *(int32x4)(&r.data.reg));
#else
	i.data.e[0] = data.e[0] | r.data.e[0];
	i.data.e[1] = data.e[1] | r.data.e[1];
	i.data.e[2] = data.e[2] | r.data.e[2];
	i.data.e[3] = data.e[3] | r.data.e[3];
#endif
	return i;
}

mpl::int4 mpl::int4::operator ^(const uint4 &r) const
{
	int4 i;
#if defined(mplGCC_SSE) || defined(mplMSVC_SSE)
	i.data.reg = _mm_xor_si128(data.reg, r.data.reg);
#elif defined(mplGCC_NEON)
	i.data.reg = veorq_s32(data.reg, *(int32x4)(&r.data.reg));
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

mpl::int4 mpl::int4::operator <<(const int4 &r) const
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

mpl::int4 mpl::int4::operator >>(const int4 &r) const
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

mpl::uint4 mpl::int4::operator ==(const int4 &r) const
{
	uint4 i;
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

mpl::uint4 mpl::int4::operator !=(const int4 &r) const
{
	uint4 i;
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

mpl::uint4 mpl::int4::operator >=(const int4 &r) const
{
	uint4 i;
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

mpl::uint4 mpl::int4::operator <=(const int4 &r) const
{
	uint4 i;
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

mpl::uint4 mpl::int4::operator >(const int4 &r) const
{
	uint4 i;
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

mpl::uint4 mpl::int4::operator >=(const int4 &r) const
{
	uint4 i;
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

#endif // MPL_MASK4_H_INCLUDED__
