#ifndef MGL_IMAGE_H_INCLUDED
#define MGL_IMAGE_H_INCLUDED

#include "mglPixel.h"
#include "mglColor.h"
#include "../MTL/mtlAsset.h"

// no size constraints
// always 32-bit RGBA
// linear access
// custom byte order
// SIMD SoA for fast pixel manipulation?
// mglImage loads TGA
class mglImage : public mtlAssetInterface
{
private:
	mglPixel32     *m_pixels;
	mglByteOrder32  m_order;
	int             m_width;
	int             m_height;

private:
	mglImage(const mglImage&) {}
	mglImage &operator=(const mglImage&) { return *this; }

public:
	mglImage( void );
	mglImage(int p_width, int p_height);
	mglImage(int p_width, int p_height, mglByteOrder32 p_order);
	mglImage(int p_width, int p_height, mglColor32 color);
	mglImage(int p_width, int p_height, mglByteOrder32 p_order, mglColor32 color);
	~mglImage( void ) { delete [] m_pixels; }

	int GetWidth( void )  const { return m_width; }
	int GetHeight( void ) const { return m_height; }
	int GetArea( void )   const { return m_width*m_height; }

	mglPixelFormat::Color GetColorMode( void )     const { return mglPixelFormat::Color_Truecolor; }
	int                   GetBytesPerPixel( void ) const { return 4; }

	mglByteOrder32 GetByteOrder( void ) const                  { return m_order; }
	void           SetByteOrderNoReorder(mglByteOrder32 order) { m_order = order; }
	void           SetByteOrder(mglByteOrder32 order);

	void Create(int p_width, int p_height);
	void Create(int p_width, int p_height, mglByteOrder32 p_order);
	void Create(int p_width, int p_height, mglColor32 color);
	void Create(int p_width, int p_height, mglByteOrder32 p_order, mglColor32 color);

	void Copy(const mglImage &image, bool copy_order = false);

	void Free( void );

	mglPixel32       *GetPixelXY(int x, int y)       { return m_pixels + x + y * m_width; }
	const mglPixel32 *GetPixelXY(int x, int y) const { return m_pixels + x + y * m_width; }

	mtlByte       *GetRedXY(int x, int y)         { return (mtlByte*)(m_pixels + x + y * m_width) + m_order.index.r; }
	mtlByte       *GetGreenXY(int x, int y)       { return (mtlByte*)(m_pixels + x + y * m_width) + m_order.index.g; }
	mtlByte       *GetBlueXY(int x, int y)        { return (mtlByte*)(m_pixels + x + y * m_width) + m_order.index.b; }
	mtlByte       *GetAlphaXY(int x, int y)       { return (mtlByte*)(m_pixels + x + y * m_width) + m_order.index.a; }
	const mtlByte *GetRedXY(int x, int y)   const { return (mtlByte*)(m_pixels + x + y * m_width) + m_order.index.r; }
	const mtlByte *GetGreenXY(int x, int y) const { return (mtlByte*)(m_pixels + x + y * m_width) + m_order.index.g; }
	const mtlByte *GetBlueXY(int x, int y)  const { return (mtlByte*)(m_pixels + x + y * m_width) + m_order.index.b; }
	const mtlByte *GetAlphaXY(int x, int y) const { return (mtlByte*)(m_pixels + x + y * m_width) + m_order.index.a; }

	mglColor32 GetColorXY(int x, int y) const;

	void DrawPixel(int x, int y, mglColor32 color);
	void DrawLine(int x1, int y1, int x2, int y2, mglColor32 color);
	void DrawCircle(int cx, int cy, int radius, mglColor32 color);
	void DrawCircleQuad(int cx, int cy, int radius, unsigned int quad_mask, mglColor32 color);
	void FillBox(int x1, int y1, int x2, int y2, mglColor32 color);
//	void FillCircle(int cx, int cy, int radius, mglColor32 color);
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, mglColor32 color);
	void DrawChar(char ch, int x, int y, mglColor32 color, int scale = 1);

	static void Blit(mglImage &dst, int dx1, int dy1, int dx2, int dy2, const mglImage &src, int sx1, int sy1, int sx2, int sy2);
};

#endif // MGL_IMAGE_H_INCLUDED
