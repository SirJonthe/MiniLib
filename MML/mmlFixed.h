//
//  mmlFixed.h
//  mmlFixed
//
//  Created by Jonathan Karlsson on 8/7/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#ifndef MML_FIXED_H_INCLUDED__
#define MML_FIXED_H_INCLUDED__

#include <climits>

//
// compile time information about our type
//
#define mml_is_signed(type) ( ((type)-1) < 0 )
#define mml_signedness(type) ( mml_is_signed(type) ? -1 : 1 )
#define mml_bitsizeof(type) ( sizeof(type) * CHAR_BIT )
#define mml_maxprecision(type) ( mml_bitsizeof(type) - mml_is_signed(type) )
#define mml_signbit(type) ( (type)(mml_is_signed(type) ? (((type)1) << (mml_bitsizeof(type) - 1)) : 0) )
#define mml_maxvalue(type) ( (type)(~mml_signbit(type)) )
#define mml_minvalue(type) ( (type)(mml_maxvalue(type) + 1) )

//
// mml_base_info
// Information about the base type used for fixed point arithmetic.
// Will also result in compiler errors if programmer tries to use
// unsupported types as base for fixed point arithmetic.
//
template < typename base_t >
class mml_base_info
{	
};

template <>
class mml_base_info<unsigned char>
{
public:
	typedef unsigned short next_type;
	typedef unsigned char prev_type;
	typedef char signed_type;
	typedef unsigned char unsigned_type;
	typedef signed_type reversed_type;
	
	static next_type upcast(next_type uc) { return uc; }
	
	static unsigned char max( void ) { return UCHAR_MAX; }
	static unsigned char min( void ) { return 0; }
	
	static unsigned int bits( void ) { return sizeof(unsigned char)*CHAR_BIT; }
	static unsigned_type bitmask( void ) { return UCHAR_MAX; }
	static unsigned_type signbit( void ) { return 0; }
	
	static bool is_signed( void ) { return false; }
};

template <>
class mml_base_info<unsigned short>
{
public:
	typedef unsigned int next_type;
	typedef unsigned char prev_type;
	typedef short signed_type;
	typedef unsigned short unsigned_type;
	typedef signed_type reversed_type;
	
	static next_type upcast(next_type us) { return us; }
	
	static unsigned short max( void ) { return USHRT_MAX; }
	static unsigned short min( void ) { return 0; }
	
	static unsigned int bits( void ) { return sizeof(unsigned short)*CHAR_BIT; }
	static unsigned_type bitmask( void ) { return USHRT_MAX; }
	static unsigned_type signbit( void ) { return 0; }
	
	static bool is_signed( void ) { return false; }
};

template <>
class mml_base_info<unsigned int>
{
public:
	typedef unsigned long long next_type;
	typedef unsigned short prev_type;
	typedef int signed_type;
	typedef unsigned int unsigned_type;
	typedef signed_type reversed_type;
	
	static next_type upcast(next_type ui) { return ui; }
	
	static unsigned int max( void ) { return UINT_MAX; }
	static unsigned int min( void ) { return 0; }
	
	static unsigned int bits( void ) { return sizeof(unsigned int)*CHAR_BIT; }
	static unsigned_type bitmask( void ) { return UINT_MAX; }
	static unsigned_type signbit( void ) { return 0; }
	
	static bool is_signed( void ) { return false; }
};

template <>
class mml_base_info<unsigned long long>
{
public:
	typedef unsigned long long next_type;
	typedef unsigned int prev_type;
	typedef long long signed_type;
	typedef unsigned long long unsigned_type;
	typedef signed_type reversed_type;
	
	static next_type upcast(next_type ul) { return ul; }
	
	static unsigned long long max( void ) { return ULLONG_MAX; }
	static unsigned long long min( void ) { return 0; }
	
	static unsigned int bits( void ) { return sizeof(unsigned long long)*CHAR_BIT; }
	static unsigned_type bitmask( void ) { return ULLONG_MAX; }
	static unsigned_type signbit( void ) { return 0; }
	
	static bool is_signed( void ) { return false; }
};

template <>
class mml_base_info<char>
{
public:
	typedef short next_type;
	typedef char prev_type;
	typedef char signed_type;
	typedef unsigned char unsigned_type;
	typedef unsigned_type reversed_type;
	
	static next_type upcast(next_type c) { return c; }
	
	static char max( void ) { return CHAR_MAX; }
	static char min( void ) { return CHAR_MIN; }
	
	static unsigned int bits( void ) { return sizeof(char)*CHAR_BIT-1; }
	static unsigned_type bitmask( void ) { return UCHAR_MAX >> 1; }
	static unsigned_type signbit( void ) { return ~bitmask(); }
	
	static bool is_signed( void ) { return true; }
};

template <>
class mml_base_info<short>
{
public:
	typedef int next_type;
	typedef char prev_type;
	typedef short signed_type;
	typedef unsigned short unsigned_type;
	typedef unsigned_type reversed_type;
	
	static next_type upcast(next_type s) { return s; }
	
	static short max( void ) { return SHRT_MAX; }
	static short min( void ) { return SHRT_MIN; }
	
	static unsigned int bits( void ) { return sizeof(short)*CHAR_BIT-1; }
	static unsigned_type bitmask( void ) { return USHRT_MAX >> 1; }
	static unsigned_type signbit( void ) { return ~bitmask(); }
	
	static bool is_signed( void ) { return true; }
};

template <>
class mml_base_info<int>
{
public:
	typedef long long next_type;
	typedef short prev_type;
	typedef int signed_type;
	typedef unsigned int unsigned_type;
	typedef unsigned_type reversed_type;
	
	static next_type upcast(next_type i) { return i; }
	
	static int max( void ) { return INT_MAX; }
	static int min( void ) { return INT_MIN; }
	
	static unsigned int bits( void ) { return sizeof(int)*CHAR_BIT-1; }
	static unsigned_type bitmask( void ) { return UINT_MAX >> 1; }
	static unsigned_type signbit( void ) { return ~bitmask(); }
	
	static bool is_signed( void ) { return true; }
};

template <>
class mml_base_info<long long>
{
public:
	typedef long long next_type;
	typedef int prev_type;
	typedef long long signed_type;
	typedef unsigned long long unsigned_type;
	typedef unsigned_type reversed_type;
	
	static next_type upcast(next_type l) { return l; }
	
	static long long max( void ) { return LLONG_MAX; }
	static long long min( void ) { return LLONG_MIN; }
	
	static unsigned int bits( void ) { return sizeof(long long)*CHAR_BIT-1; }
	static unsigned_type bitmask( void ) { return ULLONG_MAX >> 1; }
	static unsigned_type signbit( void ) { return ~bitmask(); }
	
	static bool is_signed( void ) { return true; }
};

#define mml_info mml_base_info<base_t>
#define mml_unsigned typename mml_info::unsigned_type
#define mml_next typename mml_info::next_type
#define mml_prev typename mml_info::prev_type

#if (-4 >> 1) != -2 || (-4 << 1) != -8
	#define NO_ARITHMETIC_SHIFT
	#warning Performance: Arithmetic shift of signed types is emulated.
#endif

/*#if (4 << -1) != (4 >> 1) || (4 >> -1) != (4 << 1)
	#define NO_SIGNED_SHIFT
	#warning Performance: Shifting by negative numbers is emulated.
#endif*/

//
// mml_shift_left
// Logic shift for unsigned types,
// arithmetic shift for signed types.
//
template < typename base_t >
base_t mml_shift_left(base_t x, unsigned int shift)
{
#ifdef NO_ARITHMETIC_SHIFT
	return (x << shift) | (x & mml_info::signbit());
#else
	return x << shift;
#endif
}

template <> unsigned char mml_shift_left(unsigned char x, unsigned int shift) { return x << shift; }
template <> unsigned short mml_shift_left(unsigned short x, unsigned int shift) { return x << shift; }
template <> unsigned int mml_shift_left(unsigned int x, unsigned int shift) { return x << shift; }
template <> unsigned long long mml_shift_left(unsigned long long x, unsigned int shift) { return x << shift; }

//
// mml_shift_right
// Logic shift for unsigned types,
// arithmetic shift for signed types.
//
template < typename base_t >
base_t mml_shift_right(base_t x, unsigned int shift)
{
#ifdef NO_ARITHMETIC_SHIFT
	mml_unsigned c = x < 0 ? ~((mml_unsigned)0) : (mml_unsigned)0;
	return ((x ^ c) >> shift) ^ c;
#else
	return x >> shift;
#endif
}

template <> unsigned char mml_shift_right(unsigned char x, unsigned int shift) { return x >> shift; }
template <> unsigned short mml_shift_right(unsigned short x, unsigned int shift) { return x >> shift; }
template <> unsigned int mml_shift_right(unsigned int x, unsigned int shift) { return x >> shift; }
template <> unsigned long long mml_shift_right(unsigned long long x, unsigned int shift) { return x >> shift; }

//
// mml_signed_shift_left
// Shifting by a negative number means
// shift in opposite direction.
//
template < typename base_t >
base_t mml_signed_shift_left(base_t x, int shift)
{
	return (shift < 0) ? mml_right_shift(x, (unsigned int)(-shift)) : mml_left_shift(x, (unsigned int)shift);
}

//
// mml_signed_shift_right
// Shifting by a negative number means
// shift in opposite direction.
//
template < typename base_t >
base_t mml_signed_shift_right(base_t x, int shift)
{
	return (shift < 0) ? mml_left_shift(x, (unsigned int)(-shift)) : mml_right_shift(x, (unsigned int)shift);
}

//
// unpack_one
// Unpacks a set of bits to the next larger
// format, e.g. short -> int. Interleaves
// bits so that 1011 -> 11001111.
//
template < typename base_t > mml_next mml_unpack_one(base_t x) { return 0; }

template <> unsigned short mml_unpack_one(unsigned char x)
{
	unsigned short y = (unsigned short)x;
	y = (y | (y << 4)) & 0x0F0F;
	y = (y | (y << 2)) & 0x3333;
	y = (y | (y << 1)) & 0x5555;
	return y | (y << 1);
}

template <> unsigned int mml_unpack_one(unsigned short x)
{
	unsigned int y = (unsigned int)x;
	y = (y | (y << 8)) & 0x00FF00FF;
	y = (y | (y << 4)) & 0x0F0F0F0F;
	y = (y | (y << 2)) & 0x33333333;
	y = (y | (y << 1)) & 0x55555555;
	return y | (y << 1);
}

template <> unsigned long long mml_unpack_one(unsigned int x)
{
	unsigned long long y = (unsigned long long)x;
	y = (y | (y << 16)) & 0x0000FFFF0000FFFF;
	y = (y | (y << 8))  & 0x00FF00FF00FF00FF;
	y = (y | (y << 4))  & 0x0F0F0F0F0F0F0F0F;
	y = (y | (y << 2))  & 0x3333333333333333;
	y = (y | (y << 1))  & 0x5555555555555555;
	return y | (y << 1);
}

template<> short mml_unpack_one(char x)
{
	return short(mml_unpack_one((unsigned char)x) & ~(0x4000));
}

template<> int mml_unpack_one(short x)
{
	return int(mml_unpack_one((unsigned short)x) & ~(0x40000000));
}

template<> long long mml_unpack_one(int x)
{
	return (long long)(mml_unpack_one((unsigned int)x) & ~(0x4000000000000000));
}

//
// mml_unpack
// Unpacks an arbitrary format to an arbitrary format.
//
template < typename unpack_t, typename base_t > unpack_t mml_unpack(base_t x) { return 0; }

template <> unsigned char mml_unpack(unsigned char x) { return x; }
template <> unsigned short mml_unpack(unsigned char x) { return mml_unpack_one(x); }
template <> unsigned int mml_unpack(unsigned char x) { return mml_unpack_one(mml_unpack_one(x)); }
template <> unsigned long long mml_unpack(unsigned char x) { return mml_unpack_one(mml_unpack_one(mml_unpack_one(x))); }
template <> unsigned short mml_unpack(unsigned short x) { return x; }
template <> unsigned int mml_unpack(unsigned short x) { return mml_unpack_one(x); }
template <> unsigned long long mml_unpack(unsigned short x) { return mml_unpack_one(mml_unpack_one(x)); }
template <> unsigned int mml_unpack(unsigned int x) { return x; }
template <> unsigned long long mml_unpack(unsigned int x) { return mml_unpack_one(x); }

template <> char mml_unpack(char x) { return x; }
template <> short mml_unpack(char x) { return mml_unpack_one(x); }
template <> int mml_unpack(char x) { return mml_unpack_one(mml_unpack_one(x)); }
template <> long long mml_unpack(char x) { return mml_unpack_one(mml_unpack_one(mml_unpack_one(x))); }
template <> short mml_unpack(short x) { return x; }
template <> int mml_unpack(short x) { return mml_unpack_one(x); }
template <> long long mml_unpack(short x) { return mml_unpack_one(mml_unpack_one(x)); }
template <> int mml_unpack(int x) { return x; }
template <> long long mml_unpack(int x) { return mml_unpack_one(x); }

#define mml_uniform_one mml_info::max()

//
// mml_fixed_uniform
// A fixed point format where 0.0f - 1.0f / -1.0f - 1.0f
// are the permitted values. Uses all bits in base for
// precision. Clamps addition and subtraction.
//
template < typename base_t >
class mml_fixed_uniform
{
private:
	base_t x;
	
public:
	mml_fixed_uniform( void ) : x(0) {}
	mml_fixed_uniform(double f) : x((mml_info::is_signed() ? mmlClamp(-1.0, f, 1.0) : mmlClamp(0.0, f, 1.0)) * double(mml_uniform_one)) {}
	mml_fixed_uniform(const mml_fixed_uniform<base_t> &f) : x(f.x) {}
	mml_fixed_uniform &operator=(mml_fixed_uniform<base_t> f) { x = f.x; return *this; }
	template < typename base2_t >
	explicit mml_fixed_uniform(const mml_fixed_uniform<base2_t> &f) : x(f.to_double() * double(mml_uniform_one)) {}
	
	double to_double( void ) const { return double(x) / double(mml_info::max()); }
	
	inline mml_fixed_uniform &operator+=(mml_fixed_uniform r);
	inline mml_fixed_uniform &operator-=(mml_fixed_uniform r);
	inline mml_fixed_uniform &operator*=(mml_fixed_uniform r);
	
	bool operator==(mml_fixed_uniform r) const { return x == r.x; }
	bool operator!=(mml_fixed_uniform r) const { return x != r.x; }
	bool operator<=(mml_fixed_uniform r) const { return x <= r.x; }
	bool operator>=(mml_fixed_uniform r) const { return x >= r.x; }
	bool operator<(mml_fixed_uniform r) const { return x < r.x; }
	bool operator>(mml_fixed_uniform r) const { return x > r.x; }
	
	typedef base_t base_type;
	
	base_t base( void ) const { return x; }
};

// Due to limited range of float and double, these two will not work so we disable them
template <> class mml_fixed_uniform<unsigned long long> {};
template <> class mml_fixed_uniform<long long> {};

template < typename base_t >
inline mml_fixed_uniform<base_t> &mml_fixed_uniform<base_t>::operator+=(mml_fixed_uniform<base_t> r)
{
	x = mmlClamp(
		mml_info::upcast(mml_info::min()),
		mml_info::upcast(x + r.x),
		mml_info::upcast(mml_info::max())
	);
	return *this;
}

template < typename base_t >
inline mml_fixed_uniform<base_t> &mml_fixed_uniform<base_t>::operator-=(mml_fixed_uniform<base_t> r)
{
	x = mmlClamp(
		mml_info::upcast(mml_info::min()),
		mml_info::upcast(x - r.x),
		mml_info::upcast(mml_info::max())
	);
	return *this;
}

template < typename base_t >
inline mml_fixed_uniform<base_t> &mml_fixed_uniform<base_t>::operator*=(mml_fixed_uniform<base_t> r)
{
	/*
	mml_next lx = (mml_next)x;
	mml_next rx = (mml_next)r.x;
	x = ((lx ^ rx) < 0) ?
	base_t( mml_shift_right((lx + 1) * (rx + 1), mml_info::bits()) - 1 ) :
	base_t( mml_shift_right(lx * rx, mml_info::bits()) + 1 );
	return *this;
	*/
	
	mml_next lx = (mml_next)x;
	mml_next rx = (mml_next)r.x;
	if (!mml_info::is_signed()) {
		//x = (((lx + 1) * (rx + 1)) >> mml_info::bits()) - 1;
		x = (lx * (rx + 1)) >> mml_info::bits();
	} else {
		mml_next ls = (lx < 0) ? 1 : -1;
		mml_next rs = (rx < 0) ? 1 : -1;
		x = ((lx ^ rx) < 0) ?
		base_t( mml_shift_right((lx + ls) * (rx + rs), mml_info::bits()) - 1 ) :
		base_t( mml_shift_right(lx * rx, mml_info::bits()) + 1 );
	}
	return *this;
}

template < typename base_t >
inline mml_fixed_uniform<base_t> operator+(mml_fixed_uniform<base_t> l, mml_fixed_uniform<base_t> r) { return l+=r; }
template < typename base_t >
inline mml_fixed_uniform<base_t> operator-(mml_fixed_uniform<base_t> l, mml_fixed_uniform<base_t> r) { return l-=r; }
template < typename base_t >
inline mml_fixed_uniform<base_t> operator*(mml_fixed_uniform<base_t> l, mml_fixed_uniform<base_t> r) { return l*=r; }

#define mml_real_one (1<<prec_n)

//
// mml_fixed_real
//
template < typename base_t, unsigned int prec_n >
class mml_fixed_real
{
private:
	base_t x;
	
public:
	mml_fixed_real( void ) {}
	mml_fixed_real(int i) : x(mml_shift_left((base_t)i, prec_n)) {}
	mml_fixed_real(unsigned int i) : x(mml_shift_left((base_t)i, prec_n)) {}
	mml_fixed_real(float f) : x(base_t(f * float(mml_real_one))) {}
	template < typename base2_t, unsigned int prec2_n >
	mml_fixed_real(mml_fixed_real<base2_t,prec2_n> f) : x((prec_n < prec2_n) ? mml_shift_right((base_t)f.base(), (prec2_n - prec_n)) : mml_shift_left((base_t)f.base(), (prec_n - prec2_n))) {}
	mml_fixed_real(const mml_fixed_real<base_t, prec_n> &f) : x(f.x) {}
	mml_fixed_real &operator=(mml_fixed_real<base_t, prec_n> f) { x = f.x; return *this; }
	explicit mml_fixed_real(mml_fixed_uniform<mml_prev> f) : x(base_t(f.base()) + 1) {}
	
	float to_float( void ) const { return float(x) / float(mml_real_one); }
	double to_double( void ) const { return double(x) / double(mml_real_one); }
	base_t to_int( void ) const { return mml_shift_right(x, prec_n); }
	
	inline mml_fixed_real operator+=(mml_fixed_real r);
	inline mml_fixed_real operator-=(mml_fixed_real r);
	inline mml_fixed_real operator*=(mml_fixed_real r);
	//inline mml_fixed_real< mml_next, (prec_n<<1)> operator*=(mml_fixed_real r);
	inline mml_fixed_real operator/=(mml_fixed_real r);
	
	inline mml_fixed_real operator*=(mml_fixed_uniform<base_t> r);
	
	mml_fixed_real &operator++( void ) { x += mml_real_one; return *this; }
	mml_fixed_real &operator--( void ) { x -= mml_real_one; return *this; }
	mml_fixed_real operator++( int ) { mml_fixed_real f = *this; x += mml_real_one; return f; }
	mml_fixed_real operator--( int ) { mml_fixed_real f = *this; x -= mml_real_one; return f; }
	
	bool operator==(mml_fixed_real r) const { return x == r.x; }
	bool operator!=(mml_fixed_real r) const { return x != r.x; }
	bool operator<=(mml_fixed_real r) const { return x <= r.x; }
	bool operator>=(mml_fixed_real r) const { return x >= r.x; }
	bool operator<(mml_fixed_real r) const { return x < r.x; }
	bool operator>(mml_fixed_real r) const { return x > r.x; }
	
	typedef base_t base_type;
	
	base_t base( void ) const { return x; }
	unsigned int precision( void ) const { return prec_n; }
};

template < typename base_t, unsigned int prec_n >
mml_fixed_real<base_t, prec_n> mml_fixed_real<base_t, prec_n>::operator+=(mml_fixed_real<base_t, prec_n> r)
{
	x += r.x;
	return *this;
}

template < typename base_t, unsigned int prec_n >
mml_fixed_real<base_t, prec_n> mml_fixed_real<base_t, prec_n>::operator-=(mml_fixed_real<base_t, prec_n> r)
{
	x -= r.x;
	return *this;
}

template < typename base_t, unsigned int prec_n >
mml_fixed_real<base_t, prec_n> mml_fixed_real<base_t, prec_n>::operator*=(mml_fixed_real<base_t, prec_n> r)
{
	//if (mml_info::is_signed()) {
	//	x = base_t(mml_shift_right(mml_info::upcast(x) * mml_info::upcast(r.x), prec_n));
	//} else {
		x = base_t(mml_shift_right((mml_next)x * mml_next(r.x), prec_n));
	//}
	return *this;
}

template < typename base_t, unsigned int prec_n >
mml_fixed_real<base_t, prec_n> mml_fixed_real<base_t, prec_n>::operator/=(mml_fixed_real<base_t, prec_n> r)
{
	x = base_t(mml_shift_left((mml_next)x, prec_n) / r.x);
	return *this;
}

template < typename base_t, unsigned int prec_n >
inline mml_fixed_real<base_t,prec_n> operator+(mml_fixed_real<base_t,prec_n> l, mml_fixed_real<base_t,prec_n> r) { return l+=r; }
template < typename base_t, unsigned int prec_n >
inline mml_fixed_real<base_t,prec_n> operator-(mml_fixed_real<base_t,prec_n> l, mml_fixed_real<base_t,prec_n> r) { return l-=r; }
template < typename base_t, unsigned int prec_n >
inline mml_fixed_real<base_t,prec_n> operator*(mml_fixed_real<base_t,prec_n> l, mml_fixed_real<base_t,prec_n> r) { return l*=r; }
template < typename base_t, unsigned int prec_n >
inline mml_fixed_real<base_t,prec_n> operator/(mml_fixed_real<base_t,prec_n> l, mml_fixed_real<base_t,prec_n> r) { return l/=r; }

template < typename base_t, unsigned int prec_n >
mml_fixed_real<base_t,prec_n> mml_fixed_real<base_t,prec_n>::operator*=(mml_fixed_uniform<base_t> r)
{
	mml_next lx = (mml_next)x;
	mml_next rx = (mml_next)r.base();
	//if (!mml_info::is_signed()) {
		//x = (((lx + 1) * (rx + 1)) >> mml_info::bits()) - 1;
		x = (lx * rx) >> prec_n;
	//} else {
	//	mml_next ls = (lx < 0) ? 1 : -1;
	//	mml_next rs = (rx < 0) ? 1 : -1;
	//	x = ((lx ^ rx) < 0) ?
	//	base_t( mml_shift_right((lx + ls) * (rx + rs), mml_info::bits()) - 1 ) :
	//	base_t( mml_shift_right(lx * rx, mml_info::bits()) + 1 );
	//}
	return *this;
}

template < typename base_t, unsigned int prec_n >
inline mml_fixed_real<base_t,prec_n> operator*(mml_fixed_real<base_t,prec_n> l, mml_fixed_uniform<base_t> r) { return l*=r; }
template < typename base_t, unsigned int prec_n >
inline mml_fixed_real<base_t,prec_n> operator*(mml_fixed_uniform<base_t> l, mml_fixed_real<base_t,prec_n> r) { return r*=l; }

#undef mml_info
#undef mml_unsigned
#undef mml_next
#undef NO_ARITHMETIC_SHIFT
#undef NO_SIGNED_SHIFT
#undef mml_uniform_one
#undef mml_real_one

#endif
