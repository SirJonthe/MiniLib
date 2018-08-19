#ifndef MTL_TYPE_H_INCLUDED
#define MTL_TYPE_H_INCLUDED

// http://stackoverflow.com/questions/18731075/c-fast-dynamic-type-subtype-check

typedef unsigned long long mtlTypeID;

class mtlBase
{
private:
	void * const m_objectPointer;

protected:
	static mtlTypeID GetNextTypeID( void ) { static mtlTypeID id = 0; return ++id; }
	static bool IsType(mtlTypeID id) { return (GetClassType() == id); }
	void *GetObjectPointer(mtlTypeID id) const { return (GetClassType() != id) ? NULL : m_objectPointer; }
	virtual void *GetVirtualObjectPointer(mtlTypeID id) const { return (GetClassType() != id) ? NULL : m_objectPointer; }

public:
	virtual ~mtlBase( void ) {}
	mtlBase(void *p_objectPointer) : m_objectPointer(p_objectPointer) {}

	static mtlTypeID GetClassType( void ) { return 0; }
	virtual mtlTypeID GetInstanceType( void ) const { return GetClassType(); }

	virtual bool IsInstanceType(mtlTypeID id) const { return IsType(id); }
	bool IsInstanceType(const mtlBase &base) const { return IsInstanceType(base.GetInstanceType()); }
	template < typename other_t >
	bool IsInstanceType( void ) const { return IsInstanceType(other_t::GetClassType()); }

	static bool IsClassType(mtlTypeID id) { return IsType(id); }
	static bool IsClassType(const mtlBase &base) { return IsType(base.GetInstanceType()); }
	template < typename other_t >
	static bool IsClassType( void ) { return IsType(other_t::GetClassType()); }

	template < typename cast_t >
	cast_t *Cast( void ) { return reinterpret_cast<cast_t*>(GetVirtualObjectPointer(cast_t::GetClassType())); }
	template < typename cast_t >
	const cast_t *Cast( void ) const { return reinterpret_cast<cast_t*>(GetVirtualObjectPointer(cast_t::GetClassType())); }
};

//
// Use the second template parameter to be able to make a
// distinction between two different objects that share
// the same base class. Declaration should look like the
// following:
//
// class Object : public mtlInherit<BaseObject, Object>
// ...
// Object( void ) : mtlInherit<BaseObject, Object>(this) {/*...*/}
// Object(const Object &o) : mtlInherit<BaseObject, Object>(this) {/*...*/}
//
// Compilers might also accept:
// Object( void ) : mtlInherit(this) {/*...*/}
//

template < typename base_t, typename type_t >
class mtlInherit : public base_t
{
private:
	// This type has to be of type_t. Using a same types
	// such as 'char' for all mtlInherit<base, ...> types
	// causes MSVC to optimize memory layout and make sure
	// that all mtlInherit<base, ...> share the same static
	// member despite being of different types, for instance
	// &mtlInherit<someClass, child1>::m_typeAddress == &mtlInherit<someClass, child2>::m_typeAddress
	//static type_t *m_typeAddress;

	void * const m_objectPointer;

protected:
	static bool IsType(mtlTypeID id) { return (GetClassType() != id) ? base_t::IsType(id) : true; }
	void *GetObjectPointer(mtlTypeID id) const { return (GetClassType() != id) ? base_t::GetObjectPointer(id) : m_objectPointer; }
	virtual void *GetVirtualObjectPointer(mtlTypeID id) const { return (GetClassType() != id) ? base_t::GetObjectPointer(id) : m_objectPointer; }

public:
	explicit mtlInherit(void *p_objectPointer) : m_objectPointer(p_objectPointer) {}

	static mtlTypeID GetClassType( void ) { static const mtlTypeID id = mtlBase::GetNextTypeID(); return id; }
	virtual mtlTypeID GetInstanceType( void ) const { return GetClassType(); }

	virtual bool IsInstanceType(mtlTypeID id) const { return IsType(id); }
	bool IsInstanceType(const mtlBase &base) const { return IsInstanceType(base.GetInstanceType()); }
	template < typename other_t >
	bool IsInstanceType( void ) const { return IsInstanceType(other_t::GetClassType()); }

	static bool IsClassType(mtlTypeID id) { return IsType(id); }
	static bool IsClassType(const mtlBase &base) { return IsType(base.GetInstanceType()); }
	template < typename other_t >
	static bool IsClassType( void ) { return IsType(other_t::GetClassType()); }

	//template < typename cast_t >
	//cast_t *Cast( void ) { return reinterpret_cast<cast_t*>(GetVirtualObjectPointer(cast_t::GetClassType())); }
	//template < typename cast_t >
	//const cast_t *Cast( void ) const { return reinterpret_cast<cast_t*>(GetVirtualObjectPointer(cast_t::GetClassType())); }
};

#endif
