#ifndef MTL_TYPE_H_INCLUDED__
#define MTL_TYPE_H_INCLUDED__

// http://stackoverflow.com/questions/18731075/c-fast-dynamic-type-subtype-check

typedef unsigned long long TypeID;

class mtlBase
{
public:
	static TypeID GetClassType( void ) { return 0; }
	virtual TypeID GetInstanceType( void ) const { return GetClassType(); }
};

//
// Use the second template parameter to be able to make a
// distinction between two different objects that share
// the same base class. Declaration should look like the
// following:
//
// class Object : public mtlInherit<BaseObject, Object>
// ...
//

template < typename base_t, typename type_t = int >
class mtlInherit : public base_t
{
private:
	// This type has to be of type_t. Using a small value
	// such as 'char' for all mtlInherit<base, ...> types
	// causes MSVC to optimize memory layout and make sure
	// that all mtlInherit<base, ...> share the same static
	// member despite being of different types, for instance
	// &mtlInherit<someClass, child1>::m_typeAddress == &mtlInherit<someClass, child2>::m_typeAddress
	static type_t *m_typeAddress;

public:
	static TypeID GetClassType( void ) { return (TypeID)(&m_typeAddress); }
	virtual TypeID GetInstanceType( void ) const { return GetClassType(); }

	static TypeID GetParentClassType( void ) { return base_t::GetClassType(); }
	virtual TypeID GetParentInstanceType( void ) const { return base_t::GetInstanceType(); }
};

template < typename base_t, typename type_t > type_t *mtlInherit<base_t, type_t>::m_typeAddress = NULL;

#endif
