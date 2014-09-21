#include "mglObject.h"

mglObject::mglObject(const mtlChars &p_name) :
model(), transform(), name(p_name), mask(0xffffffff),
m_collider(this), m_engine(NULL),
m_enabled(true), m_markedForDestruction(false), visible(true)
{
	OnEnable();
}

mglObject::~mglObject( void )
{
	Disable();
	if (m_collider.IsValid()) {
		delete m_collider.GetOther();
	}
}

void mglObject::Destroy( void )
{
	m_markedForDestruction = true;
	m_enabled = false;
}

void mglObject::Enable( void )
{
	if (!m_enabled && !m_markedForDestruction) {
		m_enabled = true;
		OnEnable();
	}
}

void mglObject::Disable( void )
{
	if (m_enabled) {
		m_enabled = false;
		OnDisable();
	}
}

void mglObject::SetCollider(mglCollider *p_collider)
{
	if (p_collider->m_parentObject.IsValid()) { return; }
	if (m_collider.IsValid()) {
		delete m_collider.GetOther(); // automatically disconnects
	}
	m_collider.Connect(p_collider->m_parentObject);
}

void mglObject::Reset( void )
{
	Disable();
	OnReset();
	Enable();
}
