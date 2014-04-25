//
//  mglRenderer.h
//  MiniLib
//
//  Created by Jonathan Karlsson on 4/21/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#ifndef MGL_RENDERER_H_INCLUDED__
#define MGL_RENDERER_H_INCLUDED__

class mglCamera; // Camera MUST contain transform and perspective
class mglTexture;
// mglModel renders *ITSELF*

class mglRenderer
{
public:
	virtual void SetView(const mglCamera &camera) = 0; // sets transform matrix *and* perspective matrix
	virtual void SetWorldTransform(const mglTransform &world) = 0; 
	virtual void SetTexture(const mglTexture *texture) = 0;
};

/*class mglRasterizer : public mglRenderer
{
public:
	void RenderIndex(const mmlVector<3> *vtx, const int *vidx, const mmlVector<2> *tex, const int *tidx, const mmlVector<3> *nor, const int *nidx) = 0;
	void RenderTriangles(const mmlVector<3> *vtx, const mmlVector<2> *tex, const mmlVector<3> *nor) = 0; // every three elements are a single triangle
};
 
class mglSoftRasterizer : public mglRasterizer
{
private:
	void RenderTriangle(const mmlVector<5> a, const mmlVector<5> b, const mmlVector<5> c); // x y z u v
	void RenderScanline(int x0, int x1, int y, const mmlVector<4> &v0, const mmlVector<4> &v1); // z w u v
};

class mglRaycaster : public mglRenderer
{
};

class mglRaysurfer : public mglRaycaster
{
};

class mglRaytracer : public mglRenderer
{
};*/

#endif
