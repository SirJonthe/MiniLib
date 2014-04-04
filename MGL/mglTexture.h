#ifndef MGL_TEXTURE_H_INCLUDED__
#define MGL_TEXTURE_H_INCLUDED__

#include "../MTL/mtlDirectory.h"
#include "../MTL/mtlAsset.h"
#include "mglPixel.h"

#define mglTILE 4
#define mglTILE_AREA 16
#define mglTILE_SHIFT 2
#define mglTILE_AREA_SHIFT 4
#define mglTILE_MASK 3
#define mglTILE_AREA_MASK 15

class mglTexture : public mtlAssetInterface
{
public:
	static const mglPixelFormat TargaFormat;
	static mglPixelFormat		nativeFormat;
private:
	unsigned int	*m_pixels;
	int				m_dimension;
	int				m_dimensionShift;
	int				m_dimensionMask;
	int				m_blocksPerDimShift;
	int				m_blockTileRatioShift;
private:
					mglTexture(const mglTexture&) {}
	mglTexture		&operator=(const mglTexture&) { return *this; }
	int				GetBase2Log(int x) const;
	bool			VerifyDimension(int p_dimension) const;
	inline int		GetTiledIndex(int x, int y) const;
	inline int		GetMortonIndex(int x, int y) const;
	bool			LoadTGA(const mtlDirectory &p_filename);
	void			OrderAsMorton( void );
public:
							mglTexture( void );
							mglTexture(int p_dimension, unsigned int p_color);
							~mglTexture( void );
	bool					Load(const mtlDirectory &p_filename);
	bool					Create(int p_dimension);
	bool					Create(int p_dimension, unsigned int p_color);
	void					Free( void );
	void					Copy(const mglTexture &p_texture);
	int						GetDimension( void ) const { return this->m_dimension; }
	int						GetArea( void ) const { return this->m_dimension*this->m_dimension; }
	bool					IsBad( void ) const	{ return this->m_pixels == NULL; }
	const unsigned int		*GetPixelXY(int x, int y) const { return m_pixels + GetMortonIndex(x & m_dimensionMask, y & m_dimensionMask); }
	const unsigned int		*GetPixelXY(float x, float y) const { return GetPixelXY(int(x), int(y)); }
	const unsigned int		*GetPixelUV(float u, float v) const { return GetPixelXY((int)(u * m_dimension), (int)(v * m_dimension)); }
};

int mglTexture::GetTiledIndex(int x, int y) const
{
	// Restrictions on negative bitshifting (i.e. negative left
	// shift is not same as positive right shift), texture dimension
	// must be at least TILE*TILE in size. See comments in VerifyDimension.

	//const int blockPerDim = m_dimension / TILE;
	// /* x1 = */ ( ((x >> TILE_AREA_SHIFT) + int((y & TILE_MASK) * float(float(blockPerDim) / float(TILE))) << TILE_SHIFT );

	// NOTE:
	// float(float(blockPerDim) / float(TILE) =
	// float(DIM >> TILE_SHIFT) / float(TILE) =
	// DIM >> TILE_AREA_SHIFT
	return
		// y
			(
			/* y0 = */ ((y & ~mglTILE_MASK) +
			/* y1 = */ (((x >> mglTILE_SHIFT) + (y << m_blocksPerDimShift)) & mglTILE_MASK)
			) << m_dimensionShift) +
		// x
			/* x0 = */ (x & mglTILE_MASK) +
			/* x1 = */ (((x >> mglTILE_AREA_SHIFT) + ((y & mglTILE_MASK) << m_blockTileRatioShift)) << mglTILE_SHIFT);
}

int mglTexture::GetMortonIndex(int x, int y) const
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

/*int mglTexture::GetTiledIndexSlow(int x, int y) const
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
