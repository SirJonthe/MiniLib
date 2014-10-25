#ifndef MTL_HASHTABLE_H_INCLUDED__
#define MTL_HASHTABLE_H_INCLUDED__

#include "mtlList.h"
#include "mtlArray.h"
#include "mtlString.h"

class mtlHashable
{
public:
	virtual mtlHash ToHash( void ) = 0;
	virtual ~mtlHashable( void ) {}
};

template < typename type_t >
class mtlHashTable
{
private:
	struct Entry
	{
		type_t	data;
		mtlHash	hash;
	};

private:
	mtlArray< mtlList<typename Entry> >	m_table;
	int									m_num_entries;

private:
	int GetIndex(mtlHash hash, int size) const;
	void ResizeTable(int size);

public:
	mtlHashTable( void );

	const type_t *Find(mtlHash hash) const;
	type_t *Find(mtlHash hash);
	void Insert(const type_t &item);
};

template < typename type_t >
int mtlHashTable::GetIndex(mtlHash hash, int size) const
{
	return hash.value & (size - 1);
}

template < typename type_t >
void mtlHashTable::ResizeTable(int size)
{
	mtlArray< mtlList<type_t> > new_table(size);
	for (int i = 0; i < m_table.GetSize(); ++i) {
		mtlItem<type_t> *node = m_table[i].GetFirst();
		while (node != NULL) {
			node = node->Transfer(new_table[GetIndex(node->GetItem().hash, new_table.GetSize())], NULL);
		}
	}
	m_table.Copy(new_table);
}

template < typename type_t >
mtlHashTable::mtlHashTable( void ) : m_table(128), m_num_entries(0)
{}

template < typename type_t >
const type_t *mtlHashTable::Find(mtlHash hash) const
{
	const mtlItem<typename Entry> *i = &m_table[GetIndex(hash, m_table.GetSize())].GetFirst();
	while (i != NULL && i->GetItem().hash != hash) {
		i = i->GetNext();
	}
	return i != NULL ? i->GetItem() : NULL;
}

template < typename type_t >
type_t *mtlHashTable::Find(mtlHash hash)
{
	mtlItem<typename Entry> *i = m_table[GetIndex(hash, m_table.GetSize())].GetFirst();
	while (i != NULL && i->GetItem().hash != hash) {
		i = i->GetNext();
	}
	return i != NULL ? i->GetItem() : NULL;
}

template < typename type_t >
void mtlHashTable::Insert(const type_t &item)
{
	mtlHash hash = item.ToHash();
	int index = GetIndex(hash, m_table.GetSize());

	mtlItem<typename Entry> *i = m_table[index].GetFirst();
	while (i != NULL && i->GetItem().hash != hash) {
		i = i->GetNext();
	}
	if (i != NULL) { return; }

	m_table[index].AddLast();
	m_table[index].GetLast()->GetItem().data = item;
	m_table[index].GetLast()->GetItem().hash = hash;

	++m_num_entries;
	if (float(m_num_entries) / float(m_table.GetSize()) > 0.7f) { // at 70% capacity the risk of collisions gets too high
		ResizeTable(m_table.GetSize() * 2);
	}
}

#endif // MTL_HASHTABLE_H_INCLUDED__
