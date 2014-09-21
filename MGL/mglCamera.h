#ifndef MGL_CAMERA_H_INCLUDED__
#define MGL_CAMERA_H_INCLUDED__

#include "../MML/mmlVector.h"
#include "mglObject.h"
#include "mglFramebuffer.h"

class mglRasterizer;

class mglCamera : public mglObject
{
private:
	mglRasterizer *m_rasterizer;
protected:
	mglRasterizer *GetRasterizer( void ) { return m_rasterizer; }
private:
	void OnFinish( void ) { RenderView(); }
public:
			mglCamera(const mtlChars &p_name, mglRasterizer *p_rasterizer);
			~mglCamera( void );
	void	LookAt(const mmlVector<3> &p_point);
	void	Rotate(float horizontal, float vertical);
	void	SetPerspective(float p_vFov, float p_near, float p_far);
	void	SetFramebuffer(mglFramebuffer<> *p_framebuffer);
	void	RenderView( void );
};

#endif
