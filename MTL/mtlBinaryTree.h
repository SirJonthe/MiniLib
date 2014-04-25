#ifndef MTL_BINARY_TREE_H_INCLUDED__
#define MTL_BINARY_TREE_H_INCLUDED__

#include <cstdlib>
#include "mtlList.h"

template < typename type_t > class mtlBinaryTree;

template < typename type_t >
class mtlBranch
{
	friend class mtlBinaryTree<type_t>;
private:
	type_t				m_item;
	mtlBranch<type_t>	*m_parent;
	mtlBranch<type_t>	*m_positive;
	mtlBranch<type_t>	*m_negative;
private:
	mtlBranch(const type_t &item, mtlBranch<type_t> *parent);
	mtlBranch( void ) {}
	mtlBranch<type_t>		&operator=(const mtlBranch<type_t>&) { return *this; }
	int						GetDepth(const mtlBranch<type_t> *node, int currentDepth) const;
	int						GetHeight(const mtlBranch<type_t> *node, int currentHeight) const;
	const mtlBranch<type_t>	*Find(const mtlBranch<type_t> *node, const type_t &item, bool closest) const;
	const mtlBranch<type_t>	*FindMin(const mtlBranch<type_t> *node) const;
	const mtlBranch<type_t>	*FindMax(const mtlBranch<type_t> *node) const;
public:
	const type_t			&GetItem( void ) const { return m_item; }
	const mtlBranch<type_t>	*GetParent( void ) const { return m_parent; }
	const mtlBranch<type_t>	*GetPositive( void ) const { return m_positive; }
	const mtlBranch<type_t>	*GetNegative( void ) const { return m_negative; }
	int						GetDepth( void ) const { return GetDepth(this, 0); }
	int						GetHeight( void ) const { return GetHeight(this, 0); }
	const mtlBranch<type_t>	*Find(const type_t &item) const { return Find(this, item, false); }
	const mtlBranch<type_t>	*FindClosest(const type_t &item) const { return Find(this, item, true); }
	const mtlBranch<type_t>	*FindMin( void ) const { return FindMin(this); }
	const mtlBranch<type_t>	*FindMax( void ) const { return FindMax(this); }
};

template < typename type_t >
mtlBranch<type_t>::mtlBranch(const type_t &item, mtlBranch<type_t> *parent) :
m_item(item), m_parent(parent), m_positive(NULL), m_negative(NULL)
{}

template < typename type_t >
int mtlBranch<type_t>::GetDepth(const mtlBranch<type_t> *node, int currentDepth) const
{
	if (node == NULL) { return currentDepth; }
	return GetDepth(node->m_parent, currentDepth + 1);
}

template < typename type_t >
int mtlBranch<type_t>::GetHeight(const mtlBranch<type_t> *node, int currentHeight) const
{
	if (node == NULL) { return currentHeight; }
	const int pos = GetHeight(node->m_positive, currentHeight + 1);
	const int neg = GetHeight(node->m_negative, currentHeight + 1);
	return (pos > neg) ? pos : neg;
}

template < typename type_t >
const mtlBranch<type_t> *mtlBranch<type_t>::Find(const mtlBranch<type_t> *node, const type_t &item, bool closest) const
{
	if (node->m_item == item) { return node; }
	if (item < node->m_item) {
		if (node->m_negative != NULL) { return Find(node->m_negative, item); }
	} else {
		if (node->m_positive != NULL) { return Find(node->m_positive, item); }
	}
	return closest ? node : NULL;
}

template < typename type_t >
const mtlBranch<type_t> *mtlBranch<type_t>::FindMin(const mtlBranch<type_t> *node) const
{
	if (node->m_negative == NULL) { return this; }
	return FindMin(node->m_negative);
}

template < typename type_t >
const mtlBranch<type_t> *mtlBranch<type_t>::FindMax(const mtlBranch<type_t> *node) const
{
	if (node->m_positive == NULL) { return this; }
	return FindMax(node->m_positive);
}

template < typename type_t >
class mtlBinaryTree
{
private:
	mtlBranch<type_t>	*m_root;
	int					m_size;
private:
	mtlBinaryTree &operator=(const mtlBinaryTree&) { return *this; }
	mtlBinaryTree(const mtlBinaryTree&) {}
	const mtlBranch<type_t> *Insert(mtlBranch<type_t> *parent, mtlBranch<type_t> *&node, const type_t &item);
	void ToList(mtlList<type_t> &list, const mtlBranch<type_t> *node) const;
	void ToListReversed(mtlList<type_t> &list, const mtlBranch<type_t> *node) const;
public:
	mtlBinaryTree( void );
	~mtlBinaryTree( void );
	const mtlBranch<type_t> *GetRoot( void ) const { return m_root; }
	const mtlBranch<type_t>	*Insert(const type_t &item);
	void					Clear( void );
	int						GetSize( void ) const { return m_size; }
	void					ToList(mtlList<type_t> &list) const;
	void					ToListReversed(mtlList<type_t> &list) const;
	// is_balanced
	// rebalance
};

template < typename type_t >
const mtlBranch<type_t> *mtlBinaryTree<type_t>::Insert(mtlBranch<type_t> *parent, mtlBranch<type_t> *&node, const type_t &item)
{
	if (node == NULL) {
		node = new mtlBranch<type_t>(item, parent);
		++m_size;
		return node;
	}
	if (node->m_item == item) { return node; }
	if (item < node->m_item) { return Insert(node, node->m_negative, item); }
	return Insert(node, node->m_positive, item);
}

template < typename type_t >
void mtlBinaryTree<type_t>::ToList(mtlList<type_t> &list, const mtlBranch<type_t> *node) const
{
	if (node->m_negative != NULL) {
		ToList(list, node->m_negative);
	}
	list.AddBack(node->m_item);
	if (node->m_positive != NULL) {
		ToList(list, node->m_positive);
	}
}

template < typename type_t >
void mtlBinaryTree<type_t>::ToListReversed(mtlList<type_t> &list, const mtlBranch<type_t> *node) const
{
	if (node->m_positive != NULL) {
		ToList(list, node->m_positive);
	}
	list.AddBack(node->m_item);
	if (node->m_negative != NULL) {
		ToList(list, node->m_negative);
	}
}

template < typename type_t >
mtlBinaryTree<type_t>::mtlBinaryTree( void ) :
m_root(NULL), m_size(0)
{}

template < typename type_t >
mtlBinaryTree<type_t>::~mtlBinaryTree( void )
{
	delete m_root;
}

template < typename type_t >
const mtlBranch<type_t> *mtlBinaryTree<type_t>::Insert(const type_t &item)
{
	return Insert(NULL, m_root, item);
}

template < typename type_t >
void mtlBinaryTree<type_t>::Clear( void )
{
	delete m_root;
	m_root = NULL;
	m_size = 0;
}

template < typename type_t >
void mtlBinaryTree<type_t>::ToList(mtlList<type_t> &list) const
{
	list.RemoveAll();
	if (m_root == NULL) { return; }
	ToList(list, m_root);
}

template < typename type_t >
void mtlBinaryTree<type_t>::ToListReversed(mtlList<type_t> &list) const
{
	list.RemoveAll();
	if (m_root == NULL) { return; }
	ToListReversed(list, m_root);
}

#endif
