#include "mglCollision.h"

bool mglCollision::AABBPoint3D(const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, const mmlVector<3> &point)
{
	return
		(point[0] >= aabb_min[0] && point[0] <= aabb_max[0]) &&
		(point[1] >= aabb_min[1] && point[1] <= aabb_max[1]) &&
		(point[2] >= aabb_min[2] && point[2] <= aabb_max[2]);
}

bool mglCollision::AABBAABB3D(const mmlVector<3> &a_min, const mmlVector<3> &a_max, const mmlVector<3> &b_min, const mmlVector<3> &b_max)
{
	const mmlVector<3> overlap = mmlMin(a_max, b_max) - mmlMax(a_min, b_min);
	return (overlap[0] * overlap[1] * overlap[2]) > 0.0f;
}

bool mglCollision::AABBPlane3D(const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, const mmlVector<3> &plane_normal, float plane_dist)
{
	mmlVector<3> center = (aabb_max + aabb_min) * 0.5f;
	mmlVector<3> extents = aabb_max - center;
	float projection_interval_radius = mmlDot(extents, mmlAbs(plane_normal)); // extents[0]*mmlAbs(plane_normal[0]) + extents[1]*mmlAbs(plane_normal[1]) + extents[2]*mmlAbs(plane_normal[2])
	float aabb_center_plane_dist     = mmlDot(plane_normal, center) - plane_dist;
	return mmlAbs(aabb_center_plane_dist) <= projection_interval_radius;
}

bool mglCollision::SpherePoint3D(const mmlVector<3> &cir_pos, float cir_radius, const mmlVector<3> &point)
{
	return (point - cir_pos).Len() <= cir_radius;
}

bool mglCollision::SphereSphere3D(const mmlVector<3> &a_pos, float a_radius, const mmlVector<3> &b_pos, float b_radius)
{
	return mmlAbs(b_pos - a_pos).Len() <= (a_radius + b_radius);
}

bool mglCollision::RayAABB3D(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, mglRayCollision3D &out)
{
	// TODO; Pay more attention to WHICH values are used for tmin and tmax as they will be needed for determining entry/exit normals
	mmlVector<3> t0 = (aabb_min - ray_origin) / ray_dir;
	mmlVector<3> t1 = (aabb_max - ray_origin) / ray_dir;
	mmlVector<3> min = mmlMin(t0, t1);
	mmlVector<3> max = mmlMax(t0, t1);
	float tmin = mmlMax(min[0], min[1], min[2]);
	float tmax = mmlMin(max[0], max[1], max[2]);
	if (tmax < 0.0f || tmin > tmax) { return false; }

	if (tmin < 0.0f) {
		// ray inside box, only one intersection point
		out.exit_point = out.entry_point = ray_origin + ray_dir * tmax;
	} else {
		// two intersection points
		out.entry_point  = ray_origin + ray_dir * tmin;
		out.entry_normal = ray_origin + ray_dir * tmax;
	}

	return true;
}

bool mglCollision::RaySphere3D(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &cir_pos, float cir_radius, mglRayCollision3D &out)
{
	mmlVector<3> l = cir_pos - ray_origin;
	float tca = mmlDot(l, ray_dir);
	if (tca < 0.0f) { return false; }

	float radius2 = cir_radius * cir_radius;
	float d2 = mmlDot(l, l) - tca * tca;
	if (d2 > radius2) { return false; }

	float thc = mmlSqrt(radius2 - d2);
	float t[2] = { tca - thc, tca + thc };
	if (t[0] > t[1]) { mmlSwap(t[0], t[1]); }

	if (t[0] < 0.0f) {
		if (t[1] < 0.0f) { return false; }
		out.exit_point   = out.entry_point = ray_origin + ray_dir * t[1];
		out.exit_normal  = mmlNormalize(out.exit_normal - cir_pos);
		out.entry_normal = -out.exit_normal;
	} else {
		out.entry_point  = ray_origin + ray_dir * t[0];
		out.exit_point   = ray_origin + ray_dir * t[1];
		out.entry_normal = mmlNormalize(out.entry_point - cir_pos);
		out.exit_normal  = mmlNormalize(out.exit_point - cir_pos);
	}

	return true;
}

bool mglCollision::RayPlane3D(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &plane_normal, float plane_dist, mglRayCollision3D &out)
{
	float nd = mmlDot(ray_dir, plane_normal);
	if (nd >= 0.0f) { return false; }

	float pd = mmlDot(ray_origin, plane_normal);
	float t = (plane_dist - pd) / nd;
	if (t <= 0.0f) { return false; }

	out.entry_point  = out.exit_point = ray_origin + ray_dir * t;
	out.entry_normal = plane_normal;
	out.exit_normal  = -plane_normal;

	return true;
}

void mglRayMarcher3D::SetInitialState(const mmlVector<3> &p_ray_origin, const mmlVector<3> &p_ray_dir)
{
	m_origin = p_ray_origin;
	m_direction = p_ray_dir;
	m_xyz[0] = int( p_ray_origin[0] );
	m_xyz[1] = int( p_ray_origin[1] );
	m_xyz[2] = int( p_ray_origin[2] );
	for (int i = 0; i < 3; ++i) {
		// Wrong:
			// When m_direction[i] is 0.0f then Len() returns -NaN when Inf is expected
			// See 2D version for correct implementation
		m_delta[i] = (m_direction * (1.0f / m_direction[i])).Len();
		if (m_direction[i] < 0.0f) {
			m_step[i] = -1;
			m_lengths[i] = (p_ray_origin[i] - m_xyz[i]) * m_delta[i];
		} else {
			m_step[i] = 1;
			m_lengths[i] = (m_xyz[i] + 1.0f - p_ray_origin[i]) * m_delta[i];
		}
	}
	Step(); // Step once to finish initialization (setting m_side to 0 or -1 breaks GetDistance())
}

void mglRayMarcher3D::Step( void )
{
	m_side = 0;
	for (int i = 1; i < 3; ++i) {
		if (m_lengths[m_side] > m_lengths[i]) {
			m_side = i;
		}
	}
	m_lengths[m_side] += m_delta[m_side];
	m_xyz[m_side] += m_step[m_side];
}

mmlVector<2> mglRayMarcher3D::GetImpactUV( void ) const
{
	mmlVector<3> impact = GetImpactPosition();
	float u = impact[(m_side + 1) % 3];
	float v = impact[(m_side + 2) % 3];
	return mmlVector<2>(u - floor(u), v - floor(v));
}

void mglRayMarcher2D::SetInitialState(const mmlVector<2> &p_ray_origin, const mmlVector<2> &p_ray_dir)
{
	m_origin = p_ray_origin;
	m_direction = p_ray_dir;
	m_xyz[0] = int( p_ray_origin[0] );
	m_xyz[1] = int( p_ray_origin[1] );
	for (int i = 0; i < 2; ++i) {
		// Wrong:
			// When m_direction[i] is 0.0f then Len() returns -NaN when Inf is expected
		//m_delta[i] = (m_direction * (1.0f / m_direction[i])).Len();
		// Reference:
			// double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
			// double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
		m_delta[i] = sqrt(1.0f + (m_direction[(i + 1) & 1] * m_direction[(i + 1) & 1]) / (m_direction[i] * m_direction[i]));
		if (m_direction[i] < 0.0f) {
			m_step[i] = -1;
			m_lengths[i] = (p_ray_origin[i] - m_xyz[i]) * m_delta[i];
		} else {
			m_step[i] = 1;
			m_lengths[i] = (m_xyz[i] + 1.0f - p_ray_origin[i]) * m_delta[i];
		}
	}
	Step(); // Step once to finish initialization (setting m_side to 0 or -1 breaks GetDistance())
}

void mglRayMarcher2D::Step( void )
{
	m_side = 0;
	for (int i = 1; i < 2; ++i) {
		if (m_lengths[m_side] > m_lengths[i]) {
			m_side = i;
		}
	}
	m_lengths[m_side] += m_delta[m_side];
	m_xyz[m_side] += m_step[m_side];
}

float mglRayMarcher2D::GetImpactU( void ) const
{
	mmlVector<2> impact = GetImpactPosition();
	float u = impact[(m_side + 1) % 2];
	return u - floor(u);
}
