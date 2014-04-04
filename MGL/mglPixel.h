#ifndef MGL_PIXEL_H_INCLUDED__
#define MGL_PIXEL_H_INCLUDED__

#define mglMul8(a, b) (unsigned char)((((unsigned short)(a) * (unsigned short)(b)) >> 8) + 1)
#define mglMul8x4(a, b) (unsigned int)(mglMul8U((unsigned char)(a), (unsigned char)(b)) | (mglMul8U((unsigned char)((a)>>8), (unsigned char)((b)>>8)) << 8) | (mglMul8U((unsigned char)((a)>>16), (unsigned char)((b)>>16)) << 16) | (mglMul8U((unsigned char)((a)>>16), (unsigned char)((b)>>16)) << 24))
#define mglMul16(a, b) (unsigned short)((((unsigned int)(a) * (unsigned int)(b)) >> 16) + 1)
// endian safe?
#define mglGet8(color, shift) (unsigned char)(((color) & (0x000000ff << (shift))) >> (shift))

struct mglPixelFormat
{
	unsigned int RMask;
	unsigned int GMask;
	unsigned int BMask;
	unsigned int AMask;
	unsigned int RShift;
	unsigned int GShift;
	unsigned int BShift;
	unsigned int AShift;
};

// returns the dot product between two 32 bit colors
// unsigned, so returns 0-255 range
// MAKE SURE OF 2 THINGS:
// 1) The input RGB/RGBA colors are normalized (length = 255)
// 2) For RGB normalization, make sure alpha is 0
inline unsigned char mglDot(unsigned int a, unsigned int b)
{
	return (
		mglMul8((unsigned char)a, (unsigned char)b) +
		mglMul8((unsigned char)(a>>8), (unsigned char)(b>>8)) +
		mglMul8((unsigned char)(a>>16), (unsigned char)(b>>16)) +
		mglMul8((unsigned char)(a>>24), (unsigned char)(b>>24))
	);
}

#endif
