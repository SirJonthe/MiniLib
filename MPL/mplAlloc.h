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
		void from_array(const type_t *ptr, int elem_count)
		{
			// XYZUV XYZUV XYZUV XYZUV XYZUV = XXXX YYYY ZZZZ UUUU VVVV X000 Y000 Z000 U000 V000

			const int  GROUP_SIZE      = sizeof(type_t) / sizeof(int);
			const int  WIDE_GROUP_SIZE = GROUP_SIZE * MPL_WIDTH;
			const int  NUM_WIDE_GROUPS = elem_count / MPL_WIDTH;
			const int  REMAINING_ELEMS = elem_count % MPL_WIDTH;
			const int  MEM_SIZE        = (NUM_WIDE_GROUPS + (REMAINING_ELEMS > 0 ? 1 : 0)) * MPL_WIDTH * GROUP_SIZE;

			alloc<int>(MEM_SIZE);
			int       *dst_mem = (int*)ali;
			const int *src_mem = (int*)ptr;

			if (NUM_WIDE_GROUPS > 0) {
				for (int i = 0; i < NUM_WIDE_GROUPS; ++i) {
					for (int member = 0; member < GROUP_SIZE; ++member) {
						for (int wide_index = 0; wide_index < MPL_WIDTH; ++wide_index) {
							dst_mem[member * MPL_WIDTH + wide_index] = src_mem[member + wide_index * GROUP_SIZE];
						}
					}
					dst_mem += WIDE_GROUP_SIZE;
					src_mem += WIDE_GROUP_SIZE;
				}
			}
			if (REMAINING_ELEMS > 0) {
				// here we organize the part of the array that does not align to the wide array, filling unused spots with 0
				for (int member = 0; member < GROUP_SIZE; ++member) {
					for (int wide_index = 0; wide_index < REMAINING_ELEMS; ++wide_index) {
						dst_mem[member * MPL_WIDTH + wide_index] = src_mem[member + wide_index * GROUP_SIZE];
					}
					for (int wide_index = REMAINING_ELEMS; wide_index < MPL_WIDTH; ++wide_index) {
						dst_mem[member * MPL_WIDTH + wide_index] = 0;
					}
				}
			}
		}

		template < typename type_t >
		type_t *ptr( void ) { return (type_t*)ali; }
		template < typename type_t >
		const type_t *ptr( void ) const { return (const type_t*)ali; }
	};
}

#endif // MPL_ALLOC_H_INCLUDED__
