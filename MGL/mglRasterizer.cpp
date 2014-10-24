/*
 *  mglRasterizer.cpp
 *  MiniLib
 *
 *  Created by Jonathan Karlsson on 10/27/13.
 *  Copyright 2013 Jonathan Karlsson. All rights reserved.
 *
 */

#include "../MML/mmlMath.h"
#include "../MML/mmlFixed.h"
#include "mglRasterizer.h"

const mglTexture mglTexturedRasterizer::defaultMaterial(16, 0xFF00FFFF);

/*template < int n >
class iVec
{
private:
	mmlFixed e[n];
public:
	iVec( void ) {}
	explicit iVec(const mmlVector<n> &p_vec) { for (int i = 0; i < n; ++i) { e[i] = mmlFixed(p_vec[i]); } }
	iVec<n> &operator+=(const iVec<n> &r)
	{
		for (int i = 0; i < n; ++i) { e[i] += r.e[i]; }
		return *this;
	}
	iVec<n> &operator-=(const iVec<n> &r)
	{
		for (int i = 0; i < n; ++i) { e[i] -= r.e[i]; }
		return *this;
	}
	iVec<n> &operator*=(mmlFixed s)
	{
		for (int i = 0; i < n; ++i) { e[i] *= s; }
		return *this;
	}
	iVec<n> operator+(const iVec<n> &r) const
	{
		iVec<n> l = *this;
		return l+=r;
	}
	iVec<n> operator-(const iVec<n> &r) const
	{
		iVec<n> l = *this;
		return l-=r;
	}
	iVec<n> operator*(mmlFixed r) const
	{
		iVec<n> l = *this;
		return l*=r;
	}
	mmlFixed &operator[](int i) { return e[i]; }
	const mmlFixed &operator[](int i) const { return e[i]; }
};*/

/*void mglTexturedRasterizer::RenderScanlineAffine(int x1, int x2, int, float, float, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels)
{
	const int width = framebuffer->GetWidth();
	if (x1 >= width || x2 < 0 || x1 == x2) { return; }
	const mmlVector<2> dt = (t2 - t1) * (1.0f / (x2 - x1));
	if (x1 < 0) {
		t1 += dt * float(-x1);
		x1 = 0;
	}
	if (x2 > width) { x2 = width; }

	for (int x = x1; x < x2; ++x) {
		pixels[x] = *m_currentTexture->GetPixelUV(t1[0], t1[1]);
		t1 += dt;
	}
}*/

void mglRasterizer::SetCameraTransform(const mglTransform &p_camTransform)
{
	const mmlMatrix<4,4> cameraTransformMatrix = p_camTransform.GetWorldTransformMatrix();
	m_worldToCamera = mmlInv(cameraTransformMatrix);
	m_cameraPosition = mmlVector<3>(
		cameraTransformMatrix[0][3],
		cameraTransformMatrix[1][3],
		cameraTransformMatrix[2][3]
	);
	UpdateAPIProjectionMatrix();
}

void mglRasterizer::SetProjectionMatrix(float p_vFov, float p_near, float p_far)
{
	const float scale = 1.0f / tan(mmlDegToRad(p_vFov * 0.5f));
	m_projection[0][0] = scale;	m_projection[0][1] =  0.0f;		m_projection[0][2] =  0.0f;									m_projection[0][3] = 0.0f;
	m_projection[1][0] = 0.0f;	m_projection[1][1] = -scale;	m_projection[1][2] =  0.0f;									m_projection[1][3] = 0.0f;
	m_projection[2][0] = 0.0f;	m_projection[2][1] =  0.0f;		m_projection[2][2] =  p_far / (p_far - p_near);				m_projection[2][3] = 1.0f;
	m_projection[3][0] = 0.0f;	m_projection[3][1] =  0.0f;		m_projection[3][2] = -p_far * p_near / (p_far - p_near);	m_projection[3][3] = 0.0f;

	m_infProj[0][0] = scale;	m_infProj[0][1] =  0.0f;	m_infProj[0][2] =  0.0f;	m_infProj[0][3] = 0.0f;
	m_infProj[1][0] = 0.0f;		m_infProj[1][1] = -scale;	m_infProj[1][2] =  0.0f;	m_infProj[1][3] = 0.0f;
	m_infProj[2][0] = 0.0f;		m_infProj[2][1] =  0.0f;	m_infProj[2][2] =  1.0f;	m_infProj[2][3] = 1.0f;
	m_infProj[3][0] = 0.0f;		m_infProj[3][1] =  0.0f;	m_infProj[3][2] = -p_near;	m_infProj[3][3] = 0.0f;
	
	m_nearPlane = mglPlane(mmlVector<3>(0.0f, 0.0f, p_near), mglTransform::globalAxis.forward);
	m_farPlane = mglPlane(mmlVector<3>(0.0f, 0.0f, p_far), -mglTransform::globalAxis.forward);

	m_vFov = p_vFov;
	m_nearDepth = p_near;
	m_farDepth = p_far;
}

void mglTexturedRasterizer::RenderScanlineSubAffine(int x1, int x2, int, float w1, float w2, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels)
{
	const int width = framebuffer->GetWidth();
	if (x1 >= width || x2 < 0 || x1 == x2) { return; }
	const mmlVector<2> dt = (t2 - t1) * (1.0f / (x2 - x1));
	const float dw = (w2 - w1) / (x2 - x1);
	if (x1 < 0) {
		t1 += dt * float(-x1);
		w1 += dw * float(-x1);
		x1 = 0;
	}
	if (x2 > width) { x2 = width; }

	// sub affine interpolation, perspective correction every 8 pixels per scanline
	const int affineStep = 8;
	const float fAffineStep = float(affineStep);
	const mmlVector<2> dta = dt * fAffineStep;
	const float dwa = dw * fAffineStep;
	const float dAffine = 1.0f / fAffineStep;
	mmlVector<2> affineUV1 = t1 * (1.0f / w1);
	for (int i = x1; i < x2; i+=affineStep) {
		x1 = mmlMin2(x1+affineStep, x2);
		t1 += dta;
		w1 += dwa;
		const mmlVector<2> affineUV2 = t1 * (1.0f / w1);
		const mmlVector<2> deltaUV = (affineUV2 - affineUV1) * dAffine;
		for (int x = i; x < x1; ++x) {
			pixels[x] = *m_currentTexture->GetPixelUV(affineUV1[0], affineUV1[1]);
			affineUV1 += deltaUV;
		}
		affineUV1 = affineUV2;
	}
}

void mglTexturedRasterizer::RenderScanlineCorrect(int x1, int x2, int, float w1, float w2, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels)
{
	const int width = framebuffer->GetWidth();
	if (x1 >= width || x2 < 0 || x1 == x2) { return; }
	const mmlVector<2> dt = (t2 - t1) * (1.0f / (x2 - x1));
	const float dw = (w2 - w1) / (x2 - x1);
	if (x1 < 0) {
		t1 += dt * float(-x1);
		w1 += dw * float(-x1);
		x1 = 0;
	}
	if (x2 > width) { x2 = width; }

	for (int x = x1; x < x2; ++x) {
		pixels[x] = *m_currentTexture->GetPixelUV(t1[0] / w1, t1[1] / w1);
		t1 += dt;
		w1 += dw;
	}
}

/*void mglTexturedRasterizer::RenderScanlineDitherAffine(int x1, int x2, int y, float, float, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels)
{
	static const mmlVector<2> dither[2][2] = {
		{ mmlVector<2>(0.25f, 0.00f), mmlVector<2>(0.50f, 0.75f) },
		{ mmlVector<2>(0.75f, 0.50f), mmlVector<2>(0.00f, 0.25f) }
	};
	const int width = framebuffer->GetWidth();
	if (x1 >= width || x2 < 0 || x1 == x2) { return; }
	const int ditherY = y & 1;
	const mmlVector<2> dt = (t2 - t1) * (1.0f / (x2 - x1));
	if (x1 < 0) {
		t1 += dt * float(-x1);
		x1 = 0;
	}
	if (x2 > width) { x2 = width; }
	const int dim = m_currentTexture->GetDimension();
	for (int x = x1; x < x2; ++x) {
		const mmlVector<2> ditherOffset = dither[ditherY][x&1];
		const int tx = int(t1[0] * dim + ditherOffset[0]);
		const int ty = int(t1[1] * dim + ditherOffset[1]);
		pixels[x] = *m_currentTexture->GetPixelXY(tx, ty);
		t1 += dt;
	}
}*/

void mglTexturedRasterizer::RenderScanlineDitherSubAffine(int x1, int x2, int y, float w1, float w2, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels)
{
	static const mmlVector<2> dither[2][2] = {
		{ mmlVector<2>(0.25f, 0.00f), mmlVector<2>(0.50f, 0.75f) },
		{ mmlVector<2>(0.75f, 0.50f), mmlVector<2>(0.00f, 0.25f) }
	};
	const int width = framebuffer->GetWidth();
	if (x1 >= width || x2 < 0 || x1 == x2) { return; }
	const int ditherY = y & 1;
	const mmlVector<2> dt = (t2 - t1) * (1.0f / (x2 - x1));
	const float dw = (w2 - w1) / (x2 -x1);
	if (x1 < 0) {
		t1 += dt * float(-x1);
		w1 += dw * float(-x1);
		x1 = 0;
	}
	if (x2 > width) { x2 = width; }
	const int dim = m_currentTexture->GetDimension();
	// sub affine interpolation, perspective correction every 8 pixels per scanline
	const int affineStep = 8;
	const float fAffineStep = float(affineStep);
	const mmlVector<2> dta = dt * fAffineStep;
	const float dwa = dw * fAffineStep;
	const float dAffine = 1.0f / fAffineStep;
	mmlVector<2> affineUV1 = t1 * (1.0f / w1);
	for (int i = x1; i < x2; i+=affineStep) {
		x1 = mmlMin2(x1+affineStep, x2);
		t1 += dta;
		w1 += dwa;
		const mmlVector<2> affineUV2 = t1 * (1.0f / w1);
		const mmlVector<2> deltaUV = (affineUV2 - affineUV1) * dAffine;
		for (int x = i; x < x1; ++x) {
			const mmlVector<2> ditherOffset = dither[ditherY][x&1];
			const int tx = int(affineUV1[0] * dim + ditherOffset[0]);
			const int ty = int(affineUV1[1] * dim + ditherOffset[1]);
			pixels[x] = *m_currentTexture->GetPixelXY(tx, ty);
			affineUV1 += deltaUV;
		}
		affineUV1 = affineUV2;
	}
}

void mglTexturedRasterizer::RenderScanlineDitherCorrect(int x1, int x2, int y, float w1, float w2, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels)
{
	static const mmlVector<2> dither[2][2] = {
		{ mmlVector<2>(0.25f, 0.00f), mmlVector<2>(0.50f, 0.75f) },
		{ mmlVector<2>(0.75f, 0.50f), mmlVector<2>(0.00f, 0.25f) }
	};
	const int width = framebuffer->GetWidth();
	if (x1 >= width || x2 < 0 || x1 == x2) { return; }
	const int ditherY = y & 1;
	const mmlVector<2> dt = (t2 - t1) * (1.0f / (x2 - x1));
	const float dw = (w2 - w1) / (x2 -x1);
	if (x1 < 0) {
		t1 += dt * float(-x1);
		w1 += dw * float(-x1);
		x1 = 0;
	}
	if (x2 > width) { x2 = width; }
	const int dim = m_currentTexture->GetDimension();
	for (int x = x1; x < x2; ++x) {
		const mmlVector<2> ditherOffset = dither[ditherY][x&1];
		const float w = 1.0f / w1;
		const int tx = int((t1[0] * w) * dim + ditherOffset[0]);
		const int ty = int((t1[1] * w) * dim + ditherOffset[1]);
		pixels[x] = *m_currentTexture->GetPixelXY(tx, ty);
		t1 += dt;
		w1 += dw;
	}
}

/*void mglTexturedRasterizer::RenderScanlineDitherFixed(int x1, int x2, int y, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels)
{
	static const iVec<2> dither[2][2] = {
		{ iVec<2>(mmlVector<2>(0.25f, 0.00f)), iVec<2>(mmlVector<2>(0.50f, 0.75f)) },
		{ iVec<2>(mmlVector<2>(0.75f, 0.50f)), iVec<2>(mmlVector<2>(0.00f, 0.25f)) }
	};
	if (x1 == x2) { return; } // avoid division by 0
	const int width = framebuffer->GetWidth();
	if (x1 >= width || x2 < 0 || x1 == x2) { return; }
	const int ditherY = y & 1;
	const int dim = m_currentTexture->GetDimension() - 1;
	iVec<2> tex1, tex2;
	tex1[0] = mmlFixed(t1[0] * dim);
	tex1[1] = mmlFixed(t1[1] * dim);
	tex2[0] = mmlFixed(t2[0] * dim);
	tex2[1] = mmlFixed(t2[1] * dim);
	const iVec<2> dt = (tex2 - tex1) * mmlFixed(1.0f / (x2 - x1));
	if (x1 < 0) {
		tex1 += dt * mmlFixed((short)-x1);
		x1 = 0;
	}
	if (x2 > width) { x2 = width; }
	for (int x = x1; x < x2; ++x) {
		const iVec<2> ditherOffset = dither[ditherY][x&1];
		pixels[x] = *m_currentTexture->GetPixelXY(int(tex1[0] + ditherOffset[0]) & dim, int(tex1[1] + ditherOffset[1]) & dim);
		tex1 += dt;
	}
}*/

void mglTexturedRasterizer::RenderTriangle(mmlVector<5> va, mmlVector<5> vb, mmlVector<5> vc)
{
	mmlVector<5> *a = &va, *b = &vb, *c = &vc;
	if ((*a)[1] > (*b)[1]) { mmlSwap(a, b); }
	if ((*a)[1] > (*c)[1]) { mmlSwap(a, c); }
	if ((*b)[1] > (*c)[1]) { mmlSwap(b, c); }
	
	const mmlVector<5> D12 = (*b - *a) * (1.f / ((*b)[1] - (*a)[1]));
	const mmlVector<5> D13 = (*c - *a) * (1.f / ((*c)[1] - (*a)[1]));
	const mmlVector<5> D23 = (*c - *b) * (1.f / ((*c)[1] - (*b)[1]));
	
	const int width = framebuffer->GetWidth();
	const int height = framebuffer->GetHeight();
	
	int sy1 = (int)ceil((*a)[1]);
	int sy2 = (int)ceil((*b)[1]);
	const int ey1 = mmlMin2((int)ceil((*b)[1]), height);
	const int ey2 = mmlMin2((int)ceil((*c)[1]), height);
	
	unsigned int *pixels = framebuffer->GetPixels(sy1);
	
	const float DIFF1 = ceil((*a)[1]) - (*a)[1];
	const float DIFF2 = ceil((*b)[1]) - (*b)[1];
	const float DIFF3 = ceil((*a)[1]) - (*c)[1];
	
	if (D12[0] < D13[0]){
		mmlVector<5> start = (D12 * DIFF1) + *a;
		mmlVector<5> end = (D13 * DIFF3) + *c;
		if (sy1 < 0){
			start += D12 * float(-sy1);
			end += D13 * float(-sy1); // doesn't need to be corrected any further
			pixels += width * -sy1; // doesn't need to be corrected any further
			sy1 = 0;
		}
		
		for (int y = sy1; y < ey1; ++y, pixels+=width, start+=D12, end+=D13){
			RenderScanlineSubAffine((int)start[0], (int)end[0], y, start[2], end[2], mmlVector<2>::Cast(start+3), mmlVector<2>::Cast(end+3), pixels);
		}
		
		start = (D23 * DIFF2) + *b;
		if (sy2 < 0){
			start += D23 * float(-sy2);
			sy2 = 0;
		}
		
		for (int y = sy2; y < ey2; ++y, pixels+=width, start+=D23, end+=D13){
			RenderScanlineSubAffine((int)start[0], (int)end[0], y, start[2], end[2], mmlVector<2>::Cast(start+3), mmlVector<2>::Cast(end+3), pixels);
		}
	} else {
		mmlVector<5> start = (D13 * DIFF3) + *c;
		mmlVector<5> end = (D12 * DIFF1) + *a;
		if (sy1 < 0){
			start += D13 * float(-sy1); // doesn't need to be corrected any further
			end += D12 * float(-sy1);
			pixels += width * -sy1; // doesn't need to be corrected any further
			sy1 = 0;
		}
		
		for (int y = sy1; y < ey1; ++y, pixels+=width, start+=D13, end+=D12){
			RenderScanlineSubAffine((int)start[0], (int)end[0], y, start[2], end[2], mmlVector<2>::Cast(start+3), mmlVector<2>::Cast(end+3), pixels);
		}
		
		end = (D23 * DIFF2) + *b;
		if (sy2 < 0){
			end += D23 * float(-sy2);
			sy2 = 0;
		}
		
		for (int y = sy2; y < ey2; ++y, pixels+=width, start+=D13, end+=D23){
			RenderScanlineSubAffine((int)start[0], (int)end[0], y, start[2], end[2], mmlVector<2>::Cast(start+3), mmlVector<2>::Cast(end+3), pixels);
		}
	}
}

void mglTexturedRasterizer::DrawBranch(const mglStaticModel::Node *p_node, const mmlVector<3> &p_view, const mmlMatrix<4,4> &p_finalMatrix)
{
	if (p_node != NULL) {
		mglClip clip = p_node->plane.DetermineSide(p_view);
		switch (clip) {
		case mglInFront:
			DrawBranch(p_node->back, p_view, p_finalMatrix);
			DrawTriangleList(p_node->triangles, p_finalMatrix);
			DrawBranch(p_node->front, p_view, p_finalMatrix);
			break;
		case mglBehind:
			DrawBranch(p_node->front, p_view, p_finalMatrix);
			DrawTriangleList(p_node->triangles, p_finalMatrix);
			DrawBranch(p_node->back, p_view, p_finalMatrix);
			break;
		default:
			DrawBranch(p_node->front, p_view, p_finalMatrix);
			DrawBranch(p_node->back, p_view, p_finalMatrix);
			break;
		}
	}
}

void mglTexturedRasterizer::DrawTriangleList(const mtlList< mglStaticModel::Triangle > &p_list, const mmlMatrix<4,4> &p_finalMatrix)
{
	const int centerX = framebuffer->GetWidth() >> 1;
	const int centerY = framebuffer->GetHeight() >> 1;

	mmlVector<5> tri[3];
	mmlVector<5> clipped[4];
	const mtlItem<mglStaticModel::Triangle> *node = p_list.GetFirst();
	while (node != NULL) {
		tri[0] = node->GetItem().a;
		tri[1] = node->GetItem().b;
		tri[2] = node->GetItem().c;
		mmlVector<3>::Cast(&tri[0]) *= p_finalMatrix;
		mmlVector<3>::Cast(&tri[1]) *= p_finalMatrix;
		mmlVector<3>::Cast(&tri[2]) *= p_finalMatrix;

		m_currentTexture = node->GetItem().material->GetDiffuseMap();
		if (m_currentTexture == NULL) {
			m_currentTexture = &defaultMaterial;
		}

		int numclip = 0;
		for (int i = 0; i < 3; ++i) {
			if (tri[i][2] < m_nearDepth) { ++numclip; }
		}
		if (numclip == 3) {
			node = node->GetNext();
			continue;
		}
		if (numclip == 0) {
			for (int i = 0; i < 3; ++i) {
				//mmlVector<2>::Cast(tri+i) *= (1.0f / tri[i][2]);
				const float w = 1.0f / tri[i][2];
				tri[i] *= w;
				tri[i][2] = w;
				tri[i][0] += centerX;
				tri[i][1] += centerY;
			}
			if (mmlSurfaceNormal(mmlVector<3>::Cast(tri), mmlVector<3>::Cast(tri+1), mmlVector<3>::Cast(tri+2))[2] < 0.0f) { // Am I doing this correctly? z is 1.0/z now...
				node = node->GetNext();
				continue;
			}
			RenderTriangle(tri[0], tri[1], tri[2]);
		} else {
			numclip = m_nearPlane.Clip(tri[0], tri[1], tri[2], clipped);
			for (int i = 0; i < numclip; ++i) {
				//mmlVector<2>::Cast(clipped+i) *= (1.0f / clipped[i][2]);
				const float w = 1.0f / clipped[i][2];
				clipped[i] *= w;
				clipped[i][2] = w;
				clipped[i][0] += centerX;
				clipped[i][1] += centerY;
			}
			if (mmlSurfaceNormal(mmlVector<3>::Cast(clipped), mmlVector<3>::Cast(clipped+1), mmlVector<3>::Cast(clipped+2))[2] < 0.0f) { // Am I doing this correctly? z is 1.0/z now...
				node = node->GetNext();
				continue;
			}
			switch (numclip) {
				case 4:
					RenderTriangle(clipped[2], clipped[3], clipped[0]);
				case 3:
					RenderTriangle(clipped[0], clipped[1], clipped[2]);
			}
		}

		node = node->GetNext();
	}
}

void mglTexturedRasterizer::Render(const mglModel &p_model, const mmlMatrix<4,4> &p_objToWorld)
{
	mmlVector<5> tri[3];
	mmlVector<5> clipped[4];

	//m_vertexCache.SetSize(p_model.m_vertexArray.GetSize()*3);

	const int centerX = framebuffer->GetWidth() >> 1;
	const int centerY = framebuffer->GetHeight() >> 1;
	const float FramebufferScale = (float)mmlMin2(framebuffer->GetWidth(), framebuffer->GetHeight());

	const mmlMatrix<4,4> objToCam = m_worldToCamera * p_objToWorld;
	const mmlMatrix<4,4> camToObj = mmlInv(objToCam);
	const mmlVector<3> cameraInObjectSpace = m_cameraPosition * camToObj; // is position relevant? shouldn't it be camera direction?
	const mmlMatrix<4,4> final = mmlScaleMatrix(FramebufferScale, FramebufferScale, 1.0f) * m_projection * objToCam; // scale might be applied wrong, scaling should affect movement speed

	for (int m = 0; m < p_model.GetMaterialCount(); ++m) {
		m_currentTexture = p_model.GetMaterial(m).GetProperties().GetDiffuseMap();
		if (m_currentTexture == NULL) {
			m_currentTexture = &defaultMaterial;
		}
		const mtlItem<mglFacet> *face = p_model.GetMaterial(m).GetFacets();
		while (face != NULL) {
			mmlVector<3>::Cast(tri) = p_model.GetVertex(face->GetItem().v1);
			mmlVector<3>::Cast(tri+1) = p_model.GetVertex(face->GetItem().v2);
			mmlVector<3>::Cast(tri+2) = p_model.GetVertex(face->GetItem().v3);

			// broken
			// NOTE: it's broken because the camera position isn't changing as the model rotates...
			/*const mmlVector<3> surfaceNormal = mmlSurfaceNormal(mmlVector<3>::Cast(tri), mmlVector<3>::Cast(tri+1), mmlVector<3>::Cast(tri+2));
			if (mmlDot(surfaceNormal, mmlVector<3>(0.0f, 0.0f, 0.0f)) < mmlDot(surfaceNormal, mmlVector<3>::Cast(tri))) {
				face = face->GetNext();
				continue;
			}*/

			/*mmlVector<3>::Cast(tri) = (q * mmlVector<3>::Cast(tri)) * final;
			mmlVector<3>::Cast(tri+1) = (q * mmlVector<3>::Cast(tri+1)) * final;
			mmlVector<3>::Cast(tri+2) = (q * mmlVector<3>::Cast(tri+2)) * final;*/

			mmlVector<3>::Cast(tri) *= final;
			mmlVector<3>::Cast(tri+1) *= final;
			mmlVector<3>::Cast(tri+2) *= final;

			mmlVector<2>::Cast(&tri[0][3]) = p_model.GetTexCoord(face->GetItem().t1);
			mmlVector<2>::Cast(&tri[1][3]) = p_model.GetTexCoord(face->GetItem().t2);
			mmlVector<2>::Cast(&tri[2][3]) = p_model.GetTexCoord(face->GetItem().t3);
			int numclip = 0;
			for (int i = 0; i < 3; ++i) {
				if (tri[i][2] < m_nearDepth) { ++numclip; }
			}
			if (numclip == 3) {
				face = face->GetNext();
				continue;
			}
			if (numclip == 0) {
				for (int i = 0; i < 3; ++i) {
					//mmlVector<2>::Cast(tri+i) *= (1.0f / tri[i][2]);
					const float w = 1.0f / tri[i][2];
					tri[i] *= w;
					tri[i][2] = w;
					tri[i][0] += centerX;
					tri[i][1] += centerY;
				}
				if (mmlSurfaceNormal(mmlVector<3>::Cast(tri), mmlVector<3>::Cast(tri+1), mmlVector<3>::Cast(tri+2))[2] < 0.0f) { // Am I doing this correctly? z is 1.0/z now...
					face = face->GetNext();
					continue;
				}
				RenderTriangle(tri[0], tri[1], tri[2]);
			} else {
				numclip = m_nearPlane.Clip(tri[0], tri[1], tri[2], clipped);
				for (int i = 0; i < numclip; ++i) {
					//mmlVector<2>::Cast(clipped+i) *= (1.0f / clipped[i][2]);
					const float w = 1.0f / clipped[i][2];
					clipped[i] *= w;
					clipped[i][2] = w;
					clipped[i][0] += centerX;
					clipped[i][1] += centerY;
				}
				if (mmlSurfaceNormal(mmlVector<3>::Cast(clipped), mmlVector<3>::Cast(clipped+1), mmlVector<3>::Cast(clipped+2))[2] < 0.0f) { // Am I doing this correctly? z is 1.0/z now...
					face = face->GetNext();
					continue;
				}
				switch (numclip) {
					case 4:
						RenderTriangle(clipped[2], clipped[3], clipped[0]);
					case 3:
						RenderTriangle(clipped[0], clipped[1], clipped[2]);
				}
			}
			face = face->GetNext();
		}
	}
}

void mglTexturedRasterizer::Render(const mglStaticModel &p_model, const mmlMatrix<4,4> &p_objToWorld)
{
	const float FramebufferScale = (float)mmlMin2(framebuffer->GetWidth(), framebuffer->GetHeight());
	const mmlMatrix<4,4> objToCam = m_worldToCamera * p_objToWorld;
	const mmlMatrix<4,4> camToObj = mmlInv(objToCam);
	const mmlVector<3> cameraInObjectSpace = m_cameraPosition * camToObj; // is position relevant? shouldn't it be camera direction?
	const mmlMatrix<4,4> final = mmlScaleMatrix(FramebufferScale, FramebufferScale, 1.0f) * m_projection * objToCam; // scale might be applied wrong, scaling should affect movement speed
	const mglStaticModel::Node *node = p_model.GetBinarySpaceTree();
	DrawBranch(node, cameraInObjectSpace, final);
}

void mglTexturedRasterizer::Debug_RenderTriangle(const mmlVector<4> &a, const mmlVector<4> &b, const mmlVector<4> &c, const mglTexture *texture)
{
	if (texture != NULL) {
		m_currentTexture = texture;
	} else {
		m_currentTexture = &defaultMaterial;
	}
	RenderTriangle(mmlVector<5>(a[0], a[1], 1.0f, a[2], a[3]), mmlVector<5>(b[0], b[1], 1.0f, b[2], b[3]), mmlVector<5>(c[0], c[1], 1.0f, c[2], c[3]));
}

void mglFlatRasterizer::RenderScanline(int x1, int x2, unsigned int color, unsigned int *pixels)
{
	x1 = mmlMax2(x1, 0);
	x2 = mmlMin2(x2, framebuffer->GetWidth()-1);
	for (int x = x1; x < x2; ++x) {
		pixels[x] = color;
	}
}

void mglFlatRasterizer::RenderTriangle(mmlVector<2> va, mmlVector<2> vb, mmlVector<2> vc, unsigned int color)
{
	mmlVector<2> *a = &va, *b = &vb, *c = &vc;
	if ((*a)[1] > (*b)[1]) { mmlSwap(a, b); }
	if ((*a)[1] > (*c)[1]) { mmlSwap(a, c); }
	if ((*b)[1] > (*c)[1]) { mmlSwap(b, c); }

	const float DX12 = ((*b)[0] - (*a)[0]) / ((*b)[1] - (*a)[1]);
	const float DX13 = ((*c)[0] - (*a)[0]) / ((*c)[1] - (*a)[1]);
	const float DX23 = ((*c)[0] - (*b)[0]) / ((*c)[1] - (*b)[1]);

	const int width = framebuffer->GetWidth();
	const int height = framebuffer->GetHeight();

	int sy1 = (int)ceil((*a)[1]);
	int sy2 = (int)ceil((*b)[1]);
	const int ey1 = mmlMin2((int)ceil((*b)[1]), height);
	const int ey2 = mmlMin2((int)ceil((*c)[1]), height);

	unsigned int *pixels = framebuffer->GetPixels(sy1);

	const float DIFF1 = ceil((*a)[1]) - (*a)[1];
	const float DIFF2 = ceil((*b)[1]) - (*b)[1];
	const float DIFF3 = ceil((*a)[1]) - (*c)[1];

	if (DX12 < DX13){
		float start = (DX12 * DIFF1) + (*a)[0];
		float end = (DX13 * DIFF3) + (*c)[0];
		if (sy1 < 0){
			start += DX12 * -sy1;
			end += DX13 * -sy1; // doesn't need to be corrected any further
			pixels += width * -sy1; // doesn't need to be corrected any further
			sy1 = 0;
		}

		for (int y = sy1; y < ey1; ++y, pixels+=width, start+=DX12, end+=DX13){
			RenderScanline((int)start, (int)end, color, pixels);
		}

		start = (DX23 * DIFF2) + (*b)[0];
		if (sy2 < 0){
			start += DX23 * -sy2;
			sy2 = 0;
		}

		for (int y = sy2; y < ey2; ++y, pixels+=width, start+=DX23, end+=DX13){
			RenderScanline((int)start, (int)end, color, pixels);
		}
	} else {
		float start = (DX13 * DIFF3) + (*c)[0];
		float end = (DX12 * DIFF1) + (*a)[0];
		if (sy1 < 0){
			start += DX13 * float(-sy1); // doesn't need to be corrected any further
			end += DX12 * float(-sy1);
			pixels += width * -sy1; // doesn't need to be corrected any further
			sy1 = 0;
		}

		for (int y = sy1; y < ey1; ++y, pixels+=width, start+=DX13, end+=DX12){
			RenderScanline((int)start, (int)end, color, pixels);
		}

		end = (DX23 * DIFF2) + (*b)[0];
		if (sy2 < 0){
			end += DX23 * float(-sy2);
			sy2 = 0;
		}

		for (int y = sy2; y < ey2; ++y, pixels+=width, start+=DX13, end+=DX23){
			RenderScanline((int)start, (int)end, color, pixels);
		}
	}
}

void mglFlatRasterizer::RenderBranch(const mglStaticModel::Node *p_node, const mmlVector<3> &p_view, const mmlVector<3> &p_viewDirection, const mmlMatrix<4, 4> &p_finalMatrix)
{
	if (p_node != NULL) {
		mglClip clip = p_node->plane.DetermineSide(p_view);
		switch (clip) {
		case mglInFront:
			RenderBranch(p_node->back, p_view, p_viewDirection, p_finalMatrix);
			RenderTriangleList(p_node->triangles, p_viewDirection, p_finalMatrix);
			RenderBranch(p_node->front, p_view, p_viewDirection, p_finalMatrix);
			break;
		case mglBehind:
			RenderBranch(p_node->front, p_view, p_viewDirection, p_finalMatrix);
			RenderTriangleList(p_node->triangles, p_viewDirection, p_finalMatrix);
			RenderBranch(p_node->back, p_view, p_viewDirection, p_finalMatrix);
			break;
		default:
			RenderBranch(p_node->front, p_view, p_viewDirection, p_finalMatrix);
			RenderBranch(p_node->back, p_view, p_viewDirection, p_finalMatrix);
			break;
		}
	}
}

void mglFlatRasterizer::RenderTriangleList(const mtlList<mglStaticModel::Triangle> &p_list, const mmlVector<3> &p_viewDirection, const mmlMatrix<4, 4> &p_finalMatrix)
{
	const int centerX = framebuffer->GetWidth() >> 1;
	const int centerY = framebuffer->GetHeight() >> 1;

	mmlVector<3> tri[3];
	mmlVector<3> clipped[4];
	const mtlItem<mglStaticModel::Triangle> *node = p_list.GetFirst();
	while (node != NULL) {
		tri[0] = mmlVector<3>::Cast(&node->GetItem().a);
		tri[1] = mmlVector<3>::Cast(&node->GetItem().b);
		tri[2] = mmlVector<3>::Cast(&node->GetItem().c);

		const mmlVector<3> facetDirection = mmlSurfaceNormal(tri[0], tri[1], tri[2]);
		float dot = -mmlDot(facetDirection, p_viewDirection); // p_view is not a direction vector for fuck's sake!
		if (dot <= 0.0f) {
			node = node->GetNext();
			continue;
		}

		const mmlVector<3> diffuseColor = node->GetItem().material->GetDiffuseColor();
		unsigned int iDiffuseColor =
			((unsigned char)(diffuseColor[0] * 255.0f * dot) << mglTexture::nativeFormat.RShift) |
			((unsigned char)(diffuseColor[1] * 255.0f * dot) << mglTexture::nativeFormat.GShift) |
			((unsigned char)(diffuseColor[2] * 255.0f * dot) << mglTexture::nativeFormat.BShift);

		tri[0] *= p_finalMatrix;
		tri[1] *= p_finalMatrix;
		tri[2] *= p_finalMatrix;

		int numclip = 0;
		for (int i = 0; i < 3; ++i) {
			if (tri[i][2] < m_nearDepth) { ++numclip; }
		}
		if (numclip == 3) {
			node = node->GetNext();
			continue;
		}
		if (numclip == 0) {
			for (int i = 0; i < 3; ++i) {
				const float w = 1.0f / tri[i][2];
				tri[i][0] = tri[i][0] * w + centerX;
				tri[i][1] = tri[i][1] * w + centerY;
				tri[i][2] = w;
			}
			RenderTriangle(mmlVector<2>::Cast(&tri[0]), mmlVector<2>::Cast(&tri[1]), mmlVector<2>::Cast(&tri[2]), iDiffuseColor);
		} else {
			numclip = m_nearPlane.Clip(tri[0], tri[1], tri[2], clipped);
			for (int i = 0; i < numclip; ++i) {
				const float w = 1.0f / clipped[i][2];
				clipped[i][0] = clipped[i][0] * w + centerX;
				clipped[i][1] = clipped[i][1] * w + centerY;
				clipped[i][2] = w;
			}
			switch (numclip) {
				case 4:
					RenderTriangle(mmlVector<2>::Cast(&clipped[2]), mmlVector<2>::Cast(&clipped[3]), mmlVector<2>::Cast(&clipped[0]), iDiffuseColor);
				case 3:
					RenderTriangle(mmlVector<2>::Cast(&clipped[0]), mmlVector<2>::Cast(&clipped[1]), mmlVector<2>::Cast(&clipped[2]), iDiffuseColor);
			}
		}

		node = node->GetNext();
	}
}

void mglFlatRasterizer::Render(const mglModel &p_model, const mmlMatrix<4, 4> &p_objToWorld)
{
	mmlVector<3> tri[3];
	mmlVector<3> clipped[4];

	//m_vertexCache.SetSize(p_model.m_vertexArray.GetSize()*3);

	const int centerX = framebuffer->GetWidth() >> 1;
	const int centerY = framebuffer->GetHeight() >> 1;
	const float FramebufferScale = (float)mmlMin2(framebuffer->GetWidth(), framebuffer->GetHeight());

	const mmlMatrix<4,4> objToCam = m_worldToCamera * p_objToWorld;
	const mmlMatrix<4,4> camToObj = mmlInv(objToCam);
	const mmlMatrix<4,4> final = mmlScaleMatrix(FramebufferScale, FramebufferScale, 1.0f) * m_projection * objToCam; // scale might be applied wrong, scaling should affect movement speed
	const mmlVector<3> cameraDirection = mglTransform::globalAxis.forward * mmlMatrix<3,3>(camToObj);
	
	for (int m = 0; m < p_model.GetMaterialCount(); ++m) {
		const mtlItem<mglFacet> *face = p_model.GetMaterial(m).GetFacets();
		const mmlVector<3> diffuseColor = p_model.GetMaterial(m).GetProperties().GetDiffuseColor();
		while (face != NULL) {
			tri[0] = p_model.GetVertex(face->GetItem().v1);
			tri[1] = p_model.GetVertex(face->GetItem().v2);
			tri[2] = p_model.GetVertex(face->GetItem().v3);

			const mmlVector<3> facetDirection = mmlSurfaceNormal(tri[0], tri[1], tri[2]);
			float dot = -mmlDot(facetDirection, cameraDirection);
			if (dot <= 0.0f) { face = face->GetNext(); continue; }

			unsigned int iDiffuseColor =
				((unsigned char)(diffuseColor[0] * 255.0f * dot) << mglTexture::nativeFormat.RShift) |
				((unsigned char)(diffuseColor[1] * 255.0f * dot) << mglTexture::nativeFormat.GShift) |
				((unsigned char)(diffuseColor[2] * 255.0f * dot) << mglTexture::nativeFormat.BShift);

			tri[0] *= final;
			tri[1] *= final;
			tri[2] *= final;

			int numclip = 0;
			for (int i = 0; i < 3; ++i) {
				if (tri[i][2] < m_nearDepth) { ++numclip; }
			}
			if (numclip == 3) {
				face = face->GetNext();
				continue;
			}
			if (numclip == 0) {
				for (int i = 0; i < 3; ++i) {
					const float w = 1.0f / tri[i][2];
					tri[i][0] = tri[i][0] * w + centerX;
					tri[i][1] = tri[i][1] * w + centerY;
					tri[i][2] = w;
				}
				/*if (mmlSurfaceNormal(tri[0], tri[1], tri[2])[2] < 0.0f) { // Am I doing this correctly? z is 1.0/z now...
					face = face->GetNext();
					continue;
				}*/
				RenderTriangle(mmlVector<2>::Cast(&tri[0]), mmlVector<2>::Cast(&tri[1]), mmlVector<2>::Cast(&tri[2]), iDiffuseColor);
			} else {
				numclip = m_nearPlane.Clip(tri[0], tri[1], tri[2], clipped);
				for (int i = 0; i < numclip; ++i) {
					const float w = 1.0f / clipped[i][2];
					clipped[i][0] = clipped[i][0] * w + centerX;
					clipped[i][1] = clipped[i][1] * w + centerY;
					clipped[i][2] = w;
				}
				/*if (mmlSurfaceNormal(clipped[0], clipped[1], clipped[2])[2] < 0.0f) { // Am I doing this correctly? z is 1.0/z now...
					face = face->GetNext();
					continue;
				}*/
				switch (numclip) {
					case 4:
						RenderTriangle(mmlVector<2>::Cast(&clipped[2]), mmlVector<2>::Cast(&clipped[3]), mmlVector<2>::Cast(&clipped[0]), iDiffuseColor);
					case 3:
						RenderTriangle(mmlVector<2>::Cast(&clipped[0]), mmlVector<2>::Cast(&clipped[1]), mmlVector<2>::Cast(&clipped[2]), iDiffuseColor);
				}
			}
			face = face->GetNext();
		}
	}
}

void mglFlatRasterizer::Render(const mglStaticModel &p_model, const mmlMatrix<4, 4> &p_objToWorld)
{
	const float FramebufferScale = (float)mmlMin2(framebuffer->GetWidth(), framebuffer->GetHeight());
	const mmlMatrix<4,4> objToCam = m_worldToCamera * p_objToWorld;
	const mmlMatrix<4,4> camToObj = mmlInv(objToCam);
	const mmlVector<3> cameraInObjectSpace = m_cameraPosition * camToObj; // is position relevant? shouldn't it be camera direction?
	const mmlMatrix<4,4> final = mmlScaleMatrix(FramebufferScale, FramebufferScale, 1.0f) * m_projection * objToCam; // scale might be applied wrong, scaling should affect movement speed
	const mmlVector<3> cameraDirection = mglTransform::globalAxis.forward * mmlMatrix<3,3>(camToObj);
	const mglStaticModel::Node *node = p_model.GetBinarySpaceTree();
	RenderBranch(node, cameraInObjectSpace, cameraDirection, final);
}
