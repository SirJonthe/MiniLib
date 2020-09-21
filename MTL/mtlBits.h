#ifndef MTL_BITS_H_INCLUDED
#define MTL_BITS_H_INCLUDED

#include <climits>

typedef unsigned long long int mtlFlags;
typedef unsigned char          mtlByte;

inline unsigned long long mtlEncodeMorton2(unsigned int x, unsigned int y)
{
	x = (x | (x << 8)) & 0x00FF00FF;
	x = (x | (x << 4)) & 0x0F0F0F0F;
	x = (x | (x << 2)) & 0x33333333;
	x = (x | (x << 1)) & 0x55555555;

	y = (y | (y << 8)) & 0x00FF00FF;
	y = (y | (y << 4)) & 0x0F0F0F0F;
	y = (y | (y << 2)) & 0x33333333;
	y = (y | (y << 1)) & 0x55555555;

	return x | (y << 1);
}

inline unsigned long long mtlEncodeMorton3(unsigned int x, unsigned int y, unsigned int z)
{
	x &= 0x001FFFFF;
	x = (x | (x << 16)) & 0xFF0000FF;
	x = (x | (x <<  8)) & 0x0300F00F;
	x = (x | (x <<  4)) & 0x030C30C3;
	x = (x | (x <<  2)) & 0x09249249;

	y &= 0x001FFFFF;
	y = (y | (y << 16)) & 0xFF0000FF;
	y = (y | (y <<  8)) & 0x0300F00F;
	y = (y | (y <<  4)) & 0x030C30C3;
	y = (y | (y <<  2)) & 0x09249249;

	z &= 0x001FFFFF;
	z = (z | (z << 16)) & 0xFF0000FF;
	z = (z | (z <<  8)) & 0x0300F00F;
	z = (z | (z <<  4)) & 0x030C30C3;
	z = (z | (z <<  2)) & 0x09249249;

	return x | (y << 1) | (z << 2);
}

inline unsigned int mtlDecodeMorton2X(unsigned long long code)
{
	code &= 0x55555555;
	code = (code | (code >>  1)) & 0x33333333;
	code = (code | (code >>  2)) & 0x0F0F0F0F;
	code = (code | (code >>  4)) & 0x00FF00FF;
	code = (code | (code >>  8)) & 0x0000FFFF;
	return static_cast<unsigned int>(code);
}

inline unsigned int mtlDecodeMorton2Y(unsigned long long code)
{
	code = (code >> 1) & 0x55555555;
	code = (code | (code >>  1)) & 0x33333333;
	code = (code | (code >>  2)) & 0x0F0F0F0F;
	code = (code | (code >>  4)) & 0x00FF00FF;
	code = (code | (code >>  8)) & 0x0000FFFF;
	return static_cast<unsigned int>(code);
}

inline unsigned int mtlDecodeMorton3X(unsigned long long code)
{
	code &= 0x09249249;
	code = (code | (code >>  2)) & 0x030C30C3;
	code = (code | (code >>  4)) & 0x0300F00F;
	code = (code | (code >>  8)) & 0xFF0000FF;
	code = (code | (code >> 16)) & 0x000003FF;
	return static_cast<unsigned int>(code);
}

inline unsigned int mtlDecodeMorton3Y(unsigned long long code)
{
	code = (code >> 1) & 0x09249249;
	code = (code | (code >>  2)) & 0x030C30C3;
	code = (code | (code >>  4)) & 0x0300F00F;
	code = (code | (code >>  8)) & 0xFF0000FF;
	code = (code | (code >> 16)) & 0x000003FF;
	return static_cast<unsigned int>(code);
}

inline unsigned int mtlDecodeMorton3Z(unsigned long long code)
{
	code = (code >> 2) & 0x09249249;
	code = (code | (code >>  2)) & 0x030C30C3;
	code = (code | (code >>  4)) & 0x0300F00F;
	code = (code | (code >>  8)) & 0xFF0000FF;
	code = (code | (code >> 16)) & 0x000003FF;
	return static_cast<unsigned int>(code);
}

// Used to scale integer fractions from one precision to another.
// For instance 5 bit 0-31 range value to 8 bit 0-255 (or vice versa).
// in_bits = the number of bits used for the value in value.
// out_bits = the number of bits 'value' is supposed to be scaled to.
// intermediate_precision = the number of bits used in intermediate calculations.
template < unsigned int in_bits, unsigned int out_bits, unsigned int intermediate_precision = 8 >
inline unsigned int mtlScaleBitRange(unsigned int value)
{
	return (value * (((1 << out_bits) << intermediate_precision) / ((1 << in_bits) - 1))) >> intermediate_precision;
}

// Used to scale integer fractions from one precision to another.
// For instance 5 bit 0-31 range value to 8 bit 0-255 (or vice versa).
// in_bits = the number of bits used for the value in value.
// out_bits = the number of bits 'value' is supposed to be scaled to.
// intermediate_precision = the number of bits used in intermediate calculations.
template < unsigned int intermediate_precision = 8 >
inline unsigned int mtlScaleBitRange(unsigned int value, unsigned int in_bits, unsigned int out_bits)
{
	return (value * (((1 << out_bits) << intermediate_precision) / ((1 << in_bits) - 1))) >> intermediate_precision;
}

template < typename type_t >
inline type_t mtl1BitMask(unsigned int i) { return type_t((type_t(1)) << i); }

template < typename type_t >
inline type_t mtlReadBit(type_t in_bits, unsigned int i) { return in_bits & mtl1BitMask<type_t>(i); }

template < typename type_t >
inline type_t mtlReadBit(const mtlByte *in_bits, unsigned int i) { return mtlReadBit(in_bits[i >> 3], i); }

template < typename type_t >
inline type_t mtlGetBitState(type_t in_bits, unsigned int i) { return mtlReadBit(in_bits, i) != 0 ? -1 : 0; }

template < typename type_t >
inline type_t mtlGetBitState(const mtlByte *in_bits, unsigned int i) { return mtlGetBitState(in_bits[i >> 3], i); }

template < typename type_t >
inline type_t mtlSetBit(type_t in_bits, unsigned int i) { return in_bits | mtl1BitMask<type_t>(i); }
inline void mtlSetBit(mtlByte *in_bits, unsigned int i) { in_bits[i >> 3] = mtlSetBit(in_bits[i >> 3], i); }

template < typename type_t >
inline type_t mtlClearBit(type_t in_bits, unsigned int i) { return in_bits & (~mtl1BitMask<type_t>(i)); }
inline void mtlClearBit(mtlByte *in_bits, unsigned int i) { in_bits[i >> 3] = mtlClearBit(in_bits[i >> 3], i); }

template < typename type_t >
inline type_t mtlSetBit(type_t in_bits, unsigned int i, bool state) { return state ? mtlSetBit(in_bits, i) : mtlClearBit(in_bits, i); }
inline void mtlSetBit(mtlByte *in_bits, unsigned int i, bool state) { return state ? mtlSetBit(in_bits, i) : mtlClearBit(in_bits, i); }

template < typename type_t >
inline type_t mtlToggleBit(type_t in_bits, unsigned int i) { return in_bits ^ mtl1BitMask<type_t>(i); }
inline void mtlToggleBit(mtlByte *in_bits, unsigned int i) { in_bits[i >> 3] = mtlToggleBit(in_bits[i >> 3], i); }

#endif
