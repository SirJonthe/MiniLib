#include "mglImage.h"
#include "mglRay.h"

#include "../MML/mmlMath.h"
#include "../MML/mmlFixed.h"

#include "../MTL/mtlMemory.h"
#include "../MTL/mtlBits.h"

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

mglImage::mglImage(int p_width, int p_height, mglColor32 color) : m_pixels(NULL), m_width(0), m_height(0)
{
	m_order.index.r = 0;
	m_order.index.g = 1;
	m_order.index.b = 2;
	m_order.index.a = 3;
	Create(p_width, p_height, color);
}

mglImage::mglImage(int p_width, int p_height, mglByteOrder32 p_order, mglColor32 color) : m_pixels(NULL), m_width(0), m_height(0)
{
	Create(p_width, p_height, p_order, color);
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

void mglImage::Create(int p_width, int p_height, mglColor32 color)
{
	Create(p_width, p_height);
	const int area = GetArea();
	mglPixel32 pixel = mglRGBA(color.r, color.g, color.b, color.a, m_order);
	for (int i = 0; i < area; ++i) {
		m_pixels[i] = pixel;
	}
}

void mglImage::Create(int p_width, int p_height, mglByteOrder32 p_order, mglColor32 color)
{
	m_order = p_order;
	Create(p_width, p_height, color);
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

mglColor32 mglImage::GetColorXY(int x, int y) const
{
	const mglPixel32 *pixel = GetPixelXY(x, y);
	mglColor32 color = {
		pixel->bytes[m_order.index.r],
		pixel->bytes[m_order.index.g],
		pixel->bytes[m_order.index.b],
		pixel->bytes[m_order.index.a]
	};
	return color;
}

void mglImage::DrawPixel(int x, int y, mglColor32 color)
{
	if (x > -1 && x < m_width && y > -1 && y < m_height) {
		mglPixel32 *pixel = GetPixelXY(x, y);
		pixel->bytes[m_order.index.r] = color.r;
		pixel->bytes[m_order.index.g] = color.g;
		pixel->bytes[m_order.index.b] = color.b;
		pixel->bytes[m_order.index.a] = color.a;
	}
}

void mglImage::DrawLine(int x1, int y1, int x2, int y2, mglColor32 color)
{
	if (x1 == x2 && y1 == y2) {
		DrawPixel(x1, y1, color);
		return;
	}
	mmlVector<2> p1 = mmlVector<2>((float)x1, (float)y1);
	mmlVector<2> p2 = mmlVector<2>((float)x2, (float)y2);
	mglRay2D ray;
	ray.origin = p1;
	const float len = mmlDist(p1, p2);
	ray.direction = (p2 - p1) / len;

	mglDifferentialAnalyzer2D dda;
	dda.SetInitialState(ray);

	float dist = dda.GetImpactDistance();
	while (dist < len) {

		int x = dda.GetX();
		int y = dda.GetY();

		DrawPixel(x, y, color);

		dda.Step();
		dist = dda.GetImpactDistance();
	}
}

void mglImage::DrawCircle(int cx, int cy, int radius, mglColor32 color)
{
	float error = (float)-radius;
	float x0 = (float)radius - 0.5f;
	float y0 = 0.5f;
	float cxf = (float)cx - 0.5f;
	float cyf = (float)cy - 0.5f;

	while (x0 >= y0) {
		DrawPixel((int)(cxf + x0), (int)(cyf + y0), color);
		DrawPixel((int)(cxf + y0), (int)(cyf + x0), color);

		if (x0 != 0.0f) {
			DrawPixel((int)(cxf - x0), (int)(cyf + y0), color);
			DrawPixel((int)(cxf + y0), (int)(cyf - x0), color);
		}

		if (y0 != 0.0f) {
			DrawPixel((int)(cxf + x0), (int)(cyf - y0), color);
			DrawPixel((int)(cxf - y0), (int)(cyf + x0), color);
		}

		if (x0 != 0.0f && y0 != 0.0f) {
			DrawPixel((int)(cxf - x0), (int)(cyf - y0), color);
			DrawPixel((int)(cxf - y0), (int)(cyf - x0), color);
		}

		error += y0;
		++y0;
		error += y0;

		if (error >= 0.0f) {
			--x0;
			error -= x0;
			error -= x0;
		}
	}
}

void mglImage::DrawCircleQuad(int cx, int cy, int radius, unsigned int quad_mask, mglColor32 color)
{
	quad_mask = quad_mask & 15;

	float error = (float)-radius;
	float x0 = (float)radius - 0.5f;
	float y0 = 0.5f;
	float cxf = (float)cx - 0.5f;
	float cyf = (float)cy - 0.5f;

	while (x0 >= y0) {
		if (quad_mask & 8) {
			DrawPixel((int)(cxf + x0), (int)(cyf + y0), color);
			DrawPixel((int)(cxf + y0), (int)(cyf + x0), color);
		}

		if (x0 != 0.0f) {
			if (quad_mask & 4) { DrawPixel((int)(cxf - x0), (int)(cyf + y0), color); }
			if (quad_mask & 1) { DrawPixel((int)(cxf + y0), (int)(cyf - x0), color); }
		}

		if (y0 != 0.0f) {
			if (quad_mask & 1) { DrawPixel((int)(cxf + x0), (int)(cyf - y0), color); }
			if (quad_mask & 4) { DrawPixel((int)(cxf - y0), (int)(cyf + x0), color); }
		}

		if (quad_mask & 2 && x0 != 0.0f && y0 != 0.0f) {
			DrawPixel((int)(cxf - x0), (int)(cyf - y0), color);
			DrawPixel((int)(cxf - y0), (int)(cyf - x0), color);
		}

		error += y0;
		++y0;
		error += y0;

		if (error >= 0.0f) {
			--x0;
			error -= x0;
			error -= x0;
		}
	}
}

void mglImage::FillBox(int x1, int y1, int x2, int y2, mglColor32 color)
{
	if (x1 > x2) { mmlSwap(x1, x2); }
	if (y1 > y2) { mmlSwap(y1, y2); }

	x1 = mmlMax(x1, 0);
	y1 = mmlMax(y1, 0);
	x2 = mmlMin(x2, m_width);
	y2 = mmlMin(y2, m_height);

	mglPixel32  pixel_color = mglRGBA(color.r, color.g, color.b, color.a, m_order);
	mglPixel32 *pixels = GetPixelXY(x1, y1);
	const int   yn = y2 - y1;

	for (int y = 0; y < yn; ++y) {
		for (int x = x1; x < x2; ++x) {
			pixels[x] = pixel_color;
		}
		pixels += m_width;
	}
}

//void mglImage::FillCircle(int cx, int cy, int radius, mglColor32 color)
//{
//}

void mglImage::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, mglColor32 color)
{
	if (y1 > y2) {
		mmlSwap(x1, x2);
		mmlSwap(y1, y2);
	}
	if (y1 > y3) {
		mmlSwap(x1, x3);
		mmlSwap(y1, y3);
	}
	if (y2 > y3) {
		mmlSwap(x2, x3);
		mmlSwap(y2, y3);
	}

	const float D12 = (float)(x2 - x1) / (float)(y2 - y1);
	const float D13 = (float)(x3 - x1) / (float)(y3 - y1);
	const float D23 = (float)(x3 - x2) / (float)(y3 - y2);

	int sy1 = y1;
	int sy2 = y2;
	const int ey1 = mmlMin(y2, m_height);
	const int ey2 = mmlMin(y3, m_height);

	mglPixel32  pixel_color = mglRGBA(color.r, color.g, color.b, color.a, m_order);
	mglPixel32 *pixels = m_pixels;

	if (sy1 > 0) {
		pixels += m_width * y1;
	}

	if (D12 < D13){
		float start = x1;
		float end   = x3;
		if (sy1 < 0){
			start += D12 * -sy1;
			end   += D13 * -sy1; // doesn't need to be corrected any further
			sy1    = 0;
		}

		for (int y = sy1; y < ey1; ++y, pixels+=m_width, start+=D12, end+=D13){
			const int xs = mmlMax(0, (int)start);
			const int xe = mmlMin(m_width, (int)end);
			for (int x = xs; x < xe; ++x) {
				pixels[x] = pixel_color;
			}
		}

		start = x2;
		if (sy2 < 0){
			start += D23 * float(-sy2);
			sy2 = 0;
		}

		for (int y = sy2; y < ey2; ++y, pixels+=m_width, start+=D23, end+=D13){
			const int xs = mmlMax(0, (int)start);
			const int xe = mmlMin(m_width, (int)end);
			for (int x = xs; x < xe; ++x) {
				pixels[x] = pixel_color;
			}
		}
	} else {
		float start = x3;
		float end   = x1;
		if (sy1 < 0){
			start += D13 * float(-sy1); // doesn't need to be corrected any further
			end += D12 * float(-sy1);
			sy1 = 0;
		}

		for (int y = sy1; y < ey1; ++y, pixels+=m_width, start+=D13, end+=D12){
			const int xs = mmlMax(0, (int)start);
			const int xe = mmlMin(m_width, (int)end);
			for (int x = xs; x < xe; ++x) {
				pixels[x] = pixel_color;
			}
		}

		end = x2;
		if (sy2 < 0){
			end += D23 * float(-sy2);
			sy2 = 0;
		}

		for (int y = sy2; y < ey2; ++y, pixels+=m_width, start+=D13, end+=D23){
			const int xs = mmlMax(0, (int)start);
			const int xe = mmlMin(m_width, (int)end);
			for (int x = xs; x < xe; ++x) {
				pixels[x] = pixel_color;
			}
		}
	}
}

#define font_char_px_width  6
#define font_char_px_height 6
#define font_char_first    '!'
#define font_char_last     '~'

#define font_small_char_count_width   4
#define font_small_char_count_height  24

// XBM data format
#define font_small_width  24
#define font_small_height 144
static const unsigned char font_small_bits[] = {
	0xff, 0xff, 0xff, 0x7d, 0xbd, 0x06, 0x7d, 0x1d, 0xa4, 0xfd, 0xbf, 0x06,
	0xff, 0x1f, 0x2c, 0xfd, 0xbf, 0x06, 0xff, 0xff, 0xff, 0x77, 0xdc, 0xef,
	0xff, 0xde, 0xf7, 0x41, 0xf5, 0xf7, 0x7f, 0xfb, 0xf7, 0x77, 0xf4, 0xef,
	0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xfb, 0xbe, 0xff, 0x7b, 0x1d, 0xff,
	0xfb, 0xbe, 0xef, 0xfd, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x05,
	0xff, 0xff, 0x76, 0xf1, 0x7f, 0x57, 0xff, 0xbf, 0x77, 0x7f, 0xdf, 0x07,
	0xff, 0xff, 0xff, 0x77, 0x10, 0x74, 0xf3, 0xf7, 0x75, 0x77, 0x30, 0x04,
	0x77, 0xff, 0x7d, 0x41, 0x10, 0x7c, 0xff, 0xff, 0xff, 0x41, 0x10, 0x04,
	0x7d, 0xff, 0x75, 0x41, 0xf0, 0x04, 0x5f, 0xf7, 0x75, 0x41, 0xf0, 0x05,
	0xff, 0xff, 0xff, 0xc1, 0xff, 0xdf, 0x5d, 0xbf, 0xef, 0xc1, 0xff, 0xf7,
	0x5f, 0xbf, 0xef, 0xdf, 0xdf, 0xdf, 0xff, 0xff, 0xff, 0x7f, 0x1f, 0x04,
	0xf1, 0xfe, 0x75, 0xff, 0x1d, 0x34, 0xf1, 0xfe, 0x57, 0x7f, 0xdf, 0x17,
	0xff, 0xff, 0xff, 0x41, 0x38, 0x84, 0x5d, 0xdb, 0x77, 0x41, 0xd0, 0x77,
	0x5d, 0xd7, 0x77, 0x5d, 0x10, 0x04, 0xff, 0xff, 0xff, 0x41, 0x10, 0x74,
	0x7d, 0xdf, 0x77, 0x61, 0x58, 0x04, 0x7d, 0xdf, 0x75, 0x41, 0x1f, 0x74,
	0xff, 0xff, 0xff, 0x41, 0xd0, 0xf5, 0xf7, 0xd7, 0xf5, 0xf7, 0x17, 0xf6,
	0x77, 0xd7, 0xf5, 0x41, 0xd0, 0x05, 0xff, 0xff, 0xff, 0x49, 0x17, 0x04,
	0x55, 0xd6, 0x75, 0x5d, 0xd5, 0x05, 0x5d, 0xd3, 0xf5, 0x5d, 0x17, 0xf4,
	0xff, 0xff, 0xff, 0x41, 0x30, 0x04, 0x5d, 0xd7, 0xdf, 0x5d, 0x10, 0xdc,
	0x4d, 0xfb, 0xdd, 0x41, 0x17, 0xde, 0xff, 0xff, 0xff, 0x5d, 0xd7, 0x75,
	0x5d, 0xd7, 0x75, 0x5d, 0xd7, 0x8d, 0xdd, 0x5a, 0x75, 0xc1, 0x9d, 0x74,
	0xff, 0xff, 0xff, 0x5d, 0x90, 0xf7, 0xdd, 0xd7, 0xef, 0xeb, 0xd8, 0xdf,
	0x77, 0xdf, 0xbf, 0x77, 0x90, 0x7f, 0xff, 0xff, 0xff, 0xf9, 0xfe, 0xf7,
	0x7b, 0xfd, 0xef, 0xfb, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xf9, 0x1f, 0xfc,
	0xff, 0xff, 0xff, 0x41, 0x38, 0x84, 0x5d, 0xdb, 0x77, 0x41, 0xd0, 0x77,
	0x5d, 0xd7, 0x77, 0x5d, 0x10, 0x04, 0xff, 0xff, 0xff, 0x41, 0x10, 0x74,
	0x7d, 0xdf, 0x77, 0x61, 0x58, 0x04, 0x7d, 0xdf, 0x75, 0x41, 0x1f, 0x74,
	0xff, 0xff, 0xff, 0x41, 0xd0, 0xf5, 0xf7, 0xd7, 0xf5, 0xf7, 0x17, 0xf6,
	0x77, 0xd7, 0xf5, 0x41, 0xd0, 0x05, 0xff, 0xff, 0xff, 0x49, 0x17, 0x04,
	0x55, 0xd6, 0x75, 0x5d, 0xd5, 0x05, 0x5d, 0xd3, 0xf5, 0x5d, 0x17, 0xf4,
	0xff, 0xff, 0xff, 0x41, 0x30, 0x04, 0x5d, 0xd7, 0xdf, 0x5d, 0x10, 0xdc,
	0x4d, 0xfb, 0xdd, 0x41, 0x17, 0xde, 0xff, 0xff, 0xff, 0x5d, 0xd7, 0x75,
	0x5d, 0xd7, 0x75, 0x5d, 0xd7, 0x8d, 0xdd, 0x5a, 0x75, 0xc1, 0x9d, 0x74,
	0xff, 0xff, 0xff, 0x5d, 0x30, 0xf7, 0xdd, 0xb7, 0xf7, 0xeb, 0xd8, 0xf7,
	0x77, 0xbf, 0xf7, 0x77, 0x30, 0xf7, 0xff, 0xff, 0xff, 0xf9, 0x1a, 0x04,
	0x7b, 0x1d, 0x04, 0xf7, 0x1f, 0x04, 0xfb, 0x1f, 0x04, 0xf9, 0x1f, 0x04
};

mtlByte mglExtractStencilBit(const mtlByte *stencil_bits, int num_bits_width, int x, int y)
{
	mtlByte bit = mtlReadBit<mtlByte>(stencil_bits, x + y * num_bits_width);
	return bit != 0 ? 0x0 : 0xff;
}

void mglDrawChar(char ch, const mtlByte *stencil_bits, int font_width, int char_count_width, int char_width, int char_height, mtlByte *dst, int dst_bpp, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale)
{
	typedef mml::fixed<unsigned int,16> fixed_t;

	unsigned char *dst0 = dst;

	int char_width0  = char_width;
	int char_height0 = char_height;
	char_width  *= scale;
	char_height *= scale;

	if (ch < font_char_first || ch > font_char_last) {
		ch = font_char_last + 1;
	}

	int ch_index = ch - font_char_first;
	int ch_x = (ch_index % char_count_width) * char_width0;
	int ch_y = (ch_index / char_count_width) * char_height0;

	int start_i = x < 0 ? -x : 0;
	int clip_x  = x < 0 ?  0 : x;
	int start_j = y < 0 ? -y : 0;
	int clip_y  = y < 0 ?  0 : y;
	int end_i   = (x + char_width)  >= dst_w ? dst_w - x : char_width;
	int end_j   = (y + char_height) >= dst_h ? dst_h - y : char_height;

	// Rendering and scaling using a shift per pixel
	int size_i  = end_i - start_i;
	int size_j  = end_j - start_j;

	if (size_i <= 1 || size_j <= 1) { // watch out for division by zero
		return;
	}

	fixed_t iscale   = fixed_t(scale);
	fixed_t ix       = fixed_t(start_i) / iscale;
	fixed_t iy       = fixed_t(start_j) / iscale;
	fixed_t ix_start = ix;
	fixed_t idelta_x = (fixed_t(end_i) / iscale - ix) / fixed_t(size_i - 1);
	fixed_t idelta_y = (fixed_t(end_j) / iscale - iy) / fixed_t(size_j - 1);

	for (int j = 0; j < size_j; ++j) {
		dst = dst0 + (clip_x + (clip_y + j) * dst_w) * dst_bpp;
		unsigned int bit_row = ch_y + (unsigned int)iy;
		for (int i = 0; i < size_i; ++i) {
			unsigned int bit_col = ch_x + (unsigned int)ix;
			if (mglExtractStencilBit(stencil_bits, font_width, bit_col, bit_row) != 0) {
				dst[dst_order.index.r] = r;
				dst[dst_order.index.g] = g;
				dst[dst_order.index.b] = b;
			}
			dst += dst_bpp;
			ix += idelta_x;
		}
		ix = ix_start;
		iy += idelta_y;
	}
}

void mglImage::DrawChar(char ch, int x, int y, mglColor32 color, int scale)
{
	mglDrawChar(
		ch,
		font_small_bits, font_small_width,
		font_small_char_count_width,
		font_char_px_width, font_char_px_height,
		(mtlByte*)m_pixels, 4, m_order, m_width, m_height,
		x, y,
		color.r, color.g, color.b,
		scale
	);
}

void mglImage::Blit(mglImage &dst, int dx1, int dy1, int dx2, int dy2, const mglImage &src, int sx1, int sy1, int sx2, int sy2)
{
	// clip srcRect against max borders
	sx1 = mmlMax(sx1, 0);
	sy1 = mmlMax(sy1, 0);
	sx2 = mmlMin(sx2, src.m_width);
	sy2 = mmlMin(sy2, src.m_height);

	float u1 = (float)sx1 / (float)(src.m_width - 1);
	float v1 = (float)sy1 / (float)(src.m_height - 1);
	float u2 = (float)sx2 / (float)(src.m_width - 1);
	float v2 = (float)sy2 / (float)(src.m_height - 1);
	float du = (u2 - u1) / (float)(dx2 - dx1);
	float dv = (v2 - v1) / (float)(dy2 - dy1);

	// enable a negative writable area on dst (flips blit direction)
	if (dx2 < dx1) {
		mmlSwap(dx1, dx2);
		mmlSwap(u1, u2);
		if (dx1 < 0) { // make read offset for src + clip against min borders
			u1 = u1 - du * dx1;
			dx1 = 0;
		}
	} else if (dx1 < 0) { // make read offset for src + clip against min borders
		u1 = u1 + du * -dx1;
		dx1 = 0;
	}
	if (dy2 < dy1) {
		mmlSwap(dy1, dy2);
		mmlSwap(v1, v2);
		if (dy1 < 0) { // make read offset for src + clip against min borders
			v1 = v1 - dv * dy1;
			dy1 = 0;
		}
	} else if (dy1 < 0) { // make read offset for src + clip against min borders
		v1 = v1 + dv * -dy1;
		dy1 = 0;
	}

	// clip dstRect agains max borders
	dx2 = mmlMin(dx2, dst.m_width);
	dy2 = mmlMin(dy2, dst.m_height);

	// determine writable area
	const int MAXY = dy2 - dy1;
	const int MAXX = dx2 - dx1;
	if (MAXX < 0 || MAXY < 0) { return; } // readable area is negative (probably because src is offscreen)

	mglPixel32       *dpix = dst.GetPixelXY(0, dy1);
	const mglPixel32 *spix = src.GetPixelXY(sx1, sy1);

	// draw scanlines
	float v = v1;
	for (int y = 0; y < MAXY; ++y, dpix += dst.m_width){
		float u = u1;
		for (int x = 0; x < MAXX; ++x){
//			dpix[x + dx1] = pBlend(dpix[x + dx1], pSample(src, u, v));
			dpix[x + dx1] = spix[(unsigned int)((v * src.m_height) * src.m_width + (u * src.m_width))]; // TODO: MAKE SURE THIS DOES NOT OVERFLOW!
			u += du;
		}
		v += dv;
	}
}
