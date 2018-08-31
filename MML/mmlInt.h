#ifndef MML_INT_H_INCLUDED
#define MML_INT_H_INCLUDED

#include <limits>
#include <climits>

namespace mml
{

typedef signed char        sint8;
typedef signed short       sint16;
typedef signed int         sint32;
typedef signed long long   sint64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

template < typename base_t > class int_cast {};

template <>
class int_cast<mml::sint8>
{
public:
	typedef mml::sint8  prev_t;
	typedef mml::sint16 next_t;
	typedef mml::sint8  sign_t;
};

template <>
class int_cast<mml::sint16>
{
public:
	typedef mml::sint8  prev_t;
	typedef mml::sint32 next_t;
	typedef mml::sint16 sign_t;
};

template <>
class int_cast<mml::sint32>
{
public:
	typedef mml::sint16 prev_t;
	typedef mml::sint64 next_t;
	typedef mml::sint32 sign_t;
};

template <>
class int_cast<mml::sint64>
{
public:
	typedef mml::sint32 prev_t;
	typedef mml::sint64 next_t;
	typedef mml::sint64 sign_t;
};

template <>
class int_cast<mml::uint8>
{
public:
	typedef mml::uint8  prev_t;
	typedef mml::uint16 next_t;
	typedef mml::sint8  sign_t;
};

template <>
class int_cast<mml::uint16>
{
public:
	typedef mml::uint8  prev_t;
	typedef mml::uint32 next_t;
	typedef mml::sint16 sign_t;
};

template <>
class int_cast<mml::uint32>
{
public:
	typedef mml::uint16 prev_t;
	typedef mml::uint64 next_t;
	typedef mml::sint32 sign_t;
};

template <>
class int_cast<mml::uint64>
{
public:
	typedef mml::uint32 prev_t;
	typedef mml::uint64 next_t;
	typedef mml::sint64 sign_t;
};

template < typename base_t > class va_cast              { public: typedef base_t      va_t; };
template <>                  class va_cast<mml::sint8>  { public: typedef mml::sint32 va_t; };
template <>                  class va_cast<mml::sint16> { public: typedef mml::sint32 va_t; };
template <>                  class va_cast<mml::uint8>  { public: typedef mml::uint32 va_t; };
template <>                  class va_cast<mml::uint16> { public: typedef mml::uint32 va_t; };
template <>                  class va_cast<float>       { public: typedef double      va_t; };

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
inline sint_t mmlSignedRShift(sint_t x, mml::uint32 n)
{
	return
		(x >> n)
#ifndef MML_SIGNED_SHIFT
		| (x & (1 << (MML_BITSOF(sint_t) - 1)))
#endif
	;
}

#ifndef MML_SIGNED_SHIFT
template <> inline mml::uint8  mmlSignedRShift<mml::uint8>(mml::uint8 x, mml::uint32 n)   { return (x >> n); }
template <> inline mml::uint16 mmlSignedRShift<mml::uint16>(mml::uint16 x, mml::uint32 n) { return (x >> n); }
template <> inline mml::uint32 mmlSignedRShift<mml::uint32>(mml::uint32 x, mml::uint32 n) { return (x >> n); }
template <> inline mml::uint64 mmlSignedRShift<mml::uint64>(mml::uint64 x, mml::uint32 n) { return (x >> n); }
#endif

template < typename sint_t >
inline sint_t mmlSignedLShift(sint_t x, mml::uint32 n)
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
template <> inline mml::uint8  mmlSignedLShift<mml::uint8>(mml::uint8 x, mml::uint32 n)   { return (x << n); }
template <> inline mml::uint16 mmlSignedLShift<mml::uint16>(mml::uint16 x, mml::uint32 n) { return (x << n); }
template <> inline mml::uint32 mmlSignedLShift<mml::uint32>(mml::uint32 x, mml::uint32 n) { return (x << n); }
template <> inline mml::uint64 mmlSignedLShift<mml::uint64>(mml::uint64 x, mml::uint32 n) { return (x << n); }
#endif

#endif // MML_INT_H_INCLUDED__
