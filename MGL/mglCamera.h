#ifndef MGL_CAMERA_H_INCLUDED
#define MGL_CAMERA_H_INCLUDED

#include "../MML/mmlVector.h"
#include "mglTransform.h"

class mglRasterizer;

class mglCamera
{
private:
	mglTransform m_transform;
	float        m_near;
	float        m_far;

public:
	void LookAt(const mmlVector<3> &p_point);
	void Rotate(float horizontal, float vertical);
	void SetPerspective(float p_vFov, float p_near, float p_far);
};

#endif
