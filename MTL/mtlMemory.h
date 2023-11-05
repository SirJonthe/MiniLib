#ifndef MTL_MEMORY_H_INCLUDED
#define MTL_MEMORY_H_INCLUDED

#include <cstring>

#define mtlclip0(x) ((x < 0) ? 0 : x)

// mtlClear
// Sets the value of all bytes in 'mem' to 0 for 'num' elements.
template < typename type_t >
inline void mtlClear(type_t *mem, int num) { memset(mem, 0, sizeof(type_t) * num); }

// mtlClear
// Sets the value of all bytes in type to 0.
template < typename type_t >
inline void mtlClear(type_t &mem) { memset(&mem, 0, sizeof(type_t)); }

// mtlCopy
// Copies 'src' into 'dst' byte-for-byte for 'num' elements.
template < typename type_t >
inline void mtlCopy(type_t *dst, const type_t *src, int num) { memcpy(dst, src, sizeof(type_t) * mtlclip0(num)); }

// mtlCopyOverlap
// Copyes 'src' into 'dst' byte-for-byte for 'num' elements and handles the event where the memory regions overlap.
template < typename type_t >
inline void mtlCopyOverlap(type_t *dst, const type_t *src, int num) { memmove(dst, src, sizeof(type_t) * mtlclip0(num)); }

// mtlCompare
// Compares 'a' and 'b' byte-for-byte for 'num' elements.
template < typename type_t >
inline bool mtlCompare(const type_t *a, const type_t *b, int num) { return memcmp(a, b, sizeof(type_t) * mtlclip0(num)) == 0; }

// mtlSet
// Writes the contents of 'b' into each element of 'a' for 'num' elements.
template < typename type_t >
inline void mtlSet(type_t *a, const type_t &b, int num) { for (int i = 0; i < num; ++i) { a[i] = b; } }

#undef mtlclip0

#endif
