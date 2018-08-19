#ifndef MGL_GRAPHICS_H_INCLUDED
#define MGL_GRAPHICS_H_INCLUDED

#include "mglImage.h"
#include "../MML/mmlVector.h"
#include "../MGL/mglPixel.h"

void mglDrawPixel(int x, int y, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglDrawLine(int x1, int y1, int x2, int y2, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglDrawCircle(int cx, int cy, float rad, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglDrawCircleQuadrant(int cx, int cy, int rad, unsigned int quadrant_mask, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());

void mglFillBox(int x1, int y1, int x2, int y2, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglFillCircle(int cx, int cy, int rad, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());
void mglFillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, mtlByte r, mtlByte g, mtlByte b, mtlByte *pixels, int bpp, int width, int height, mglByteOrder32 byte_order = mglVideoByteOrder());

#define mglFont_CharWidthPx  6
#define mglFont_CharHeightPx 6
#define mglFont_FirstChar    '!'
#define mglFont_LastChar     '~'

void mglDrawChar(char ch, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);

#endif // MGL_GRAPHICS_H_INCLUDED
