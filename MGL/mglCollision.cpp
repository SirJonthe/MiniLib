#include "mglCollision.h"

mglCollision::Plane mglCollision::PlaneFromTriangle(const mmlVector<3> &tri_a, const mmlVector<3> &tri_b, const mmlVector<3> &tri_c)
{
	mmlVector<3> poly_normal = mmlSurfaceNormal(tri_a, tri_b, tri_c);
	return mglCollision::Plane{ poly_normal, -mmlDot(tri_a, poly_normal) };
}

mglCollision::Plane mglCollision::PlaneFromNormalAndPoint(const mmlVector<3> &normal, const mmlVector<3> &point_on_plane)
{
	return mglCollision::Plane{ normal, -mmlDot(point_on_plane, normal) };
}

mglCollision::AABB mglCollision::AABBFromCenterAndHalfExtents(const mmlVector<3> &center, const mmlVector<3> &half_extents)
{
	mmlVector<3> abs = mmlAbs(half_extents);
	return mglCollision::AABB{ center - abs, center + abs };
}

mmlVector<3> mglCollision::CenterOfAABB(const mglCollision::AABB &box)
{
	return mmlLerp(box.min, box.max, 0.5f);
}

mmlVector<3> mglCollision::HalfExtentsOfAABB(const mglCollision::AABB &box)
{
	return mmlAbs(box.max) - CenterOfAABB(box);
}

float mglCollision::SignedDistanceToPlane(const mmlVector<3> &point, const mmlVector<3> &plane_normal, float plane_dist)
{
	return mmlDot(plane_normal, point) - plane_dist;
}

mmlVector<3> mglCollision::ClosestPointOnPlane(const mmlVector<3> &point, const mmlVector<3> &plane_normal, float plane_dist)
{
	float dist = mglCollision::SignedDistanceToPlane(point, plane_normal, plane_dist);
	// if plane_normal is not normalized
	// dist = dist / mmlDot(plane_normal, plane_normal);
	return point - plane_normal * dist;
}

bool mglCollision::PointInTri(const mmlVector<3> &point, const mmlVector<3> &tri_a, const mmlVector<3> &tri_b, const mmlVector<3> &tri_c)
{
	mmlVector<3> a = tri_a - point;
	mmlVector<3> b = tri_b - point;
	mmlVector<3> c = tri_c - point;
	mmlVector<3> u = mmlCross(b, c);
	mmlVector<3> v = mmlCross(c, a);
	mmlVector<3> w = mmlCross(a, b);
	return mmlDot(u, v) >= 0.0f && mmlDot(u, w) >= 0.0f;
}

bool mglCollision::Plane_Plane(const mmlVector<3> &plane1_normal, float plane1_dist, const mmlVector<3> &plane2_normal, float plane2_dist, mglPlaneCollisionLine3D *out)
{
	const mmlVector<3> p3_normal = mmlCross(plane1_normal, plane2_normal);
	const float        det       = p3_normal.Len2();

	// If determinant == 0, planes are parallel => no intersection
	if (mmlIsApproxEqual(det, 0.0f) == false) {
		if (out != NULL) {
			out->point = ((mmlCross(p3_normal, plane2_normal) * plane1_dist) + (mmlCross(plane1_normal, p3_normal) * plane2_dist)) / det;
			out->normal = p3_normal;
		}
		return true;
	}
	return false;
}

bool mglCollision::AABB_Point(const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, const mmlVector<3> &point)
{
	return
		(point[0] >= aabb_min[0] && point[0] <= aabb_max[0]) &&
		(point[1] >= aabb_min[1] && point[1] <= aabb_max[1]) &&
		(point[2] >= aabb_min[2] && point[2] <= aabb_max[2]);
}

bool mglCollision::AABB_AABB(const mmlVector<3> &a_min, const mmlVector<3> &a_max, const mmlVector<3> &b_min, const mmlVector<3> &b_max)
{
	mmlVector<3> a_center = mmlLerp(a_min, a_max, 0.5f);
	mmlVector<3> a_half_extents = a_max - a_center;
	mmlVector<3> b_center = mmlLerp(b_min, b_max, 0.5f);
	mmlVector<3> b_half_extents = b_max - b_center;
	for (int i = 0; i < 3; ++i) {
		if (mmlAbs(a_center[i] - b_center[i]) > (a_half_extents[i] + b_half_extents[i])) { return false; }
	}
	return true;
}

bool mglCollision::AABB_Plane(const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, const mmlVector<3> &plane_normal, float plane_dist)
{
	mmlVector<3> center = mmlLerp(aabb_min, aabb_max, 0.5f);
	mmlVector<3> extents = aabb_max - center;
	float projection_interval_radius = mmlDot(extents, mmlAbs(plane_normal));
	float aabb_center_plane_dist     = mmlDot(plane_normal, center) - plane_dist;
	return mmlAbs(aabb_center_plane_dist) <= projection_interval_radius;
}

mglClip mglCollision::Classify_AABB_Plane(const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, const mmlVector<3> &plane_normal, float plane_dist)
{
	mmlVector<3> center = mmlLerp(aabb_min, aabb_max, 0.5f);
	mmlVector<3> extents = aabb_max - center;
	float projection_interval_radius = mmlDot(extents, mmlAbs(plane_normal));
	return mglCollision::Classify_Sphere_Plane(center, projection_interval_radius, plane_normal, plane_dist);
}

bool mglCollision::Sphere_Point(const mmlVector<3> &cir_pos, float cir_radius, const mmlVector<3> &point)
{
	return (point - cir_pos).Len() <= cir_radius;
}

bool mglCollision::Sphere_Sphere(const mmlVector<3> &a_pos, float a_radius, const mmlVector<3> &b_pos, float b_radius)
{
	return mmlAbs(b_pos - a_pos).Len() <= (a_radius + b_radius);
}

bool mglCollision::Sphere_Plane(const mmlVector<3> &cir_pos, float cir_radius, const mmlVector<3> &plane_normal, float plane_dist)
{
	mmlVector<3> closest = mglCollision::ClosestPointOnPlane(cir_pos, plane_normal, plane_dist);
	return mmlDist(closest, cir_pos) <= cir_radius;
}

mglClip mglCollision::Classify_Sphere_Plane(const mmlVector<3> &cir_pos, float cir_radius, const mmlVector<3> &plane_normal, float plane_dist)
{
	float dist = mglCollision::SignedDistanceToPlane(cir_pos, plane_normal, plane_dist);
	if (dist >  cir_radius) { return mglInFront; }
	if (dist < -cir_radius) { return mglBehind; }
	return mglClipping;
}

bool mglCollision::Ray_AABB(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &aabb_min, const mmlVector<3> &aabb_max, mglRayCollision3D *out)
{
	const mmlMatrix<3,3> Normals = mmlMatrix<3,3>::Identity(); // these are three normals on the AABB (the other three are the negative normals)

	mmlVector<3> t0 = (aabb_min - ray_origin) / ray_dir; // one of the 3 sides connected to the min point (all normals are -1)
	mmlVector<3> t1 = (aabb_max - ray_origin) / ray_dir; // one of the 3 sides connected to the max point (all normals are +1)
	mmlVector<3> min;
	mmlVector<3> max;
	for (int i = 0; i < 3; ++i) {
		if (t0[i] < t1[i]) {
			min[i] = t0[i];
			max[i] = t1[i];
		} else {
			min[i] = t1[i];
			max[i] = t0[i];
		}
	}
	float tmin = mmlMax(min[0], min[1], min[2]);
	float tmax = mmlMin(max[0], max[1], max[2]);

	if (tmax < 0.0f || tmin > tmax) { return false; }

	int min_normal_index[3];
	int max_normal_index[3];
	for (int i = 0; i < 3; ++i) {
		if (t0[i] < t1[i]) {
			min_normal_index[i] = i + 3;
			max_normal_index[i] = i;
		} else {
			min_normal_index[i] = i;
			max_normal_index[i] = i + 3;
		}
	}
	int tmin_normal_index = (min[0] > min[1]) ? (min[0] > min[2] ? min_normal_index[0] : min_normal_index[2]) : (min[1] > min[2] ? min_normal_index[1] : min_normal_index[2]);
	int tmax_normal_index = (max[0] < max[1]) ? (max[0] < max[2] ? max_normal_index[0] : max_normal_index[2]) : (max[1] < max[2] ? max_normal_index[1] : max_normal_index[2]);

	if (out != NULL) {
		if (tmin < 0.0f) {
			// ray inside box, only one intersection point
			out->entry.point    = ray_origin;
			out->entry.normal   = ray_dir;
			out->entry.distance = 0.0f;

			out->exit.point    = ray_origin + ray_dir * tmax;
			out->exit.normal   = Normals[tmax_normal_index % 3] * (tmax_normal_index < 3 ? -1.0f : 1.0f);
			out->exit.distance = tmax;
		} else {
			// two intersection points
			out->entry.point    = ray_origin + ray_dir * tmin;
			out->entry.normal   = Normals[tmin_normal_index % 3] * (tmin_normal_index < 3 ? -1.0f : 1.0f);
			out->entry.distance = tmin;

			out->exit.point    = ray_origin + ray_dir * tmax;
			out->exit.normal   = Normals[tmax_normal_index % 3] * (tmax_normal_index < 3 ? -1.0f : 1.0f);
			out->exit.distance = tmax;
		}
	}

	return true;
}

bool mglCollision::Ray_Sphere(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &cir_pos, float cir_radius, mglRayCollision3D *out)
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
		if (t[1] < 0.0f) {
			return false;
		} else if (out != NULL) {
			// ray inside sphere
			out->entry.point    = ray_origin;
			out->entry.normal   = ray_dir;
			out->entry.distance = 0.0f;

			out->exit.point    = ray_origin + ray_dir * t[1];
			out->exit.normal   = mmlNormalize(out->exit.normal - cir_pos);
			out->exit.distance = t[1];
		}
	} else if (out != NULL) {
		out->entry.point    = ray_origin + ray_dir * t[0];
		out->entry.normal   = mmlNormalize(out->entry.point - cir_pos);
		out->entry.distance = t[0];

		out->exit.point     = ray_origin + ray_dir * t[1];
		out->exit.normal    = mmlNormalize(out->exit.point - cir_pos);
		out->exit.distance  = t[1];
	}

	return true;
}

bool mglCollision::Ray_Plane(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &plane_normal, float plane_dist, mglRayCollisionPoint3D *out)
{
	// ray eq: r = origin + direction * projection_distance
	// plane eq: direction . point_on_plane + distance_to_origin = 0
	// intersection
	// plane_direction . (ray_origin + ray_direction * ray_projection_distance) + plane_distance_to_origin = 0
	// ray_projection_distance = -(plane_normal . ray_origin + plane_distance_to_origin) / (plane_normal . ray_direction)

	float nd = mmlDot(plane_normal, ray_dir);
	if (nd >= 0.0f) { return false; }

	float pd = mmlDot(plane_normal, ray_origin);
	float t = -(pd + plane_dist) / nd;
	if (t <= 0.0f) { return false; }

	if (out != NULL) {
		out->point    = ray_origin + ray_dir * t;
		out->normal   = plane_normal;
		out->distance = t;
	}
	return true;
}

bool mglCollision::Ray_Plane(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &plane_normal, const mmlVector<3> &plane_point, mglRayCollisionPoint3D *out)
{
	const float plane_dist = -mmlDot(plane_point, plane_normal);
	return mglCollision::Ray_Plane(ray_origin, ray_dir, plane_normal, plane_dist, out);
}

bool mglCollision::Ray_Tri(const mmlVector<3> &ray_origin, const mmlVector<3> &ray_dir, const mmlVector<3> &tri_a, const mmlVector<3> &tri_b, const mmlVector<3> &tri_c, mglRayCollisionPoint3D *out)
{
	mmlVector<3> poly_normal = mmlSurfaceNormal(tri_a, tri_b, tri_c);
	float plane_dist = -mmlDot(tri_a, poly_normal);
	mglRayCollisionPoint3D tmp;
	if (out == NULL) { out = &tmp; }
	if (!mglCollision::Ray_Plane(ray_origin, ray_dir, poly_normal, plane_dist, out)) { return false; } // if true, out contains all info we need from the collision
	return PointInTri(out->point, tri_a, tri_b, tri_c);
}

// Used to correct velocity so that A0 + VEL does not result in an A1 that clips geometry.
// First you need to determine if A0 + uncorrected VEL = uncorrected A1 intersects geometry by casting ray (constructed from A0 -> uncorrected A1) against geometry.
//	Also determine if ray contact occurs within the span between the points A0 and the uncorrected A1.
//	If valid contact has occurred the surface/collision normal NORMAL is used to correct VEL by calling VEL = ClipVelocityByContactNormal(VEL, NORMAL).
//	The corrected VEL is added to A0 to produce a corrected A1 that slides along geometry rather than clips through it.
mmlVector<3> mglCollision::ClipVelocityByContactNormal(const mmlVector<3> &vel, const mmlVector<3> &contact_normal)
{
	return vel - ((vel * contact_normal) * contact_normal);
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
		m_delta[i] = float(sqrt(double(1.0f + (m_direction[(i + 1) & 1] * m_direction[(i + 1) & 1]) / (m_direction[i] * m_direction[i]))));
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
	return u - float(floor(double(u)));
}
