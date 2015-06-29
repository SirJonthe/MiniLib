#ifndef MGL_COLOR_H_INCLUDED__
#define MGL_COLOR_H_INCLUDED__

#include "../MML/mmlMath.h"

/*union mglColor32
{
#if defined(__linux__) || defined(__linux) || defined(linux) || defiend(__gnu_linux) || defined(__gnu_linux__)
	struct { unsigned char b, g, r, a; } rgba;
#else
	struct { unsigned char b, g, r, a; } rgba;
#endif
	// __APPLE__
	// __WIN32__ / __WIN32 / _WIN32 / WIN32
	unsigned char bytes[4];
	unsigned int color;
};*/

class mglColor
{
public:
	float r, g, b;
public:
	mglColor( void ) { /* do nothing */ }
	mglColor(float R, float G, float B, bool clamp = false)
	{
		SetRGB(R, G, B);
		if (clamp) { Clamp(); }
	}
	mglColor &operator+=(const mglColor &p_rhs)
	{
		r += p_rhs.r;
		g += p_rhs.g;
		b += p_rhs.b;
		return *this;
	}
	mglColor &operator-=(const mglColor &p_rhs)
	{
		r -= p_rhs.r;
		g -= p_rhs.g;
		b -= p_rhs.b;
		return *this;
	}
	mglColor &operator*=(const mglColor &p_rhs)
	{
		r *= p_rhs.r;
		g *= p_rhs.g;
		b *= p_rhs.b;
		return *this;
	}
	
	void Set(float R, float G, float B)
	{
		r = R;
		g = G;
		b = B;
	}
	
	mglColor &Clamp( void )
	{
		r = mmlClamp(0.f, r, 1.f);
		g = mmlClamp(0.f, g, 1.f);
		b = mmlClamp(0.f, b, 1.f);
		return *this;
	}
};

mglColor operator+(mglColor p_lhs, const mglColor &p_rhs) { return p_lhs+=p_rhs; }
mglColor operator-(mglColor p_lhs, const mglColor &p_rhs) { return p_lhs-=p_rhs; }
mglColor operator*(mglColor p_lhs, const mglColor &p_rhs) { return p_lhs*=p_rhs; }

#endif
