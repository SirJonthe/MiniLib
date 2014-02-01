#ifndef MTL_LIST_H_INCLUDED__
#define MTL_LIST_H_INCLUDED__

#include <cstddef>

template < typename type_t > class mtlList;

template < typename type_t >
class mtlNode
{
	friend class mtlList<type_t>;
private:
	mtlList<type_t>	*m_parent;
	mtlNode<type_t>	*m_next;
	mtlNode<type_t>	*m_prev;
private:
					mtlNode( void ); // not allowed to create stack instance
					mtlNode(const mtlNode<type_t>&) {}
	mtlNode<type_t>	&operator=(const mtlNode<type_t>&) { return *this; }
					mtlNode(const type_t &p_value, mtlList<type_t> *p_parent, mtlNode<type_t> *p_next, mtlNode<type_t> *p_prev);
public:
	type_t value;
public:
	inline explicit					mtlNode(const type_t &p_value);
	inline							~mtlNode( void );
	inline const mtlList<type_t>	*GetParent( void ) const;
	inline mtlList<type_t>			*GetParent( void );
	inline const mtlNode<type_t>	*GetNext( void ) const;
	inline mtlNode<type_t>			*GetNext( void );
	inline const mtlNode<type_t>	*GetPrev( void ) const;
	inline mtlNode<type_t>			*GetPrev( void );
	inline mtlNode<type_t>			*Remove( void );
};

template < typename type_t >
class mtlList
{
private:
	mtlNode<type_t>	*m_front;
	mtlNode<type_t>	*m_back;
	int				m_size;
private:
					mtlList(const mtlList<type_t>&) {}
	mtlList<type_t>	&operator=(const mtlList<type_t>&) { return *this; }
public:
	inline							mtlList( void );
	inline							~mtlList( void );
	void							PushBack(const type_t &p_value);
	void							PushFront(const type_t &p_value);
	void							PopBack( void );
	void							PopFront( void );
	mtlNode<type_t>					*Insert(const type_t &p_value, mtlNode<type_t> *p_node);
	mtlNode<type_t>					*Insert(mtlList<type_t> &p_list, mtlNode<type_t> *p_node);
	mtlNode<type_t>					*Remove(mtlNode<type_t> *p_node);
	void							Split(mtlNode<type_t> *p_begin, int p_num, mtlList<type_t> &p_out);
	static void						Swap(mtlNode<type_t> *p_a, mtlNode<type_t> *p_b);
	void							Promote(mtlNode<type_t> *p_node);
	void							Demote(mtlNode<type_t> *p_node);
	void							Copy(const mtlList<type_t> &p_list);
	void							Free( void );
	inline int						GetSize( void ) const;
	inline mtlNode<type_t>			*GetBack( void );
	inline const mtlNode<type_t>	*GetBack( void ) const;
	inline mtlNode<type_t>			*GetFront( void );
	inline const mtlNode<type_t>	*GetFront( void ) const;
};

template < typename type_t >
mtlNode<type_t>::mtlNode(const type_t &p_value, mtlList<type_t> *p_parent, mtlNode<type_t> *p_next, mtlNode<type_t> *p_prev) :
m_parent(p_parent), m_next(p_next), m_prev(p_prev), value(p_value)
{
	if (m_prev != NULL) { m_prev->m_next = this; }
	if (m_next != NULL) { m_next->m_prev = this; }
}

template < typename type_t >
mtlNode<type_t>::mtlNode( void ) :
m_parent(NULL), m_next(NULL), m_prev(NULL)
{}

template < typename type_t >
mtlNode<type_t>::mtlNode(const type_t &p_value) :
m_parent(NULL), m_next(NULL), m_prev(NULL), value(p_value)
{}

template < typename type_t >
mtlNode<type_t>::~mtlNode( void )
{
	m_parent = NULL;
	delete m_next;
}

template < typename type_t >
const mtlList<type_t> *mtlNode<type_t>::GetParent( void ) const
{
	return m_parent;
}

template < typename type_t >
mtlList<type_t> *mtlNode<type_t>::GetParent( void )
{
	return m_parent;
}

template < typename type_t >
const mtlNode<type_t> *mtlNode<type_t>::GetNext( void ) const
{
	return m_next;
}

template < typename type_t >
mtlNode<type_t> *mtlNode<type_t>::GetNext( void )
{
	return m_next;
}

template < typename type_t >
const mtlNode<type_t> *mtlNode<type_t>::GetPrev( void ) const
{
	return m_prev;
}

template < typename type_t >
mtlNode<type_t> *mtlNode<type_t>::GetPrev( void )
{
	return m_prev;
}

template < typename type_t >
mtlNode<type_t> *mtlNode<type_t>::Remove( void )
{
	return m_parent->Remove(this);
}

template < typename type_t >
mtlList<type_t>::mtlList( void ) :
m_front(NULL), m_back(NULL), m_size(0)
{}

template < typename type_t >
mtlList<type_t>::~mtlList( void )
{
	delete m_front;
}

template < typename type_t >
void mtlList<type_t>::PushBack(const type_t &p_value)
{
	mtlNode<type_t> *node = new mtlNode<type_t>(p_value, this, NULL, m_back);
	m_back = node;
	if (m_front == NULL) { m_front = m_back; }
	++m_size;
}

template < typename type_t >
void mtlList<type_t>::PushFront(const type_t &p_value)
{
	mtlNode<type_t> *node = new mtlNode<type_t>(p_value, this, m_front, NULL);
	m_front = node;
	if (m_back == NULL) { m_back = m_front; }
	++m_size;
}

template < typename type_t >
void mtlList<type_t>::PopBack( void )
{
	mtlNode<type_t> *node = m_back;
	m_back = m_back->m_prev;
	if (m_back == NULL) { m_front = NULL; }
	node->m_next = NULL;
	delete node;
	--m_size;
}

template < typename type_t >
void mtlList<type_t>::PopFront( void )
{
	mtlNode<type_t> *node = m_front;
	m_front = m_front->m_next;
	if (m_front == NULL) { m_back = NULL; }
	node->m_next = NULL;
	delete node;
	--m_size;
}

template < typename type_t >
mtlNode<type_t> *mtlList<type_t>::Insert(const type_t &p_value, mtlNode<type_t> *p_node)
{
	if (p_node == NULL) {
		PushBack(p_value);
		return m_back;
	} else if (p_node->m_parent != this) {
		return NULL;
	} else if (p_node == m_front) {
		PushFront(p_value);
		return m_front;
	}
	mtlNode<type_t> *node = new mtlNode<type_t>(p_value, this, p_node, p_node->m_prev);
	++m_size;
	return p_node->m_prev;
}

template < typename type_t >
mtlNode<type_t> *mtlList<type_t>::Insert(mtlList<type_t> &p_list, mtlNode<type_t> *p_node)
{
	mtlNode<type_t> *retNode = p_list.m_front;
	if (p_node == NULL) {
		if (m_front == NULL) {
			m_front = p_list->m_front;
			m_back = p_list->m_back;
		} else {
			m_back->m_next = p_list.m_front;
			m_back = p_list.m_back;
		}
	} else if (p_node->m_parent != this) {
		return NULL;
	} else if (p_node == m_front) {
		m_front->m_prev = p_list.m_back;
		p_list.m_back->m_next = m_front;
		m_front = p_list.m_front;
	} else {
		p_node->m_next = p_list.m_front;
		p_list.m_front->m_prev = p_node;
		p_list.m_back->m_next = p_node->m_next;
	}
	m_size += p_list.m_size;
	p_list.m_front = p_list->m_back = NULL;
	p_list.m_size = 0;
	return retNode;
}

template < typename type_t >
mtlNode<type_t> *mtlList<type_t>::Remove(mtlNode<type_t> *p_node)
{
	if (p_node->m_parent != this) { return NULL; }
	mtlNode<type_t> *next = p_node->m_next;
	if (p_node == m_back) { m_back = m_back->m_prev; }
	if (p_node == m_front) { m_front = m_front->m_next; }
	if (p_node->m_next != NULL) { p_node->m_next->m_prev = p_node->m_prev; }
	if (p_node->m_prev != NULL) { p_node->m_prev->m_next = p_node->m_next; }
	p_node->m_next = NULL;
	delete p_node;
	--m_size;
	return next;
}

template < typename type_t >
void mtlList<type_t>::Split(mtlNode<type_t> *p_begin, int p_num, mtlList<type_t> &p_out)
{
	if (p_num <= 0 || p_begin->m_parent != this || &p_out == this) { return; }

	mtlNode<type_t> *end = p_begin;
	end->m_parent = &p_out;
	for (int i = 1; i < p_num; ++i) {
		end = end->GetNext();
		end->m_parent = &p_out;
	}
	
	delete p_out.m_front;

	p_out.m_front = p_begin;
	p_out.m_back = end;
	if (p_begin == m_front) { m_front = end->m_next; }
	if (end == m_back) { m_back = p_begin->m_prev; }
	
	if (p_begin->m_prev != NULL) { p_begin->m_prev->m_next = end->m_next; }
	p_begin->m_prev = NULL;
	if (end->m_next != NULL) { end->m_next->m_prev = p_begin->m_prev; }
	end->m_next = NULL;

	p_out.m_size = p_num;
	m_size -= p_num;
}

template < typename type_t >
void mtlList<type_t>::Swap(mtlNode<type_t> *p_a, mtlNode<type_t> *p_b)
{
	if (p_a == p_a->m_parent->m_front) { p_a->m_parent->m_front = p_b; }
	if (p_a == p_a->m_parent->m_back) { p_a->m_parent->m_back = p_b; }
	if (p_b == p_b->m_parent->m_front) { p_b->m_parent->m_front = p_a; }
	if (p_b == p_b->m_parent->m_back) { p_b->m_parent->m_back = p_a; }
	if (p_a->m_next != NULL) { p_a->m_next->m_prev = p_b; }
	if (p_a->m_prev != NULL) { p_a->m_prev->m_next = p_b; }
	if (p_b->m_next != NULL) { p_b->m_next->m_prev = p_a; }
	if (p_b->m_prev != NULL) { p_b->m_prev->m_next = p_a; }
	mtlNode<type_t> *tempNode = p_a->m_next;
	p_a->m_next = p_b->m_next;
	p_b->m_next = tempNode;
	tempNode = p_a->m_prev;
	p_a->m_prev = p_b->m_prev;
	p_b->m_prev = tempNode;
	mtlList<type_t> *tempParent = p_a->m_parent;
	p_a->m_parent = p_b->m_parent;
	p_b->m_parent = tempParent;
}

template < typename type_t >
void mtlList<type_t>::Promote(mtlNode<type_t> *p_node)
{
	if (p_node->m_parent != this) { return; }
	if (p_node == m_back)		{ m_back = p_node->m_prev; }
	if (p_node->m_next != NULL)	{ p_node->m_next->m_prev = p_node->m_prev; }
	if (p_node->m_prev != NULL)	{ p_node->m_prev->m_next = p_node->m_next; }
	m_front->m_prev = p_node;
	p_node->m_next = m_front;
	p_node->m_prev = NULL;
	m_front = p_node;
}

template < typename type_t >
void mtlList<type_t>::Demote(mtlNode<type_t> *p_node)
{
	if (p_node->m_parent != this) { return; }
	if (p_node == m_front)		{ m_front = m_front->m_next; }
	if (p_node->m_next != NULL)	{ p_node->m_next->m_prev = p_node->m_prev; }
	if (p_node->m_prev != NULL)	{ p_node->m_prev->m_next = p_node->m_next; }
	m_back->m_next = p_node;
	p_node->m_next = NULL;
	p_node->m_prev = m_back;
	m_back = p_node;
}

template < typename type_t >
void mtlList<type_t>::Copy(const mtlList<type_t> &p_list)
{
	if (this != &p_list) {
		delete m_front;
		m_size = p_list.m_size;
		mtlNode<type_t> *src = p_list.m_front;
		if (src != NULL) {
			m_back = m_front = new mtlNode<type_t>(src->value, this, NULL, NULL);
			src = src->m_next;
			while (src != NULL) {
				m_back = new mtlNode<type_t>(src->value, this, NULL, m_back);
				src = src->m_next;
			}
		}
	}
}

template < typename type_t >
void mtlList<type_t>::Free( void )
{
	delete m_front;
	m_front = m_back = NULL;
	m_size = 0;
}

template < typename type_t >
int mtlList<type_t>::GetSize( void ) const
{
	return m_size;
}

template < typename type_t>
mtlNode<type_t> *mtlList<type_t>::GetBack( void )
{
	return m_back;
}

template < typename type_t>
const mtlNode<type_t> *mtlList<type_t>::GetBack( void ) const
{
	return m_back;
}

template < typename type_t>
mtlNode<type_t> *mtlList<type_t>::GetFront( void )
{
	return m_front;
}

template < typename type_t>
const mtlNode<type_t> *mtlList<type_t>::GetFront( void ) const
{
	return m_front;
}

#endif
