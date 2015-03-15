#ifndef MTL_POINTER_H_INCLUDED__
#define MTL_POINTER_H_INCLUDED__

#include "mtlList.h"

template < typename type_t >
class mtlReference;

template < typename type_t >
class mtlShared
{
	friend class mtlReference<type_t>;

private:
	type_t	*m_obj; // delete if count==0
	int		*m_count; // increment on create, decrement on destroy, only delete if watchers==0
	int		*m_watchers; // only delete this is watchers==0

public:
	mtlShared( void );
	~mtlShared( void );
	template < typename derived_t >
	mtlShared(const mtlShared<derived_t> &shared) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
	{
		if (!shared.IsNull()) {
			m_obj = shared.m_obj;
			m_count = shared.m_count;
			m_watchers = shared.m_watchers;
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
			m_obj = shared.m_obj; // we rely on the compiler to tell us if this is a mismatch
			m_count = shared.m_count;
			m_watchers = shared.m_watchers;
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
	int GetShareCount( void ) const;

	template < typename derived_t >
	static mtlShared<type_t> Create( void );
	static mtlShared<type_t> Create( void );
};

template < typename type_t >
mtlShared<type_t>::mtlShared( void ) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
{
}

template < typename type_t >
mtlShared<type_t>::~mtlShared( void )
{
	Delete();
}

template < typename type_t >
mtlShared<type_t>::mtlShared(const mtlShared<type_t> &shared) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
{
	if (!shared.IsNull()) {
		m_obj = shared.m_obj;
		m_count = shared.m_count;
		m_watchers = shared.m_watchers;
		++(*m_count);
	}
}

template < typename type_t >
mtlShared<type_t> &mtlShared<type_t>::operator=(const mtlShared<type_t> &shared)
{
	if (m_obj != shared.m_obj) {
		Delete();
		m_obj = shared.m_obj;
		m_count = shared.m_count;
		m_watchers = shared.m_watchers;
		if (!IsNull()) { ++(*m_count); }
	}
	return *this;
}

template < typename type_t >
type_t *mtlShared<type_t>::GetShared( void )
{
	return m_obj;
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
	m_watchers = new int;
	*m_count = 1;
	*m_watchers = 0;
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
			if ((*m_watchers) == 0) {
				delete m_count;
				delete m_watchers;
			}
		}
	}
	m_obj = NULL;
	m_count = NULL;
	m_watchers = NULL;
}

template < typename type_t >
bool mtlShared<type_t>::IsNull( void ) const
{
	return m_obj == NULL;
}

template < typename type_t >
int mtlShared<type_t>::GetShareCount( void ) const
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
	mtlShared<type_t> a;
	a.New<type_t>();
	return a;
}

template < typename type_t >
class mtlExclusive
{
	friend class mtlReference<type_t>;

private:
	type_t	*m_obj;
	int		*m_watchers;

private:
	mtlExclusive(const mtlExclusive&) : m_obj(NULL), m_watchers(NULL) {}
	mtlExclusive &operator=(const mtlExclusive&) { return *this; }

public:
	mtlExclusive( void );
	~mtlExclusive( void );

	type_t *GetExclusive( void ) { return m_obj; }
	const type_t *GetExclusive( void ) const { return m_obj; }
	type_t *operator->( void ) { return m_obj; }
	const type_t *operator->( void ) const { return m_obj; }

	template < typename derived_t >
	void New( void );
	void New( void );
	void Delete( void );

	bool IsNull( void ) const;
};

template < typename type_t >
mtlExclusive<type_t>::mtlExclusive( void ) : m_obj(NULL), m_watchers(NULL)
{}

template < typename type_t >
mtlExclusive<type_t>::~mtlExclusive( void )
{
	Delete();
}

template < typename type_t >
template < typename derived_t >
void mtlExclusive<type_t>::New( void )
{
	Delete();
	m_obj = new type_t;
	m_watchers = new int;
	*m_watchers = 0;
}

template < typename type_t >
void mtlExclusive<type_t>::New( void )
{
	New<type_t>();
}

template < typename type_t >
void mtlExclusive<type_t>::Delete( void )
{
	if (m_obj != NULL) {
		delete m_obj;
		if (*m_watchers == 0) {
			delete m_watchers;
		}
	}
	m_obj = NULL;
	m_watchers = NULL;
}

template < typename type_t >
bool mtlExclusive<type_t>::IsNull( void ) const
{
	return m_obj == NULL;
}

template < typename type_t >
class mtlReference
{
private:
	type_t	*m_obj;
	int		*m_count; // do nothing with this (delete this if watchers==0 on destroy)
	int		*m_watchers; // incement this on create, decrement on destroy

public:
	mtlReference( void );
	~mtlReference( void );

	template < typename derived_t >
	mtlReference(const mtlShared<derived_t> &shared) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
	{
		if (!shared.IsNull()) {
			m_obj = shared.m_obj;
			m_count = shared.m_count;
			m_watchers = shared.m_watchers;
			++(*m_watchers);
		}
	}
	mtlReference(const mtlShared<type_t> &shared);

	template < typename derived_t >
	mtlReference &operator=(const mtlShared<derived_t> &shared)
	{
		// we can't rely on m_obj having same address despite pointing to same data (not same types)
		// instead, compare m_count address
		if (m_count != shared.m_count) {
			Delete();
			m_obj = shared.m_obj; // we rely on the compiler to tell us if this is a mismatch
			m_count = shared.m_count;
			m_watchers = shared.m_watchers;
			if (!IsNull()) { ++(*m_watchers); }
		}
		return *this;
	}
	mtlReference &operator=(const mtlShared<type_t> &shared);

	template < typename derived_t >
	mtlReference(const mtlExclusive<derived_t> &excl) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
	{
		if (!excl.IsNull()) {
			m_obj = excl.m_obj;
			m_count = excl.m_watchers;
			m_watchers = excl.m_watchers;
			++(*m_watchers);
		}
	}
	mtlReference(const mtlExclusive<type_t> &excl);

	template < typename derived_t >
	mtlReference &operator=(const mtlExclusive<derived_t> &excl)
	{
		if (m_watchers != excl.m_watchers) {
			Delete();
			m_obj = excl.m_obj;
			m_count = excl.m_watchers;
			m_watchers = excl.m_watchers;
			if (!IsNull()) { ++(*m_watchers); }
		}
		return *this;
	}
	mtlReference &operator=(const mtlExclusive<type_t> &excl);

	template < typename derived_t >
	mtlReference(const mtlReference<derived_t> &ref) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
	{
		if (!ref.IsNull()) {
			m_obj = ref.m_obj;
			m_count = ref.m_count;
			m_watchers = ref.m_watchers;
			++(*m_watchers);
		}
	}
	mtlReference(const mtlReference<type_t> &ref);

	template < typename derived_t >
	mtlReference &operator=(const mtlReference<derived_t> &ref)
	{
		// we can't rely on m_obj having same address despite pointing to same data (not same types)
		// instead, compare m_count address
		if (m_count != ref.m_count) {
			Delete();
			m_obj = ref.m_obj; // we rely on the compiler to tell us if this is a mismatch
			m_count = ref.m_count;
			m_watchers = ref.m_watchers;
			if (!IsNull()) { ++(*m_watchers); }
		}
		return *this;
	}
	mtlReference &operator=(const mtlReference<type_t> &ref);

	type_t *GetReference( void );
	const type_t *GetReference( void ) const;
	type_t *operator->( void ) { return GetReference(); }
	const type_t *operator->( void ) const { return GetReference(); }

	void Delete( void );

	bool IsNull( void ) const;
	int GetShareCount( void ) const;
};

template < typename type_t >
mtlReference<type_t>::mtlReference( void ) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
{}

template < typename type_t >
mtlReference<type_t>::~mtlReference( void )
{
	Delete();
}

template < typename type_t >
mtlReference<type_t>::mtlReference(const mtlShared<type_t> &shared) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
{
	if (!shared.IsNull()) {
		m_obj = shared.m_obj;
		m_count = shared.m_count;
		m_watchers = shared.m_watchers;
		++(*m_watchers);
	}
}

template < typename type_t >
mtlReference<type_t> &mtlReference<type_t>::operator=(const mtlShared<type_t> &shared)
{
	if (m_obj != shared.m_obj) {
		Delete();
		m_obj = shared.m_obj;
		m_count = shared.m_count;
		m_watchers = shared.m_watchers;
		if (!IsNull()) { ++(*m_watchers); }
	}
	return *this;
}

template < typename type_t >
mtlReference<type_t>::mtlReference(const mtlExclusive<type_t> &excl) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
{
	if (!excl.IsNull()) {
		m_obj = excl.m_obj;
		m_count = excl.m_watchers;
		m_watchers = excl.m_watchers;
		++(*m_watchers);
	}
}

template < typename type_t >
mtlReference<type_t> &mtlReference<type_t>::operator=(const mtlExclusive<type_t> &excl)
{
	if (m_obj != excl.m_obj) {
		Delete();
		m_obj = excl.m_obj;
		m_count = excl.m_watchers;
		m_watchers = excl.m_watchers;
		if (!IsNull()) { ++(*m_watchers); }
	}
	return *this;
}

template < typename type_t >
mtlReference<type_t>::mtlReference(const mtlReference<type_t> &ref) : m_obj(NULL), m_count(NULL), m_watchers(NULL)
{
	if (!ref.IsNull()) {
		m_obj = ref.m_obj;
		m_count = ref.m_count;
		m_watchers = ref.m_watchers;
		++(*m_watchers);
	}
}

template < typename type_t >
mtlReference<type_t> &mtlReference<type_t>::operator=(const mtlReference<type_t> &ref)
{
	if (m_obj != ref.m_obj) {
		Delete();
		m_obj = ref.m_obj;
		m_count = ref.m_count;
		m_watchers = ref.m_watchers;
		if (!IsNull()) { ++(*m_watchers); }
	}
	return *this;
}

template < typename type_t >
type_t *mtlReference<type_t>::GetReference( void )
{
	return IsNull() ? NULL : m_obj;
}

template < typename type_t >
const type_t *mtlReference<type_t>::GetReference( void ) const
{
	return IsNull() ? NULL : m_obj;
}

template < typename type_t >
void mtlReference<type_t>::Delete( void )
{
	if (m_count != NULL) {
		--(*m_watchers);
		if (*m_watchers == 0 && *m_count == 0) {
			delete m_count;
			delete m_watchers;
		}
	}
	m_obj = NULL;
	m_count = NULL;
	m_watchers = NULL;
}

template < typename type_t >
bool mtlReference<type_t>::IsNull( void ) const
{
	return m_count == NULL || *m_count == 0;
}

template < typename type_t >
int mtlReference<type_t>::GetShareCount( void ) const
{
	return IsNull() ? 0 : *m_count;
}

#endif
