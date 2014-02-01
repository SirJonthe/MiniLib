/*
 *  mglRasterizer.h
 *  MiniLib
 *
 *  Created by Jonathan Karlsson on 10/27/13.
 *  Copyright 2013 Jonathan Karlsson. All rights reserved.
 *
 */

#ifndef MGL_RASTERIZER_H_INCLUDED__
#define MGL_RASTERIZER_H_INCLUDED__

//#include "../MTL/mtlArray.h"
#include "../MML/mmlVector.h"
#include "../MML/mmlMatrix.h"
#include "mglPlane.h"
#include "mglModel.h"
#include "mglTransform.h"
#include "mglFramebuffer.h"

// http://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/

class mglRasterizer
{
public:
	const mglFramebuffer<> *framebuffer;
protected:
	mmlMatrix<4,4>	m_worldToCamera, m_projection;
	mmlVector<3>	m_cameraPosition;
	mglPlane		m_nearPlane, m_farPlane;
	float			m_nearDepth, m_farDepth, m_vFov;
public:
	explicit		mglRasterizer(const mglFramebuffer<> *p_framebuffer) : framebuffer(p_framebuffer) {}
	virtual			~mglRasterizer( void ) {}
	void			SetCameraTransform(const mglTransform &p_camTransform);
	void			SetProjectionMatrix(float p_vFov, float p_near, float p_far);
	virtual void	Render(const mglModel &p_model, const mmlMatrix<4,4> &p_objToWorld) = 0;
	virtual void	Render(const mglStaticModel &p_model, const mmlMatrix<4,4> &p_objToWorld) = 0;
};

class mglSWUVRasterizer : public mglRasterizer
{
private:
	static const mglTexture		defaultMaterial;
private:
	const mglTexture			*m_currentTexture;
	//mtlArray< mmlVector<4> >	m_vertexCache; // transformed xyzw components, store locally to promote multi-core rendering
private:
	//void RenderScanlineAffine(int x1, int x2, int, float, float, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels);
	void RenderScanlineSubAffine(int x1, int x2, int, float w1, float w2, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels);
	void RenderScanlineCorrect(int x1, int x2, int, float w1, float w2, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels);
	//void RenderScanlineDitherAffine(int x1, int x2, int y, float, float, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels);
	void RenderScanlineDitherSubAffine(int x1, int x2, int y, float w1, float w2, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels);
	void RenderScanlineDitherCorrect(int x1, int x2, int y, float w1, float w2, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels);
	//void RenderScanlineDitherFixed(int x1, int x2, int y, mmlVector<2> t1, mmlVector<2> t2, unsigned int *pixels);
	void RenderTriangle(mmlVector<5> va, mmlVector<5> vb, mmlVector<5> vc);
	void DrawBranch(const mglStaticModel::Node *p_node, const mmlVector<3> &p_view, const mmlMatrix<4,4> &p_finalMatrix);
	void DrawTriangleList(const mtlList< mglStaticModel::Poly > &p_list, const mmlMatrix<4,4> &p_finalMatrix);
public:
	explicit	mglSWUVRasterizer(const mglFramebuffer<> *p_framebuffer) : mglRasterizer(p_framebuffer) { /* m_vertexCache.poolMemory = true; */ }
	void		Render(const mglModel &p_model, const mmlMatrix<4,4> &p_objToWorld);
	void		Render(const mglStaticModel &p_model, const mmlMatrix<4,4> &p_objToWorld);
	void		Debug_RenderTriangle(const mmlVector<4> &a, const mmlVector<4> &b, const mmlVector<4> &c, const mglTexture *texture);
};

class mglFlatRasterizer : public mglRasterizer
{
private:
	void RenderScanline(int x1, int x2, unsigned int color, unsigned int *pixels);
	void RenderTriangle(mmlVector<2> va, mmlVector<2> vb, mmlVector<2> vc, unsigned int color);
	void RenderBranch(const mglStaticModel::Node *p_node, const mmlVector<3> &p_view, const mmlVector<3> &p_viewDirection, const mmlMatrix<4,4> &p_finalMatrix);
	void RenderTriangleList(const mtlList< mglStaticModel::Poly > &p_list, const mmlVector<3> &p_viewDirection, const mmlMatrix<4,4> &p_finalMatrix);
public:
	explicit	mglFlatRasterizer(const mglFramebuffer<> *p_framebuffer) : mglRasterizer(p_framebuffer) {}
	void		Render(const mglModel &p_model, const mmlMatrix<4, 4> &p_objToWorld);
	void		Render(const mglStaticModel &p_model, const mmlMatrix<4, 4> &p_objToWorld);
};

#endif
