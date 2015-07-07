#include "mglImage.h"
#include "../MTL/mtlMemory.h"

mglImage::mglImage( void ) : m_pixels(NULL), m_width(0), m_height(0)
{
	m_order.index.r = 0;
	m_order.index.g = 1;
	m_order.index.b = 2;
	m_order.index.a = 3;
}

mglImage::mglImage(int p_width, int p_height) : m_pixels(NULL), m_width(0), m_height(0)
{
	m_order.index.r = 0;
	m_order.index.g = 1;
	m_order.index.b = 2;
	m_order.index.a = 3;
	Create(p_width, p_height);
}

mglImage::mglImage(int p_width, int p_height, mglByteOrder32 p_order) : m_pixels(NULL), m_width(0), m_height(0)
{
	Create(p_width, p_height, p_order);
}

mglImage::mglImage(int p_width, int p_height, mtlByte r, mtlByte g, mtlByte b, mtlByte a) : m_pixels(NULL), m_width(0), m_height(0)
{
	m_order.index.r = 0;
	m_order.index.g = 1;
	m_order.index.b = 2;
	m_order.index.a = 3;
	Create(p_width, p_height, r, g, b, a);
}

mglImage::mglImage(int p_width, int p_height, mglByteOrder32 p_order, mtlByte r, mtlByte g, mtlByte b, mtlByte a) : m_pixels(NULL), m_width(0), m_height(0)
{
	Create(p_width, p_height, p_order, r, g, b, a);
}

void mglImage::SetByteOrder(mglByteOrder32 order)
{
	const int area = GetArea();
	for (int i = 0; i < area; ++i) {
		mglPixel32 src = m_pixels[i];
		mglPixel32 dst;
		dst.bytes[order.index.r] = src.bytes[m_order.index.r];
		dst.bytes[order.index.g] = src.bytes[m_order.index.g];
		dst.bytes[order.index.b] = src.bytes[m_order.index.b];
		dst.bytes[order.index.a] = src.bytes[m_order.index.a];
		m_pixels[i] = dst;
	}
	m_order = order;
}

void mglImage::Create(int p_width, int p_height)
{
	if (p_width*p_height != m_width*m_height) {
		Free();
		m_pixels = new mglPixel32[p_width*p_height];
	}
	m_width = p_width;
	m_height = p_height;
}

void mglImage::Create(int p_width, int p_height, mglByteOrder32 p_order)
{
	m_order = p_order;
	Create(p_width, p_height);
}

void mglImage::Create(int p_width, int p_height, mtlByte r, mtlByte g, mtlByte b, mtlByte a)
{
	Create(p_width, p_height);
	const int area = GetArea();
	mglPixel32 color = mglRGBA(r, g, b, a, m_order);
	for (int i = 0; i < area; ++i) {
		m_pixels[i] = color;
	}
}

void mglImage::Create(int p_width, int p_height, mglByteOrder32 p_order, mtlByte r, mtlByte g, mtlByte b, mtlByte a)
{
	m_order = p_order;
	Create(p_width, p_height, r, g, b, a);
}

void mglImage::Copy(const mglImage &image, bool copy_order)
{
	if (copy_order) { m_order = image.m_order; }
	Create(image.m_width, image.m_height);
	const int area = GetArea();
	for (int i = 0; i < area; ++i) {
		mglPixel32 dst;
		mglPixel32 src = image.m_pixels[i];
		dst.bytes[m_order.index.r] = src.bytes[image.m_order.index.r];
		dst.bytes[m_order.index.g] = src.bytes[image.m_order.index.g];
		dst.bytes[m_order.index.b] = src.bytes[image.m_order.index.b];
		dst.bytes[m_order.index.a] = src.bytes[image.m_order.index.a];
		m_pixels[i] = dst;
	}
}

void mglImage::Free( void )
{
	delete [] m_pixels;
	m_pixels = NULL;
	m_width = 0;
	m_height = 0;
}
