#ifndef MGL_TRANSFORM_H_INCLUDED__
#define MGL_TRANSFORM_H_INCLUDED__

#include "../MML/mmlQuaternion.h"
#include "../MML/mmlVector.h"
#include "../MTL/mtlString.h"

struct mglAxis
{
	mmlVector<3> right, up, forward;
			mglAxis( void ) { SetIdentity(); }
	void	RotateByAxisAngle(const mmlVector<3> &p_axis, float p_angle);
	void	SetIdentity( void );
};

class mglTransform
{
public:
	static const mglAxis globalAxis;
public:
	mglTransform	*parent;
	mmlQuaternion	rotation;
	mmlVector<3>	position;
	mtlString		name;
public:
					mglTransform( void );
					mglTransform(const mglTransform &p_transform);
	mglTransform	&operator=(const mglTransform &p_transform);
	mglTransform	GetWorldTransform( void ) const;
	mmlMatrix<4,4>	GetLocalTransformMatrix( void ) const;
	mmlMatrix<4,4>	GetWorldTransformMatrix( void ) const;
	mmlVector<3>	GetLocalRightAxis( void ) const { return rotation * mglTransform::globalAxis.right; }
	mmlVector<3>	GetLocalUpAxis( void ) const { return rotation * mglTransform::globalAxis.up; }
	mmlVector<3>	GetLocalForwardAxis( void ) const { return rotation * mglTransform::globalAxis.forward; }
	void			RotateLocal(const mmlVector<3> &p_axis, float p_angle) { rotation.RotateByAxisAngle(p_axis, p_angle); }
	void			RotateGlobal(const mmlVector<3> &p_axis, float p_angle) { rotation.RotateByAxisAngle(rotation * p_axis, p_angle); } // DON'T THINK THIS IS CORRECT!
	void			MoveGlobal(const mmlVector<3> &p_vector) { position += p_vector; }
	void			MoveLocal(const mmlVector<3> &p_vector) { position += rotation * p_vector; }
};

#endif
