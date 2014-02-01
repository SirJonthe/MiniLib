#ifndef MGL_COLOR_H_INCLUDED__
#define MGL_COLOR_H_INCLUDED__

#include "../MML/mmlMath.h"

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
	
	void Clamp( void )
	{
		r = mmlClamp(0.f, r, 1.f);
		g = mmlClamp(0.f, g, 1.f);
		b = mmlClamp(0.f, b, 1.f);
	}
};

mglColor operator+(mglColor p_lhs, const mglColor &p_rhs) { return p_lhs+=p_rhs; }
mglColor operator-(mglColor p_lhs, const mglColor &p_rhs) { return p_lhs-=p_rhs; }
mglColor operator*(mglColor p_lhs, const mglColor &p_rhs) { return p_lhs*=p_rhs; }

#endif
