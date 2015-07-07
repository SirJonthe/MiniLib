#ifndef MGL_IMAGE_H_INCLUDED__
#define MGL_IMAGE_H_INCLUDED__

#include "mglPixel.h"
#include "../MTL/mtlAsset.h"

// KNOWN BUGS:
	// TGA reader can't load compressed 16-bit grayscale images (Chunk size exceeds pixel count)
	// TGA reader can't load compressed 24-bit truecolor images (Chunk size exceeds pixel count)

// no size constraints
// always 32-bit RGBA
// linear access
// custom
class mglImage : public mtlAssetInterface
{
private:
	mglPixel32    *m_pixels;
	mglByteOrder32 m_order;
	int            m_width;
	int            m_height;

private:
	mglImage(const mglImage&) {}
	mglImage &operator=(const mglImage&) { return*this; }

public:
	mglImage( void );
	mglImage(int p_width, int p_height);
	mglImage(int p_width, int p_height, mglByteOrder32 p_order);
	mglImage(int p_width, int p_height, mtlByte r, mtlByte g, mtlByte b, mtlByte a = 0xff);
	mglImage(int p_width, int p_height, mglByteOrder32 p_order, mtlByte r, mtlByte g, mtlByte b, mtlByte a = 0xff);
	~mglImage( void ) { delete [] m_pixels; }

	int GetWidth( void )  const { return m_width; }
	int GetHeight( void ) const { return m_height; }
	int GetArea( void )   const { return m_width*m_height; }

	mglPixelFormat::Color GetColorMode( void ) const     { return mglPixelFormat::Color_Truecolor; }
	int                   GetBytesPerPixel( void ) const { return 4; }

	mglByteOrder32 GetByteOrder( void ) const                  { return m_order; }
	void           SetByteOrderNoReorder(mglByteOrder32 order) { m_order = order; }
	void           SetByteOrder(mglByteOrder32 order);

	void Create(int p_width, int p_height);
	void Create(int p_width, int p_height, mglByteOrder32 p_order);
	void Create(int p_width, int p_height, mtlByte r, mtlByte g, mtlByte b, mtlByte a = 0xff);
	void Create(int p_width, int p_height, mglByteOrder32 p_order, mtlByte r, mtlByte g, mtlByte b, mtlByte a = 0xff);

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
};

#endif // MGL_IMAGE_H_INCLUDED__
