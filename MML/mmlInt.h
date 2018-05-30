#ifndef MML_INT_H_INCLUDED__
#define MML_INT_H_INCLUDED__

#include <limits>
#include <climits>

namespace mml
{

typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

template < typename base_t > class int_cast {};

template <>
class int_cast<mml::s8>
{
public:
	typedef mml::s8  prev_t;
	typedef mml::s16 next_t;
	typedef mml::s8  sign_t;
};

template <>
class int_cast<mml::s16>
{
public:
	typedef mml::s8  prev_t;
	typedef mml::s32 next_t;
	typedef mml::s16 sign_t;
};

template <>
class int_cast<mml::s32>
{
public:
	typedef mml::s16 prev_t;
	typedef mml::s64 next_t;
	typedef mml::s32 sign_t;
};

template <>
class int_cast<mml::s64>
{
public:
	typedef mml::s32 prev_t;
	typedef mml::s64 next_t;
	typedef mml::s64 sign_t;
};

template <>
class int_cast<mml::u8>
{
public:
	typedef mml::u8  prev_t;
	typedef mml::u16 next_t;
	typedef mml::s8  sign_t;
};

template <>
class int_cast<mml::u16>
{
public:
	typedef mml::u8  prev_t;
	typedef mml::u32 next_t;
	typedef mml::s16 sign_t;
};

template <>
class int_cast<mml::u32>
{
public:
	typedef mml::u16 prev_t;
	typedef mml::u64 next_t;
	typedef mml::s32 sign_t;
};

template <>
class int_cast<mml::u64>
{
public:
	typedef mml::u32 prev_t;
	typedef mml::u64 next_t;
	typedef mml::s64 sign_t;
};

template < typename base_t > class va_cast           { public: typedef base_t   va_t; };
template <>                  class va_cast<mml::s8>  { public: typedef mml::s32 va_t; };
template <>                  class va_cast<mml::s16> { public: typedef mml::s32 va_t; };
template <>                  class va_cast<mml::u8>  { public: typedef mml::u32 va_t; };
template <>                  class va_cast<mml::u16> { public: typedef mml::u32 va_t; };
template <>                  class va_cast<float>    { public: typedef double   va_t; };

}

#define MML_BITSOF(type) (sizeof(type) * CHAR_BIT)
#define MML_MAXOF(type)  (std::numeric_limits<type>::max())
#define MML_MINOF(type)  (std::numeric_limits<type>::min())

#if ((-2 >> 1) == (-2 / 2)) && ((-2 << 1) == (-2 * 2))
	#define MML_SIGNED_SHIFT 1
#endif

/*#if !(((4 << -1) != (4 >> 1)) || ((4 >> -1) != (4 << 1)))
	#define MML_NEG_SHIFT 1
#endif*/

template < typename sint_t >
inline sint_t mmlSignedRShift(sint_t x, mml::u32 n)
{
	return
		(x >> n)
#ifndef MML_SIGNED_SHIFT
		| (x & (1 << (MML_BITSOF(sint_t) - 1)))
#endif
	;
}

#ifndef MML_SIGNED_SHIFT
template <> inline mml::u8  mmlSignedRShift<mml::u8>(mml::u8 x, mml::u32 n)   { return (x >> n); }
template <> inline mml::u16 mmlSignedRShift<mml::u16>(mml::u16 x, mml::u32 n) { return (x >> n); }
template <> inline mml::u32 mmlSignedRShift<mml::u32>(mml::u32 x, mml::u32 n) { return (x >> n); }
template <> inline mml::u64 mmlSignedRShift<mml::u64>(mml::u64 x, mml::u32 n) { return (x >> n); }
#endif

template < typename sint_t >
inline sint_t mmlSignedLShift(sint_t x, mml::u32 n)
{
	return
#ifndef MML_SIGNED_SHIFT
		x * (1 << n)
#else
		(x << n)
#endif
	;
}

#ifndef MML_SIGNED_SHIFT
template <> inline mml::u8  mmlSignedLShift<mml::u8>(mml::u8 x, mml::u32 n)   { return (x << n); }
template <> inline mml::u16 mmlSignedLShift<mml::u16>(mml::u16 x, mml::u32 n) { return (x << n); }
template <> inline mml::u32 mmlSignedLShift<mml::u32>(mml::u32 x, mml::u32 n) { return (x << n); }
template <> inline mml::u64 mmlSignedLShift<mml::u64>(mml::u64 x, mml::u32 n) { return (x << n); }
#endif

#endif // MML_INT_H_INCLUDED__
