/*
 *  mglCollider.cpp
 *  MGLTest
 *
 *  Created by Jonathan Karlsson on 11/30/13.
 *  Copyright 2013 Jonathan Karlsson. All rights reserved.
 *
 */

#include "../MML/mmlVector.h"
#include "mglObject.h"
#include "mglCollider.h"

mglRay mglCollider::GetRayInObjectSpace(const mglRay &p_ray) const
{
	mglRay ray = p_ray;
	if (m_parentObject.IsValid()) {
		const mmlMatrix<4,4> toObj = mmlInv(m_parentObject->transform.GetWorldTransformMatrix());
		ray.origin *= toObj;
		ray.direction *= mmlMatrix<3,3>(toObj);
	}
	return ray;
}

mglBoxCollider::mglBoxCollider( void ) : mglCollider(mglCollider::Box), min(0.0f, 0.0f, 0.0f), max(0.0f, 0.0f, 0.0f)
{}

mglBoxCollider::mglBoxCollider(const mmlVector<3> &p_min, const mmlVector<3> &p_max) : mglCollider(mglCollider::Box), min(p_min), max(p_max)
{}

bool mglBoxCollider::GetCollision(mglRay p_ray, mglRayCollision &p_info)
{
	// POSSIBLE BUG: I'm pretty sure this gives false positives
	
	p_ray = GetRayInObjectSpace(p_ray);
	
	const mmlVector<3> tmin = (min - p_ray.origin) / p_ray.direction;
	const mmlVector<3> tmax = (max - p_ray.origin) / p_ray.direction;
	const mmlVector<3> realMin = mmlMin(tmin, tmax);
	const mmlVector<3> realMax = mmlMax(tmin, tmax);
	
	const float minmax = mmlMin3(realMax[0], realMax[1], realMax[2]);
	const float maxmin = mmlMax3(realMin[0], realMin[1], realMin[2]);
	
	if (minmax >= maxmin) {
		p_info.material = NULL;
		p_info.point = p_ray.origin + p_ray.direction * maxmin;
		//p_info.normal = ;
		//p_info.reflection = mmlReflect(p_ray.direction, p_info.normal);
		return true;
	}
	return false;
}

bool mglMeshCollider::GetCollision(mglRay p_ray, mglRayCollision &p_info)
{
	if (mesh.GetAsset() == NULL) { return false; }
	p_ray = GetRayInObjectSpace(p_ray);
	for (int m = 0; m < mesh.GetAsset()->GetMaterialCount(); ++m) {
		const mglMaterialIndex *material = &mesh.GetAsset()->GetMaterial(m);
		const mtlNode<mglFacet> *facet = material->GetFacets();
		while (facet != NULL) {
			const mmlVector<3> a = mesh.GetAsset()->GetVertex(facet->GetItem().v1);
			const mmlVector<3> b = mesh.GetAsset()->GetVertex(facet->GetItem().v2);
			const mmlVector<3> c = mesh.GetAsset()->GetVertex(facet->GetItem().v3);
			mglPlaneCollider pc(a, b, c); // no parent, object space is identity
			if (pc.GetCollision(p_ray, p_info)) {
				// Compute vectors
				const mmlVector<3> v0 = c - a;
				const mmlVector<3> v1 = b - a;
				const mmlVector<3> v2 = p_info.point - a;

				// Compute dot products
				const float dot00 = mmlDot(v0, v0);
				const float dot01 = mmlDot(v0, v1);
				const float dot02 = mmlDot(v0, v2);
				const float dot11 = mmlDot(v1, v1);
				const float dot12 = mmlDot(v1, v2);

				// Compute barycentric coordinates
				const float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
				const float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
				const float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

				// Check if point is in triangle
				if ((u >= 0) && (v >= 0) && (u + v < 1)) {
					p_info.object = GetParentObject();
					p_info.material = &material->GetProperties();
					return true;
				}
			}
			facet = facet->GetNext();
		}
	}
	return false;
}

mglPlaneCollider::mglPlaneCollider(const mmlVector<3> &p_position, const mmlVector<3> &p_normal) : mglCollider(mglCollider::Plane), plane(p_position, p_normal)
{}

mglPlaneCollider::mglPlaneCollider(const mmlVector<3> &a, const mmlVector<3> &b, const mmlVector<3> &c) : mglCollider(mglCollider::Plane), plane(a, b, c)
{}

mglPlaneCollider::mglPlaneCollider(const mglPlane &p_plane) : mglCollider(mglCollider::Plane), plane(p_plane)
{}

bool mglPlaneCollider::GetCollision(mglRay p_ray, mglRayCollision &p_info)
{
	p_ray = GetRayInObjectSpace(p_ray);
	if (mmlDot(p_ray.direction, plane.GetNormal()) <= 0.0f) {
		const float d = mmlDot(plane.GetPosition() - p_ray.origin, plane.GetNormal()) / mmlDot(p_ray.direction, plane.GetNormal());
		p_info.point = p_ray.origin + p_ray.direction * d;
		p_info.normal = plane.GetNormal();
		p_info.reflection = mmlReflect(p_ray.direction, p_info.normal);
		return true;
	}
	return false;
}
