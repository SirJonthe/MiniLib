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
	mtlArray< mtlList<type_t> > m_table;

public:
	const type_t *GetEntry(mtlHash h) const;
	type_t *GetEntry(mtlHash h);
	const type_t *GetEntry(const type_t &item) const;
	type_t *GetEntry(const type_t &item);
};

#endif // MTL_HASHTABLE_H_INCLUDED__
