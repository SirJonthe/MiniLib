#include "mglTransform.h"

void mglAxis::RotateByAxisAngle(const mmlVector<3> &p_axis, float p_angle)
{
	const mmlQuaternion q(p_axis, p_angle);
	right	= q * right;
	up		= q * up;
	forward	= q * forward;
}

void mglAxis::SetIdentity( void )
{
	right	= mmlVector<3>(1.0f, 0.0f, 0.0f);
	up		= mmlVector<3>(0.0f, 1.0f, 0.0f);
	forward	= mmlVector<3>(0.0f, 0.0f, 1.0f);
}

const mglAxis mglTransform::globalAxis = mglAxis();

mglTransform::mglTransform( void ) :
	parent(NULL),
	rotation(), position(0.0f, 0.0f, 0.0f)
{
	rotation.SetIdentity();
}

mglTransform::mglTransform(const mglTransform &p_transform) :
	parent(p_transform.parent),
	rotation(p_transform.rotation), position(p_transform.position)
{ name.Copy(p_transform.name); }

mglTransform &mglTransform::operator=(const mglTransform &p_transform)
{
	if (this != &p_transform) {
		parent = p_transform.parent;
		rotation = p_transform.rotation;
		position = p_transform.position;
		name.Copy(p_transform.name);
	}
	return *this;
}

mglTransform mglTransform::GetWorldTransform( void ) const
{
	if (parent != NULL) {
		mglTransform transform = parent->GetWorldTransform();
		transform.position = transform.position + transform.rotation * position;
		transform.rotation = transform.rotation * rotation;
		return transform;
	}
	return *this;
}

mmlMatrix<4,4> mglTransform::GetLocalTransformMatrix( void ) const
{
	mmlMatrix<4,4> m = rotation.GetEulerMatrix();
	m[0][3] = position[0];
	m[1][3] = position[1];
	m[2][3] = position[2];
	return m;
}

mmlMatrix<4,4> mglTransform::GetWorldTransformMatrix( void ) const
{
	mglTransform fullTransform = GetWorldTransform();
	mmlMatrix<4,4> m = fullTransform.rotation.GetEulerMatrix();
	m[0][3] = fullTransform.position[0];
	m[1][3] = fullTransform.position[1];
	m[2][3] = fullTransform.position[2];
	return m;
}
