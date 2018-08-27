#include "mglTransform.h"

void mglAxis::RotateByAxisAngle(const mmlVector<3> &p_axis, float p_angle)
{
	const mmlQuaternion q(p_axis, p_angle);
	m_rot[0] = q * m_rot[0];
	m_rot[1] = q * m_rot[1];
	m_rot[2] = q * m_rot[2];
}

const mglAxis mglTransform::globalAxis = mglAxis();

mglTransform::mglTransform( void ) :
	//m_parent(NULL),
	rotation(), position(0.0f, 0.0f, 0.0f), name(), parent(NULL)
{
	rotation.SetIdentity();
}

mglTransform::mglTransform(const mglTransform &p_transform) :
	//m_parent(p_transform.m_parent),
	rotation(p_transform.rotation), position(p_transform.position), parent(NULL)
{ name.Copy(p_transform.name); }

mglTransform::mglTransform(const mmlMatrix<4,4> &p_transform) :
rotation(p_transform), position(p_transform[0][3], p_transform[1][3], p_transform[2][3]), name(), parent(NULL)
{}

mglTransform::mglTransform(const mmlVector<3> &p_position, const mmlMatrix<3,3> &p_rotation) :
rotation(p_rotation), position(p_position), name(), parent(NULL)
{}

mglTransform::mglTransform(const mmlVector<3> &p_position, const mmlVector<3> &p_axis, float p_angle) :
rotation(p_axis, p_angle), position(p_position), name(), parent(NULL)
{}

mglTransform::mglTransform(const mmlVector<3> &p_position, float p_head, float p_pitch, float p_roll) :
rotation(p_head, p_pitch, p_roll), position(p_position), name(), parent(NULL)
{}

mglTransform::mglTransform(const mmlVector<3> &p_position, const mmlQuaternion &p_rotation) :
rotation(p_rotation), position(p_position), name(), parent(NULL)
{}

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
#include <iostream>
bool mglTransform::SetParentTransform(const mglTransform *p_parent, bool p_preserveTransform)
{
	// make sure there is no cyclical relation
	if (parent != NULL) {
		const mglTransform *parentCheck = this;
		while (parentCheck != NULL) {
			if (p_parent == parentCheck) {
				return false;
			}
			parentCheck = parentCheck->parent;
		}
	}
	if (p_preserveTransform) {
		if (p_parent == NULL) {
			mglTransform worldTransform = GetWorldTransform();
			position = worldTransform.position;
			rotation = worldTransform.rotation;
			std::cout << "detach" << std::endl;
		} else {
			// I want to modify so that worldTransform = parentTransform
			
			// old position
			// old world position
			// new position
			// new world position
			
			mmlMatrix<4,4> worldTransform = GetWorldTransformMatrix();
			mmlMatrix<4,4> parentTransform = p_parent->GetWorldTransformMatrix();
			
			std::cout << "old local: " << position[0] << ";" << position[1] << ";" << position[2] << std::endl;
			std::cout << "old world: " << worldTransform[0][3] << ";" << worldTransform[1][3] << ";" << worldTransform[2][3] << std::endl;
			
			mmlMatrix<4,4> newTransform = worldTransform * mmlInv(parentTransform);
			position = mmlVector<3>(newTransform[0][3], newTransform[1][3], newTransform[2][3]);
			rotation.FromEulerMatrix(newTransform);
			
			std::cout << "new local: " << position[0] << ";" << position[1] << ";" << position[2] << std::endl;
			mmlVector<3> newPos = GetWorldPosition();
			std::cout << "new world: " << newPos[0] << ";" << newPos[1] << ";" << newPos[2] << std::endl;
		}
	}
	parent = p_parent;
	return true;
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

mmlVector<3> mglTransform::GetWorldPosition( void ) const
{
	return GetWorldTransform().position;
}

mmlQuaternion mglTransform::GetWorldRotation( void ) const
{
	return GetWorldTransform().rotation;
}

mmlMatrix<4,4> mglTransform::GetLocalTransformMatrix( void ) const
{
	mmlMatrix<4,4> m = rotation.GetEulerMatrix4();
	m[0][3] = position[0];
	m[1][3] = position[1];
	m[2][3] = position[2];
	return m;
}

mmlMatrix<4,4> mglTransform::GetWorldTransformMatrix( void ) const
{
	mglTransform fullTransform = GetWorldTransform();
	mmlMatrix<4,4> m = fullTransform.rotation.GetEulerMatrix4();
	m[0][3] = fullTransform.position[0];
	m[1][3] = fullTransform.position[1];
	m[2][3] = fullTransform.position[2];
	return m;
}

mmlMatrix<3,3> mglTransform::GetLocalRotationMatrix3( void ) const
{
	return rotation.GetEulerMatrix3();
}

mmlMatrix<3,3> mglTransform::GetWorldRotationMatrix3( void ) const
{
	if (parent != NULL) {
		return parent->GetWorldRotationMatrix3() * GetLocalRotationMatrix3();
	}
	return GetLocalRotationMatrix3();
}

mmlMatrix<4,4> mglTransform::GetLocalRotationMatrix4( void ) const
{
	return rotation.GetEulerMatrix4();
}

mmlMatrix<4,4> mglTransform::GetWorldRotationMatrix4( void ) const
{
	if (parent != NULL) {
		return parent->GetWorldRotationMatrix4() * GetLocalRotationMatrix4();
	}
	return GetLocalRotationMatrix4();
}

void mglTransform::RotateAroundLocal(const mmlVector<3> &p_position, const mmlVector<3> &p_axis, float p_angle)
{
	mmlQuaternion rotationQuaternion(p_axis, p_angle);
	rotation = rotationQuaternion * rotation;
	position -= p_position;
	position = rotationQuaternion * position;
	position += p_position;
}

void mglTransform::RotateAroundGlobal(mmlVector<3> p_position, mmlVector<3> p_axis, float p_angle)
{
	const mmlMatrix<4,4> invWorldTransform = mmlInv(GetWorldTransformMatrix());
	const mmlMatrix<3,3> invWorldRotation = mmlMatrix<3,3>(invWorldTransform);
	p_position = p_position * invWorldTransform;
	p_axis = p_axis * invWorldRotation;
	RotateAroundLocal(p_position, p_axis, p_angle);
}

void mglTransform::LookAt(const mmlVector<3> &p_point)
{
	const mmlVector<3> NormalTo = mmlNormalize(p_point - position);
	const mmlVector<3> LocalForward = GetLocalForwardAxis();
	const mmlVector<3> Axis = mmlNormalize(mmlCross(NormalTo, LocalForward));
	const float Angle = (mmlDot(mmlCross(Axis, LocalForward), NormalTo) < 0.0f) ? mmlAng(NormalTo, LocalForward) : -mmlAng(NormalTo, LocalForward);
	rotation = mmlQuaternion(Axis, Angle) * rotation;
}
