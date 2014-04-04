//
//  OGL_Rasterizer.h
//  MiniLib
//
//  Created by Jonathan Karlsson on 3/28/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#ifndef OGL_RASTERIZER_H_INCLUDED__
#define OGL_RASTERIZER_H_INCLUDED__

#include "OGL_Engine.h"
#include "../MGL/mglRasterizer.h"

class OGL_Rasterizer : public mglRasterizer
{
private:
	mmlMatrix<4,4>	ConvertToOGLMatrix(const mmlMatrix<4,4> &m1) const;
	void			UpdateAPIProjectionMatrix( void );
public:
	explicit	OGL_Rasterizer(const mglFramebuffer<> *p_framebuffer) : mglRasterizer(p_framebuffer) {}
	void		Render(const mglModel &p_model, const mmlMatrix<4,4> &p_objToWorld);
	void		Render(const mglStaticModel &p_model, const mmlMatrix<4,4> &p_objToWorld) {}
};

#endif
