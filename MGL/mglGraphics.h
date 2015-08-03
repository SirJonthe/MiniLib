#ifndef MGL_GRAPHICS_H_INCLUDED__
#define MGL_GRAPHICS_H_INCLUDED__

#include "../MML/mmlVector.h"
#include "../MTL/mtlBits.h"
//#include "../MTL/mtlType.h"
#include "../MTL/mtlArray.h"
#include "../MGL/mglPixel.h"

void mglDrawPixel(int x, int y, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglDrawLine(mmlVector<2> p1, mmlVector<2> p2, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglDrawCircle(mmlVector<2> mid, float rad, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglDrawCircleQuadrant(mmlVector<2> mid, float rad, int quadrant, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());

void mglFillBox(mmlVector<2> p1, mmlVector<2> p2, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
//void mglFillCircle(mmlVector<2> mid, float rad, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglFillTriangle(mmlVector<2> p1, mmlVector<2> p2, mmlVector<2> p3, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());

/*struct mglColor32
{
	mtlByte r, g, b, a;
};

struct mglPixelSurface
{
	mtlByte        *pixels;
	int             bytes_per_pixel;
	int             width;
	int             height;
	mglByteOrder32  byte_order;
};

class mglBox;
class mglCircle;

class mglPrimitive
{
public:
	virtual ~mglPrimitive( void ) {}

	virtual void Draw(mglColor32 color, mglPixelSurface surface) const = 0;
	virtual void Fill(mglColor32 color, mglPixelSurface surface) const = 0;

	virtual mglBox    GetBoundingBox( void )    const = 0;
	virtual mglCircle GetBoundingCircle( void ) const = 0;

	virtual bool IsInside(mmlVector<2> xy)  const = 0;
	bool         IsInside(float x, float y) const { return IsInside(mmlVector<2>(x, y)); }
};

class mglLine : mtlInherit<mglPrimitive, mglLine>
{
private:
	mmlVector<2> a, b;

public:
	void Draw(mglColor32 color, mglPixelSurface surface) const;
	void Fill(mglColor32 color, mglPixelSurface surface) const;

	mglBox    GetBoundingBox( void ) const;
	mglCircle GetBoundingCircle( void ) const;

	bool IsInside(mmlVector<2> xy) const;
};

class mglBox : mtlInherit<mglPrimitive, mglBox>
{
private:
	mmlVector<2> a, b;

public:
	void Draw(mglColor32 color, mglPixelSurface surface) const;
	void Fill(mglColor32 color, mglPixelSurface surface) const;

	mglBox    GetBoundingBox( void ) const;
	mglCircle GetBoundingCircle( void ) const;

	bool IsInside(mmlVector<2> xy) const;

	mglBox Clipped(mglBox box);
};

class mglCircle : mtlInherit<mglPrimitive, mglCircle>
{
private:
	mmlVector<2> mid;
	float        rad;
};

//class mglEllipsoid : mtlInherit<mglPrimitive, mglEllipsoid> {};

class mglPolygon : mtlInherit<mglPrimitive, mglPolygon>
{
private:
	mtlArray< mmlVector<2> > vert;

public:
	mglPolygon Clipped(mmlVector<2> plane_normal, mmlVector<2> plane_point) const;
};

class mglTriangle : mtlInherit<mglPrimitive, mglTriangle>
{
private:
	mmlVector<2> a, b, c;

public:
	mglPolygon AsPolygon( void ) const;
};*/

#endif // MGL_GRAPHICS_H_INCLUDED__
