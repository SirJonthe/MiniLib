#ifndef MPL_COMMON_H
#define MPL_COMMON_H

#define MPL_COMPILER_UNKNOWN 0
#define MPL_COMPILER_MSVC    1
#define MPL_COMPILER_GCC     2

#define MPL_SIMD_NONE   0
#define MPL_SIMD_SSE    1
#define MPL_SIMD_AVX256 2
#define MPL_SIMD_AVX512 3
#define MPL_SIMD_NEON   4

// add support for AVX-256 and AVX-512

// Detect instruction set
#if defined(__GNUC__)
	#define MPL_COMPILER MPL_COMPILER_GCC
	#ifdef MPL_FALLBACK_SCALAR
		#define MPL_SIMD MPL_SIMD_NONE
	#elif defined(__AVX__)
		#define MPL_SIMD MPL_SIMD_AVX256
	#elif defined(__SSE__)
		#define MPL_SIMD  MPL_SIMD_SSE
	#elif defined(__ARM_NEON__)
		// Enable on Raspberry Pi 2 using these compiler flags:
			// -mcpu=cortex-a7
			// -mfloat-abi=hard
			// -mfpu=neon-vfpv4
		#define MPL_SIMD MPL_SIMD_NEON
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

// SIMD instruction definitions
#if MPL_SIMD == MPL_SIMD_SSE
	#define MPL_WIDTH         4
	#define MPL_BYTE_ALIGN    16
	#define MPL_BLOCK_X       2
	#define MPL_BLOCK_Y       2
	#define MPL_OFFSETS       { 0, 1, 2, 3 }
	#define MPL_X_OFFSETS     { 0, 1 }
	#define MPL_Y_OFFSETS     MPL_X_OFFSETS
#elif MPL_SIMD == MPL_SIMD_NEON
	#define MPL_WIDTH         4
	#define MPL_BYTE_ALIGN    4
	#define MPL_BLOCK_X       2
	#define MPL_BLOCK_Y       2
	#define MPL_OFFSETS       { 0, 1, 2, 3 }
	#define MPL_X_OFFSETS     { 0, 1 }
	#define MPL_Y_OFFSETS     MPL_X_OFFSETS
#elif MPL_SIMD == MPL_SIMD_AVX256
	#define MPL_WIDTH         8
	#define MPL_BYTE_ALIGN    32
	#define MPL_BLOCK_X       4
	#define MPL_BLOCK_Y       2
	#define MPL_OFFSETS       { 0, 1, 2, 3, 4, 5, 6, 7 }
	#define MPL_X_OFFSETS     { 0, 1, 2, 3 }
	#define MPL_Y_OFFSETS     { 0, 1 }
#elif MPL_SIMD == MPL_SIMD_AVX512
	#define MPL_WIDTH         16
	#define MPL_BYTE_ALIGN    64
	#define MPL_BLOCK_X       4
	#define MPL_BLOCK_Y       4
	#define MPL_OFFSETS       { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }
	#define MPL_X_OFFSETS     { 0, 1, 2, 3 }
	#define MPL_Y_OFFSETS     MPL_X_OFFSETS
#elif MPL_SIMD == MPL_SIMD_NONE
	#define MPL_WIDTH         1
	#define MPL_BYTE_ALIGN    1
	#define MPL_BLOCK_X       1
	#define MPL_BLOCK_Y       1
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
#if MPL_SIMD == MPL_SIMD_AVX256 || MPL_SIMD == MPL_SIMD_AVX512
	#include <immintrin.h>
#elif MPL_SIMD == MPL_SIMD_SSE
	#include <xmmintrin.h>
#elif MPL_SIMD == MPL_SIMD_NEON
	#include <arm_neon.h>
#endif

#if ((-2 >> 1) == (-2 / 2)) && ((-2 << 1) == (-2 * 2))
	#define MPL_SIGNED_SHIFT 1
#endif

#define MPL_UNS_MAX (~0U)
#define MPL_TRUE MPL_UNS_MAX
#define MPL_FALSE 0

#endif // MPL_COMMON_H
