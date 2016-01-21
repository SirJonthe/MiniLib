#ifndef MGL_TEXT_H_INCLUDED__
#define MGL_TEXT_H_INCLUDED__

#include "../MTL/mtlBits.h"
#include "../MTL/mtlString.h"
#include "mglImage.h"

#define mglFontBig_CharWidthPx    10
#define mglFontBig_CharHeightPx   16
#define mglFontSmall_CharWidthPx  6
#define mglFontSmall_CharHeightPx 6
#define mglFont_FirstChar         '!'
#define mglFont_LastChar          '~'

void mglDrawCharBig(char ch, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);
void mglDrawCharSmall(char ch, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);

/*void mglTextBig(const mtlChars &text, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);
void mglTextBig(char ch, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);
void mglTextBig(int num, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);
void mglTextBig(float num, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);

void mglTextSmall(const mtlChars &text, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);
void mglTextSmall(char ch, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);
void mglTextSmall(int num, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);
void mglTextSmall(float num, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);*/

#endif // MGL_TEXT_H_INCLUDED__
