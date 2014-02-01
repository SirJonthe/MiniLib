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
		ray.ReverseTransform(m_parentObject->transform.GetWorldTransform());
	}
	return ray;
}

bool mglMeshCollider::GetCollision(mglRay p_ray, mglRayCollision &p_info)
{
	if (mesh.GetAsset() == NULL) { return false; }
	p_ray = GetRayInObjectSpace(p_ray);
	for (int m = 0; m < mesh.GetAsset()->GetMaterialCount(); ++m) {
		const mglMaterial *material = &mesh.GetAsset()->GetMaterial(m);
		const mtlNode<mglFacet> *facet = material->GetFacets();
		while (facet != NULL) {
			const mmlVector<3> a = mesh.GetAsset()->GetVertex(facet->value.v1);
			const mmlVector<3> b = mesh.GetAsset()->GetVertex(facet->value.v2);
			const mmlVector<3> c = mesh.GetAsset()->GetVertex(facet->value.v3);
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
					p_info.material = material;
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

bool mglPlaneCollider::GetCollision(mglRay p_ray, mglRayCollision &p_info)
{
	p_ray = GetRayInObjectSpace(p_ray);
	if (mmlDot(p_ray.direction, plane.GetNormal()) <= 0.0f) {
		const float d = mmlDot(plane.GetPosition() - p_ray.position, plane.GetNormal()) / mmlDot(p_ray.direction, plane.GetNormal());
		p_info.point = p_ray.position + p_ray.direction * d;
		if ((p_info.point - p_ray.position).Len() > p_ray.length) {
			return false;
		}
		p_info.normal = plane.GetNormal();
		p_info.reflection = mmlReflect(p_ray.direction, p_info.normal);
		return true;
	}
	return false;
}
