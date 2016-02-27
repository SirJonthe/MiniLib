#include "mglGraphics.h"
#include "mglRay.h"

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

/*void mglLine::Draw(mglColor32 color, mglPixelSurface surface) const
{
	mglRay2D ray;
	ray.origin = pa;
	const float len = mmlDist(pa, pb);
	ray.direction = (pb - pa) / len;

	mglDifferentialAnalyzer2D dda;
	dda.SetInitialState(ray);

	float dist = dda.GetImpactDistance();
	while (dist < len) {

		int x = dda.GetX();
		int y = dda.GetY();

		if (x > -1 && x < surface.width && y > -1 && y < surface.height) {
			mtlByte *pixel = surface.pixels + (x + y * surface.width) * surface.bytes_per_pixel;
			pixel[surface.byte_order.index.r] = color.r;
			pixel[surface.byte_order.index.g] = color.g;
			pixel[surface.byte_order.index.b] = color.b;
		}

		dda.Step();
		dist = dda.GetImpactDistance();
	}
}

void mglLine::Fill(mglColor32 color, mglPixelSurface surface) const
{
	Draw(color, surface);
}

mglBox mglLine::GetBoundingBox( void ) const
{
	return mglBox(a, b);
}

mglCircle mglLine::GetBoundingCircle( void ) const
{

	return mglCircle();
}

void mglLine::IsInside(mmlVector<2>) const
{
	return false;
}

void mglBox::Draw(mglColor32 color, mglPixelSurface surface) const
{

}

void mglBox::Fill(mglColor32 color, mglPixelSurface surface) const
{
	mglBox draw_area = mglBox(mmlVector<2>(0.0f, 0.0f), mmlVector<2>(float(surface.width), float(surface.height)));
	draw_area = draw_area.Clipped(*this);
	int x0 = (int)draw_area.a[0] * surface.bytes_per_pixel;
	int y0 = (int)draw_area.a[1];
	int x1 = (int)draw_area.b[0] * surface.bytes_per_pixel;
	int y1 = (int)draw_area.b[1];
	int yn = y1 - y0;
	const int stride = surface.width * surface.bytes_per_pixel;
	mtlByte *pixels = surface.pixels + stride * y0 + x0 * surface.bytes_per_pixel;
	for (int y = 0; y < yn; ++y) {
		for (int x = x0; x < x1; x+=surface.bytes_per_pixel) {
			pixels[x+surface.byte_order.index.r] = color.r;
			pixels[x+surface.byte_order.index.g] = color.g;
			pixels[x+surface.byte_order.index.b] = color.b;
		}
	}
}

mglBox mglBox::Clipped(mglBox box)
{
	mglBox out;
	out.a = mmlMax2(a, box.a);
	out.b = mmlMin2(b, box.b);
	return out;
}*/
