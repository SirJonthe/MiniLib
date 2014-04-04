#ifndef MGL_PIXMAP_H_INCLUDED__
#define MGL_PIXMAP_H_INCLUDED__

#include "../MML/mmlVector.h"
#include "mglTransform.h"

class mglObject;
class mglMaterial;

class mglRay
{
public:
	mmlVector<3>	position;
	mmlVector<3>	direction;
	unsigned int	mask;
public:
	mglRay( void ) : position(), direction(), mask(0xffffffff) {}
};

struct mglRayCollision
{
	mglObject			*object;
	const mglMaterial	*material;
	mmlVector<3>		point;
	mmlVector<3>		reflection;
	mmlVector<3>		normal;
};

// generizise this for 2/3 dimensions
// ability to set size of discrete steps
class mglDifferentialAnalyzer
{
private:
	mmlVector<3>	m_position;
	mmlVector<3>	m_direction;
	mmlVector<3>	m_delta;
	int				m_xyz[3];
	int				m_step[3];
	int				m_side;
public:
						mglDifferentialAnalyzer( void ) : m_side(-1) {}
	explicit			mglDifferentialAnalyzer(const mglRay &p_ray) { SetInitialState(p_ray); }
	void				SetInitialState(const mglRay &p_ray);
	void				Step( void );
	int					GetX( void ) const { return m_xyz[0]; }
	int					GetY( void ) const { return m_xyz[1]; }
	int					GetZ( void ) const { return m_xyz[2]; }
	int					GetSide( void ) const { return m_side; }
	const mmlVector<3>	&GetPosition( void ) const { return m_position; }
};

#endif
