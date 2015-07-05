#ifndef MGL_TEXTURE_H_INCLUDED__
#define MGL_TEXTURE_H_INCLUDED__

#include "../MTL/mtlAsset.h"
#include "mglPixel.h"

// variable bit depth (problem; how does the programmer set depth manually if Load interface is only common interface for assets?)
// SIMD (RGBA SoA)
// compression (vector quantization)
// z order swizzle
// device independent by default (i.e. red in byte index 0, green in byte index 1, etc.) (same problem as bit depth)
// unpack pixels on access
class mglTexture : public mtlAssetInterface
{
private:
	//mglByte       *m_pixels;
	mglPixel32    *m_pixels;
	int            m_width;
	int            m_height;
	int            m_width_mask;
	int            m_height_mask;
	int            m_width_shift;
	int            m_height_shift;
	mglByteOrder32 m_order; // only used if bytes per pixel is 4
	mglPixelFormat m_format;

private:
	mglTexture(const mglTexture &) {}
	mglTexture &operator=(const mglTexture&) { return *this; }

	bool       VerifyDimension(int dimension) const;
	mglPixel32 UnpackTGAPixel(unsigned char *pixel_data, int bpp, int type) const;
	bool       LoadTGA(const mtlDirectory &p_filename);
	//bool       LoadVPZ(const mtlDirectory &p_filename); // [V]ector quantisized, [P]acked, [Z] order image (own format)

public:
	mglTexture( void );
	mglTexture(int width, int height);
	~mglTexture( void ) { delete [] m_pixels; }

	int GetWidth( void )  const { return m_width; }
	int GetHeight( void ) const { return m_height; }
	int GetArea( void )   const { return m_width*m_height; }

	bool Create(int width, int height);

	//bool CreateFrom(const mglImage &image); // create texture from image (note that we would have to handle dimensions that are not supported)

	bool Load(const mtlDirectory &p_filename);

	void Free( void );

	// take bpp into account when we change to variable bit depth
	mglPixel32 GetPixelXY(int x, int y)     const { int index = (x & m_width_mask) + ((y & m_height_mask) << m_width_shift); return m_pixels[index]; }
	mglPixel32 GetPixelUV(float u, float v) const { return GetPixelXY(u * m_width, v * m_height); }
	//void SetPixel(mglByte r, mglByte g, mglByte b, mglByte a = 0xff) {}

	mglByteOrder32 GetByteOrder( void ) const { return m_order; }
	void SetByteOrder(mglByteOrder32 order) { m_order = order; }

	mglPixelFormat GetPixelFormat( void ) const { return m_format; }
	//void SetPixelFormat(mglPixelFormat format, mglByteOrder32 order);
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
