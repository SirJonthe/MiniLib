#ifndef MTL_POINTER_H_INCLUDED__
#define MTL_POINTER_H_INCLUDED__

#include "mtlList.h"

template < typename type_t >
class mtlShared
{
private:
	type_t	*m_obj;
	int		*m_count;
public:
	mtlShared( void );
	~mtlShared( void );
	template < typename derived_t >
	mtlShared(const mtlShared<derived_t> &shared) : m_obj(NULL), m_count(NULL)
	{
		if (!shared.IsNull()) {
			m_obj = shared.m_obj;
			m_count = shared.m_count;
			++(*m_count);
		}
	}
	mtlShared(const mtlShared<type_t> &shared);
	template < typename derived_t >
	mtlShared &operator=(const mtlShared<derived_t> &shared)
	{
		// we can't rely on m_obj having same address despite pointing to same data (not same types)
		// instead, compare m_count address
		if (m_count != shared.m_count) {
			Delete();
			m_obj = shared.m_obj;
			m_count = shared.m_count;
			if (!IsNull()) { ++(*m_count); }
		}
		return *this;
	}
	mtlShared &operator=(const mtlShared<type_t> &shared);
	type_t *GetShared( void );
	const type_t *GetShared( void ) const;
	type_t *operator->( void ) { return GetShared(); }
	const type_t *operator->( void ) const { return GetShared(); }
	template < typename derived_t >
	void New( void );
	void New( void );
	void Delete( void );
	bool IsNull( void ) const;
	int GetCount( void ) const;
	template < typename derived_t >
	static mtlShared<type_t> Create( void );
	static mtlShared<type_t> Create( void );
};

template < typename type_t >
mtlShared<type_t>::mtlShared( void ) : m_obj(NULL), m_count(NULL)
{
}

template < typename type_t >
mtlShared<type_t>::~mtlShared( void )
{
	Delete();
}

template < typename type_t >
type_t *mtlShared<type_t>::GetShared( void )
{
	return m_obj;
}

template < typename type_t >
mtlShared<type_t>::mtlShared(const mtlShared<type_t> &shared) : m_obj(NULL), m_count(NULL)
{
	if (!shared.IsNull()) {
		m_obj = shared.m_obj;
		m_count = shared.m_count;
		++(*m_count);
	}
}

template < typename type_t >
mtlShared<type_t> &mtlShared<type_t>::operator=(const mtlShared<type_t> &shared)
{
	// we can't rely on m_obj having same address despite pointing to same data (not same types)
	// instead, compare m_count address
	if (m_count != shared.m_count) {
		Delete();
		m_obj = shared.m_obj;
		m_count = shared.m_count;
		if (!IsNull()) { ++(*m_count); }
	}
	return *this;
}

template < typename type_t >
const type_t *mtlShared<type_t>::GetShared( void ) const
{
	return m_obj;
}

template < typename type_t >
template < typename derived_t >
void mtlShared<type_t>::New( void )
{
	Delete();
	m_obj = new derived_t;
	m_count = new int;
	*m_count = 1;
}

template < typename type_t >
void mtlShared<type_t>::New( void )
{
	New<type_t>();
}

template < typename type_t >
void mtlShared<type_t>::Delete( void )
{
	if (!IsNull()) {
		--(*m_count);
		if ((*m_count) == 0) {
			delete m_obj;
			delete m_count;
		}
	}
	m_obj = NULL;
	m_count = NULL;
}

template < typename type_t >
bool mtlShared<type_t>::IsNull( void ) const
{
	return m_obj == NULL;
}

template < typename type_t >
int mtlShared<type_t>::GetCount( void ) const
{
	return m_count != NULL ? *m_count : 0;
}

template < typename type_t >
template < typename derived_t >
mtlShared<type_t> mtlShared<type_t>::Create( void )
{
	mtlShared<type_t> a;
	a.New<derived_t>();
	return a;
}

template < typename type_t >
mtlShared<type_t> mtlShared<type_t>::Create( void )
{
	return Create<type_t>();
}

template < typename type_t >
class mtlReference;

template < typename type_t >
class mtlExclusive
{
	friend class mtlReference<type_t>;
private:
	mtlShared<type_t>								m_obj; // why a mtlShared? because then we can copy-construct internally without duplicating data
	mtlShared< mtlList< mtlReference<type_t> > >	m_tracker;
private:
	mtlExclusive(const mtlExclusive &exclusive);
	mtlExclusive &operator=(const mtlExclusive &exclusive);
public:
	mtlExclusive( void );
	~mtlExclusive( void );
	template < typename derived_t >
	void New( void );
	void New( void );
	void Delete( void );
	template < typename derived_t >
	static mtlExclusive<type_t> Create( void );
	static mtlExclusive<type_t> Create( void );
	bool IsNull( void ) const;
	type_t *GetExclusive( void );
	const type_t *GetExclusive( void ) const;
};

template < typename type_t >
mtlExclusive<type_t>::mtlExclusive(const mtlExclusive<type_t> &exclusive) : m_obj(exclusive.m_obj), m_tracker(exclusive.m_tracker)
{
}

template < typename type_t >
mtlExclusive<type_t> &mtlExclusive<type_t>::operator=(const mtlExclusive<type_t> &exclusive)
{
	if (&exclusive != this) {
		Delete();
		m_obj = exclusive.m_obj;
		m_tracker = exclusive.m_tracker;
	}
	return *this;
}

template < typename type_t >
template < typename derived_t >
void mtlExclusive<type_t>::New( void )
{
	Delete();
	m_obj.New<derived_t>();
	m_tracker.New();
}

template < typename type_t >
void mtlExclusive<type_t>::New( void )
{
	New<type_t>();
}

template < typename type_t >
void mtlExclusive<type_t>::Delete( void )
{
	m_obj.Delete();
	if (m_obj.IsNull() && !m_tracker.IsNull()) {
		mtlNode< mtlReference<type_t> > *n = m_tracker->GetFirst();
		while (n != NULL) {
			n->GetItem().m_reference = NULL;
			n->GetItem().m_trackerItem = NULL;
			n = n->GetNext();
		}
		m_tracker.Delete();
	}
}

template < typename type_t >
template < typename derived_t >
mtlExclusive<type_t> mtlExclusive<type_t>::Create( void )
{
	mtlExclusive<type_t> o;
	o.New<derived_t>();
	return o;
}

template < typename type_t >
mtlExclusive<type_t> mtlExclusive<type_t>::Create( void )
{
	return Create<type_t>();
}

template < typename type_t >
bool mtlExclusive<type_t>::IsNull( void ) const
{
	return m_obj.IsNull();
}

template < typename type_t >
type_t *mtlExclusive<type_t>::GetExclusive( void )
{
	return m_obj.GetShared();
}

template < typename type_t >
const type_t *mtlExclusive<type_t>::GetExclusive( void ) const
{
	return m_obj.GetShared();
}

template < typename type_t >
class mtlReference
{
	friend class mtlExclusive<type_t>;
private:
	type_t							*m_reference;
	mtlNode< mtlReference<type_t> >	*m_trackerItem;
public:
	mtlReference( void );
	mtlReference(const mtlReference &ref);
	mtlReference(mtlExclusive<type_t> &excl);
	mtlReference &operator=(const mtlReference<type_t> &ref);
	mtlReference &operator=(mtlExclusive<type_t> &ref);
	~mtlReference( void );
	void Delete( void );
	bool IsNull( void ) const;
	type_t *GetReference( void );
	const type_t *GetReference( void ) const;
};

template < typename type_t >
mtlReference<type_t>::mtlReference( void ) : m_reference(NULL), m_trackerItem(NULL)
{
}

template < typename type_t >
mtlReference<type_t>::mtlReference(const mtlReference<type_t> &ref) : m_reference(NULL), m_trackerItem(NULL)
{
	if (ref.m_reference != NULL) {
		*this = *ref.m_reference;
	}
}

template < typename type_t >
mtlReference<type_t>::mtlReference(mtlExclusive<type_t> &excl) : m_reference(excl.m_obj.GetShared()), m_trackerItem(NULL)
{
	if (!excl.IsNull()) {
		excl.m_tracker->AddLast(this);
		m_trackerItem = excl.m_tracker->GetLast();
	}
}

template < typename type_t >
mtlReference<type_t> &mtlReference<type_t>::operator=(const mtlReference<type_t> &ref)
{
	if (&ref != this) {
		Delete();
		if (ref.m_reference != NULL) {
			*this = *ref.m_reference;
		}
	}
	return *this;
}

template < typename type_t >
mtlReference<type_t> &mtlReference<type_t>::operator=(mtlExclusive<type_t> &excl)
{
	if (excl.m_obj.GetShared() != m_reference) {
		Delete();
		m_reference = excl.m_obj.GetShared();
		if (m_reference != NULL) {
			excl.m_tracker->AddLast(this);
			m_trackerItem = excl.m_tracker->GetLast();
		}
	}
}

template < typename type_t >
mtlReference<type_t>::~mtlReference( void )
{
	Delete();
}


template < typename type_t >
void mtlReference<type_t>::Delete( void )
{
	if (m_reference != NULL) {
		m_reference = NULL;
		m_trackerItem->Remove();
		m_trackerItem = NULL;
	}
}

template < typename type_t >
bool mtlReference<type_t>::IsNull( void ) const
{
	return m_reference != NULL;
}

template < typename type_t >
type_t *mtlReference<type_t>::GetReference( void )
{
	return m_reference;
}

template < typename type_t >
const type_t *mtlReference<type_t>::GetReference( void ) const
{
	return m_reference;
}

#endif
