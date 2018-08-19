#ifndef MGL_TEXTURE_H_INCLUDED
#define MGL_TEXTURE_H_INCLUDED

#include "../MTL/mtlAsset.h"
#include "../MTL/mtlBits.h"
#include "mglPixel.h"

// SIMD (RGBA SoA)
	// Unsure how this will benefit random access (Mostly benefits pixel manipulation)
// compression (vector quantization)
// compression (YUV)
	// requires decompression to RGBA by multiplying YUV color by 3x3 matrix
// dithering to remove vector quantization artifacts?
// add mip mapping
class mglTexture : public mtlAssetInterface
{
private:
	struct CodeNode // don't know if this is correct
	{
		mglPixel32 codes[0xff];
		CodeNode *left;
		CodeNode *right;
	};

private:
	mtlByte        *m_pixels;
	int             m_width;
	int             m_height;
	int             m_width_mask;
	int             m_height_mask;
	int             m_width_shift;
	int             m_height_shift;
	mglPixelFormat  m_format;
	mtlString       m_format_str;

private:
	mglTexture(const mglTexture &) {}
	mglTexture &operator=(const mglTexture&) { return *this; }

	bool       VerifyDimension(int dimension) const;
	void       UnpackTGAPixel(mtlByte *out, const unsigned char *pixel_data, int bpp, int type) const;
	bool       LoadTGA(const mtlPath &p_filename);
	bool       LoadPQZ(const mtlPath &p_filename) { return false; } // [P]acked [Q]uantized [Z]-order image
	void       Swizzle_Z( void );
	void       Pack_SOA( void ) {} // stores in SoA
	void       Compress_VQ(const mtlByte *pixels, int total_size); // uses Vector Quantization to compress (super duper slow???)
	mglPixel32 DecodePixel(const mtlByte *in) const; // retrieves a pixel (reverses bit depth, morton order, compression, SIMD)
	//void       EncodePixel(mglPixel32 in, mtlByte *out); // set the color of a pixel

public:
	mglTexture( void );
	mglTexture(int width, int height);
	mglTexture(int width, int height, mglPixelFormat format);
	~mglTexture( void ) { delete [] m_pixels; }

	int GetWidth( void )  const { return m_width; }
	int GetHeight( void ) const { return m_height; }
	int GetArea( void )   const { return m_width << m_height_shift; }

	bool Create(int width, int height);
	bool Create(int width, int height, mglPixelFormat format);

	//bool CreateFrom(const mglImage &image); // create texture from image (note that we would have to handle dimensions that are not supported)

	bool Load(const mtlPath &p_filename);

	void Free( void );

	// take bpp into account when we change to variable bit depth
	mglPixel32 GetPixelXY(int x, int y)     const { return DecodePixel(m_pixels + mtlEncodeMorton2(x & m_width_mask, y & m_height_mask) * m_format.bytes_per_pixel); }
	mglPixel32 GetPixelXY(float x, float y) const { return GetPixelXY(int(x), int(y)); }
	mglPixel32 GetPixelUV(float u, float v) const { return GetPixelXY(u * m_width, v * m_height); }

	mglPixelFormat        GetPixelFormat( void )   const { return m_format; }
	int                   GetBytesPerPixel( void ) const { return m_format.bytes_per_pixel; }
	mglPixelFormat::Color GetColorMode( void )     const { return m_format.color; }
	static mglByteOrder32 GetByteOrder( void );

	const char *Debug_GetFormatString( void ) const { return m_format_str.GetChars(); }
};

/*
#define mglTILE 4
#define mglTILE_AREA 16
#define mglTILE_SHIFT 2
#define mglTILE_AREA_SHIFT 4
#define mglTILE_MASK 3
#define mglTILE_AREA_MASK 15

int mglTexture::GetTiledIndexSlow(int x, int y) const
{
	const int blockPerDim = m_dimension / mglTILE;
	int x0 = x % mglTILE;
	int x1 = x / mglTILE_AREA * mglTILE; // I *think* this resolves issues where there are more pixes in a full dimension than the number of pixels in the area of a single tile
	int x2 = int(y % mglTILE * float(float(blockPerDim) / float(mglTILE))) * mglTILE; // the floating point conversions can be converted to a variable bitshift that can go either positive (multiplication by integer) or negative (multiplication by fractal)
	int x3 = 0;
	int y0 = y / mglTILE * mglTILE; // the pitch of the main row of tiles to read (I can pretty much guarantee this is CORRECT)
	int y1 = x / mglTILE % mglTILE;
	int y2 = y * blockPerDim % mglTILE;
	int y3 = 0;
	int xf = x0 + x1 + x2 + x3;
	int yf = y0 + y1 + y2 + y3;

	return yf*m_dimension + xf;
}*/

#endif
