#ifndef MGL_TEXT_H_INCLUDED__
#define MGL_TEXT_H_INCLUDED__

#include "../MTL/mtlBits.h"
#include "../MTL/mtlString.h"
#include "mglImage.h"

void mglTextBig(const mtlChars &text, mglImage &dst, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);
void mglTextSmall(const mtlChars &text, mglImage &dst, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale = 1);

#endif // MGL_TEXT_H_INCLUDED__
