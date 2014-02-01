/*
A class that provides a two-way connection
between classes that have a mutual relationship.

1) Add mtlDuplex<A,B> as member in class A (ex. call member instance 'mutual')
	Initialize mutual with class instance address ('this')
2) Add mtlDuplex<B,A> as member in class B (ex. call member instance 'mutual')
	Initialize mutual with class instance address ('this')
3) Connect classes using mutual
	a.mutual.Connect(b.mutual)
	or
	b.mutual.Connect(a.mutual)
	(doesn't matter which)
4)
	B public interface is now accessable through a.mutual->
	conversely
	A public interface is now accessable through b.mutual->
5) Verify connection using IsValid() 
6) Disconnect using
	a.mutual.Disconnect()
	or
	b.mutual.Disconnect()
	(doesn't matter which)
7) Automatically disconnects correctly on destruction

NOTE: Not thread safe!

Copyright (c) Jonathan Karlsson, 2013
*/

#ifndef MTL_DUPLEX_H_INCLUDED__
#define MTL_DUPLEX_H_INCLUDED

template < typename self_t, typename other_t >
class mtlDuplex
{
	friend class mtlDuplex<other_t, self_t>;
private:
	self_t * const m_self;
	mtlDuplex<other_t, self_t> *m_other;
public:
	mtlDuplex(self_t * const p_self) : m_self(p_self), m_other(NULL) {}
	~mtlDuplex( void )
	{
		Disconnect();
	}
protected:
	virtual void OnConnect( void ) {}
	virtual void OnDisconnect( void ) {}
public:
	const other_t *	operator->( void ) const { return m_other->m_self; }
	other_t *		operator->( void ) { return m_other->m_self; }
public:
	bool			IsValid( void ) const { return m_other != NULL; }
	bool			IsConnected(const mtlDuplex<other_t, self_t> &b) const { return this != b.m_other; }
	bool			Debug_IsBroken( void ) const;
	void			Connect(mtlDuplex<other_t, self_t> &b);
	void			Disconnect( void );
	other_t *		GetOther( void ) { return m_other->m_self; }
	const other_t *	GetOther( void ) const { return m_other->m_self; }
};

template < typename self_t, typename other_t >
bool mtlDuplex<self_t, other_t>::Debug_IsBroken( void ) const
{
	if (!IsValid()) { return false; }
	return (this == m_other->m_other && m_other != m_other->m_self) || (m_other == m_other->m_self && this != m_other->m_other);
}

template < typename self_t, typename other_t >
void mtlDuplex<self_t, other_t>::Connect(mtlDuplex<other_t, self_t> &b)
{
	if (IsConnected(b)) { return; }
	Disconnect();
	b.Disconnect();
	m_other = &b;
	b.m_other = this;
	OnConnect();
	m_other->OnConnect();
}

template < typename self_t, typename other_t >
void mtlDuplex<self_t, other_t>::Disconnect( void )
{
	if (IsValid()) {
		OnDisconnect();
		m_other->OnDisconnect();
		m_other->m_other = NULL;
		m_other = NULL;
	}
}

#endif
