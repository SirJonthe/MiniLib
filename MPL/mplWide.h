#ifndef MPL_WIDE_H_INCLUDED__
#define MPL_WIDE_H_INCLUDED__

#include <math.h>
#include "mplCommon.h"

// https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/ARM-NEON-Intrinsics.html#ARM-NEON-Intrinsics

namespace mpl {

	class wide_float;
	class wide_int;

#if MPL_SIMD == MPL_SIMD_SSE

	#if MPL_COMPILER == MPL_COMPILER_MSVC
		__declspec(align(MPL_BYTE_ALIGN))
	#endif
	union wide_bool
	{
		friend class wide_float;
		friend class wide_int;
		template < int n >
		friend class wide_fixed;

	private:
		__m128  f;
		__m128i u;

	public:
		wide_bool( void ) {}
		wide_bool(const wide_bool &b) : f(b.f) {}
		wide_bool(bool b) : u(_mm_set1_epi32(b ? MPL_TRUE : MPL_FALSE)) {}
		wide_bool(const bool *b) : u(_mm_setr_epi32(b[0] ? MPL_TRUE : MPL_FALSE, b[1] ? MPL_TRUE : MPL_FALSE, b[2] ? MPL_TRUE : MPL_FALSE, b[3] ? MPL_TRUE : MPL_FALSE)) {}

		wide_bool operator||(const wide_bool &r) const { wide_bool o; o.u = _mm_or_si128(u, r.u); return o; }
		wide_bool operator&&(const wide_bool &r) const { wide_bool o; o.u = _mm_and_si128(u, r.u); return o; }

		wide_bool operator|(const wide_bool &r) const { wide_bool o; o.u = _mm_or_si128(u, r.u); return o; }
		wide_bool operator&(const wide_bool &r) const { wide_bool o; o.u = _mm_and_si128(u, r.u); return o; }

		wide_bool operator!( void ) const { wide_bool o; o.u = _mm_xor_si128(u, _mm_set1_epi32(-1)); return o; }

		bool all_fail( void ) const { return _mm_movemask_epi8(u) == 0x0000; }
		bool all_pass( void ) const { return _mm_movemask_epi8(u) == 0xffff; }
	}
	#if MPL_COMPILER == MPL_COMPILER_GCC
		__attribute__((aligned(MPL_BYTE_ALIGN)))
	#endif
	;

	#if MPL_COMPILER == MPL_COMPILER_MSVC
		__declspec(align(MPL_BYTE_ALIGN))
	#endif
	class wide_float
	{
		friend class wide_int;
		template < int n >
		friend class wide_fixed;

	private:
		__m128 f;

	public:
		wide_float( void ) {}
		wide_float(const wide_float &r) : f(r.f) {}
		wide_float(float val) : f(_mm_set1_ps(val)) {}
		explicit wide_float(const float *in) : f(_mm_loadu_ps(in)) {}
		inline explicit wide_float(const wide_int &r);

		wide_float &operator+=(const wide_float &r) { f = _mm_add_ps(f, r.f); return *this; }
		wide_float &operator-=(const wide_float &r) { f = _mm_sub_ps(f, r.f); return *this; }
		wide_float &operator*=(const wide_float &r) { f = _mm_mul_ps(f, r.f); return *this; }
		wide_float &operator/=(const wide_float &r) { f = _mm_div_ps(f, r.f); return *this; }
		wide_float &operator|=(const wide_float &r) { f = _mm_or_ps(f, r.f); return *this; }
		wide_float &operator&=(const wide_float &r) { f = _mm_and_ps(f, r.f); return *this; }

		wide_float operator+(const wide_float &r) const { wide_float o; o.f = _mm_add_ps(f, r.f); return o; }
		wide_float operator-(const wide_float &r) const { wide_float o; o.f = _mm_sub_ps(f, r.f); return o; }
		wide_float operator*(const wide_float &r) const { wide_float o; o.f = _mm_mul_ps(f, r.f); return o; }
		wide_float operator/(const wide_float &r) const { wide_float o; o.f = _mm_div_ps(f, r.f); return o; }
		wide_float operator|(const wide_float &r) const { wide_float o; o.f = _mm_or_ps(f, r.f); return o; }
		wide_float operator&(const wide_float &r) const { wide_float o; o.f = _mm_and_ps(f, r.f); return o; }

		static wide_float max(const wide_float &a, const wide_float &b) { wide_float o; o.f = _mm_max_ps(a.f, b.f); return o; }
		static wide_float min(const wide_float &a, const wide_float &b) { wide_float o; o.f = _mm_min_ps(a.f, b.f); return o; }
		static wide_float sqrt(const wide_float &x) { wide_float o; o.f = _mm_sqrt_ps(x.f); return o; }

		wide_bool operator==(const wide_float &r) const { wide_bool o; o.f = _mm_cmpeq_ps(f, r.f); return o; }
		wide_bool operator!=(const wide_float &r) const { wide_bool o; o.f = _mm_cmpneq_ps(f, r.f); return o; }
		wide_bool operator< (const wide_float &r) const { wide_bool o; o.f = _mm_cmplt_ps(f, r.f); return o; }
		wide_bool operator<=(const wide_float &r) const { wide_bool o; o.f = _mm_cmple_ps(f, r.f); return o; }
		wide_bool operator> (const wide_float &r) const { wide_bool o; o.f = _mm_cmpgt_ps(f, r.f); return o; }
		wide_bool operator>=(const wide_float &r) const { wide_bool o; o.f = _mm_cmpge_ps(f, r.f); return o; }

		void to_scalar(float *out) const { _mm_storeu_ps(out, f); }

		static wide_float merge(const wide_float &l, const wide_float &r, const wide_bool &l_mask)
		{
			wide_bool r_mask;
			r_mask.u = _mm_andnot_si128(l_mask.u, _mm_set1_epi32(-1));
			wide_float rc, lc;
			lc.f = _mm_and_ps(l.f, l_mask.f);
			rc.f = _mm_and_ps(r.f, r_mask.f);
			rc.f = _mm_or_ps(rc.f, lc.f);
			return rc;
		}
	}
	#if MPL_COMPILER == MPL_COMPILER_GCC
		__attribute__((aligned(MPL_BYTE_ALIGN)))
	#endif
	;

	#if MPL_COMPILER == MPL_COMPILER_MSVC
		__declspec(align(MPL_BYTE_ALIGN))
	#endif
	template < int n >
	class wide_fixed
	{
		friend class wide_int;

	private:
		__m128i i;

	public:
		wide_fixed( void ) {}
		wide_fixed(const wide_fixed &r) : i(r.i) {}
		wide_fixed(int val) : i(_mm_slli_epi32(_mm_set1_epi32(val), n)) {}
		wide_fixed(float val) : i(_mm_cvttps_epi32(_mm_mul_ps(_mm_set1_ps(val), _mm_set1_ps(1 << n)))) {}
		explicit wide_fixed(const int *in) : i(_mm_slli_epi32(_mm_loadu_si128((__m128i*)in), n)) {}
		explicit wide_fixed(const float *in) : i(_mm_cvttps_epi32(_mm_mul_ps(_mm_loadu_ps(in), _mm_set1_ps(1 << n)))) {}
		inline explicit wide_fixed(const wide_int &r);
		inline explicit wide_fixed(const wide_float &r);

		wide_fixed &operator+=(const wide_fixed &r) { i = _mm_add_epi32(i, r.i); return *this; }
		wide_fixed &operator-=(const wide_fixed &r) { i = _mm_sub_epi32(i, r.i); return *this; }
		wide_fixed &operator*=(const wide_fixed &r) {
			__m128i tmp1 = _mm_mul_epu32(i, r.i);
			__m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(i, 4), _mm_srli_si128(r.i, 4));
			i = _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0)));
			i = _mm_srai_epi32(i, n);
			return *this;
		}
		wide_fixed &operator|=(const wide_fixed &r) { i = _mm_or_si128(i, r.i); return *this; }
		wide_fixed &operator&=(const wide_fixed &r) { i = _mm_and_si128(i, r.i); return *this; }

		wide_fixed operator+(const wide_fixed &r) const { wide_fixed o; o.i = _mm_add_epi32(i, r.i); return o; }
		wide_fixed operator-(const wide_fixed &r) const { wide_fixed o; o.i = _mm_sub_epi32(i, r.i); return o; }
		wide_fixed operator*(const wide_fixed &r) const { wide_fixed o; o.i = i; o *= r; return o; }
		wide_fixed operator|(const wide_fixed &r) const { wide_fixed o; o.i = _mm_or_si128(i, r.i); return o; }
		wide_fixed operator&(const wide_fixed &r) const { wide_fixed o; o.i = _mm_and_si128(i, r.i); return o; }

		wide_bool operator==(const wide_fixed &r) const { wide_bool out; out.u =_mm_cmpeq_epi32(i, r.i); return out; }
		wide_bool operator!=(const wide_fixed &r) const { wide_bool out; out.u = _mm_andnot_si128(_mm_cmpeq_epi32(i, r.i), _mm_set1_epi32(-1)); return out; }
		wide_bool operator< (const wide_fixed &r) const { wide_bool out; out.u = _mm_cmplt_epi32(i, r.i); return out; }
		wide_bool operator<=(const wide_fixed &r) const { wide_bool out; out.u = _mm_andnot_si128(_mm_cmpgt_epi32(i, r.i), _mm_set1_epi32(-1)); return out; }
		wide_bool operator> (const wide_fixed &r) const { wide_bool out; out.u = _mm_cmpgt_epi32(i, r.i); return out; }
		wide_bool operator>=(const wide_fixed &r) const { wide_bool out; out.u = _mm_andnot_si128(_mm_cmplt_epi32(i, r.i), _mm_set1_epi32(-1)); return out; }

		void to_scalar(int *out) const { _mm_storeu_si128((__m128i*)out, _mm_srai_epi32(i, n)); }

		static wide_fixed merge(const wide_fixed &l, const wide_fixed &r, const wide_bool &l_mask)
		{
			wide_bool r_mask;
			r_mask.u = _mm_andnot_si128(l_mask.u, _mm_set1_epi32(-1));
			wide_fixed rc, lc;
			lc.i = _mm_and_si128(l.i, l_mask.u);
			rc.i = _mm_and_si128(r.i, r_mask.u);
			rc.i = _mm_or_si128(rc.i, lc.i);
			return rc;
		}
	}
	#if MPL_COMPILER == MPL_COMPILER_GCC
		__attribute__((aligned(MPL_BYTE_ALIGN)))
	#endif
	;

	#if MPL_COMPILER == MPL_COMPILER_MSVC
		__declspec(align(MPL_BYTE_ALIGN))
	#endif
	class wide_int
	{
		friend class wide_float;
		template < int n >
		friend class wide_fixed;

	private:
		__m128i i;

	public:
		wide_int( void ) {}
		wide_int(const wide_int &r) : i(r.i) {}
		wide_int(int val) : i(_mm_set1_epi32(val)) {}
		explicit wide_int(const int *in) : i(_mm_loadu_si128((__m128i*)in)) {}
		template < int n >
		inline explicit wide_int(const wide_fixed<n> &f);
		inline explicit wide_int(const wide_float &r);

		wide_int &operator+=(const wide_int &r) { i = _mm_add_epi32(i, r.i); return *this; }
		wide_int &operator-=(const wide_int &r) { i = _mm_sub_epi32(i, r.i); return *this; }
		wide_int &operator*=(const wide_int &r) {
			__m128i tmp1 = _mm_mul_epu32(i, r.i);
			__m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(i, 4), _mm_srli_si128(r.i, 4));
			i = _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0)));
			return *this;
		}
		wide_int &operator|=(const wide_int &r) { i = _mm_or_si128(i, r.i); return *this; }
		wide_int &operator&=(const wide_int &r) { i = _mm_and_si128(i, r.i); return *this; }

		wide_int operator+(const wide_int &r) const { wide_int o; o.i = _mm_add_epi32(i, r.i); return o; }
		wide_int operator-(const wide_int &r) const { wide_int o; o.i = _mm_sub_epi32(i, r.i); return o; }
		wide_int operator*(const wide_int &r) const { wide_int o; o.i = i; o *= r; return o; }
		wide_int operator|(const wide_int &r) const { wide_int o; o.i = _mm_or_si128(i, r.i); return o; }
		wide_int operator&(const wide_int &r) const { wide_int o; o.i = _mm_and_si128(i, r.i); return o; }

		wide_bool operator==(const wide_int &r) const { wide_bool out; out.u =_mm_cmpeq_epi32(i, r.i); return out; }
		wide_bool operator!=(const wide_int &r) const { wide_bool out; out.u = _mm_andnot_si128(_mm_cmpeq_epi32(i, r.i), _mm_set1_epi32(-1)); return out; }
		wide_bool operator< (const wide_int &r) const { wide_bool out; out.u = _mm_cmplt_epi32(i, r.i); return out; }
		wide_bool operator<=(const wide_int &r) const { wide_bool out; out.u = _mm_andnot_si128(_mm_cmpgt_epi32(i, r.i), _mm_set1_epi32(-1)); return out; }
		wide_bool operator> (const wide_int &r) const { wide_bool out; out.u = _mm_cmpgt_epi32(i, r.i); return out; }
		wide_bool operator>=(const wide_int &r) const { wide_bool out; out.u = _mm_andnot_si128(_mm_cmplt_epi32(i, r.i), _mm_set1_epi32(-1)); return out; }

		void to_scalar(int *out) const { _mm_storeu_si128((__m128i*)out, i); }

		static wide_int merge(const wide_int &l, const wide_int &r, const wide_bool &l_mask)
		{
			wide_bool r_mask;
			r_mask.u = _mm_andnot_si128(l_mask.u, _mm_set1_epi32(-1));
			wide_int rc, lc;
			lc.i = _mm_and_si128(l.i, l_mask.u);
			rc.i = _mm_and_si128(r.i, r_mask.u);
			rc.i = _mm_or_si128(rc.i, lc.i);
			return rc;
		}
	}
	#if MPL_COMPILER == MPL_COMPILER_GCC
		__attribute__((aligned(MPL_BYTE_ALIGN)))
	#endif
	;

	wide_float::wide_float(const wide_int &r) : f(_mm_cvtepi32_ps(r.i)) {}

	template < int n >
	wide_fixed<n>::wide_fixed(const wide_int &r) : i(_mm_slli_epi32(r.i, n)) {}
	template < int n >
	wide_fixed<n>::wide_fixed(const wide_float &r) : i(_mm_cvttps_epi32(_mm_mul_ps(r.f, _mm_set1_ps(1 << n)))) {}

	wide_int::wide_int(const wide_float &r)   : i(_mm_cvttps_epi32(r.f)) {}
	template < int n >
	wide_int::wide_int(const wide_fixed<n> &f) : i(_mm_srai_epi32(f.i, n)) {}

#elif MPL_SIMD == MPL_SIMD_AVX256

	// __m256, __m256i

#elif MPL_SIMD == MPL_SIMD_AVX512

	// __m512, __m512i

#elif MPL_SIMD == MPL_SIMD_NEON && MPL_COMPILER == MPL_COMPILER_GCC

	union wide_bool
	{
		friend class wide_float;
		friend class wide_int;
		template < int n >
		friend class wide_fixed;

	private:
		uint32x4_t  u;
		float32x4_t f;

	public:
		wide_bool( void ) {}
		wide_bool(const wide_bool &b) : f(b.f) {}
		wide_bool(bool b) : u(vdupq_n_u32(b ? MPL_TRUE : MPL_FALSE)) {}
		wide_bool(const bool *b) {
			u = vsetq_lane_u32(b[0] ? MPL_TRUE : MPL_FALSE, u, 0);
			u = vsetq_lane_u32(b[1] ? MPL_TRUE : MPL_FALSE, u, 1);
			u = vsetq_lane_u32(b[2] ? MPL_TRUE : MPL_FALSE, u, 2);
			u = vsetq_lane_u32(b[3] ? MPL_TRUE : MPL_FALSE, u, 3);
		}

		//wide_bool( void ) : u(vdupq_n_u32(0)) {}

		wide_bool operator||(const wide_bool &r) const { wide_bool o; o.u = vorrq_u32(u, r.u); return o; }
		wide_bool operator&&(const wide_bool &r) const { wide_bool o; o.u = vandq_u32(u, r.u); return o; }

		wide_bool operator|(const wide_bool &r) const { wide_bool o; o.u = vorrq_u32(u, r.u); return o; }
		wide_bool operator&(const wide_bool &r) const { wide_bool o; o.u = vandq_u32(u, r.u); return o; }

		wide_bool operator!( void ) const { wide_bool o; o.u = veorq_u32(u, vdupq_n_u32(~0)); return o; }

		bool all_fail( void ) const
		{
			// http://stackoverflow.com/questions/15389539/fastest-way-to-test-a-128-bit-neon-register-for-a-value-of-0-using-intrinsics
			uint64x2_t v0   = vreinterpretq_u64_u32(u);
			uint64x1_t v0or = vorr_u64(vget_high_u64(v0), vget_low_u64(v0));
			uint32x2_t v1   = vreinterpret_u32_u64(v0or);
			return (vget_lane_u32(v1, 0) | vget_lane_u32(v1, 1)) == 0;
		}

		bool all_pass( void ) const
		{
			uint64x2_t v0    = vreinterpretq_u64_u32(u);
			uint64x1_t v0and = vand_u64(vget_high_u64(v0), vget_low_u64(v0));
			uint32x2_t v1    = vreinterpret_u32_u64(v0and);
			return ((vget_lane_u32(v1, 0) & vget_lane_u32(v1, 1)) == (uint32_t)(-1));
		}

		// bool all_pass

	} __attribute__((aligned(MPL_BYTE_ALIGN)));

	class wide_float
	{
		friend class wide_int;
		template < int n >
		friend class wide_fixed;

	private:
		float32x4_t f;

	public:
		wide_float( void ) {}
		wide_float(const wide_float &r) : f(r.f) {}
		wide_float(float val) : f(vdupq_n_f32(val)) {}
		explicit wide_float(const float *in) : f(vld1q_f32(in)) {}
		inline explicit wide_float(const wide_int &r);

		wide_float &operator+=(const wide_float &r) { f = vaddq_f32(f, r.f); return *this; }
		wide_float &operator-=(const wide_float &r) { f = vsubq_f32(f, r.f); return *this; }
		wide_float &operator*=(const wide_float &r) { f = vmulq_f32(f, r.f); return *this; }
		wide_float &operator/=(const wide_float &r) {
			float32x4_t inv;
			inv = vrecpeq_f32(r.f);
			// A few iterations of Newton-Raphson for accuracy
			inv = vmulq_f32(vrecpsq_f32(r.f, inv), inv);
			inv = vmulq_f32(vrecpsq_f32(r.f, inv), inv);
			// Multiply by 1/r
			f = vmulq_f32(f, inv);
			return *this;
		}
		wide_float &operator|=(const wide_float &r) { *(uint32x4_t*)(&f) = vorrq_u32(*(uint32x4_t*)(&f), *(uint32x4_t*)(&r.f)); return *this; }
		wide_float &operator&=(const wide_float &r) { *(uint32x4_t*)(&f) = vandq_u32(*(uint32x4_t*)(&f), *(uint32x4_t*)(&r.f)); return *this; }

		wide_float operator+(const wide_float &r) const { wide_float o; o.f = vaddq_f32(f, r.f); return o; }
		wide_float operator-(const wide_float &r) const { wide_float o; o.f = vsubq_f32(f, r.f); return o; }
		wide_float operator*(const wide_float &r) const { wide_float o; o.f = vmulq_f32(f, r.f); return o; }
		wide_float operator/(const wide_float &r) const { wide_float o; o.f = f; o /= r; return o; }
		wide_float operator|(const wide_float &r) const { wide_float o; *(uint32x4_t*)(&o.f) = vorrq_u32(*(uint32x4_t*)(&f), *(uint32x4_t*)(&r.f)); return o; }
		wide_float operator&(const wide_float &r) const { wide_float o; *(uint32x4_t*)(&o.f) = vandq_u32(*(uint32x4_t*)(&f), *(uint32x4_t*)(&r.f)); return o; }

		static wide_float max(const wide_float &a, const wide_float &b) { wide_float o; o.f = vmaxq_f32(a.f, b.f); return o; }
		static wide_float min(const wide_float &a, const wide_float &b) { wide_float o; o.f = vminq_f32(a.f, b.f); return o; }
		static wide_float sqrt(const wide_float &x) {
			wide_float o;
			o.f = vrsqrteq_f32(x.f);
			o.f = vrecpeq_f32(o.f);
			o.f = vmulq_f32(vrecpsq_f32(x.f, o.f), o.f);
			o.f = vmulq_f32(vrecpsq_f32(x.f, o.f), o.f);
			return o;
		}

		wide_bool operator==(const wide_float &r) const { wide_bool o; o.u = vceqq_f32(f, r.f); return o; }
		wide_bool operator!=(const wide_float &r) const { wide_bool o; o.u = vmvnq_u32(vceqq_f32(f, r.f)); return o; }
		wide_bool operator< (const wide_float &r) const { wide_bool o; o.u = vcltq_f32(f, r.f); return o; }
		wide_bool operator<=(const wide_float &r) const { wide_bool o; o.u = vcleq_f32(f, r.f); return o; }
		wide_bool operator> (const wide_float &r) const { wide_bool o; o.u = vcgtq_f32(f, r.f); return o; }
		wide_bool operator>=(const wide_float &r) const { wide_bool o; o.u = vcgeq_f32(f, r.f); return o; }

		void to_scalar(float *out) const { vst1q_f32(out, f); }

		static wide_float merge(const wide_float &l, const wide_float &r, const wide_bool &l_mask)
		{
			// NOTE: Watch out for compiler optimizations!!!
			wide_bool r_mask;
			r_mask.u = vmvnq_u32(l_mask.u);
			wide_float rc, lc;
			*(uint32x4_t*)(&lc.f) = vandq_u32(*(uint32x4_t*)(&l.f), l_mask.u);
			*(uint32x4_t*)(&rc.f) = vandq_u32(*(uint32x4_t*)(&r.f), r_mask.u);
			*(uint32x4_t*)(&rc.f) = vorrq_u32(*(uint32x4_t*)(&rc.f), *(uint32x4_t*)(&lc.f));
			return rc;
		}

	} __attribute__((aligned(MPL_BYTE_ALIGN)));

	template < int n >
	class wide_fixed
	{
		friend class wide_int;

	private:
		int32x4_t i;

	public:
		wide_fixed( void ) {}
		wide_fixed(const wide_fixed &r) : i(r.i) {}
		wide_fixed(int val) : i(vshlq_n_s32(vdupq_n_s32(val), n)) {}
		wide_fixed(float val) : i() {}
		explicit wide_fixed(const int *in) : i(vld1q_s32(in)) {}
		//explicit wide_fixed(const float *in) : i(vld1q_s32(in)) {  } // convert to int, arithmetic shift left by n
		inline explicit wide_fixed(const wide_int &r);
		inline explicit wide_fixed(const wide_float &r);

		wide_fixed &operator+=(const wide_fixed &r) { i = vaddq_s32(i, r.i); return *this; }
		wide_fixed &operator-=(const wide_fixed &r) { i = vsubq_s32(i, r.i); return *this; }
		wide_fixed &operator*=(const wide_fixed &r) { i = vshrq_n_s32(vmulq_s32(i, r.i), n); return *this; }
		wide_fixed &operator|=(const wide_fixed &r) { i = vorrq_s32(i, r.i); return *this; }
		wide_fixed &operator&=(const wide_fixed &r) { i = vandq_s32(i, r.i); return *this; }

		wide_fixed operator+(const wide_fixed &r) { wide_fixed o; o.i = vaddq_s32(i, r.i); return o; }
		wide_fixed operator-(const wide_fixed &r) { wide_fixed o; o.i = vsubq_s32(i, r.i); return o; }
		wide_fixed operator*(const wide_fixed &r) { wide_fixed o; o.i = vshrq_n_s32(vmulq_s32(i, r.i), n); return o; }
		wide_fixed operator|(const wide_fixed &r) { wide_fixed o; o.i = vorrq_s32(i, r.i); return o; }
		wide_fixed operator&(const wide_fixed &r) { wide_fixed o; o.i = vandq_s32(i, r.i); return o; }

		wide_bool operator==(const wide_fixed &r) const { wide_bool o; o.u = vceqq_s32(i, r.i); return o; }
		wide_bool operator!=(const wide_fixed &r) const { wide_bool o; o.u = vmvnq_u32(vceqq_s32(i, r.i)); return o; }
		wide_bool operator< (const wide_fixed &r) const { wide_bool o; o.u = vcltq_s32(i, r.i); return o; }
		wide_bool operator<=(const wide_fixed &r) const { wide_bool o; o.u = vcleq_s32(i, r.i); return o; }
		wide_bool operator> (const wide_fixed &r) const { wide_bool o; o.u = vcgtq_s32(i, r.i); return o; }
		wide_bool operator>=(const wide_fixed &r) const { wide_bool o; o.u = vcgeq_s32(i, r.i); return o; }

		void to_scalar(int *out) const { vst1q_s32(out, i); }

		static wide_fixed merge(const wide_fixed &l, const wide_fixed &r, const wide_bool &l_mask)
		{
			wide_bool r_mask;
			r_mask.u = vmvnq_u32(l_mask.u);
			wide_fixed rc, lc;
			lc.i = vandq_s32(l.i, l_mask.u);
			rc.i = vandq_s32(r.i, r_mask.u);
			rc.i = vorrq_s32(rc.i, lc.i);
			return rc;
		}

	} __attribute__((aligned(MPL_BYTE_ALIGN)));

	class wide_int
	{
		friend class wide_float;
		template < int n >
		friend class wide_fixed;

	private:
		int32x4_t i;

	public:
		wide_int( void ) {}
		wide_int(const wide_int &r) : i(r.i) {}
		wide_int(int val) : i(vdupq_n_s32(val)) {}
		explicit wide_int(const int *in) : i(vld1q_s32(in)) {}
		inline explicit wide_int(const wide_float &r);
		template < int n >
		inline explicit wide_int(const wide_fixed<n> &r);

		wide_int &operator+=(const wide_int &r) { i = vaddq_s32(i, r.i); return *this; }
		wide_int &operator-=(const wide_int &r) { i = vsubq_s32(i, r.i); return *this; }
		wide_int &operator*=(const wide_int &r) { i = vmulq_s32(i, r.i); return *this; }
		wide_int &operator|=(const wide_int &r) { i = vorrq_s32(i, r.i); return *this; }
		wide_int &operator&=(const wide_int &r) { i = vandq_s32(i, r.i); return *this; }

		wide_int operator+(const wide_int &r) { wide_int o; o.i = vaddq_s32(i, r.i); return o; }
		wide_int operator-(const wide_int &r) { wide_int o; o.i = vsubq_s32(i, r.i); return o; }
		wide_int operator*(const wide_int &r) { wide_int o; o.i = vmulq_s32(i, r.i); return o; }
		wide_int operator|(const wide_int &r) { wide_int o; o.i = vorrq_s32(i, r.i); return o; }
		wide_int operator&(const wide_int &r) { wide_int o; o.i = vandq_s32(i, r.i); return o; }

		wide_bool operator==(const wide_int &r) const { wide_bool o; o.u = vceqq_s32(i, r.i); return o; }
		wide_bool operator!=(const wide_int &r) const { wide_bool o; o.u = vmvnq_u32(vceqq_s32(i, r.i)); return o; }
		wide_bool operator< (const wide_int &r) const { wide_bool o; o.u = vcltq_s32(i, r.i); return o; }
		wide_bool operator<=(const wide_int &r) const { wide_bool o; o.u = vcleq_s32(i, r.i); return o; }
		wide_bool operator> (const wide_int &r) const { wide_bool o; o.u = vcgtq_s32(i, r.i); return o; }
		wide_bool operator>=(const wide_int &r) const { wide_bool o; o.u = vcgeq_s32(i, r.i); return o; }

		void to_scalar(int *out) const { vst1q_s32(out, i); }

		static wide_int merge(const wide_int &l, const wide_int &r, const wide_bool &l_mask)
		{
			wide_bool r_mask;
			r_mask.u = vmvnq_u32(l_mask.u);
			wide_int rc, lc;
			lc.i = vandq_s32(l.i, *(int32x4_t*)(&l_mask.u));
			rc.i = vandq_s32(r.i, *(int32x4_t*)(&r_mask.u));
			rc.i = vorrq_s32(rc.i, lc.i);
			return rc;
		}

	} __attribute__((aligned(MPL_BYTE_ALIGN)));

	wide_float::wide_float(const wide_int &r) : f(vcvtq_f32_s32(r.i)) {}

	template < int n >
	wide_fixed<n>::wide_fixed(const wide_int &r) : i(vshlq_n_s32(r.i, n)) {}
	template < int n >
	wide_fixed<n>::wide_fixed(const wide_float &r) : i(vcvtq_s32_f32(vmulq_n_f32(r.f, 1 << n))) {}

	wide_int::wide_int(const wide_float &r) : i(vcvtq_s32_f32(r.f)) {}
	template < int n >
	wide_int::wide_int(const wide_fixed<n> &r) : i(vshrq_n_s32(r.i, n)) {}

#elif MPL_SIMD == MPL_SIMD_NONE

	union wide_bool
	{
		friend class wide_float;
		friend class wide_int;
		template < int n >
		friend class wide_fixed;

	private:
		unsigned int u;
		float        f;

	public:
		wide_bool( void ) {}
		wide_bool(const wide_bool &b) : u(b.u) {}
		wide_bool(bool b) : u(b ? MPL_TRUE : MPL_FALSE) {}
		wide_bool(const bool *b) : u(b[0] ? MPL_TRUE : MPL_FALSE) {}

		wide_bool operator||(const wide_bool &r) const { wide_bool o; o.u = (u && r.u) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator&&(const wide_bool &r) const { wide_bool o; o.u = (u || r.u) ? MPL_TRUE : MPL_FALSE; return o; }

		wide_bool operator|(const wide_bool &r) const { wide_bool o; o.u = (u & r.u) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator&(const wide_bool &r) const { wide_bool o; o.u = (u | r.u) ? MPL_TRUE : MPL_FALSE; return o; }

		wide_bool operator!( void ) const { wide_bool o; o.u = ~u; return o; }

		bool all_fail( void ) const { return u == MPL_FALSE; }
		bool all_pass( void ) const { return u == MPL_TRUE; }
	};

	class wide_float
	{
		friend class wide_int;
		template < int n >
		friend class wide_fixed;

	private:
		float f;

	public:
		wide_float( void ) {}
		wide_float(const wide_float &r) : f(r.f) {}
		wide_float(float val) : f(val) {}
		explicit wide_float(const float *in) : f(*in) {}
		inline explicit wide_float(const wide_int &i);

		wide_float &operator+=(const wide_float &r) { f += r.f; return *this; }
		wide_float &operator-=(const wide_float &r) { f -= r.f; return *this; }
		wide_float &operator*=(const wide_float &r) { f *= r.f; return *this; }
		wide_float &operator/=(const wide_float &r) { f /= r.f; return *this; }
		wide_float &operator|=(const wide_float &r) { *(unsigned int*)(&f) |= *(unsigned int*)(&r.f); return *this; }
		wide_float &operator&=(const wide_float &r) { *(unsigned int*)(&f) &= *(unsigned int*)(&r.f); return *this; }

		wide_float operator+(const wide_float &r) const { return f + r.f; }
		wide_float operator-(const wide_float &r) const { return f - r.f; }
		wide_float operator*(const wide_float &r) const { return f * r.f; }
		wide_float operator/(const wide_float &r) const { return f / r.f; }
		wide_float operator|(const wide_float &r) const { unsigned int o = *(unsigned int*)(&f) | *(unsigned int*)(&r.f); return *(float*)(&o); }
		wide_float operator&(const wide_float &r) const { unsigned int o = *(unsigned int*)(&f) & *(unsigned int*)(&r.f); return *(float*)(&o); }

		static wide_float max(const wide_float &a, const wide_float &b) { return a.f < b.f ? b.f : a.f; }
		static wide_float min(const wide_float &a, const wide_float &b) { return a.f < b.f ? a.f : b.f; }
		static wide_float sqrt(const wide_float &x) { return ::sqrt(x.f); }

		wide_bool operator==(const wide_float &r) const { wide_bool o; o.u = (f == r.f) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator!=(const wide_float &r) const { wide_bool o; o.u = (f != r.f) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator< (const wide_float &r) const { wide_bool o; o.u = (f <  r.f) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator<=(const wide_float &r) const { wide_bool o; o.u = (f <= r.f) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator> (const wide_float &r) const { wide_bool o; o.u = (f >  r.f) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator>=(const wide_float &r) const { wide_bool o; o.u = (f >= r.f) ? MPL_TRUE : MPL_FALSE; return o; }

		void to_scalar(float *out) const { *out = f; }

		static wide_float merge(const wide_float &l, const wide_float &r, const wide_bool &l_mask)
		{
			// This function needs to be way more complicated than it should be.
			// The compiler optimizes casting between float* and int* to shit.
			union bits
			{
				float        f;
				unsigned int u;
			};

			bits a = { l.f };
			bits b = { r.f };
			bits o;
			o.u = (a.u & l_mask.u) | (b.u & ~l_mask.u);
			return wide_float(o.f);
		}
	};

	template < int n >
	class wide_fixed
	{
		friend class wide_int;

	private:
		int i;

	public:
		wide_fixed( void ) {}
		wide_fixed(const wide_fixed &f) : i(f.i) {}
		wide_fixed(int val) : i(val<<n) {}
		wide_fixed(float val) : i(val * (1<<n)) {}
		explicit wide_fixed(const int *in) : i(*in << n) {}
		explicit wide_fixed(const float *in) : i(*in * (1<<n)) {}
		inline explicit wide_fixed(const wide_int &r);
		inline explicit wide_fixed(const wide_float &r);

		wide_fixed &operator+=(const wide_fixed &r) { i += r.i; return *this; }
		wide_fixed &operator-=(const wide_fixed &r) { i -= r.i; return *this; }
		wide_fixed &operator*=(const wide_fixed &r) { i *= r.i; return *this; }
		wide_fixed &operator|=(const wide_fixed &r) { i |= r.i; return *this; }
		wide_fixed &operator&=(const wide_fixed &r) { i &= r.i; return *this; }

		wide_fixed operator+(const wide_fixed &r) const { return i + r.i; }
		wide_fixed operator-(const wide_fixed &r) const { return i - r.i; }
		wide_fixed operator*(const wide_fixed &r) const { return i * r.i; }
		wide_fixed operator|(const wide_fixed &r) const { return i | r.i; }
		wide_fixed operator&(const wide_fixed &r) const { return i & r.i; }

		wide_bool operator==(const wide_fixed &r) const { wide_bool o; o.u = (i == r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator!=(const wide_fixed &r) const { wide_bool o; o.u = (i != r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator< (const wide_fixed &r) const { wide_bool o; o.u = (i <  r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator<=(const wide_fixed &r) const { wide_bool o; o.u = (i <= r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator> (const wide_fixed &r) const { wide_bool o; o.u = (i >  r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator>=(const wide_fixed &r) const { wide_bool o; o.u = (i >= r.i) ? MPL_TRUE : MPL_FALSE; return o; }

		void to_scalar(int *out) const { *out = i >> n; }

		static wide_fixed merge(const wide_fixed &l, const wide_fixed &r, const wide_bool &l_mask)
		{
			unsigned int o = (*(unsigned int*)(&l.i) & l_mask.u) | (*(unsigned int*)(&r.i) & ~l_mask.u);
			wide_fixed out;
			out.i = *(int*)&o;
			return out;
		}
	};

	class wide_int
	{
		friend class wide_float;
		template < int n >
		friend class wide_fixed;

	private:
		int i;

	public:
		wide_int( void ) {}
		wide_int(const wide_int &r) : i(r.i) {}
		wide_int(int val) : i(val) {}
		explicit wide_int(const int *in) : i(*in) {}
		inline explicit wide_int(const wide_float &f);
		template < int n >
		inline explicit wide_int(const wide_fixed<n> &f);

		wide_int &operator+=(const wide_int &r) { i += r.i; return *this; }
		wide_int &operator-=(const wide_int &r) { i -= r.i; return *this; }
		wide_int &operator*=(const wide_int &r) { i *= r.i; return *this; }
		wide_int &operator|=(const wide_int &r) { i |= r.i; return *this; }
		wide_int &operator&=(const wide_int &r) { i &= r.i; return *this; }

		wide_int operator+(const wide_int &r) const { return i + r.i; }
		wide_int operator-(const wide_int &r) const { return i - r.i; }
		wide_int operator*(const wide_int &r) const { return i * r.i; }
		wide_int operator|(const wide_int &r) const { return i | r.i; }
		wide_int operator&(const wide_int &r) const { return i & r.i; }

		wide_bool operator==(const wide_int &r) const { wide_bool o; o.u = (i == r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator!=(const wide_int &r) const { wide_bool o; o.u = (i != r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator< (const wide_int &r) const { wide_bool o; o.u = (i <  r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator<=(const wide_int &r) const { wide_bool o; o.u = (i <= r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator> (const wide_int &r) const { wide_bool o; o.u = (i >  r.i) ? MPL_TRUE : MPL_FALSE; return o; }
		wide_bool operator>=(const wide_int &r) const { wide_bool o; o.u = (i >= r.i) ? MPL_TRUE : MPL_FALSE; return o; }

		void to_scalar(int *out) const { *out = i; }

		static wide_int merge(const wide_int &l, const wide_int &r, const wide_bool &l_mask)
		{
			unsigned int o = (*(unsigned int*)(&l.i) & l_mask.u) | (*(unsigned int*)(&r.i) & ~l_mask.u);
			return wide_int(*(int*)(&o));
		}
	};

	wide_float::wide_float(const wide_int &r) : f((float)r.i) {}

	template < int n >
	wide_fixed<n>::wide_fixed(const wide_int &r) : i(r.i << n) {}
	template < int n >
	wide_fixed<n>::wide_fixed(const wide_float &r) : i((int)(r.f * (1 << n))) {}

	template < int n >
	wide_int::wide_int(const wide_fixed<n> &r) : i(r.i >> n) {}
	wide_int::wide_int(const wide_float &r) : i((int)r.f) {}

#endif

}

#endif // MPL_WIDE_H_INCLUDED__
