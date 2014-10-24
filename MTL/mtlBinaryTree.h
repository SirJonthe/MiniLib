#ifndef MTL_BINARY_TREE_H_INCLUDED__
#define MTL_BINARY_TREE_H_INCLUDED__

#include <cstdlib>
#include "mtlList.h"

template < typename type_t > class mtlBinaryTree;

template < typename type_t >
class mtlNode
{
	friend class mtlBinaryTree<type_t>;
private:
	type_t					m_item;
	mtlBinaryTree<type_t>	*m_tree;
	mtlNode<type_t>		*m_parent;
	mtlNode<type_t>		*m_right;
	mtlNode<type_t>		*m_left;

private:
	mtlNode(const type_t &item, mtlNode<type_t> *parent, mtlBinaryTree<type_t> *tree);
	mtlNode( void ) {}
	mtlNode<type_t>		&operator=(const mtlNode<type_t>&) { return *this; }

	static int						GetDepth(const mtlNode<type_t> *node, int currentDepth);
	static int						GetHeight(const mtlNode<type_t> *node, int currentHeight);
	template < typename compare_t >
	static const mtlNode<type_t>	*Find(const mtlNode<type_t> *node, const compare_t &item, bool closest);
	template < typename compare_t >
	static mtlNode<type_t>		*Find(mtlNode<type_t> *node, const compare_t &item, bool closest);
	static bool						IsBalanced(const mtlNode<type_t> *node, int permittedDifference);

public:
	const type_t			&GetItem( void ) const { return m_item; }
	const mtlNode<type_t>	*GetParent( void ) const { return m_parent; }
	mtlNode<type_t>		*GetParent( void ) { return m_parent; }
	const mtlNode<type_t>	*GetRight( void ) const { return m_right; }
	mtlNode<type_t>		*GetRight( void ) { return m_right; }
	const mtlNode<type_t>	*GetLeft( void ) const { return m_left; }
	mtlNode<type_t>		*GetLeft( void ) { return m_left; }
	int						GetDepth( void ) const { return GetDepth(this, 0); }
	int						GetHeight( void ) const { return GetHeight(this, 0); }
	template < typename compare_t >
	const mtlNode<type_t>	*Find(const compare_t &item) const { return Find(this, item, false); }
	template < typename compare_t >
	mtlNode<type_t>		*Find(const compare_t &item) { return Find(this, item, false); }
	template < typename compare_t >
	const mtlNode<type_t>	*FindClosest(const compare_t &item) const { return Find(this, item, true); }
	template < typename compare_t >
	mtlNode<type_t>		*FindClosest(const compare_t &item) { return Find(this, item, true); }
	const mtlNode<type_t>	*FindMin( void ) const;
	mtlNode<type_t>		*FindMin( void );
	const mtlNode<type_t>	*FindMax( void ) const;
	mtlNode<type_t>		*FindMax( void );
	bool					IsBalanced(int permittedDifference = 1) const { return IsBalanced(this, permittedDifference); }
	mtlNode<type_t>		*Remove( void );
	template < typename func_t >
	void InOrder(const func_t &fn) const
	{
		if (m_left != NULL) { m_left->InOrder(fn); }
		fn(m_item);
		if (m_right != NULL) { m_right->InOrder(fn); }

	}
	template < typename func_t >
	void InReverseOrder(const func_t &fn) const
	{
		if (m_right != NULL) { m_right->InOrder(fn); }
		fn(m_item);
		if (m_left != NULL) { m_left->InOrder(fn); }
	}
	template < typename func_t >
	void InBreadth(const func_t &fn) const
	{
		mtlList<const mtlNode<type_t>*> queue;
		queue.AddLast(this);
		while (queue.GetSize() != 0) {
			const mtlNode<type_t> *front = queue.GetFirst();
			fn(front->GetItem());
			queue.RemoveFirst();
			if (front->m_left != NULL) { queue.AddLast(front->m_left); }
			if (front->m_right != NULL) { queue.AddLast(front->m_right); }
		}
	}
	template < typename func_t >
	void InBreadthReverse(const func_t &fn) const
	{
		mtlList<const mtlNode<type_t>*> queue;
		queue.AddLast(this);
		while (queue.GetSize() != 0) {
			const mtlNode<type_t> *front = queue.GetFirst();
			fn(front->GetItem());
			queue.RemoveFirst();
			if (front->m_right != NULL) { queue.AddLast(front->m_right); }
			if (front->m_left != NULL) { queue.AddLast(front->m_left); }
		}
	}
};

template < typename type_t >
mtlNode<type_t>::mtlNode(const type_t &item, mtlNode<type_t> *parent, mtlBinaryTree<type_t> *tree) :
m_item(item), m_tree(tree), m_parent(parent), m_right(NULL), m_left(NULL)
{}

template < typename type_t >
int mtlNode<type_t>::GetDepth(const mtlNode<type_t> *node, int currentDepth)
{
	if (node == NULL) { return currentDepth; }
	return GetDepth(node->m_parent, currentDepth + 1);
}

template < typename type_t >
int mtlNode<type_t>::GetHeight(const mtlNode<type_t> *node, int currentHeight)
{
	if (node == NULL) { return currentHeight; }
	const int pos = GetHeight(node->m_right, currentHeight + 1);
	const int neg = GetHeight(node->m_left, currentHeight + 1);
	return (pos > neg) ? pos : neg;
}

template < typename type_t >
template < typename compare_t >
const mtlNode<type_t> *mtlNode<type_t>::Find(const mtlNode<type_t> *node, const compare_t &item, bool closest)
{
	if (node->m_item == item) { return node; }
	if (node->m_item > item) {
		if (node->m_left != NULL) { return Find(node->m_left, item, closest); }
	} else {
		if (node->m_right != NULL) { return Find(node->m_right, item, closest); }
	}
	return closest ? node : NULL;
}

template < typename type_t >
template < typename compare_t >
mtlNode<type_t> *mtlNode<type_t>::Find(mtlNode<type_t> *node, const compare_t &item, bool closest)
{
	if (node->m_item == item) { return node; }
	if (node->m_item > item) {
		if (node->m_left != NULL) { return Find(node->m_left, item, closest); }
	} else {
		if (node->m_right != NULL) { return Find(node->m_right, item, closest); }
	}
	return closest ? node : NULL;
}


template < typename type_t >
const mtlNode<type_t> *mtlNode<type_t>::FindMin( void ) const
{
	const mtlNode<type_t> *n = this;
	while (n->m_left != NULL) {
		n = n->m_left;
	}
	return n;
}

template < typename type_t >
mtlNode<type_t> *mtlNode<type_t>::FindMin( void )
{
	mtlNode<type_t> *n = this;
	while (n->m_left != NULL) {
		n = n->m_left;
	}
	return n;
}

template < typename type_t >
const mtlNode<type_t> *mtlNode<type_t>::FindMax( void ) const
{
	const mtlNode<type_t> *n = this;
	while (n->m_right != NULL) {
		n = n->m_right;
	}
	return n;
}

template < typename type_t >
mtlNode<type_t> *mtlNode<type_t>::FindMax( void )
{
	mtlNode<type_t> *n = this;
	while (n->m_right != NULL) {
		n = n->m_right;
	}
	return n;
}

// Find	a more efficient way of traversing...
template < typename type_t >
bool mtlNode<type_t>::IsBalanced(const mtlNode<type_t> *node, int permittedDifference)
{
	return node == NULL || (IsBalanced(node->m_left) && IsBalanced(node->m_right) && (abs(GetHeight(node->m_left, 0) - GetHeight(node->m_right, 0)) <= permittedDifference));
}

template < typename type_t >
mtlNode<type_t> *mtlNode<type_t>::Remove( void )
{
	return m_tree->Remove(this);
}

template < typename type_t >
class mtlBinaryTree
{
private:
	mtlNode<type_t>	*m_root;
	int					m_size;

private:
	mtlBinaryTree &operator=(const mtlBinaryTree&) { return *this; }
	mtlBinaryTree(const mtlBinaryTree&) {}
	mtlNode<type_t> *Insert(mtlNode<type_t> *parent, mtlNode<type_t> *&node, const type_t &item);
	void Delete(mtlNode<type_t> *node);

public:
	mtlBinaryTree( void );
	~mtlBinaryTree( void );
	const mtlNode<type_t>	*GetRoot( void ) const { return m_root; }
	mtlNode<type_t>			*GetRoot( void ) { return m_root; }
	mtlNode<type_t>			*Insert(const type_t &item);
	void					RemoveAll( void );
	int						GetSize( void ) const { return m_size; }
	mtlNode<type_t>			*Remove(mtlNode<type_t> *node);
	bool					IsEmpty( void ) const;
	// rebalance
};

template < typename type_t >
mtlNode<type_t> *mtlBinaryTree<type_t>::Insert(mtlNode<type_t> *parent, mtlNode<type_t> *&node, const type_t &item)
{
	if (node == NULL) {
		node = new mtlNode<type_t>(item, parent, this);
		++m_size;
		return node;
	}
	if (node->m_item == item) { return node; }
	if (node->m_item > item) { return Insert(node, node->m_left, item); }
	return Insert(node, node->m_right, item);
}

template < typename type_t >
void mtlBinaryTree<type_t>::Delete(mtlNode<type_t> *node)
{
	if (node != NULL) {
		Delete(node->m_left);
		Delete(node->m_right);
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
mtlNode<type_t> *mtlBinaryTree<type_t>::Insert(const type_t &item)
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
mtlNode<type_t> *mtlBinaryTree<type_t>::Remove(mtlNode<type_t> *node)
{
	if (node == NULL || node->m_tree != this) { return NULL; }

	// return the address of the node that assumes this place
	mtlNode<type_t> *returnNode = NULL; // the node that takes the place of the removed one
	mtlNode<type_t> **childNodePointer = NULL; // points to the left or right parent pointer corresponding to the node to be removed
	if (node->m_parent != NULL) {
		if (node->m_parent->m_left == node) {
			childNodePointer = &node->m_parent->m_left;
		} else {
			childNodePointer = &node->m_parent->m_right;
		}
	}

	if (node->m_left != NULL && node->m_right != NULL) { // positive and negative are guaranteed to be non-null

		// What sub-tree do we take a node from and insert in the removed node's location?
		// If we only take from one side all the time the tree will become unbalanced.
		// In this case we try to analytically randomize what subtree we pick by using the removed node's address as the determining factor.
		// Don't use LSB as it is unlikely to be 1 due to address alignment.
		mtlNode<type_t> *side = NULL;
		if (((unsigned long long)node & (1<<4))) {
			returnNode = node->m_right->FindMin();
			side = returnNode->m_right;
		} else {
			returnNode = node->m_left->FindMax();
			side = returnNode->m_left;
		}

		if (returnNode->m_parent != node) {
			if (returnNode->m_parent->m_left == returnNode) {
				returnNode->m_parent->m_left = side;
			} else {
				returnNode->m_parent->m_right = side;
			}
		} else {
			if (node->m_left == returnNode) {
				node->m_left = returnNode->m_left;
			} else {
				node->m_right = returnNode->m_right;
			}
		}

		returnNode->m_left = node->m_left;
		returnNode->m_right = node->m_right;
		if (returnNode->m_left != NULL) { returnNode->m_left->m_parent = returnNode; }
		if (returnNode->m_right != NULL) { returnNode->m_right->m_parent = returnNode; }
	} else {
		returnNode = node->m_left != NULL ? node->m_left : node->m_right; // pick the non-null one, if there is one
	}

	if(returnNode != NULL) {
		returnNode->m_parent = node->m_parent;
	}

	if (node == m_root) {
		m_root = returnNode;
	} else if (childNodePointer != NULL) {
		*childNodePointer = returnNode;
	}
	delete node;
	--m_size;
	return returnNode;
}

template < typename type_t >
bool mtlBinaryTree<type_t>::IsEmpty( void ) const
{
	return m_root == NULL;
}

#endif
