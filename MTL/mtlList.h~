#ifndef MTL_LIST_H_INCLUDED__
#define MTL_LIST_H_INCLUDED__

#include <cstddef>

// put mtlNode inside mtlList
// add Merge sort
// Insert

template < typename type_t > class mtlList;

template < typename type_t >
class mtlNode
{
	friend class mtlList<type_t>;
private:
	type_t			m_item;
	mtlList<type_t>	*m_parent;
	mtlNode<type_t>	*m_next;
	mtlNode<type_t>	*m_prev;
private:
					mtlNode( void ); // not allowed to create stack instance
					mtlNode(const mtlNode<type_t>&) {}
	mtlNode<type_t>	&operator=(const mtlNode<type_t>&) { return *this; }
					mtlNode(const type_t &p_item, mtlList<type_t> *p_parent, mtlNode<type_t> *p_next, mtlNode<type_t> *p_prev);
					~mtlNode( void );
public:
	inline explicit					mtlNode(const type_t &p_item);
	inline const mtlList<type_t>	*GetParent( void ) const;
	inline mtlList<type_t>			*GetParent( void );
	inline const mtlNode<type_t>	*GetNext( void ) const;
	inline mtlNode<type_t>			*GetNext( void );
	inline const mtlNode<type_t>	*GetPrev( void ) const;
	inline mtlNode<type_t>			*GetPrev( void );
	inline mtlNode<type_t>			*Remove( void );
	inline const type_t				&GetItem( void ) const;
	inline type_t					&GetItem( void );
	inline void						Insert(const type_t *p_item) { m_parent->Insert(p_item); }
};

template < typename type_t >
class mtlList
{
private:
	mtlNode<type_t>	*m_first;
	mtlNode<type_t>	*m_last;
	int				m_size;
private:
					mtlList(const mtlList<type_t>&) {}
	mtlList<type_t>	&operator=(const mtlList<type_t>&) { return *this; }
public:
	inline							mtlList( void );
	inline							~mtlList( void );
	void							AddLast(const type_t &p_value);
	void							AddFirst(const type_t &p_value);
	void							RemoveLast( void );
	void							RemoveFirst( void );
	mtlNode<type_t>					*Insert(const type_t &p_value, mtlNode<type_t> *p_node);
	mtlNode<type_t>					*Insert(mtlList<type_t> &p_list, mtlNode<type_t> *p_node);
	mtlNode<type_t>					*Remove(mtlNode<type_t> *p_node);
	void							Split(mtlNode<type_t> *p_begin, int p_num, mtlList<type_t> &p_out);
	static void						Swap(mtlNode<type_t> *p_a, mtlNode<type_t> *p_b);
	void							Promote(mtlNode<type_t> *p_node);
	void							Demote(mtlNode<type_t> *p_node);
	void							Copy(const mtlList<type_t> &p_list);
	void							RemoveAll( void );
	inline int						GetSize( void ) const;
	inline mtlNode<type_t>			*GetLast( void );
	inline const mtlNode<type_t>	*GetLast( void ) const;
	inline mtlNode<type_t>			*GetFirst( void );
	inline const mtlNode<type_t>	*GetFirst( void ) const;
};

template < typename type_t >
mtlNode<type_t>::mtlNode( void ) :
m_parent(NULL), m_next(NULL), m_prev(NULL)
{}

template < typename type_t >
mtlNode<type_t>::mtlNode(const type_t &p_item, mtlList<type_t> *p_parent, mtlNode<type_t> *p_next, mtlNode<type_t> *p_prev) :
m_parent(p_parent), m_next(p_next), m_prev(p_prev), m_item(p_item)
{
	if (m_prev != NULL) { m_prev->m_next = this; }
	if (m_next != NULL) { m_next->m_prev = this; }
}

template < typename type_t >
mtlNode<type_t>::~mtlNode( void )
{
}

template < typename type_t >
mtlNode<type_t>::mtlNode(const type_t &p_item) :
m_parent(NULL), m_next(NULL), m_prev(NULL), m_item(p_item)
{}

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
const type_t &mtlNode<type_t>::GetItem( void ) const
{
	return m_item;
}

template < typename type_t >
type_t &mtlNode<type_t>::GetItem( void )
{
	return m_item;
}

template < typename type_t >
mtlList<type_t>::mtlList( void ) :
m_first(NULL), m_last(NULL), m_size(0)
{}

template < typename type_t >
mtlList<type_t>::~mtlList( void )
{
	RemoveAll();
}

template < typename type_t >
void mtlList<type_t>::AddLast(const type_t &p_value)
{
	mtlNode<type_t> *node = new mtlNode<type_t>(p_value, this, NULL, m_last);
	m_last = node;
	if (m_first == NULL) { m_first = m_last; }
	++m_size;
}

template < typename type_t >
void mtlList<type_t>::AddFirst(const type_t &p_value)
{
	mtlNode<type_t> *node = new mtlNode<type_t>(p_value, this, m_first, NULL);
	m_first = node;
	if (m_last == NULL) { m_last = m_first; }
	++m_size;
}

template < typename type_t >
void mtlList<type_t>::RemoveLast( void )
{
	mtlNode<type_t> *node = m_last;
	m_last = m_last->m_prev;
	if (m_last == NULL) { m_first = NULL; }
	delete node;
	--m_size;
}

template < typename type_t >
void mtlList<type_t>::RemoveFirst( void )
{
	mtlNode<type_t> *node = m_first;
	m_first = m_first->m_next;
	if (m_first == NULL) { m_last = NULL; }
	delete node;
	--m_size;
}

template < typename type_t >
mtlNode<type_t> *mtlList<type_t>::Insert(const type_t &p_value, mtlNode<type_t> *p_node)
{
	if (p_node == NULL) {
		AddLast(p_value);
		return m_last;
	} else if (p_node->m_parent != this) {
		return NULL;
	} else if (p_node == m_first) {
		AddFirst(p_value);
		return m_first;
	}
	mtlNode<type_t> *node = new mtlNode<type_t>(p_value, this, p_node, p_node->m_prev);
	++m_size;
	return p_node->m_prev;
}

template < typename type_t >
mtlNode<type_t> *mtlList<type_t>::Insert(mtlList<type_t> &p_list, mtlNode<type_t> *p_node)
{
	mtlNode<type_t> *retNode = p_list.m_first;
	if (p_node == NULL) {
		if (m_first == NULL) {
			m_first = p_list->m_first;
			m_last = p_list->m_last;
		} else {
			m_last->m_next = p_list.m_first;
			m_last = p_list.m_last;
		}
	} else if (p_node->m_parent != this) {
		return NULL;
	} else if (p_node == m_first) {
		m_first->m_prev = p_list.m_last;
		p_list.m_last->m_next = m_first;
		m_first = p_list.m_first;
	} else {
		p_node->m_next = p_list.m_first;
		p_list.m_first->m_prev = p_node;
		p_list.m_last->m_next = p_node->m_next;
	}
	m_size += p_list.m_size;
	p_list.m_first = p_list->m_last = NULL;
	p_list.m_size = 0;
	return retNode;
}

template < typename type_t >
mtlNode<type_t> *mtlList<type_t>::Remove(mtlNode<type_t> *p_node)
{
	if (p_node->m_parent != this) { return NULL; }
	mtlNode<type_t> *next = p_node->m_next;
	if (p_node == m_last) { m_last = m_last->m_prev; }
	if (p_node == m_first) { m_first = m_first->m_next; }
	if (p_node->m_next != NULL) { p_node->m_next->m_prev = p_node->m_prev; }
	if (p_node->m_prev != NULL) { p_node->m_prev->m_next = p_node->m_next; }
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
	
	p_out.Free();

	p_out.m_first = p_begin;
	p_out.m_last = end;
	if (p_begin == m_first) { m_first = end->m_next; }
	if (end == m_last) { m_last = p_begin->m_prev; }
	
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
	if (p_a == p_a->m_parent->m_first) { p_a->m_parent->m_first = p_b; }
	if (p_a == p_a->m_parent->m_last) { p_a->m_parent->m_last = p_b; }
	if (p_b == p_b->m_parent->m_first) { p_b->m_parent->m_first = p_a; }
	if (p_b == p_b->m_parent->m_last) { p_b->m_parent->m_last = p_a; }
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
	if (p_node == m_last)		{ m_last = p_node->m_prev; }
	if (p_node->m_next != NULL)	{ p_node->m_next->m_prev = p_node->m_prev; }
	if (p_node->m_prev != NULL)	{ p_node->m_prev->m_next = p_node->m_next; }
	m_first->m_prev = p_node;
	p_node->m_next = m_first;
	p_node->m_prev = NULL;
	m_first = p_node;
}

template < typename type_t >
void mtlList<type_t>::Demote(mtlNode<type_t> *p_node)
{
	if (p_node->m_parent != this) { return; }
	if (p_node == m_first)		{ m_first = m_first->m_next; }
	if (p_node->m_next != NULL)	{ p_node->m_next->m_prev = p_node->m_prev; }
	if (p_node->m_prev != NULL)	{ p_node->m_prev->m_next = p_node->m_next; }
	m_last->m_next = p_node;
	p_node->m_next = NULL;
	p_node->m_prev = m_last;
	m_last = p_node;
}

template < typename type_t >
void mtlList<type_t>::Copy(const mtlList<type_t> &p_list)
{
	if (this != &p_list) {
		RemoveAll();
		m_size = p_list.m_size;
		mtlNode<type_t> *src = p_list.m_first;
		if (src != NULL) {
			m_last = m_first = new mtlNode<type_t>(src->value, this, NULL, NULL);
			src = src->m_next;
			while (src != NULL) {
				m_last = new mtlNode<type_t>(src->value, this, NULL, m_last);
				src = src->m_next;
			}
		}
	}
}

template < typename type_t >
void mtlList<type_t>::RemoveAll( void )
{
	mtlNode<type_t> *node = m_first;
	while (node != NULL) {
		mtlNode<type_t> *next = node->GetNext();
		delete node;
		node = next;
	}
	m_first = m_last = NULL;
	m_size = 0;
}

template < typename type_t >
int mtlList<type_t>::GetSize( void ) const
{
	return m_size;
}

template < typename type_t>
mtlNode<type_t> *mtlList<type_t>::GetLast( void )
{
	return m_last;
}

template < typename type_t>
const mtlNode<type_t> *mtlList<type_t>::GetLast( void ) const
{
	return m_last;
}

template < typename type_t>
mtlNode<type_t> *mtlList<type_t>::GetFirst( void )
{
	return m_first;
}

template < typename type_t>
const mtlNode<type_t> *mtlList<type_t>::GetFirst( void ) const
{
	return m_first;
}

#endif
