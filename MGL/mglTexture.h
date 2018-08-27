#ifndef MGL_TEXTURE_H_INCLUDED
#define MGL_TEXTURE_H_INCLUDED

#include "../MTL/mtlAsset.h"
#include "../MTL/mtlBits.h"
#include "../MTL/mtlArray.h"
#include "mglPixel.h"
#include "mglColor.h"

// always ordered RGBA, although compression may vary
// real-time compression

// A more up to date version exists in render_fun

class mglTexture : public mtlAssetInterface
{
private:
	struct MipMap
	{
		mtlByte       *pixels;
		int            width;
		int            height;
		int            width_mask;
		int            height_mask;
		int            width_shift;
		int            height_shift;
		int            level;
		mglPixelFormat format;

		MipMap( void );
		~MipMap( void );

		void       Swizzle( void );
		void       CreateFrom(const MipMap &m);
		int        GetPixelIndex(int x, int y) const;
		mglPixel32 GetPixelXY(int x, int y)    const;
	};

private:
	mtlArray<MipMap>  m_mips;
	MipMap           *m_main;
	mtlString         m_format_str;

private:
	mglTexture(const mglTexture &) {}
	mglTexture &operator=(const mglTexture&) { return *this; }

	bool       VerifyDimension(int dimension) const;
	void       UnpackTGAPixel(mtlByte *out, const unsigned char *pixel_data, int bpp, int type) const;
	bool       LoadTGA(const mtlPath &p_filename);

public:
	mglTexture( void );
	mglTexture(int width, int height);
	mglTexture(int width, int height, mglPixelFormat format);

	int GetWidth( void )  const { return m_main->width; }
	int GetHeight( void ) const { return m_main->height; }
	int GetArea( void )   const { return m_main->width << m_main->height_shift; }

	bool Create(int width, int height);
	bool Create(int width, int height, mglPixelFormat format);

	void UpdateMipMaps( void );

	//bool CreateFrom(const mglImage &image); // create texture from image (note that we would have to handle dimensions that are not supported)

	bool Load(const mtlPath &p_filename);

	void Free( void );

	mglPixel32 GetPixelXY(int x, int y, int mip = 0)     const { return m_mips[mip].GetPixelXY(x, y); }
	mglPixel32 GetPixelXY(float x, float y, int mip = 0) const { return GetPixelXY(int(x), int(y), mip); }
	mglPixel32 GetPixelUV(float u, float v, int mip = 0) const { return GetPixelXY(u * m_main->width, v * m_main->height, mip); }

	mglColor32 GetColorXY(int x, int y, int mip = 0)     const { return mglDecodePixel(GetPixelXY(x, y, mip), m_mips[mip].format.byte_order); }
	mglColor32 GetColorXY(float x, float y, int mip = 0) const { return mglDecodePixel(GetPixelXY(x, y, mip), m_mips[mip].format.byte_order); }
	mglColor32 GetColorUV(float u, float v, int mip = 0) const { return mglDecodePixel(GetPixelUV(u, v, mip), m_mips[mip].format.byte_order); }

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
