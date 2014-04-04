#ifndef MGL_OBJECT_H_INCLUDED__
#define MGL_OBJECT_H_INCLUDED__

#include "../MTL/mtlString.h"
#include "../MTL/mtlList.h"
#include "mglModel.h"
#include "mglCollider.h"
#include "mglTransform.h"

class mglEngine;

class mglObject
{
	friend class mglEngine;
public:
	mtlAsset<mglModel>					model;
	//mtlAsset<mglStaticModel>			model;
	mglTransform						transform;
	mtlString							name;
	unsigned int						mask;
private:
	mtlDuplex<mglObject, mglCollider>	m_collider;
	mglEngine							*m_engine;
	bool								m_enabled;
	bool								m_markedForDestruction;
public:
	bool								visible;
private:
				mglObject(const mglObject&) : m_collider(this) {}
	mglObject	&operator=(const mglObject&) { return *this; }
private:
	bool GetCollision(const mglRay &p_ray, mglRayCollision &p_collision);
protected:
	mglEngine			*GetEngine( void ) { return m_engine; }
	const mglEngine		*GetEngine( void ) const { return m_engine; }
protected:
	virtual void OnCreate( void )						{ /* Called when attached to engine */ }
	virtual void OnDestroy( void )						{ /* Called when deleted from engine, not when called DestroyAllObjects */ }
	virtual void OnUpdate( void )						{ /* Is called once every frame */ }
	virtual void OnFinish( void )						{ /* Is called once every frame after Update, OnPreRender, before OnPostRender */ }
	virtual void OnPreRender( void )					{ /* Is called right before object is rendered */ }
	virtual void OnPostRender( void )					{ /* Is called right after object is rendered */ }
	virtual void OnCollisionStart(mglObject &collider)	{ /* Called once for every new object collision */ }
	virtual void OnCollisionStay(mglObject &collider)	{ /* Called once per frame for every object collision */ }
	virtual void OnCollisionStop(mglObject &collider)	{ /* Called once for every object not colliding anymore */ }
	virtual void OnEnable( void )						{ /* Called once when object is enabled */ }
	virtual void OnDisable( void )						{ /* Called once when object is disabled */ }
	virtual void OnReset( void )						{ /* Resets the object to default values */ }
	virtual void OnKeyDown(unsigned int key)			{ /* Called for every object when a key is pressed */ }
	virtual void OnKeyUp(unsigned int key)				{ /* Called for every object when a key is released */ }
	virtual void OnMouseMove(int x, int y)				{ /* Called for every object when the mouse is moved */ }
public:
	explicit			mglObject(const mtlChars &p_name);
	virtual				~mglObject( void );
	void				Destroy( void );
	void				Enable( void );
	void				Disable( void );
	bool				IsEnabled( void ) const { return m_enabled; }
	void				SetCollider(mglCollider *p_collider);
	const mglCollider	*GetCollider( void ) const { return m_collider.GetOther(); }
	void				Reset( void );
	template < typename type_t >
	bool				IsType(type_t *&p_type) { p_type = dynamic_cast<type_t*>(this); return p_type != NULL; }
	template < typename type_t >
	bool				IsType(const type_t *&p_type) const { p_type = dynamic_cast<type_t*>(this); return p_type != NULL; }
};

//#include "mglEngine.h"

#endif
