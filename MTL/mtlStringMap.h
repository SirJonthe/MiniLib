#ifndef MTL_STRINGMAP_INCLUDED__
#define MTL_STRINGMAP_INCLUDED__

#include "mtlString.h"
#include "mtlBinaryTree.h"
#include "mtlPointer.h"

template < typename type_t >
class mtlStringMap
{
private:
	struct Entry
	{
		mtlString			name;
		mtlShared<type_t>	entry;
		Entry( void ) : name(), entry() {}
		Entry(const Entry &e) : entry(e.entry) { name.Copy(e.name); }
	};
	struct HashNode
	{
		typedef mtlList<Entry> List;
		typedef mtlNode<Entry> ListNode;
		mtlHash					hash;
		mutable mtlShared<List>	entries;

		bool operator>(mtlHash r) const { return hash > r; }
		bool operator==(mtlHash r) const { return hash == r; }
		bool operator>(const HashNode &r) const { return hash > r.hash; }
		bool operator==(const HashNode &r) const { return hash == r.hash; }
	};
private:
	mtlBinaryTree<HashNode> m_table;
public:
	const mtlBranch<HashNode> *GetNode(const mtlChars &name) const;
	mtlBranch<HashNode> *GetNode(const mtlChars &name);
public:
	template < typename derived_t >
	type_t			*CreateEntry(const mtlChars &name);
	type_t			*CreateEntry(const mtlChars &name);
	void			RemoveEntry(const mtlChars &name);
	void			RemoveAll( void );
	const type_t	*GetEntry(const mtlChars &name) const;
	type_t			*GetEntry(const mtlChars &name);
};

template < typename type_t >
const mtlBranch<typename mtlStringMap<type_t>::HashNode> *mtlStringMap<type_t>::GetNode(const mtlChars &name) const
{
	return m_table.GetRoot() != NULL ? m_table.GetRoot()->Find(mtlHash(name)) : NULL;
}

template < typename type_t >
mtlBranch<typename mtlStringMap<type_t>::HashNode> *mtlStringMap<type_t>::GetNode(const mtlChars &name)
{
	return m_table.GetRoot() != NULL ? m_table.GetRoot()->Find(mtlHash(name)) : NULL;
}

template < typename type_t >
template < typename derived_t >
type_t *mtlStringMap<type_t>::CreateEntry(const mtlChars &name)
{
	mtlHash hash(name);
	const mtlBranch<HashNode> *b = m_table.GetRoot();
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
	mtlBranch<HashNode> *hashNode = GetNode(name);
    const typename HashNode::ListNode *node = hashNode->GetItem()->entries->GetFirst();
	while (node != NULL) {
        if (node->GetItem()->name.Compare(name)) {
			node->Remove();
			break;
		}
		node = node->GetNext();
	}
    if (hashNode->GetItem()->entries->GetSize() == 0) {
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
	mtlBranch<HashNode> *b = GetNode(name);
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
	mtlBranch<HashNode> *b = GetNode(name);
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

#endif // MTL_STRINGMAP_INCLUDED__
