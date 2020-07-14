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

struct uint128 { mml::uint64 hi, lo; };

inline mml::uint128 mul128(mml::uint64 l, mml::uint64 r)
{
	/* First calculate all of the cross products. */
	mml::uint64 lo_lo = (l & 0xffffffff) * (r & 0xffffffff);
	mml::uint64 hi_lo = (l >> 32)        * (r & 0xffffffff);
	mml::uint64 lo_hi = (l & 0xffffffff) * (r >> 32);
	mml::uint64 hi_hi = (l >> 32)        * (r >> 32);

	/* Now add the products together. These will never overflow. */
	mml::uint64 cross = (lo_lo >> 32) + (hi_lo & 0xffffffff) + lo_hi;
	mml::uint64 high  = (hi_lo >> 32) + (cross >> 32)        + hi_hi;
	mml::uint64 low   = (cross << 32) | (lo_lo & 0xffffffff);

	return mml::uint128 { high, low };
}

inline mml::uint64 div128(mml::uint128 l, mml::uint64 r)
{
	mml::uint64 q = l.lo << 1; // quotient
	mml::uint64 rem = l.hi; // remainder

	mml::uint64 carry = l.lo >> 63;
	mml::uint64 temp_carry = 0;

	for (mml::uint64 i = 0; i < 64; ++i) {
		temp_carry = rem >> 63;
		rem <<= 1;
		rem |= carry;
		carry = temp_carry;

		if(carry == 0) {
			if(rem >= r) {
				carry = 1;
			} else {
				temp_carry = q >> 63;
				q <<= 1;
				q |= carry;
				carry = temp_carry;
				continue;
			}
		}

		rem -= r;
		rem -= (1 - carry);
		carry = 1;
		temp_carry = q >> 63;
		q <<= 1;
		q |= carry;
		carry = temp_carry;
	}

	return q;
}

template < typename base_t > class int_cast {};

template <>
class int_cast<mml::sint8>
{
public:
	typedef mml::sint8  prev_t;
	typedef mml::sint16 next_t;
	typedef mml::sint8  sign_t;
	typedef mml::uint8  usign_t;
};

template <>
class int_cast<mml::sint16>
{
public:
	typedef mml::sint8  prev_t;
	typedef mml::sint32 next_t;
	typedef mml::sint16 sign_t;
	typedef mml::uint16 usign_t;
};

template <>
class int_cast<mml::sint32>
{
public:
	typedef mml::sint16 prev_t;
	typedef mml::sint64 next_t;
	typedef mml::sint32 sign_t;
	typedef mml::uint32 usign_t;
};

template <>
class int_cast<mml::sint64>
{
public:
	typedef mml::sint32 prev_t;
	typedef mml::sint64 next_t;
	typedef mml::sint64 sign_t;
	typedef mml::uint64 usign_t;
};

template <>
class int_cast<mml::uint8>
{
public:
	typedef mml::uint8  prev_t;
	typedef mml::uint16 next_t;
	typedef mml::sint8  sign_t;
	typedef mml::uint8  usign_t;
};

template <>
class int_cast<mml::uint16>
{
public:
	typedef mml::uint8  prev_t;
	typedef mml::uint32 next_t;
	typedef mml::sint16 sign_t;
	typedef mml::uint16 usign_t;
};

template <>
class int_cast<mml::uint32>
{
public:
	typedef mml::uint16 prev_t;
	typedef mml::uint64 next_t;
	typedef mml::sint32 sign_t;
	typedef mml::uint32 usign_t;
};

template <>
class int_cast<mml::uint64>
{
public:
	typedef mml::uint32 prev_t;
	typedef mml::uint64 next_t;
	typedef mml::sint64 sign_t;
	typedef mml::uint64 usign_t;
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
