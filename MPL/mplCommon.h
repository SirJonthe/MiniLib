#ifndef MPL_COMMON_H_INCLUDED__
#define MPL_COMMON_H_INCLUDED__

#define mplUnknown	0
#define mplMSVC		1
#define mplGCC		2

// Define
#if defined(__GNUC__)

	#define mplCompiler mplGCC

	#ifdef __SSE__
		#define mplGCC_SSE
	#endif

	#ifdef __ARM_NEON__
		#define mplGCC_NEON
	#endif

#elif defined(_MSC_VER)

	#define mplCompiler mplMSVC

	#ifndef _M_CEE_PURE
		#define mplMSVC_SSE
	#endif

#else

	#define mplCompiler mplUnknown

#endif

// Include intrinsics files
#if defined(mplMSVC_SSE) || defined(mplGCC_SSE)

	#include <xmmintrin.h>

#elif defined(mplGCC_NEON) || defined(mplMSVC_NEON)

	#include <arm_neon.h>

#endif

// let user include OpenMP herself (headers are not needed even if we use pragma directives)


// a for loop that is optionally threaded (I think declaring directives inside macro might be undefined in C/C++, so this might not work)
#ifdef _OPENMP

	#define mpl_for \
		#pragma omp parallel for \
		for

#else

	#define mpl_for \
		for

#endif
