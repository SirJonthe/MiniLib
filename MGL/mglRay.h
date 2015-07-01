#ifndef MGL_PIXMAP_H_INCLUDED__
#define MGL_PIXMAP_H_INCLUDED__

#include "../MTL/mtlBits.h"
#include "../MML/mmlVector.h"
#include "mglTransform.h"

//class mglObject;
class mglMaterial;

class mglRay3
{
public:
	mmlVector<3> origin;
	mmlVector<3> direction;
	mtlFlags     mask;

public:
	mglRay3( void ) : origin(), direction(), mask(0xffffffffffffffff) {}
};

struct mglRayCollision3
{
	//mglObject         *object;
	const mglMaterial *material;
	mmlVector<3>       point;
	mmlVector<3>       reflection;
	mmlVector<3>       normal;
};

// generizise this for 2/3 dimensions
// ability to set size of discrete steps (use for traversing large empty spaces faster)
// PROBLEM: only works perfectly for positive subspace (because -0.7 rounds *up* to 0 while 0.7 rounds *down* to 0)
class mglDifferentialAnalyzer3
{
private:
	mmlVector<3> m_origin;
	mmlVector<3> m_lengths;
	mmlVector<3> m_direction;
	mmlVector<3> m_delta;
	int          m_xyz[3];
	int          m_step[3];
	int          m_side;

public:
						mglDifferentialAnalyzer3( void ) : m_side(0) {}
	explicit            mglDifferentialAnalyzer3(const mglRay3 &p_ray) { SetInitialState(p_ray); }

	void                SetInitialState(const mglRay3 &p_ray);

	void                Step( void );
	int                 GetX( void ) const { return m_xyz[0]; }
	int                 GetY( void ) const { return m_xyz[1]; }
	int                 GetZ( void ) const { return m_xyz[2]; }

	int                 GetIndex(int axis) const { return m_xyz[axis]; }

	int                 GetImpactAxis( void ) const { return m_side; }
	float               GetImpactDistance( void ) const { return fabs((m_xyz[m_side] - m_origin[m_side] + (1 - m_step[m_side]) / 2) / m_direction[m_side]); }
	mmlVector<3>        GetImpactPosition( void ) const { return m_origin + m_direction * GetImpactDistance(); }
	mmlVector<2>        GetImpactUV( void ) const;

	const mmlVector<3> &GetOrigin( void ) const { return m_origin; }
};

class mglRay2
{
public:
	mmlVector<2> origin;
	mmlVector<2> direction;
	mtlFlags     mask;

public:
	mglRay2( void ) : origin(), direction(), mask(0xffffffffffffffff) {}
};

class mglRayCollision2
{
	const mglMaterial *material;
	mmlVector<2>       point;
	mmlVector<2>       reflection;
	mmlVector<2>       normal;
};

class mglDifferentialAnalyzer2
{
private:
	mmlVector<2> m_origin;
	mmlVector<2> m_lengths;
	mmlVector<2> m_direction;
	mmlVector<2> m_delta;
	int          m_xyz[2];
	int          m_step[2];
	int          m_side;

public:
						mglDifferentialAnalyzer2( void ) : m_side(0) {}
	explicit            mglDifferentialAnalyzer2(const mglRay2 &p_ray) { SetInitialState(p_ray); }

	void                SetInitialState(const mglRay2 &p_ray);

	void                Step( void );
	int                 GetX( void ) const { return m_xyz[0]; }
	int                 GetY( void ) const { return m_xyz[1]; }

	int                 GetIndex(int axis) const { return m_xyz[axis]; }

	int                 GetImpactAxis( void ) const { return m_side; }
	float               GetImpactDistance( void ) const { return fabs((m_xyz[m_side] - m_origin[m_side] + (1 - m_step[m_side]) / 2) / m_direction[m_side]); }
	mmlVector<2>        GetImpactPosition( void ) const { return m_origin + m_direction * GetImpactDistance(); }
	float               GetImpactU( void ) const;

	const mmlVector<2> &GetOrigin( void ) const { return m_origin; }
};

#endif
