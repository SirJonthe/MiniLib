#ifndef MPL_COMMON_H_INCLUDED
#define MPL_COMMON_H_INCLUDED

#define MPL_COMPILER_UNKNOWN 0
#define MPL_COMPILER_MSVC    1
#define MPL_COMPILER_GCC     2

#define MPL_SIMD_NONE    0
#define MPL_SIMD_SSE     1
#define MPL_SIMD_AVX256  2
#define MPL_SIMD_AVX512  3
#define MPL_SIMD_NEON    4
#define MPL_SIMD_ALTIVEC 5

static const char *MPL_SIMD_STRING[] = {
	static_cast<const char*>("None"),
	static_cast<const char*>("SSE2+"),
	static_cast<const char*>("AVX256"),
	static_cast<const char*>("AVX512"),
	static_cast<const char*>("NEON"),
	static_cast<const char*>("AltiVec")
};

// add support for AVX-256 and AVX-512 (#include <immintrin.h> on gcc and msvc)
// low priority: add support for PowerPC Altivec (#include <altivec.h> on gcc)
	// need a computer to test it on

// Detect instruction set
#if defined(__GNUC__)
	#define MPL_COMPILER MPL_COMPILER_GCC
	#ifdef MPL_FALLBACK_SCALAR
		#define MPL_SIMD MPL_SIMD_NONE
	#elif defined(__AVX__)
		#define MPL_SIMD MPL_SIMD_AVX256
		#ifdef __AVX2__
			#define MPL_SIMD_VER 2
		#endif
	#elif defined(__AVX512__)
		#define MPL_SIMD MPL_SIMD_AVX512
	#elif defined(__SSE__)
		#define MPL_SIMD  MPL_SIMD_SSE
		#ifdef __SSE4__
			#define MPL_SIMD_VER 4
		#elif defined(__SSE3__)
			#define MPL_SIMD_VER 3
		#elif defined(__SSE2__)
			#define MPL_SIMD_VER 2
		#endif
	#elif defined(__ARM_NEON__)
		// Enable on Raspberry Pi 2/3 using these compiler flags:
			// -mcpu=cortex-a7
			// -mfloat-abi=hard
			// -mfpu=neon-vfpv4
		#define MPL_SIMD MPL_SIMD_NEON
	#elif defined(__VEC__) && defined(__ALTIVEC__)
		// Use the "-maltivec" flag to enable PowerPC AltiVec support
		#define MPL_SIMD MPL_SIMD_ALTIVEC
	#else
		#define MPL_SIMD MPL_SIMD_NONE
		#warning No SIMD support, falling back to scalar
	#endif
#elif defined(_MSC_VER)
	#define MPL_COMPILER MPL_COMPILER_MSVC
	#ifdef MPL_FALLBACK_SCALAR
		#define MPL_SIMD MPL_SIMD_NONE
	#elif !defined(_M_CEE_PURE)
		#define MPL_SIMD MPL_SSE
	#else
		#define MPL_SIMD MPL_SIMD_NONE
		#warning No SIMD support, falling back to scalar
	#endif
#else
	#define MPL_COMPILER MPL_COMPILER_UNKNOWN
	#define MPL_SIMD     MPL_SIMD_NONE
	#warning No SIMD support, falling back to scalar
#endif

#ifndef MPL_SIMD_VER
	#define MPL_SIMD_VER 1
#endif

// SIMD instruction definitions
#if MPL_SIMD == MPL_SIMD_SSE
	#define MPL_WIDTH         4
	#define MPL_BYTE_ALIGN    16
	#define MPL_BLOCK_X       2
	#define MPL_BLOCK_Y       2
	#define MPL_NO_OFFSETS    { 0, 0, 0, 0 }
	#define MPL_OFFSETS       { 0, 1, 2, 3 }
	#define MPL_X_OFFSETS     { 0, 1, 0, 1 }
	#define MPL_Y_OFFSETS     { 0, 0, 1, 1 }
#elif MPL_SIMD == MPL_SIMD_NEON
	#define MPL_WIDTH         4
	#define MPL_BYTE_ALIGN    16
	#define MPL_BLOCK_X       2
	#define MPL_BLOCK_Y       2
	#define MPL_NO_OFFSETS    { 0, 0, 0, 0 }
	#define MPL_OFFSETS       { 0, 1, 2, 3 }
	#define MPL_X_OFFSETS     { 0, 1, 0, 1 }
	#define MPL_Y_OFFSETS     { 0, 0, 1, 1 }
#elif MPL_SIMD == MPL_SIMD_AVX256
	#define MPL_WIDTH         8
	#define MPL_BYTE_ALIGN    32
	#define MPL_BLOCK_X       4
	#define MPL_BLOCK_Y       2
	#define MPL_NO_OFFSETS    { 0, 0, 0, 0, 0, 0, 0, 0 }
	#define MPL_OFFSETS       { 0, 1, 2, 3, 4, 5, 6, 7 }
	#define MPL_X_OFFSETS     { 0, 1, 2, 3, 0, 1, 2, 3 }
	#define MPL_Y_OFFSETS     { 0, 0, 0, 0, 1, 1, 1, 1 }
#elif MPL_SIMD == MPL_SIMD_AVX512
	#define MPL_WIDTH         16
	#define MPL_BYTE_ALIGN    64
	#define MPL_BLOCK_X       4
	#define MPL_BLOCK_Y       4
	#define MPL_NO_OFFSETS    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0 }
	#define MPL_OFFSETS       { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }
	#define MPL_X_OFFSETS     { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1,  2,  3,  0,  1,  2,  3 }
	#define MPL_Y_OFFSETS     { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2,  2,  2,  3,  3,  3,  3 }
#elif MPL_SIMD == MPL_SIMD_ALTIVEC
	#define MPL_WIDTH         4
	#define MPL_BYTE_ALIGN    16
	#define MPL_BLOCK_X       2
	#define MPL_BLOCK_Y       2
	#define MPL_NO_OFFSETS    { 0, 0, 0, 0 }
	#define MPL_OFFSETS       { 0, 1, 2, 3 }
	#define MPL_X_OFFSETS     { 0, 1, 0, 1 }
	#define MPL_Y_OFFSETS     { 0, 0, 1, 1 }
#elif MPL_SIMD == MPL_SIMD_NONE
	#define MPL_WIDTH         1
	#define MPL_BYTE_ALIGN    1
	#define MPL_BLOCK_X       1
	#define MPL_BLOCK_Y       1
	#define MPL_NO_OFFSETS    { 0 }
	#define MPL_OFFSETS       { 0 }
	#define MPL_X_OFFSETS     MPL_OFFSETS
	#define MPL_Y_OFFSETS     MPL_OFFSETS
#endif

// Common defines
#define MPL_BYTE_ALIGN_MASK (~(MPL_BYTE_ALIGN - 1))
#define MPL_WIDTH_MASK      (MPL_WIDTH - 1)
#define MPL_WIDTH_INVMASK   (~MPL_WIDTH_MASK)
#define MPL_BLOCK_X_MASK    (MPL_BLOCK_X - 1)
#define MPL_BLOCK_Y_MASK    (MPL_BLOCK_Y - 1)
#define MPL_BLOCK_X_INVMASK (~MPL_BLOCK_X_MASK)
#define MPL_BLOCK_Y_INVMASK (~MPL_BLOCK_Y_MASK)
#define MPL_FLOOR(X)        ((X) & MPL_WIDTH_INVMASK)
#define MPL_CEIL(X)         MPL_FLOOR((X) + MPL_WIDTH_MASK)

// Include appropriate headers
// Some notes,
// Catchall include for MSVC is "#include <intrin.h>"
// Catchall include for GCC is "#include <x86intrin.h>"
#if MPL_SIMD == MPL_SIMD_AVX256 || MPL_SIMD == MPL_SIMD_AVX512
	#include <immintrin.h>
#elif MPL_SIMD == MPL_SIMD_SSE
	#include <xmmintrin.h>
#elif MPL_SIMD == MPL_SIMD_NEON
	#include <arm_neon.h>
#elif MPL_SIMD == MPL_SIMD_ALTIVEC
	#include <altivec.h>
	// undef these to avoid future collisions
	#undef vector
	#undef pixel
	#undef bool
#endif

#if ((-2 >> 1) == (-2 / 2)) && ((-2 << 1) == (-2 * 2))
	#define MPL_SIGNED_SHIFT 1
#endif

#define MPL_UNS_MAX (~0U)
#define MPL_TRUE_BITS MPL_UNS_MAX
#define MPL_FALSE_BITS 0

#endif // MPL_COMMON_H_INCLUDED__
