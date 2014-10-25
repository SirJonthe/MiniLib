#ifndef MTL_LIST_H_INCLUDED__
#define MTL_LIST_H_INCLUDED__

#include <cstddef>

template < typename type_t > class mtlList;

template < typename type_t >
class mtlItem
{
	friend class mtlList<type_t>;

private:
	type_t			m_item;
	mtlList<type_t>	*m_parent;
	mtlItem<type_t>	*m_next;
	mtlItem<type_t>	*m_prev;

private:
					mtlItem( void ); // not allowed to create stack instance
					mtlItem(const mtlItem<type_t>&) {}
	mtlItem<type_t>	&operator=(const mtlItem<type_t>&) { return *this; }
					mtlItem(const type_t &p_item, mtlList<type_t> *p_parent, mtlItem<type_t> *p_next, mtlItem<type_t> *p_prev);

public:
	inline explicit					mtlItem(const type_t &p_item);
	inline const mtlList<type_t>	*GetParent( void ) const;
	inline mtlList<type_t>			*GetParent( void );
	inline const mtlItem<type_t>	*GetNext( void ) const;
	inline mtlItem<type_t>			*GetNext( void );
	inline const mtlItem<type_t>	*GetPrev( void ) const;
	inline mtlItem<type_t>			*GetPrev( void );
	inline mtlItem<type_t>			*Remove( void );
	inline const type_t				&GetItem( void ) const;
	inline type_t					&GetItem( void );
	inline mtlItem<type_t>			*Insert(const type_t &p_data);
	inline mtlItem<type_t>			*Insert(mtlItem<type_t> *p_item);
	inline mtlItem<type_t>			*Insert(mtlList<type_t> &src);
	inline mtlItem<type_t>			*Transfer(mtlList<type_t> &dst, mtlItem<type_t> *at);
};

template < typename type_t >
class mtlList
{
private:
	mtlItem<type_t>	*m_first;
	mtlItem<type_t>	*m_last;
	int				m_size;
private:
					mtlList(const mtlList<type_t>&) {}
	mtlList<type_t>	&operator=(const mtlList<type_t>&) { return *this; }

public:
	inline							mtlList( void );
	inline							~mtlList( void );
	void							AddLast(const type_t &p_value);
	type_t							&AddLast( void );
	void							AddFirst(const type_t &p_value);
	type_t							&AddFirst( void );
	void							RemoveLast( void );
	void							RemoveFirst( void );
	mtlItem<type_t>					*Insert(const type_t &p_value, mtlItem<type_t> *p_at);
	mtlItem<type_t>					*Insert(mtlList<type_t> &p_list, mtlItem<type_t> *p_at);
	mtlItem<type_t>					*Insert(mtlItem<type_t> *p_item, mtlItem<type_t> *p_at);
	mtlItem<type_t>					*InsertSort(const type_t &p_value);
	mtlItem<type_t>					*InsertSort(mtlItem<type_t> *p_item);
	mtlItem<type_t>					*Remove(mtlItem<type_t> *p_item);
	void							Split(mtlItem<type_t> *p_begin, int p_num, mtlList<type_t> &p_out);
	static void						Swap(mtlItem<type_t> *p_a, mtlItem<type_t> *p_b);
	void							Promote(mtlItem<type_t> *p_item);
	void							Demote(mtlItem<type_t> *p_item);
	void							Copy(const mtlList<type_t> &p_list);
	void							RemoveAll( void );
	inline int						GetSize( void ) const;
	inline mtlItem<type_t>			*GetLast( void );
	inline const mtlItem<type_t>	*GetLast( void ) const;
	inline mtlItem<type_t>			*GetFirst( void );
	inline const mtlItem<type_t>	*GetFirst( void ) const;
};

template < typename type_t >
mtlItem<type_t>::mtlItem( void ) :
m_item(), m_parent(NULL), m_next(NULL), m_prev(NULL)
{}

template < typename type_t >
mtlItem<type_t>::mtlItem(const type_t &p_item, mtlList<type_t> *p_parent, mtlItem<type_t> *p_next, mtlItem<type_t> *p_prev) :
m_item(p_item), m_parent(p_parent), m_next(p_next), m_prev(p_prev)
{
	if (m_prev != NULL) { m_prev->m_next = this; }
	if (m_next != NULL) { m_next->m_prev = this; }
}

template < typename type_t >
mtlItem<type_t>::mtlItem(const type_t &p_item) :
m_item(p_item), m_parent(NULL), m_next(NULL), m_prev(NULL)
{}

template < typename type_t >
const mtlList<type_t> *mtlItem<type_t>::GetParent( void ) const
{
	return m_parent;
}

template < typename type_t >
mtlList<type_t> *mtlItem<type_t>::GetParent( void )
{
	return m_parent;
}

template < typename type_t >
const mtlItem<type_t> *mtlItem<type_t>::GetNext( void ) const
{
	return m_next;
}

template < typename type_t >
mtlItem<type_t> *mtlItem<type_t>::GetNext( void )
{
	return m_next;
}

template < typename type_t >
const mtlItem<type_t> *mtlItem<type_t>::GetPrev( void ) const
{
	return m_prev;
}

template < typename type_t >
mtlItem<type_t> *mtlItem<type_t>::GetPrev( void )
{
	return m_prev;
}

template < typename type_t >
mtlItem<type_t> *mtlItem<type_t>::Remove( void )
{
	return m_parent->Remove(this);
}

template < typename type_t >
const type_t &mtlItem<type_t>::GetItem( void ) const
{
	return m_item;
}

template < typename type_t >
type_t &mtlItem<type_t>::GetItem( void )
{
	return m_item;
}

template < typename type_t >
mtlItem<type_t> *mtlItem<type_t>::Insert(const type_t &p_data)
{
	return m_parent->Insert(p_data);
}

template < typename type_t >
mtlItem<type_t> *mtlItem<type_t>::Insert(mtlItem<type_t> *p_item)
{
	return m_parent->Insert(p_item);
}

template < typename type_t >
mtlItem<type_t> *mtlItem<type_t>::Insert(mtlList<type_t> &src)
{
	return m_parent->Insert(src);
}

template < typename type_t >
mtlItem<type_t> *mtlItem<type_t>::Transfer(mtlList<type_t> &dst, mtlItem<type_t> *at)
{
	mtlItem<type_t> *next = GetNext();
	dst.Insert(this, at);
	return next;
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
	mtlItem<type_t> *node = new mtlItem<type_t>(p_value, this, NULL, m_last);
	m_last = node;
	if (m_first == NULL) { m_first = m_last; }
	++m_size;
}

template < typename type_t >
type_t &mtlList<type_t>::AddLast( void )
{
	mtlItem<type_t> *node = new mtlItem<type_t>;
	node->m_parent = this;
	node->m_next = NULL;
	node->m_prev = m_last;
	m_last = node;
	if (m_first == NULL) { m_first = m_last; }
	++m_size;
	return node->m_item;
}

template < typename type_t >
void mtlList<type_t>::AddFirst(const type_t &p_value)
{
	mtlItem<type_t> *node = new mtlItem<type_t>(p_value, this, m_first, NULL);
	m_first = node;
	if (m_last == NULL) { m_last = m_first; }
	++m_size;
}

template < typename type_t >
type_t &mtlList<type_t>::AddFirst( void )
{
	mtlItem<type_t> *node = new mtlItem<type_t>;
	node->m_parent = this;
	node->m_next = m_first;
	node->m_prev = NULL;
	m_first = node;
	if (m_last == NULL) { m_last = m_first; }
	++m_size;
	return node->m_item;
}

template < typename type_t >
void mtlList<type_t>::RemoveLast( void )
{
	mtlItem<type_t> *node = m_last;
	m_last = m_last->m_prev;
	if (m_last == NULL) { m_first = NULL; }
	delete node;
	--m_size;
}

template < typename type_t >
void mtlList<type_t>::RemoveFirst( void )
{
	mtlItem<type_t> *node = m_first;
	m_first = m_first->m_next;
	if (m_first == NULL) { m_last = NULL; }
	delete node;
	--m_size;
}

template < typename type_t >
mtlItem<type_t> *mtlList<type_t>::Insert(const type_t &p_value, mtlItem<type_t> *p_at)
{
	if (p_at == NULL) {
		AddLast(p_value);
		return m_last;
	} else if (p_at->m_parent != this) {
		return NULL;
	} else if (p_at == m_first) {
		AddFirst(p_value);
		return m_first;
	}
	mtlItem<type_t> *node = new mtlItem<type_t>(p_value, this, p_at, p_at->m_prev); // Looks like error, but list now references this memory
	++m_size;
	return p_at->m_prev;
}

template < typename type_t >
mtlItem<type_t> *mtlList<type_t>::Insert(mtlList<type_t> &p_list, mtlItem<type_t> *p_at)
{
	mtlItem<type_t> *retNode = p_list.m_first;
	if (p_at == NULL) {
		if (m_first == NULL) {
			m_first = p_list->m_first;
			m_last = p_list->m_last;
		} else {
			m_last->m_next = p_list.m_first;
			m_last = p_list.m_last;
		}
	} else if (p_at->m_parent != this) {
		return NULL;
	} else if (p_at == m_first) {
		m_first->m_prev = p_list.m_last;
		p_list.m_last->m_next = m_first;
		m_first = p_list.m_first;
	} else {
		p_at->m_next = p_list.m_first;
		p_list.m_first->m_prev = p_at;
		p_list.m_last->m_next = p_at->m_next;
	}
	m_size += p_list.m_size;
	p_list.m_first = p_list->m_last = NULL;
	p_list.m_size = 0;
	return retNode;
}

template < typename type_t >
mtlItem<type_t> *mtlList<type_t>::Insert(mtlItem<type_t> *p_item, mtlItem<type_t> *p_at)
{
	mtlList<type_t> out;
	p_item->GetParent()->Split(p_item, 1, out);
	return Insert(out, p_at);
}

template < typename type_t >
mtlItem<type_t> *mtlList<type_t>::InsertSort(const type_t &p_value)
{
	mtlItem<type_t> *node = m_first;
	while (node != NULL && node->m_item < p_value) {
		node = node->m_next;
	}
	return Insert(p_value, node);
}

template < typename type_t >
mtlItem<type_t> *mtlList<type_t>::InsertSort(mtlItem<type_t> *p_item)
{
	mtlItem<type_t> *node = m_first;
	while (node != NULL && node->m_item < p_item->m_item) {
		node = node->GetNext();
	}
	return Insert(p_item, node);
}

template < typename type_t >
mtlItem<type_t> *mtlList<type_t>::Remove(mtlItem<type_t> *p_item)
{
	if (p_item->m_parent != this) { return NULL; }
	mtlItem<type_t> *next = p_item->m_next;
	if (p_item == m_last) { m_last = m_last->m_prev; }
	if (p_item == m_first) { m_first = m_first->m_next; }
	if (p_item->m_next != NULL) { p_item->m_next->m_prev = p_item->m_prev; }
	if (p_item->m_prev != NULL) { p_item->m_prev->m_next = p_item->m_next; }
	delete p_item;
	--m_size;
	return next;
}

template < typename type_t >
void mtlList<type_t>::Split(mtlItem<type_t> *p_begin, int p_num, mtlList<type_t> &p_out)
{
	if (p_num <= 0 || p_begin->m_parent != this || &p_out == this) { return; }

	mtlItem<type_t> *end = p_begin;
	end->m_parent = &p_out;
	for (int i = 1; i < p_num; ++i) {
		end = end->GetNext();
		end->m_parent = &p_out;
	}
	
	p_out.RemoveAll();

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
void mtlList<type_t>::Swap(mtlItem<type_t> *p_a, mtlItem<type_t> *p_b)
{
	if (p_a == p_a->m_parent->m_first) { p_a->m_parent->m_first = p_b; }
	if (p_a == p_a->m_parent->m_last) { p_a->m_parent->m_last = p_b; }
	if (p_b == p_b->m_parent->m_first) { p_b->m_parent->m_first = p_a; }
	if (p_b == p_b->m_parent->m_last) { p_b->m_parent->m_last = p_a; }
	if (p_a->m_next != NULL) { p_a->m_next->m_prev = p_b; }
	if (p_a->m_prev != NULL) { p_a->m_prev->m_next = p_b; }
	if (p_b->m_next != NULL) { p_b->m_next->m_prev = p_a; }
	if (p_b->m_prev != NULL) { p_b->m_prev->m_next = p_a; }
	mtlItem<type_t> *tempNode = p_a->m_next;
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
void mtlList<type_t>::Promote(mtlItem<type_t> *p_item)
{
	if (p_item->m_parent != this) { return; }
	if (p_item == m_last)		{ m_last = p_item->m_prev; }
	if (p_item->m_next != NULL)	{ p_item->m_next->m_prev = p_item->m_prev; }
	if (p_item->m_prev != NULL)	{ p_item->m_prev->m_next = p_item->m_next; }
	m_first->m_prev = p_item;
	p_item->m_next = m_first;
	p_item->m_prev = NULL;
	m_first = p_item;
}

template < typename type_t >
void mtlList<type_t>::Demote(mtlItem<type_t> *p_item)
{
	if (p_item->m_parent != this) { return; }
	if (p_item == m_first)		{ m_first = m_first->m_next; }
	if (p_item->m_next != NULL)	{ p_item->m_next->m_prev = p_item->m_prev; }
	if (p_item->m_prev != NULL)	{ p_item->m_prev->m_next = p_item->m_next; }
	m_last->m_next = p_item;
	p_item->m_next = NULL;
	p_item->m_prev = m_last;
	m_last = p_item;
}

template < typename type_t >
void mtlList<type_t>::Copy(const mtlList<type_t> &p_list)
{
	if (this != &p_list) {
		RemoveAll();
		m_size = p_list.m_size;
		mtlItem<type_t> *src = p_list.m_first;
		if (src != NULL) {
			m_last = m_first = new mtlItem<type_t>(src->value, this, NULL, NULL);
			src = src->m_next;
			while (src != NULL) {
				m_last = new mtlItem<type_t>(src->value, this, NULL, m_last);
				src = src->m_next;
			}
		}
	}
}

template < typename type_t >
void mtlList<type_t>::RemoveAll( void )
{
	mtlItem<type_t> *node = m_first;
	while (node != NULL) {
		mtlItem<type_t> *next = node->GetNext();
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
mtlItem<type_t> *mtlList<type_t>::GetLast( void )
{
	return m_last;
}

template < typename type_t>
const mtlItem<type_t> *mtlList<type_t>::GetLast( void ) const
{
	return m_last;
}

template < typename type_t>
mtlItem<type_t> *mtlList<type_t>::GetFirst( void )
{
	return m_first;
}

template < typename type_t>
const mtlItem<type_t> *mtlList<type_t>::GetFirst( void ) const
{
	return m_first;
}

#endif
