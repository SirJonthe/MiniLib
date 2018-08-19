#include "mglGraphics.h"
#include "mglRay.h"

#include "../MML/mmlFixed.h"

#include "../MTL/mtlBits.h"

void mglDrawPixel(int x, int y, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order)
{
	if (x > -1 && x < width && y > -1 && y < height) {
		mtlByte *pixel = pixels + (x + y * width) * bpp;
		pixel[byte_order.index.r] = r;
		pixel[byte_order.index.g] = g;
		pixel[byte_order.index.b] = b;
	}
}

void mglDrawLine(mmlVector<2> p1, mmlVector<2> p2, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order)
{
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

		mglDrawPixel(x, y, r, g, b, pixels, bpp, width, height, byte_order);

		dda.Step();
		dist = dda.GetImpactDistance();
	}
}

void mglDrawCircle(mmlVector<2> mid, float rad, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order)
{
	float error = (float)-rad;
	float x0 = (float)rad - 0.5f;
	float y0 = 0.5f;
	float cx = (float)mid[0] - 0.5f;
	float cy = (float)mid[1] - 0.5f;

	while (x0 >= y0) {
		mglDrawPixel((int)(cx + x0), (int)(cy + y0), r, g, b, pixels, bpp, width, height, byte_order);
		mglDrawPixel((int)(cx + y0), (int)(cy + x0), r, g, b, pixels, bpp, width, height, byte_order);

		if (x0 != 0.0f) {
			mglDrawPixel((int)(cx - x0), (int)(cy + y0), r, g, b, pixels, bpp, width, height, byte_order);
			mglDrawPixel((int)(cx + y0), (int)(cy - x0), r, g, b, pixels, bpp, width, height, byte_order);
		}

		if (y0 != 0.0f) {
			mglDrawPixel((int)(cx + x0), (int)(cy - y0), r, g, b, pixels, bpp, width, height, byte_order);
			mglDrawPixel((int)(cx - y0), (int)(cy + x0), r, g, b, pixels, bpp, width, height, byte_order);
		}

		if (x0 != 0.0f && y0 != 0.0f) {
			mglDrawPixel((int)(cx - x0), (int)(cy - y0), r, g, b, pixels, bpp, width, height, byte_order);
			mglDrawPixel((int)(cx - y0), (int)(cy - x0), r, g, b, pixels, bpp, width, height, byte_order);
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

void mglDrawCircleQuadrant(mmlVector<2> mid, float rad, unsigned int quadrant_mask, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order)
{
	quadrant_mask = quadrant_mask & 15;

	float error = (float)-rad;
	float x0 = (float)rad - 0.5f;
	float y0 = 0.5f;
	float cx = (float)mid[0] - 0.5f;
	float cy = (float)mid[1] - 0.5f;

	while (x0 >= y0) {
		if (quadrant_mask & 8) {
			mglDrawPixel((int)(cx + x0), (int)(cy + y0), r, g, b, pixels, bpp, width, height, byte_order);
			mglDrawPixel((int)(cx + y0), (int)(cy + x0), r, g, b, pixels, bpp, width, height, byte_order);
		}

		if (x0 != 0.0f) {
			if (quadrant_mask & 4) mglDrawPixel((int)(cx - x0), (int)(cy + y0), r, g, b, pixels, bpp, width, height, byte_order);
			if (quadrant_mask & 1) mglDrawPixel((int)(cx + y0), (int)(cy - x0), r, g, b, pixels, bpp, width, height, byte_order);
		}

		if (y0 != 0.0f) {
			if (quadrant_mask & 1) mglDrawPixel((int)(cx + x0), (int)(cy - y0), r, g, b, pixels, bpp, width, height, byte_order);
			if (quadrant_mask & 4) mglDrawPixel((int)(cx - y0), (int)(cy + x0), r, g, b, pixels, bpp, width, height, byte_order);
		}

		if (quadrant_mask & 2 && x0 != 0.0f && y0 != 0.0f) {
			mglDrawPixel((int)(cx - x0), (int)(cy - y0), r, g, b, pixels, bpp, width, height, byte_order);
			mglDrawPixel((int)(cx - y0), (int)(cy - x0), r, g, b, pixels, bpp, width, height, byte_order);
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

void mglFillBox(mmlVector<2> p1, mmlVector<2> p2, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order)
{
	if (p1[0] > p2[1]) { mmlSwap(p1[0], p2[0]); }
	if (p1[1] > p2[1]) { mmlSwap(p1[1], p2[1]); }
	const int x0 = mmlMax((int)p1[0], 0) * bpp;
	const int y0 = mmlMax((int)p1[1], 0);
	const int x1 = mmlMin((int)p2[0], width) * bpp;
	const int y1 = mmlMin((int)p2[1], height);
	const int yn = y1 - y0;
	const int stride = width * bpp;
	pixels += y0 * stride;
	for (int y = 0; y < yn; ++y) {
		for (int x = x0; x < x1; x += bpp) {
			pixels[x + byte_order.index.r] = r;
			pixels[x + byte_order.index.g] = g;
			pixels[x + byte_order.index.b] = b;
		}
		pixels += stride;
	}
}

void mglFillTriangle(mmlVector<2> p1, mmlVector<2> p2, mmlVector<2> p3, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order)
{
	mmlVector<2> *va = &p1, *vb = &p2, *vc = &p3;
	if ((*va)[1] > (*vb)[1]) { mmlSwap(va, vb); }
	if ((*va)[1] > (*vc)[1]) { mmlSwap(va, vc); }
	if ((*vb)[1] > (*vc)[1]) { mmlSwap(vb, vc); }

	const float D12 = ((*vb)[0] - (*va)[0]) / ((*vb)[1] - (*va)[1]);
	const float D13 = ((*vc)[0] - (*va)[0]) / ((*vc)[1] - (*va)[1]);
	const float D23 = ((*vc)[0] - (*vb)[0]) / ((*vc)[1] - (*vb)[1]);

	int sy1 = (int)ceil((*va)[1]);
	int sy2 = (int)ceil((*vb)[1]);
	const int ey1 = mmlMin((int)ceil((*vb)[1]), height);
	const int ey2 = mmlMin((int)ceil((*vc)[1]), height);

	const float DIFF1 = ceil((*va)[1]) - (*va)[1];
	const float DIFF2 = ceil((*vb)[1]) - (*vb)[1];
	const float DIFF3 = ceil((*va)[1]) - (*vc)[1];

	const int stride = width * bpp;

	if (sy1 > 0) {
		pixels += stride * sy1;
	}

	if (D12 < D13){
		float start = (D12 * DIFF1) + (*va)[0];
		float end = (D13 * DIFF3) + (*vc)[0];
		if (sy1 < 0){
			start += D12 * float(-sy1);
			end += D13 * float(-sy1); // doesn't need to be corrected any further
			sy1 = 0;
		}

		for (int y = sy1; y < ey1; ++y, pixels+=stride, start+=D12, end+=D13){
			const int x0 = mmlMax(0, (int)start) * bpp;
			const int x1 = mmlMin(width, (int)end) * bpp;
			for (int x = x0; x < x1; x+=bpp) {
				pixels[x + byte_order.index.r] = r;
				pixels[x + byte_order.index.g] = g;
				pixels[x + byte_order.index.b] = b;
			}
		}

		start = (D23 * DIFF2) + (*vb)[0];
		if (sy2 < 0){
			start += D23 * float(-sy2);
			sy2 = 0;
		}

		for (int y = sy2; y < ey2; ++y, pixels+=stride, start+=D23, end+=D13){
			const int x0 = mmlMax(0, (int)start) * bpp;
			const int x1 = mmlMin(width, (int)end) * bpp;
			for (int x = x0; x < x1; x+=bpp) {
				pixels[x + byte_order.index.r] = r;
				pixels[x + byte_order.index.g] = g;
				pixels[x + byte_order.index.b] = b;
			}
		}
	} else {
		float start = (D13 * DIFF3) + (*vc)[0];
		float end = (D12 * DIFF1) + (*va)[0];
		if (sy1 < 0){
			start += D13 * float(-sy1); // doesn't need to be corrected any further
			end += D12 * float(-sy1);
			sy1 = 0;
		}

		for (int y = sy1; y < ey1; ++y, pixels+=stride, start+=D13, end+=D12){
			const int x0 = mmlMax(0, (int)start) * bpp;
			const int x1 = mmlMin(width, (int)end) * bpp;
			for (int x = x0; x < x1; x+=bpp) {
				pixels[x + byte_order.index.r] = r;
				pixels[x + byte_order.index.g] = g;
				pixels[x + byte_order.index.b] = b;
			}
		}

		end = (D23 * DIFF2) + (*vb)[0];
		if (sy2 < 0){
			end += D23 * float(-sy2);
			sy2 = 0;
		}

		for (int y = sy2; y < ey2; ++y, pixels+=stride, start+=D13, end+=D23){
			const int x0 = mmlMax(0, (int)start) * bpp;
			const int x1 = mmlMin(width, (int)end) * bpp;
			for (int x = x0; x < x1; x+=bpp) {
				pixels[x + byte_order.index.r] = r;
				pixels[x + byte_order.index.g] = g;
				pixels[x + byte_order.index.b] = b;
			}
		}
	}
}

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
	// Don't use CHAR_BITS:
	// Each byte in the XBM format has 8 bits of data regardless of if the byte is larger than 8 bits
	//stencil_bits += y * (num_bits_width >> 3);
	//stencil_bits += x >> 3;
	//unsigned char bit = mtlReadBit(*stencil_bits, x & 7);
	mtlByte bit = mtlReadBit<mtlByte>(stencil_bits, x + y * num_bits_width);
	return bit != 0 ? 0x0 : 0xff;
}

void mglDrawChar(char ch, const mtlByte *stencil_bits, int font_width, int char_count_width, int char_width, int char_height, mtlByte *dst, int dst_bpp, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale)
{
	typedef mml::fixed<unsigned int,16> fixed;

	unsigned char *dst0 = dst;

	int char_width0  = char_width;
	int char_height0 = char_height;
	char_width  *= scale;
	char_height *= scale;

	if (ch < mglFont_FirstChar || ch > mglFont_LastChar) {
		ch = mglFont_LastChar + 1;
	}

	int ch_index = ch - mglFont_FirstChar;
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

	fixed iscale   = fixed(scale);
	fixed ix       = fixed(start_i) / iscale;
	fixed iy       = fixed(start_j) / iscale;
	fixed ix_start = ix;
	fixed idelta_x = (fixed(end_i) / iscale - ix) / fixed(size_i - 1);
	fixed idelta_y = (fixed(end_j) / iscale - iy) / fixed(size_j - 1);

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

void mglDrawChar(char ch, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale)
{
	mglDrawChar(
		ch,
		font_small_bits, font_small_width,
		font_small_char_count_width,
		mglFont_CharWidthPx, mglFont_CharHeightPx,
		dst, dst_bytes_per_pixel, dst_order, dst_w, dst_h,
		x, y,
		r, g, b,
		scale
	);
}
