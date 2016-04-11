#ifndef MGL_PIXEL_H_INCLUDED__
#define MGL_PIXEL_H_INCLUDED__

#include "../MTL/mtlBits.h"

// I can use MML:s mml_fixed_uniform<unsigned char> instead
#define mglMul8(a, b) (unsigned char)((((unsigned short)(a) * (unsigned short)(b)) >> 8) + 1)
#define mglMul8x4(a, b) (unsigned int)(mglMul8((unsigned char)(a), (unsigned char)(b)) | (mglMul8((unsigned char)((a)>>8), (unsigned char)((b)>>8)) << 8) | (mglMul8((unsigned char)((a)>>16), (unsigned char)((b)>>16)) << 16) | (mglMul8((unsigned char)((a)>>16), (unsigned char)((b)>>16)) << 24))
#define mglMul16(a, b) (unsigned short)((((unsigned int)(a) * (unsigned int)(b)) >> 16) + 1)

union mglPixel32
{
	unsigned int color;
	mtlByte      bytes[sizeof(unsigned int)];
};

union mglByteOrder32
{
	unsigned int hash;
	struct { unsigned char r, g, b, a; } index;
};

struct mglPixelFormat
{
	int            bytes_per_pixel; // traditionally 1, 2, 3, 4
	mglByteOrder32 byte_order;
	enum Color {
		Color_Grayscale, // 1 byte = gray, 2 bytes = gray+alpha
		Color_Truecolor  // 2 bytes = rgba5551, 3 = rgb888, 4 = rgba8888
	} color;
};

inline mglByteOrder32 mglVideoByteOrder( void )
{
	// the following should take care of endianness
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux) || defined(__gnu_linux__)
	static const mglByteOrder32 standard_order = { 0x03000102 }; // r=2, g=1, b=0, a=3
#elif defined(__APPLE__)
	static const mglByteOrder32 standard_order = { 0x00030201 }; // r=1, g=2, b=3, a=0
#else
	static const mglByteOrder32 standard_order = { 0x03000102 }; // r=2, g=1, b=0, a=3 (just a legitimate guess)
#endif
	// __WIN32__ / __WIN32 / _WIN32 / WIN32
	return standard_order;
}

inline mglPixel32 mglRGBA(mtlByte r, mtlByte g, mtlByte b, mtlByte a, mglByteOrder32 fmt = mglVideoByteOrder())
{
	mglPixel32 out;
	out.bytes[fmt.index.r] = r;
	out.bytes[fmt.index.g] = g;
	out.bytes[fmt.index.b] = b;
	out.bytes[fmt.index.a] = a;
	return out;
}

inline mglPixel32 mglRGB(mtlByte r, mtlByte g, mtlByte b, mglByteOrder32 fmt = mglVideoByteOrder())
{

	mglPixel32 out;
	out.bytes[fmt.index.r] = r;
	out.bytes[fmt.index.g] = g;
	out.bytes[fmt.index.b] = b;
	out.bytes[fmt.index.a] = 0xff;
	return out;
}

#endif
