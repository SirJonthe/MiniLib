#ifndef MGL_PIXMAP_H_INCLUDED
#define MGL_PIXMAP_H_INCLUDED

#include "../MTL/mtlArray.h"
#include "../MTL/mtlBits.h"
#include "../MML/mmlVector.h"
#include "mglTransform.h"

struct mglRayCollisionPoint3D
{
	mmlVector<3> point, normal;
	float        distance;
};

struct mglRayCollision3D
{
	mglRayCollisionPoint3D entry, exit;
};

namespace mglCollision
{
	struct AABB   { mmlVector<3> min, max; };
	struct Sphere { mmlVector<3> center; float radius; };
	struct Plane  { mmlVector<3> normal; float distance; };
	struct Tri    { mmlVector<3> a, b, c; };
	struct Ray    { mmlVector<3> origin, direction; };

	mmlVector<3> ClosestPointOnPlane(const mmlVector<3> &point, const mmlVector<3> &plane_normal, float plane_dist);
	bool PointInTri(const mmlVector<3> &point, const mmlVector<3> &tri_a, const mmlVector<3> &tri_b, const mmlVector<3> &tri_c);

	bool AABB_Point(const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, const mmlVector<3> &point);
	bool AABB_AABB(const mmlVector<3> &a_min, const mmlVector<3> &a_max, const mmlVector<3> &b_min, const mmlVector<3> &b_max);
	bool AABB_Plane(const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, const mmlVector<3> &plane_normal, float plane_dist);

	bool Sphere_Point(const mmlVector<3> &cir_pos, float cir_radius, const mmlVector<3> &point);
	bool Sphere_Sphere(const mmlVector<3> &a_pos, float a_radius, const mmlVector<3> &b_pos, float b_radius);
	bool Sphere_Plane(const mmlVector<3> &cir_pos, float cir_radius, const mmlVector<3> &plane_normal, float plane_dist);

//	bool Tri_AABB(const mmlVector<3> &tri_a, const mmlVector<3> &tri_b, const mmlVector<3> &tri_c, const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max);
//	bool Tri_Sphere(const mmlVector<3> &tri_a, const mmlVector<3> &tri_b, const mmlVector<3> &tri_c, const mmlVector<3> &cir_pos, float cir_radius);
//	bool Tri_Tri(const mmlVector<3> &a_a, const mmlVector<3> &a_b, const mmlVector<3> &a_c, const mmlVector<3> &b_a, const mmlVector<3> &b_b, const mmlVector<3> &b_c);

	bool Ray_AABB(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, mglRayCollision3D *out = NULL);
	bool Ray_Sphere(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &cir_pos, float cir_radius, mglRayCollision3D *out = NULL);
	bool Ray_Plane(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &plane_normal, float plane_dist, mglRayCollisionPoint3D *out = NULL);
	bool Ray_Tri(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &tri_a, const mmlVector<3> &tri_b, const mmlVector<3> &tri_c, mglRayCollisionPoint3D *out = NULL);

	mmlVector<3> CorrectVelocity(const mmlVector<3> &vel, const mmlVector<3> &contact_normal);
}

// generizise this for 2/3 dimensions
// ability to set size of discrete steps (use for traversing large empty spaces faster)
// PROBLEM: only works perfectly for positive subspace (because -0.7 rounds *up* to 0 while 0.7 rounds *down* to 0)
class mglRayMarcher3D
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
						mglRayMarcher3D( void ) : m_side(0) {}
	explicit            mglRayMarcher3D(const mmlVector<3> &p_ray_origin, const mmlVector<3> &p_ray_dir) { SetInitialState(p_ray_origin, p_ray_dir); }

	void                SetInitialState(const mmlVector<3> &p_ray_origin, const mmlVector<3> &p_ray_dir);

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

class mglRayMarcher2D
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
						mglRayMarcher2D( void ) : m_side(0) {}
	explicit            mglRayMarcher2D(const mmlVector<2> &p_ray_origin, const mmlVector<2> &p_ray_dir) { SetInitialState(p_ray_origin, p_ray_dir); }

	void                SetInitialState(const mmlVector<2> &p_ray_origin, const mmlVector<2> &p_ray_dir);

	void                Step( void );
	int                 GetX( void ) const { return m_xyz[0]; }
	int                 GetY( void ) const { return m_xyz[1]; }

	int                 GetIndex(int axis) const { return m_xyz[axis]; }

	int                 GetImpactAxis( void )     const { return m_side; }
	float               GetImpactDistance( void ) const { return fabs((m_xyz[m_side] - m_origin[m_side] + (1 - m_step[m_side]) / 2) / m_direction[m_side]); }
	mmlVector<2>        GetImpactPosition( void ) const { return m_origin + m_direction * GetImpactDistance(); }
	float               GetImpactU( void )        const;

	const mmlVector<2> &GetOrigin( void ) const { return m_origin; }
};

#endif
