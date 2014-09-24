/*
 *  mglCollider.h
 *  MiniLib
 *
 *  Created by Jonathan Karlsson on 8/31/13.
 *  Copyright 2013 Jonathan Karlsson. All rights reserved.
 *
 */

#ifndef MGL_COLLIDER_H_INCLUDED__
#define MGL_COLLIDER_H_INCLUDED__

#include "../MTL/mtlAsset.h"
#include "../MTL/mtlDuplex.h"
#include "mglModel.h"
#include "mglRay.h"
#include "mglPlane.h"

class mglObject;
class mglBoxCollider;
class mglSphereCollider;
class mglPlaneCollider;
class mglMeshCollider;
class mglStaticMeshCollider;

class mglCollider
{
	friend class mglObject;
public:
	enum ColliderType
	{
		None,
		Box,
		Sphere,
		Mesh,
		StaticMesh,
		Plane
	};
private:
	mtlDuplex<mglCollider, mglObject>	m_parentObject;
	const ColliderType					m_colliderType;
protected:
	mglRay		GetRayInObjectSpace(const mglRay &p_ray) const;
	static bool	BoxBox(const mglBoxCollider *c1, const mglBoxCollider *c2);
	static bool	BoxSphere(const mglBoxCollider *c1, const mglSphereCollider *c2);
	static bool	BoxPlane(const mglBoxCollider *c1, const mglPlaneCollider *c2);
	static bool	SphereSphere(const mglSphereCollider *c1, const mglSphereCollider *c2);
	static bool	SpeherePlane(const mglSphereCollider *c1, const mglPlaneCollider *c2);
	static bool	PlanePlane(const mglPlaneCollider *c1, const mglPlaneCollider *c2);
public:
					mglCollider(mglCollider::ColliderType p_colliderType) : m_parentObject(this), m_colliderType(p_colliderType)  {}
	virtual			~mglCollider( void ) { m_parentObject.Disconnect(); }
	virtual bool	GetCollision(mglRay p_ray, mglRayCollision &p_info) = 0;
	mglObject		*GetParentObject( void ) { return m_parentObject.GetOther(); }
	const mglObject	*GetParentObject( void ) const { return m_parentObject.GetOther(); }
	unsigned int	GetColliderType( void ) const { return m_colliderType; }
};

class mglBoxCollider : public mglCollider
{
public:
	mmlVector<3> min;
	mmlVector<3> max;
public:
			mglBoxCollider( void );
			mglBoxCollider(const mmlVector<3> &p_min, const mmlVector<3> &p_max);
	bool	GetCollision(mglRay p_ray, mglRayCollision &p_info);
};

class mglSphereCollider : public mglCollider
{
public:
	float radius;
public:
			mglSphereCollider( void ) : mglCollider(mglCollider::Sphere) {}
	bool	GetCollision(mglRay p_ray, mglRayCollision &p_info)
	{
		return false;
	}
};

class mglPlaneCollider : public mglCollider
{
public:
	mglPlane plane;
public:
				mglPlaneCollider(const mmlVector<3> &p_position, const mmlVector<3> &p_normal);
				mglPlaneCollider(const mmlVector<3> &a, const mmlVector<3> &b, const mmlVector<3> &c);
	explicit	mglPlaneCollider(const mglPlane &p_plane);
	bool		GetCollision(mglRay p_ray, mglRayCollision &p_info);
};

class mglMeshCollider : public mglCollider
{
public:
	mtlAsset<mglModel> mesh;
public:
	explicit	mglMeshCollider(mtlAsset<mglModel> &p_mesh) : mglCollider(mglCollider::Mesh), mesh(p_mesh) {}
	explicit	mglMeshCollider(const mtlChars &p_meshFilename) : mglCollider(mglCollider::Mesh), mesh(mtlAsset<mglModel>::Load(p_meshFilename)) {}
	bool		GetCollision(mglRay p_ray, mglRayCollision &p_info);
};

class mglStaticMeshCollider : public mglCollider
{
public:
	mtlAsset<mglStaticModel> mesh;
public:
	explicit	mglStaticMeshCollider(mtlAsset<mglStaticModel> &p_mesh) : mglCollider(mglCollider::StaticMesh), mesh(p_mesh) {}
	explicit	mglStaticMeshCollider(const mtlChars &p_meshFilename) : mglCollider(mglCollider::StaticMesh), mesh(mtlAsset<mglStaticModel>::Load(p_meshFilename)) {}
	bool		GetCollision(mglRay p_ray, mglRayCollision &p_info)
	{
		return false;
	}
};

#endif
