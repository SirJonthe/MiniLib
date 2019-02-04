#ifndef MTL_STRINGMAP_INCLUDED
#define MTL_STRINGMAP_INCLUDED

#include "mtlString.h"
#include "mtlBinaryTree.h"
#include "mtlPointer.h"
#include "mtlList.h"

template < typename type_t >
class mtlStringMap
{
private:
	struct Entry
	{
		mtlString         name;
		mtlShared<type_t> entry;
		Entry( void ) : name(), entry() {}
		Entry(const Entry &e) : entry(e.entry) { name.Copy(e.name); }
	};

	struct HashNode
	{
		typedef mtlList<Entry> List;
		typedef mtlItem<Entry> ListNode;

		mtlHash                 hash;
		mutable mtlShared<List> entries;

		bool operator >  (mtlHash r)         const { return hash > r; }
		bool operator == (mtlHash r)         const { return hash == r; }
		bool operator >  (const HashNode &r) const { return hash > r.hash; }
		bool operator == (const HashNode &r) const { return hash == r.hash; }
	};

	struct NodeStorer
	{
		mtlList<const HashNode*> m_list;
		void operator()(const HashNode &node) { m_list.AddLast(&node); }
	};

private:
	mtlBinaryTree<HashNode> m_table;

public:
	const mtlNode<HashNode> *GetNode(const mtlChars &name) const;
	mtlNode<HashNode>       *GetNode(const mtlChars &name);

public:
	template < typename derived_t >
	type_t       *CreateEntry(const mtlChars &name);
	type_t       *CreateEntry(const mtlChars &name);
	void          RemoveEntry(const mtlChars &name);
	void          RemoveAll( void );
	const type_t *GetEntry(const mtlChars &name) const;
	type_t       *GetEntry(const mtlChars &name);
	void          Copy(const mtlStringMap<type_t> &map);
	void          ToList(mtlList<const type_t*> &out_list) const;
};

template < typename type_t >
const mtlNode<typename mtlStringMap<type_t>::HashNode> *mtlStringMap<type_t>::GetNode(const mtlChars &name) const
{
	return m_table.GetRoot() != NULL ? m_table.GetRoot()->Find(mtlHash(name)) : NULL;
}

template < typename type_t >
mtlNode<typename mtlStringMap<type_t>::HashNode> *mtlStringMap<type_t>::GetNode(const mtlChars &name)
{
	return m_table.GetRoot() != NULL ? m_table.GetRoot()->Find(mtlHash(name)) : NULL;
}

template < typename type_t >
template < typename derived_t >
type_t *mtlStringMap<type_t>::CreateEntry(const mtlChars &name)
{
	mtlHash hash(name);
	const mtlNode<HashNode> *b = m_table.GetRoot();
	if (b != NULL) {
        b = b->Find(hash);
	}
	if (b != NULL) {
        typename HashNode::ListNode *n = b->GetItem().entries->GetFirst();
		while (n != NULL) {
			if (n->GetItem().name.Compare(name)) {
				return n->GetItem().entry.GetShared();
			}
			n = n->GetNext();
		}
	} else {
		HashNode node;
		node.hash = hash;
		b = m_table.Insert(node);
		b->GetItem().entries.New();
	}
	Entry entry;
	entry.name.Copy(name);
	entry.entry.template New<derived_t>(); // Jesus...
    b->GetItem().entries->AddLast(entry);
    return b->GetItem().entries->GetLast()->GetItem().entry.GetShared(); // puke worthy
}

template < typename type_t >
type_t *mtlStringMap<type_t>::CreateEntry(const mtlChars &name)
{
	return CreateEntry<type_t>(name);
}

template < typename type_t >
void mtlStringMap<type_t>::RemoveEntry(const mtlChars &name)
{
	mtlNode<HashNode> *hashNode = GetNode(name);
	typename HashNode::ListNode *node = hashNode->GetItem().entries->GetFirst();
	while (node != NULL) {
		if (node->GetItem().name.Compare(name)) {
			node->Remove();
			break;
		}
		node = node->GetNext();
	}
	if (hashNode->GetItem().entries->GetSize() == 0) {
		hashNode->Remove();
	}
}

template < typename type_t >
void mtlStringMap<type_t>::RemoveAll( void )
{
	m_table.RemoveAll();
}

template < typename type_t >
const type_t *mtlStringMap<type_t>::GetEntry(const mtlChars &name) const
{
	const mtlNode<HashNode> *b = GetNode(name);
	if (b == NULL) { return NULL; }
    typename HashNode::ListNode *n = b->GetItem().entries->GetFirst();
	while (n != NULL) {
		if (n->GetItem().name.Compare(name)) {
			return n->GetItem().entry.GetShared();
		}
		n = n->GetNext();
	}
	return NULL;
}

template < typename type_t >
type_t *mtlStringMap<type_t>::GetEntry(const mtlChars &name)
{
	mtlNode<HashNode> *b = GetNode(name);
	if (b == NULL) { return NULL; }
    typename HashNode::ListNode *n = b->GetItem().entries->GetFirst();
	while (n != NULL) {
		if (n->GetItem().name.Compare(name)) {
			return n->GetItem().entry.GetShared();
		}
		n = n->GetNext();
	}
	return NULL;
}

template < typename type_t >
void mtlStringMap<type_t>::Copy(const mtlStringMap<type_t> &map)
{
	m_table.Copy(map.m_table);
}

template < typename type_t >
void mtlStringMap<type_t>::ToList(mtlList<const type_t*> &out_list) const
{
	out_list.RemoveAll();
	NodeStorer tmp_list;
	if (m_table.GetRoot() != NULL) {
		m_table.GetRoot()->InOrder(tmp_list);
	}
	mtlItem<const HashNode*> *tmp_iter = tmp_list.m_list.GetFirst();
	while (tmp_iter != NULL) {
		mtlItem<Entry> *entry_iter = tmp_iter->GetItem()->entries.GetShared() != NULL ? tmp_iter->GetItem()->entries.GetShared()->GetFirst() : NULL;
		while (entry_iter != NULL) {
			if (entry_iter->GetItem().entry.GetShared() != NULL) {
				out_list.AddLast(entry_iter->GetItem().entry.GetShared());
			}
			entry_iter = entry_iter->GetNext();
		}
		tmp_iter = tmp_iter->GetNext();
	}
}

#endif // MTL_STRINGMAP_INCLUDED__
