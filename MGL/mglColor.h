#ifndef MGL_COLOR_H_INCLUDED
#define MGL_COLOR_H_INCLUDED

// Good resource:
// https://github.com/alikins/sortpal.js/blob/master/sortpal.js

#include "../MTL/mtlBits.h"

struct mglColor32
{
	mtlByte r, g, b, a;
};

#define mglColorName(color_space) mglColor##color_space
#define mglColorDef3(color_space, x, y, z) \
	float x, y, z; \
	mglColorName(color_space)( void ) : x(0.0f), y(0.0f), z(0.0f) {} \
	mglColorName(color_space)(float _##x, float _##y, float _##z) : x(_##x), y(_##y), z(_##z) {} \
	mglColorName(color_space)(const mglColorName(color_space) &x##y##z) : x(x##y##z.x), y(x##y##z.y), z(x##y##z.z) {}
#define mglColorDef4(color_space, x, y, z, w) \
	float x, y, z, w; \
	mglColorName(color_space)( void ) : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {} \
	mglColorName(color_space)(float _##x, float _##y, float _##z, float _##w) : x(_##x), y(_##y), z(_##z), w(_##w) {} \
	mglColorName(color_space)(const mglColorName(color_space) &x##y##z##w) : x(x##y##z##w.x), y(x##y##z##w.y), z(x##y##z##w.z), w(x##y##z##w.w) {}

class mglColorRGB
{
public:
	mglColorDef3(RGB, r, g, b)
};

class mglColorYUV
{
public:
	mglColorDef3(YUV, y, u, v)
	explicit mglColorYUV(const mglColorRGB &rgb);
};

class mglColorXYZ
{
public:
	mglColorDef3(XYZ, x, y, z)
	explicit mglColorXYZ(const mglColorRGB &rgb);
};

class mglColorCMYK
{
public:
	mglColorDef4(CMYK, c, m, y, k)
	explicit mglColorCMYK(const mglColorRGB &rgb);
};

class mglColorHSL
{
public:
	mglColorDef3(HSL, h, s, l)
	explicit mglColorHSL(const mglColorRGB &rgb);
};

class mglColorHSVC
{
public:
	mglColorDef4(HSVC, h, s, v, c)
	explicit mglColorHSVC(const mglColorRGB &rgb);
};

#undef mglColorName
#undef mglColorDef3
#undef mglColorDef4

#endif // MGL_COLOR_H_INCLUDED
