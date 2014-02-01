#ifndef MTL_ENDIAN_H_INCLUDED__
#define MTL_ENDIAN_H_INCLUDED__

#include <limits.h>
#include <stdint.h>

#if sizeof(unsigned int) != 4
#error "Incorrect size of unsigned int."
#endif

static const union
{
	char bits8[4];
	unsigned int bits32;
} mtlEndian = { { 0, 1, 2, 3 } };

#define MTL_BIG_ENDIAN	0x03020100ul
#define MTL_LITTLE_ENDIAN 0x00010203ul
#define MTL_PDP_ENDIAN 0x01000302ul

#define MTL_ENDIAN (mtlEndian.bits32)

#define MTL_CPLUSPLUS_XX 0
#define MTL_CPLUSPLUS_98 1
#define MTL_CPLUSPLUS_11 2

#if __cplusplus == 199711L
#define MTL_CPLUSPLUS_VER MTL_CPLUSPLUS_98
#else if __cplusplus == 201103L
#define MTL_CPLUSPLUS_VER MTL_CPLUSPLUS_11
#else
// we need a fallback, the programmer must find out the C++ version some other way
#define MTL_CPLUSPLUS_VER MTL_CPLUSPLUS_XX
#endif


#endif
