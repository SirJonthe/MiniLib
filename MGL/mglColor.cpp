#include "mglColor.h"
#include "../MML/mmlMath.h"

mglColorYUV::mglColorYUV(const mglColorRGB &rgb) :
	y(( 0.299f * rgb.r) + (0.587f * rgb.g) + (0.114f * rgb.b)),
	u(( 0.500f * rgb.r) - (0.419f * rgb.g) - (0.081f * rgb.b)),
	v((-0.169f * rgb.r) - (0.331f * rgb.g) + (0.500f * rgb.b))
{}

mglColorXYZ::mglColorXYZ(const mglColorRGB &rgb) :
	x(0.412453f * rgb.r + 0.357580f * rgb.g + 0.180423f * rgb.b),
	y(0.212671f * rgb.r + 0.715160f * rgb.g + 0.072169f * rgb.b),
	z(0.019334f * rgb.r + 0.119193f * rgb.g + 0.950227f * rgb.b)
{}

mglColorCMYK::mglColorCMYK(const mglColorRGB &rgb)
{
	if (rgb.r == 0.0f && rgb.g == 0.0f && rgb.b == 0.0f) {
		c = 0.0f;
		m = 0.0f;
		y = 0.0f;
		k = 1.0f;
	} else {
		c = 1.0f - rgb.r;
		m = 1.0f - rgb.g;
		y = 1.0f - rgb.b;

		k = mmlMin3(c, m, y);
		c = (c - k) / (1.0f - k);
		m = (m - k) / (1.0f - k);
		y = (y - k) / (1.0f - k);
	}
}

mglColorHSL::mglColorHSL(const mglColorRGB &rgb)
{
	float max = mmlMax3(rgb.r, rgb.g, rgb.b);
	float min = mmlMin3(rgb.r, rgb.g, rgb.b);
	l = (max + min) / 2.0f;
	if (l > 0.0f) {
		float diff = max - min;
		s = (l < 0.5f) ? diff / (max + min) : diff / (2.0f - diff);
		if (max == rgb.r) {
			h = (rgb.g - rgb.b) / diff + (rgb.g < rgb.b ? 6.0f : 0.0f);
		} else if (max == rgb.g) {
			h = (rgb.b - rgb.r) / diff + 2.0f;
		} else {
			h = (rgb.r - rgb.g) / diff + 4.0f;
		}
		h /= 6.0f;
	} else {
		s = 0.0f;
		h = 0.0f;
	}
}

mglColorHSVC::mglColorHSVC(const mglColorRGB &rgb)
{
	float max = mmlMax3(rgb.r, rgb.g, rgb.b);
	float min = mmlMin3(rgb.r, rgb.g, rgb.b);
	v = max;
	c = max - min;
	s = max == 0.0f ? 0.0f : c / max;
	if (max == min) {
		h = 0.0f;
	} else {
		if (max == rgb.r) {
			h = (rgb.g - rgb.b) / c + (rgb.g < rgb.b ? 6.0f : 0.0f);
		} else if (max == rgb.g) {
			h = (rgb.b - rgb.r) / c + 2.0f;
		} else {
			h = (rgb.r - rgb.g) / c + 4.0f;
		}
		h /= 6.0f;
	}
}
