#ifndef MGL_TRANSFORM_H_INCLUDED__
#define MGL_TRANSFORM_H_INCLUDED__

#include "../MML/mmlQuaternion.h"
#include "../MML/mmlVector.h"
#include "../MTL/mtlString.h"

struct mglAxis
{
	mmlVector<3>	right, up, forward;
					mglAxis( void ) { SetIdentity(); }
	void			RotateByAxisAngle(const mmlVector<3> &p_axis, float p_angle);
	void			SetIdentity( void );
	mmlMatrix<3,3>	GetRotationMatrix( void ) const;
};

// Introduce constraints

class mglTransform
{
public:
	static const mglAxis globalAxis;
/*private:
	const mglTransform	*m_parent;*/
public:
	mmlQuaternion		rotation;
	mmlVector<3>		position;
	mtlString			name;
	const mglTransform	*parent;
public:
						mglTransform( void );
						mglTransform(const mglTransform &p_transform);
	explicit			mglTransform(const mmlMatrix<4,4> &p_transform);
						mglTransform(const mmlVector<3> &p_position, const mmlMatrix<3,3> &p_rotation);
						mglTransform(const mmlVector<3> &p_position, const mmlVector<3> &p_axis, float p_angle);
						mglTransform(const mmlVector<3> &p_position, float p_head, float p_pitch, float p_roll);
						mglTransform(const mmlVector<3> &p_position, const mmlQuaternion &p_rotation);
	mglTransform		&operator=(const mglTransform &p_transform);
	bool				SetParentTransform(const mglTransform *p_parent, bool p_preserveTransform = false);
	const mglTransform	*GetParentTransform( void ) const { return parent; }
	mglTransform		GetWorldTransform( void ) const;
	mmlVector<3>		GetWorldPosition( void ) const;
	mmlQuaternion		GetWorldRotation( void ) const;
	mmlMatrix<4,4>		GetLocalTransformMatrix( void ) const;
	mmlMatrix<4,4>		GetWorldTransformMatrix( void ) const;
	mmlMatrix<3,3>		GetLocalRotationMatrix3( void ) const;
	mmlMatrix<3,3>		GetWorldRotationMatrix3( void ) const;
	mmlMatrix<4,4>		GetLocalRotationMatrix4( void ) const;
	mmlMatrix<4,4>		GetWorldRotationMatrix4( void ) const;
	const mmlVector<3>	&GetLocalPosition( void ) const { return position; }
	mmlVector<3>		&GetLocalPosition( void ) { return position; }
	const mmlQuaternion	&GetLocalRotation( void ) const { return rotation; }
	mmlQuaternion		&GetLocalRotation( void ) { return rotation; }
	mmlVector<3>		GetLocalRightAxis( void ) const { return rotation * mglTransform::globalAxis.right; }
	mmlVector<3>		GetLocalUpAxis( void ) const { return rotation * mglTransform::globalAxis.up; }
	mmlVector<3>		GetLocalForwardAxis( void ) const { return rotation * mglTransform::globalAxis.forward; }
	mmlVector<3>		GetWorldRightAxis( void ) const { return GetWorldRotation() * mglTransform::globalAxis.right; }
	mmlVector<3>		GetWorldUpAxis( void ) const { return GetWorldRotation() * mglTransform::globalAxis.up; }
	mmlVector<3>		GetWorldForwardAxis( void ) const { return GetWorldRotation() * mglTransform::globalAxis.forward; }
	void				RotateGlobal(const mmlVector<3> &p_axis, float p_angle) { rotation.RotateByAxisAngle(p_axis, p_angle); }
	void				RotateLocal(const mmlVector<3> &p_axis, float p_angle) { rotation.RotateByAxisAngle(rotation * p_axis, p_angle); } // DON'T THINK THIS IS CORRECT!
	void				RotateAroundLocal(const mmlVector<3> &p_position, const mmlVector<3> &p_axis, float p_angle);
	void				RotateAroundGlobal(mmlVector<3> p_position, mmlVector<3> p_axis, float p_angle);
	void				MoveGlobal(const mmlVector<3> &p_vector) { position += p_vector; }
	void				MoveLocal(const mmlVector<3> &p_vector) { position += rotation * p_vector; }
	void				SetIdentityRotation( void ) { rotation.SetIdentity(); }
	void				LookAt(const mmlVector<3> &p_point);
	//void				Constrain(float x, float y, float z, float rx, float ry, float rz);
};

#endif
