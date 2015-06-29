#ifndef MGL_TEXTURE_H_INCLUDED__
#define MGL_TEXTURE_H_INCLUDED__

#include "../MTL/mtlDirectory.h"
#include "../MTL/mtlAsset.h"
#include "mglPixel.h"

// Remove swizzling?
// Does not sit well when trying to load pixel data to graphics memory
// Can still manually swizzle data
// I have to support multiple pixel depths
	// Traversing large grayscale images causes a lot of wasteful cache misses

// KNOWN BUGS:
	// TGA reader can't load compressed 16-bit grayscale images (Chunk size exceeds pixel count)
	// TGA reader can't load compressed 24-bit truecolor images (Chunk size exceeds pixel count)

class mglTexture : public mtlAssetInterface
{
private:
	mglPixel         *m_pixels;
	int               m_width;
	int               m_height;
	int               m_width_mask;
	int               m_height_mask;
	mglPixelFormat32  m_format;

private:
	mglTexture(const mglTexture&) {}
	mglTexture &operator=(const mglTexture&) { return *this; }
	int         GetBase2Log(int x) const;
	bool        VerifyDimension(int p_dimension) const;
	inline int  GetMortonIndex(int x, int y) const;
	mglPixel    UnpackTGAPixel(unsigned char *pixel_data, int bpp, int type) const;
	bool        LoadTGA(const mtlDirectory &p_filename);
	void        OrderAsMorton( void );

public:
	mglTexture( void );
	mglTexture(int p_width, int p_height);
	mglTexture(int p_width, int p_height, unsigned int p_color);
	~mglTexture( void );

	bool Load(const mtlDirectory &p_filename);
	bool Create(int p_width, int p_height);
	bool Create(int p_width, int p_height, unsigned int p_color);
	void Free( void );
	void Copy(const mglTexture &p_texture, bool copy_format = true);

	int GetWidth( void ) const  { return this->m_width; }
	int GetHeight( void ) const { return this->m_height; }
	int GetArea( void ) const   { return this->m_width*this->m_height; }

	bool IsBad( void ) const { return this->m_pixels == NULL; }

	const mglPixel *GetPixelXY(int x, int y) const     { return m_pixels + GetMortonIndex(x & m_width_mask, y & m_height_mask); }
	const mglPixel *GetPixelXY(float x, float y) const { return GetPixelXY(int(x), int(y)); }
	const mglPixel *GetPixelUV(float u, float v) const { return GetPixelXY((int)(u * m_width), (int)(v * m_height)); }

	mglPixel *GetPixelXY(int x, int y)     { return m_pixels + GetMortonIndex(x & m_width_mask, y & m_height_mask); }
	mglPixel *GetPixelXY(float x, float y) { return GetPixelXY(int(x), int(y)); }
	mglPixel *GetPixelUV(float u, float v) { return GetPixelXY((int)(u * m_width), (int)(v * m_height)); }

	const unsigned char *GetRedXY(int x, int y) const       { return ((const unsigned char*)GetPixelXY(x, y)) + m_format.index.r; }
	const unsigned char *GetGreenXY(int x, int y) const     { return ((const unsigned char*)GetPixelXY(x, y)) + m_format.index.g; }
	const unsigned char *GetBlueXY(int x, int y) const      { return ((const unsigned char*)GetPixelXY(x, y)) + m_format.index.b; }
	const unsigned char *GetAlphaXY(int x, int y) const     { return ((const unsigned char*)GetPixelXY(x, y)) + m_format.index.a; }
	const unsigned char *GetRedXY(float x, float y) const   { return ((const unsigned char*)GetPixelXY(x, y)) + m_format.index.r; }
	const unsigned char *GetGreenXY(float x, float y) const { return ((const unsigned char*)GetPixelXY(x, y)) + m_format.index.g; }
	const unsigned char *GetBlueXY(float x, float y) const  { return ((const unsigned char*)GetPixelXY(x, y)) + m_format.index.b; }
	const unsigned char *GetAlphaXY(float x, float y) const { return ((const unsigned char*)GetPixelXY(x, y)) + m_format.index.a; }
	const unsigned char *GetRedUV(float u, float v) const   { return ((const unsigned char*)GetPixelUV(u, v)) + m_format.index.r; }
	const unsigned char *GetGreenUV(float u, float v) const { return ((const unsigned char*)GetPixelUV(u, v)) + m_format.index.g; }
	const unsigned char *GetBlueUV(float u, float v) const  { return ((const unsigned char*)GetPixelUV(u, v)) + m_format.index.b; }
	const unsigned char *GetAlphaUV(float u, float v) const { return ((const unsigned char*)GetPixelUV(u, v)) + m_format.index.a; }

	unsigned char *GetRedXY(int x, int y)       { return ((unsigned char*)GetPixelXY(x, y)) + m_format.index.r; }
	unsigned char *GetGreenXY(int x, int y)     { return ((unsigned char*)GetPixelXY(x, y)) + m_format.index.g; }
	unsigned char *GetBlueXY(int x, int y)      { return ((unsigned char*)GetPixelXY(x, y)) + m_format.index.b; }
	unsigned char *GetAlphaXY(int x, int y)     { return ((unsigned char*)GetPixelXY(x, y)) + m_format.index.a; }
	unsigned char *GetRedXY(float x, float y)   { return ((unsigned char*)GetPixelXY(x, y)) + m_format.index.r; }
	unsigned char *GetGreenXY(float x, float y) { return ((unsigned char*)GetPixelXY(x, y)) + m_format.index.g; }
	unsigned char *GetBlueXY(float x, float y)  { return ((unsigned char*)GetPixelXY(x, y)) + m_format.index.b; }
	unsigned char *GetAlphaXY(float x, float y) { return ((unsigned char*)GetPixelXY(x, y)) + m_format.index.a; }
	unsigned char *GetRedUV(float u, float v)   { return ((unsigned char*)GetPixelUV(u, v)) + m_format.index.r; }
	unsigned char *GetGreenUV(float u, float v) { return ((unsigned char*)GetPixelUV(u, v)) + m_format.index.g; }
	unsigned char *GetBlueUV(float u, float v)  { return ((unsigned char*)GetPixelUV(u, v)) + m_format.index.b; }
	unsigned char *GetAlphaUV(float u, float v) { return ((unsigned char*)GetPixelUV(u, v)) + m_format.index.a; }

	const mglPixelFormat32 &GetFormat( void ) const { return m_format; }
	void                    SetFormat(const mglPixelFormat32 &format);
};

int mglTexture::GetMortonIndex(int x, int y) const // x and y must be 0-65535
{
	x = (x | (x << 8)) & 0x00FF00FF;
	x = (x | (x << 4)) & 0x0F0F0F0F;
	x = (x | (x << 2)) & 0x33333333;
	x = (x | (x << 1)) & 0x55555555;

	y = (y | (y << 8)) & 0x00FF00FF;
	y = (y | (y << 4)) & 0x0F0F0F0F;
	y = (y | (y << 2)) & 0x33333333;
	y = (y | (y << 1)) & 0x55555555;

	return x | (y << 1);
}

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
