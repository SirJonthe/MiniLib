#ifndef MTL_MEMORY_H_INCLUDED__
#define MTL_MEMORY_H_INCLUDED__

#include <cstring>

#define __mtlclip0(x) (x < 0) ? 0 : x

template < typename type_t >
void mtlClear(type_t *mem, int num) { memset(mem, 0, sizeof(type_t) * num); }

template < typename type_t >
void mtlClear(type_t &mem) { memset(&mem, 0, sizeof(type_t)); }

template < typename type_t >
void mtlCopy(type_t *dst, const type_t *src, int num) { memcpy(dst, src, sizeof(type_t) * __mtlclip0(num)); }

template < typename type_t >
void mtlCopyOverlap(type_t *dst, const type_t *src, int num) { memmove(dst, src, sizeof(type_t) * __mtlclip0(num)); }

template < typename type_t >
bool mtlCompare(const type_t *a, const type_t *b, int num) { return memcmp(a, b, sizeof(type_t) * __mtlclip0(num)) == 0; }

#undef __mtlclip0

#endif
