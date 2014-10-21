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
	type_t					m_item;
	mtlBinaryTree<type_t>	*m_tree;
	mtlBranch<type_t>		*m_parent;
	mtlBranch<type_t>		*m_positive;
	mtlBranch<type_t>		*m_negative;
private:
	mtlBranch(const type_t &item, mtlBranch<type_t> *parent, mtlBinaryTree<type_t> *tree);
	mtlBranch( void ) {}
	mtlBranch<type_t>		&operator=(const mtlBranch<type_t>&) { return *this; }
	int						GetDepth(const mtlBranch<type_t> *node, int currentDepth) const;
	int						GetHeight(const mtlBranch<type_t> *node, int currentHeight) const;
	template < typename compare_t >
	const mtlBranch<type_t>	*Find(const mtlBranch<type_t> *node, const compare_t &item, bool closest) const;
	template < typename compare_t >
	mtlBranch<type_t>		*Find(mtlBranch<type_t> *node, const compare_t &item, bool closest);
	const mtlBranch<type_t>	*FindMin(const mtlBranch<type_t> *node) const;
	mtlBranch<type_t>		*FindMin(mtlBranch<type_t> *node);
	const mtlBranch<type_t>	*FindMax(const mtlBranch<type_t> *node) const;
	mtlBranch<type_t>		*FindMax(mtlBranch<type_t> *node);
	bool					IsBalanced(const mtlBranch<type_t> *node, int permittedDifference) const;
public:
	const type_t			&GetItem( void ) const { return m_item; }
	const mtlBranch<type_t>	*GetParent( void ) const { return m_parent; }
	mtlBranch<type_t>		*GetParent( void ) { return m_parent; }
	const mtlBranch<type_t>	*GetPositive( void ) const { return m_positive; }
	mtlBranch<type_t>		*GetPositive( void ) { return m_positive; }
	const mtlBranch<type_t>	*GetNegative( void ) const { return m_negative; }
	mtlBranch<type_t>		*GetNegative( void ) { return m_negative; }
	int						GetDepth( void ) const { return GetDepth(this, 0); }
	int						GetHeight( void ) const { return GetHeight(this, 0); }
	template < typename compare_t >
	const mtlBranch<type_t>	*Find(const compare_t &item) const { return Find(this, item, false); }
	template < typename compare_t >
	mtlBranch<type_t>		*Find(const compare_t &item) { return Find(this, item, false); }
	template < typename compare_t >
	const mtlBranch<type_t>	*FindClosest(const compare_t &item) const { return Find(this, item, true); }
	template < typename compare_t >
	mtlBranch<type_t>		*FindClosest(const compare_t &item) { return Find(this, item, true); }
	const mtlBranch<type_t>	*FindMin( void ) const { return FindMin(this); }
	mtlBranch<type_t>		*FindMin( void ) { return FindMin(this); }
	const mtlBranch<type_t>	*FindMax( void ) const { return FindMax(this); }
	mtlBranch<type_t>		*FindMax( void ) { return FindMax(this); }
	bool					IsBalanced(int permittedDifference = 1) const { return IsBalanced(this, permittedDifference); }
	mtlBranch<type_t>		*Remove( void );
};

template < typename type_t >
mtlBranch<type_t>::mtlBranch(const type_t &item, mtlBranch<type_t> *parent, mtlBinaryTree<type_t> *tree) :
m_item(item), m_tree(tree), m_parent(parent), m_positive(NULL), m_negative(NULL)
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
template < typename compare_t >
const mtlBranch<type_t> *mtlBranch<type_t>::Find(const mtlBranch<type_t> *node, const compare_t &item, bool closest) const
{
	if (node->m_item == item) { return node; }
	if (node->m_item > item) {
		if (node->m_negative != NULL) { return Find(node->m_negative, item, closest); }
	} else {
		if (node->m_positive != NULL) { return Find(node->m_positive, item, closest); }
	}
	return closest ? node : NULL;
}

template < typename type_t >
template < typename compare_t >
mtlBranch<type_t> *mtlBranch<type_t>::Find(mtlBranch<type_t> *node, const compare_t &item, bool closest)
{
	if (node->m_item == item) { return node; }
	if (node->m_item > item) {
		if (node->m_negative != NULL) { return Find(node->m_negative, item, closest); }
	} else {
		if (node->m_positive != NULL) { return Find(node->m_positive, item, closest); }
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
mtlBranch<type_t> *mtlBranch<type_t>::FindMin(mtlBranch<type_t> *node)
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
mtlBranch<type_t> *mtlBranch<type_t>::FindMax(mtlBranch<type_t> *node)
{
	if (node->m_positive == NULL) { return this; }
	return FindMax(node->m_positive);
}

// Find	a more efficient way of traversing...
template < typename type_t >
bool mtlBranch<type_t>::IsBalanced(const mtlBranch<type_t> *node, int permittedDifference) const
{
	return node == NULL || (IsBalanced(node->m_negative) && IsBalanced(node->m_positive) && (abs(GetHeight(node->m_negative, 0) - GetHeight(node->m_positive, 0)) <= permittedDifference));
}

template < typename type_t >
mtlBranch<type_t> *mtlBranch<type_t>::Remove( void )
{
	return m_tree->Remove(this);
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
	mtlBranch<type_t> *Insert(mtlBranch<type_t> *parent, mtlBranch<type_t> *&node, const type_t &item);
	void ToList(mtlList<type_t> &list, const mtlBranch<type_t> *node) const;
	void ToListReversed(mtlList<type_t> &list, const mtlBranch<type_t> *node) const;
	void Delete(mtlBranch<type_t> *node);
public:
	mtlBinaryTree( void );
	~mtlBinaryTree( void );
	const mtlBranch<type_t> *GetRoot( void ) const { return m_root; }
	mtlBranch<type_t>		*GetRoot( void ) { return m_root; }
	mtlBranch<type_t>		*Insert(const type_t &item);
	void					RemoveAll( void );
	int						GetSize( void ) const { return m_size; }
	void					ToList(mtlList<type_t> &list) const;
	void					ToListReversed(mtlList<type_t> &list) const;
	mtlBranch<type_t>		*Remove(mtlBranch<type_t> *node);
	// rebalance
};

template < typename type_t >
mtlBranch<type_t> *mtlBinaryTree<type_t>::Insert(mtlBranch<type_t> *parent, mtlBranch<type_t> *&node, const type_t &item)
{
	if (node == NULL) {
		node = new mtlBranch<type_t>(item, parent, this);
		++m_size;
		return node;
	}
	if (node->m_item == item) { return node; }
	if (node->m_item > item) { return Insert(node, node->m_negative, item); }
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
void mtlBinaryTree<type_t>::Delete(mtlBranch<type_t> *node)
{
	if (node != NULL) {
		Delete(node->m_negative);
		Delete(node->m_positive);
		delete node;
	}
}

template < typename type_t >
mtlBinaryTree<type_t>::mtlBinaryTree( void ) :
m_root(NULL), m_size(0)
{}

template < typename type_t >
mtlBinaryTree<type_t>::~mtlBinaryTree( void )
{
	RemoveAll();
}

template < typename type_t >
mtlBranch<type_t> *mtlBinaryTree<type_t>::Insert(const type_t &item)
{
	return Insert(NULL, m_root, item);
}

template < typename type_t >
void mtlBinaryTree<type_t>::RemoveAll( void )
{
	Delete(m_root);
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

template < typename type_t >
mtlBranch<type_t> *mtlBinaryTree<type_t>::Remove(mtlBranch<type_t> *node)
{
	if (node->m_tree != this) { return NULL; }

	// return the address of the node that assumes this place
	mtlBranch<type_t> *returnBranch = NULL;
	mtlBranch<type_t> **nodePointer = NULL;
	if (node->m_parent != NULL) {
		if (node->m_parent->m_negative == node) {
			nodePointer = &node->m_parent->m_negative;
		} else {
			nodePointer = &node->m_parent->m_positive;
		}
	}

	if (node->m_negative == NULL) {
		if (node->m_positive == NULL) {
			returnBranch = NULL;
		} else {
			returnBranch = node->m_negative;
		}
	} else if (node->m_positive == NULL) {
		returnBranch = node->m_positive;
	} else {
		returnBranch = node->m_positive->FindMin();

		if (returnBranch->m_parent != node) {
			// only the smallest node's positive side may be non-null (otherwise it would not be smallest)
			if (returnBranch->m_parent->m_negative == returnBranch) { // parent will never be null
				returnBranch->m_parent->m_negative = returnBranch->m_positive;
			} else {
				returnBranch->m_parent->m_positive = returnBranch->m_positive;
			}
		}

		returnBranch->m_negative = node->m_negative;
		returnBranch->m_positive = node->m_positive;
	}

	if(returnBranch != NULL) {
		returnBranch->m_parent = node->m_parent;
	}

	if (node == m_root) {
		m_root = returnBranch;
	} else if (nodePointer != NULL) {
		*nodePointer = returnBranch;
	}
	delete node;
	return returnBranch;
}

#endif
