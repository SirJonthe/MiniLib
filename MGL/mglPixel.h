#ifndef MGL_PIXEL_H_INCLUDED__
#define MGL_PIXEL_H_INCLUDED__

#define mglMul8(a, b) (unsigned char)((((unsigned short)(a) * (unsigned short)(b)) >> 8) + 1)
#define mglMul8x4(a, b) (unsigned int)(mglMul8((unsigned char)(a), (unsigned char)(b)) | (mglMul8((unsigned char)((a)>>8), (unsigned char)((b)>>8)) << 8) | (mglMul8((unsigned char)((a)>>16), (unsigned char)((b)>>16)) << 16) | (mglMul8((unsigned char)((a)>>16), (unsigned char)((b)>>16)) << 24))
#define mglMul16(a, b) (unsigned short)((((unsigned int)(a) * (unsigned int)(b)) >> 16) + 1)

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

union mglPixel
{
	unsigned char	rgba[sizeof(unsigned int)];
	unsigned int	color;
};

namespace mglPixelManip
{
	inline unsigned char Mul(unsigned char a, unsigned char b)
	{
		return (unsigned char)((((unsigned short)a * (unsigned short)b) >> 8) + 1);
	}
	inline unsigned int Mul(unsigned int a, unsigned int b) {
		return
			(unsigned int)(Mul((unsigned char)a, (unsigned char)b)) |
			(((unsigned int)(Mul((unsigned char)(a >> 8), (unsigned char)(b >> 8)))) << 8) |
			(((unsigned int)(Mul((unsigned char)(a >> 16), (unsigned char)(b >> 16)))) << 16) |
			(((unsigned int)(Mul((unsigned char)(a >> 24), (unsigned char)(b >> 24)))) << 24)
		;
	}
	// returns the dot product between two 32 bit colors
	// unsigned, so returns 0-255 range
	// MAKE SURE OF 2 THINGS:
	// 1) The input RGB/RGBA colors are normalized (length = 255)
	// 2) For RGB normalization, make sure alpha is 0
	inline unsigned char Dot(unsigned int a, unsigned int b)
	{
		return (
			Mul((unsigned char)a, (unsigned char)b) +
			Mul((unsigned char)(a >> 8), (unsigned char)(b >> 8)) +
			Mul((unsigned char)(a >> 16), (unsigned char)(b >> 16)) +
			Mul((unsigned char)(a >> 24), (unsigned char)(b >> 24))
		);
	}
};

#endif
