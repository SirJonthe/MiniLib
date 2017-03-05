#ifndef MPL_ALLOC_H_INCLUDED__
#define MPL_ALLOC_H_INCLUDED__

#include "mplCommon.h"

namespace mpl
{
	class aligned
	{
	private:
		char *mem;
		char *ali;

	public:
		aligned( void ) : mem(NULL), ali(NULL) {}
		~aligned( void ) { free(); }

		void alloc(size_t elem_count)
		{
			this->free();
			if (elem_count > 0) {
				mem = new char[elem_count + MPL_BYTE_ALIGN];
				ali = (char*)((size_t)(mem + MPL_BYTE_ALIGN) & MPL_BYTE_ALIGN_MASK);
			}
		}
		template < typename type_t >
		void alloc(size_t elem_count)
		{
			alloc(elem_count * sizeof(type_t));
		}
		void free( void )
		{
			delete [] mem;
			mem = NULL;
			ali = NULL;
		}

		template < typename type_t >
		type_t *ptr( void ) { return (type_t*)ali; }
		template < typename type_t >
		const type_t *ptr( void ) const { return (const type_t*)ali; }
	};
}

#endif // MPL_ALLOC_H_INCLUDED__
