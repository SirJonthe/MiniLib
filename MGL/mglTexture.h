#ifndef MGL_TEXTURE_H_INCLUDED__
#define MGL_TEXTURE_H_INCLUDED__

#include "../MTL/mtlAsset.h"
#include "mglPixel.h"

// optimized for random access
// variable bit depth
// size constraints
// SIMD (RGBA SoA)
// morton order
// compression
// sample coord wrap
// device independent by default (i.e. red in byte index 0, green in byte index 1, etc.)
class mglTexture : public mtlAssetInterface
{
private:
	mglByte       *m_pixels;
	int            m_width;
	int            m_height;
	int            m_width_mask;
	int            m_height_mask;
	mglByteOrder32 m_order; // only used if bytes per pixel is 4
	mglPixelFormat m_format;

private:
	mglTexture(const mglTexture &) {}
	mglTexture &operator=(const mglTexture&) { return *this; }

	bool       VerifyDimension(int dimension) const;
	mglPixel32 UnpackTGAPixel(unsigned char *pixel_data, int bpp, int type) const;
	bool       LoadTGA(const mtlDirectory &p_filename);

public:
	mglTexture( void );
	mglTexture(int width, int height);
	mglTexture(int width, int height, mglPixelFormat format);
	mglTexture(int width, int height, mglByte r, mglByte g, mglByte b, mglByte a = 0xff);
	mglTexture(int width, int height, mglPixelFormat format, mglByte r, mglByte g, mglByte b, mglByte a = 0xff);
	~mglTexture( void ) { delete [] m_pixels; }

	int GetWidth( void )  const { return m_width; }
	int GetHeight( void ) const { return m_height; }
	int GetArea( void )   const { return m_width*m_height; }

	bool Create(int width, int height);
	bool Create(int width, int height, mglPixelFormat format);
	bool Create(int width, int height, mglByte r, mglByte g, mglByte b, mglByte a = 0xff);
	bool Create(int width, int height, mglPixelFormat format, mglByte r, mglByte g, mglByte b, mglByte a = 0xff);

	bool Load(const mtlDirectory &p_filename);

	void Free( void );

	mglPixelFormat GetPixelFormat( void ) const { return m_format; }
	void SetPixelFormat(mglPixelFormat format, mglByteOrder32 order);
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
