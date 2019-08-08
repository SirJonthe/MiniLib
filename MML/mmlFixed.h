//
//  mmlFixed.h
//
//  Created by Jonathan Karlsson on 2016-11-27.
//  Copyright (c) 2016 Jonathan Karlsson. All rights reserved.
//

#ifndef MML_FIXED_H_INCLUDED
#define MML_FIXED_H_INCLUDED

#include "mmlInt.h"

#define lng_int(type) typename mml::int_cast<type>::next_t
#define srt_int(type) typename mml::int_cast<type>::prev_t

namespace mml
{

template < typename base_t >
class unit_range
{
	template < typename unit_t, typename int_t > friend int_t  operator*(unit_range<unit_t> x, int_t y);

private:
	base_t x;

public:
	unit_range( void ) : x() {}
	unit_range(double a) : x(base_t(a * MML_MAXOF(base_t))) {}
	unit_range(base_t a) : x(a) {}

	base_t base( void ) const { return x; }

	template < typename uint_t >
	uint_t mul_unsigned(uint_t y)
	{
		lng_int(base_t) a = x;
		lng_int(uint_t) b = y;
		return (b * (a + 1)) >> MML_BITSOF(base_t);
	}

	unit_range<base_t> operator*(unit_range<base_t> r)
	{
		lng_int(base_t) a = x;
		lng_int(base_t) b = r.x;
		return unit_range<base_t>(((a + 1) * (b + 1) - 1) >> MML_BITSOF(base_t));
	}

	bool operator < (unit_range r) const { return x <  r.x; }
	bool operator <=(unit_range r) const { return x <= r.x; }
	bool operator > (unit_range r) const { return x >  r.x; }
	bool operator >=(unit_range r) const { return x >= r.x; }
	bool operator ==(unit_range r) const { return x == r.x; }
	bool operator !=(unit_range r) const { return x != r.x; }
};

template <> class unit_range<mml::sint8>  {};
template <> class unit_range<mml::sint16> {};
template <> class unit_range<mml::sint32> {};
template <> class unit_range<mml::sint64> {};

template < typename base_t, typename int_t > int_t  operator*(unit_range<base_t> x, int_t y)
{
	lng_int(base_t) a = x.x;
	lng_int(int_t)  b = y;
	return (b * (a + 1)) >> MML_BITSOF(base_t);
}

template < typename base_t, typename int_t > int_t operator*(int_t y, unit_range<base_t> x) { return x * y; }

template < typename base_t, mml::uint32 prec >
class fixed
{
	template < typename t, mml::uint32 p> friend class fixed;
	template < typename unit_t, typename fixed_t, mml::uint32 n > friend fixed<fixed_t, n> operator*(unit_range<unit_t> x, fixed<fixed_t, n> y);
	template < typename unit_t, typename fixed_t, mml::uint32 n > friend fixed<fixed_t, n> operator*(fixed<fixed_t, n> y, unit_range<unit_t> x);

private:
	base_t x;

public:
	fixed( void ) : x() {}
	template < typename type_t >
	fixed(type_t a) : x(base_t(a * (1 << prec))) {}
	template < typename t, mml::uint32 p >
	fixed(const fixed<t, p> &r) : x(prec < p ? base_t(mmlSignedRShift(r.x, (p - prec))) : base_t(r.x * (1 << (prec - p)))) {}
	template < typename t >
	fixed(const fixed<t,prec> &r) : x(base_t(r.x)) {}

	typedef fixed<lng_int(base_t),prec>   long_t;
	typedef fixed<srt_int(base_t),prec>   short_t;
	typedef fixed<lng_int(base_t),prec*2> double_t;
	typedef fixed<srt_int(base_t),prec/2> half_t;

	operator base_t( void ) const { return mmlSignedRShift(x, prec); }

	template < typename type_t >
	fixed &operator=(type_t r) { x = r * (1 << prec); return *this; }
	template < typename t, mml::uint32 p >
	fixed &operator=(fixed<t,p> r) { x = prec < p ? base_t(mmlSignedRShift(r.x, (p - prec))) : base_t(r.x * (1 << (prec - p))); return *this; }
	template < typename t >
	fixed &operator=(fixed<t,prec> r) { x = base_t(r.x); return *this; }
	fixed &operator+=(fixed r) { x += r.x; return *this; }
	fixed &operator-=(fixed r) { x -= r.x; return *this; }
	fixed &operator*=(fixed r) { x = mmlSignedRShift(x * r.x, prec); return *this; }
	fixed &operator/=(fixed r) { x = base_t((lng_int(base_t)(x) * (1 << prec)) / r.x); return *this; }

	fixed operator-( void ) const { fixed o; o.x = -x; return o; }

	bool operator < (fixed r) const { return x <  r.x; }
	bool operator <=(fixed r) const { return x <= r.x; }
	bool operator > (fixed r) const { return x >  r.x; }
	bool operator >=(fixed r) const { return x >= r.x; }
	bool operator ==(fixed r) const { return x == r.x; }
	bool operator !=(fixed r) const { return x != r.x; }
};

template < typename base_t, mml::uint32 prec >
fixed<base_t, prec> operator+(fixed<base_t, prec> l, fixed<base_t, prec> r) { return l += r; }
template < typename base_t, mml::uint32 prec >
fixed<base_t, prec> operator-(fixed<base_t, prec> l, fixed<base_t, prec> r) { return l -= r; }
template < typename base_t, mml::uint32 prec >
fixed<base_t, prec> operator*(fixed<base_t, prec> l, fixed<base_t, prec> r) { return l *= r; }
template < typename base_t, mml::uint32 prec >
fixed<base_t, prec> operator/(fixed<base_t, prec> l, fixed<base_t, prec> r) { return l /= r; }

template < typename unit_t, typename fixed_t, mml::uint32 n >
fixed<fixed_t, n> operator*(unit_range<unit_t> x, fixed<fixed_t, n> y)
{
	fixed<fixed_t, n> o;
	o.x = x * y.x;
	return o;
}

template < typename unit_t, typename fixed_t, mml::uint32 n >
fixed<fixed_t, n> operator*(fixed<fixed_t, n> y, unit_range<unit_t> x)
{
	fixed<fixed_t, n> o;
	o.x = y.x * x;
	return o;
}

}

#undef lng_int
#undef srt_int

//
// unpack_one
// Unpacks a set of bits to the next larger
// format, e.g. short -> int. Interleaves
// bits so that 1011 -> 11001111.
//
//template < typename base_t > inline mml_next mml_unpack_one(base_t x) { return 0; }

//template <> inline unsigned short mml_unpack_one(unsigned char x)
//{
//	unsigned short y = (unsigned short)x;
//	y = (y | (y << 4)) & 0x0F0F;
//	y = (y | (y << 2)) & 0x3333;
//	y = (y | (y << 1)) & 0x5555;
//	return y | (y << 1);
//}

//template <> inline unsigned int mml_unpack_one(unsigned short x)
//{
//	unsigned int y = (unsigned int)x;
//	y = (y | (y << 8)) & 0x00FF00FF;
//	y = (y | (y << 4)) & 0x0F0F0F0F;
//	y = (y | (y << 2)) & 0x33333333;
//	y = (y | (y << 1)) & 0x55555555;
//	return y | (y << 1);
//}

//template <> inline unsigned long long mml_unpack_one(unsigned int x)
//{
//	unsigned long long y = (unsigned long long)x;
//	y = (y | (y << 16)) & 0x0000FFFF0000FFFF;
//	y = (y | (y << 8))  & 0x00FF00FF00FF00FF;
//	y = (y | (y << 4))  & 0x0F0F0F0F0F0F0F0F;
//	y = (y | (y << 2))  & 0x3333333333333333;
//	y = (y | (y << 1))  & 0x5555555555555555;
//	return y | (y << 1);
//}

//template<> inline short mml_unpack_one(char x)
//{
//	return short(mml_unpack_one((unsigned char)x) & ~(0x4000));
//}

//template<> inline int mml_unpack_one(short x)
//{
//	return int(mml_unpack_one((unsigned short)x) & ~(0x40000000));
//}

//template<> inline long long mml_unpack_one(int x)
//{
//	return (long long)(mml_unpack_one((unsigned int)x) & ~(0x4000000000000000));
//}

//
// mml_unpack
// Unpacks an arbitrary format to an arbitrary format.
//
//template < typename unpack_t, typename base_t > inline unpack_t mml_unpack(base_t x) { return 0; }

//template <> inline unsigned char mml_unpack(unsigned char x) { return x; }
//template <> inline unsigned short mml_unpack(unsigned char x) { return mml_unpack_one(x); }
//template <> inline unsigned int mml_unpack(unsigned char x) { return mml_unpack_one(mml_unpack_one(x)); }
//template <> inline unsigned long long mml_unpack(unsigned char x) { return mml_unpack_one(mml_unpack_one(mml_unpack_one(x))); }
//template <> inline unsigned short mml_unpack(unsigned short x) { return x; }
//template <> inline unsigned int mml_unpack(unsigned short x) { return mml_unpack_one(x); }
//template <> inline unsigned long long mml_unpack(unsigned short x) { return mml_unpack_one(mml_unpack_one(x)); }
//template <> inline unsigned int mml_unpack(unsigned int x) { return x; }
//template <> inline unsigned long long mml_unpack(unsigned int x) { return mml_unpack_one(x); }

//template <> inline char mml_unpack(char x) { return x; }
//template <> inline short mml_unpack(char x) { return mml_unpack_one(x); }
//template <> inline int mml_unpack(char x) { return mml_unpack_one(mml_unpack_one(x)); }
//template <> inline long long mml_unpack(char x) { return mml_unpack_one(mml_unpack_one(mml_unpack_one(x))); }
//template <> inline short mml_unpack(short x) { return x; }
//template <> inline int mml_unpack(short x) { return mml_unpack_one(x); }
//template <> inline long long mml_unpack(short x) { return mml_unpack_one(mml_unpack_one(x)); }
//template <> inline int mml_unpack(int x) { return x; }
//template <> inline long long mml_unpack(int x) { return mml_unpack_one(x); }

#endif
