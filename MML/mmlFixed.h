// Copyright (c) Jonathan Karlsson 2012
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_FIXED_H__
#define MML_FIXED_H__

#include "../MTL/mtlString.h"

#define mmlFIXED_SHIFT 16
#define mmlFIXED_UP(x) (int)((x) * (1<<mmlFIXED_SHIFT))
#define mmlFIXED_DOWN(x) ((x) >> mmlFIXED_SHIFT)
#define mmlFIXED_MUL(x, y) (int)(((long long int)(x) * (long long int)(y)) >> mmlFIXED_SHIFT)
#define mmlFIXED_DIV(x, y) (int)(((long long int)(x) << mmlFIXED_SHIFT) / (y))

class mmlFixed
{
private:
	int x;
public:
	explicit mmlFixed(float f) : x(mmlFIXED_UP(f)) {}
	explicit mmlFixed(short i) : x(((int)i)<<mmlFIXED_SHIFT) {}
	mmlFixed( void ) : x(0) {}
public:
	operator int( void ) const
	{
		return mmlFIXED_DOWN(x);
	}
	mmlFixed &operator+=(mmlFixed r)
	{
		this->x += r.x;
		return *this;
	}
	mmlFixed operator+(mmlFixed r) const
	{
		mmlFixed l = *this;
		return (l+=r);
	}
	mmlFixed &operator-=(mmlFixed r)
	{
		this->x -= r.x;
		return *this;
	}
	mmlFixed operator-(mmlFixed r) const
	{
		mmlFixed l = *this;
		return (l-=r);
	}
	mmlFixed operator*(mmlFixed r) const
	{
		mmlFixed l;
		l.x = mmlFIXED_MUL(this->x, r.x);
		return l;
	}
	mmlFixed &operator*=(mmlFixed r)
	{
		this->x = mmlFIXED_MUL(this->x, r.x);
		return *this;
	}
	mmlFixed operator/(mmlFixed r) const
	{
		mmlFixed l;
		l.x = mmlFIXED_DIV(this->x, r.x);
		return l;
	}
	mmlFixed &operator/=(mmlFixed r)
	{
		this->x = mmlFIXED_DIV(this->x, r.x);
		return *this;
	}
	bool operator==(mmlFixed r) { return x == r.x; }
	bool operator!=(mmlFixed r) { return x != r.x; }
	bool operator<=(mmlFixed r) { return x <= r.x; }
	bool operator>=(mmlFixed r) { return x >= r.x; }
	bool operator<(mmlFixed r) { return x < r.x; }
	bool operator>(mmlFixed r) { return x > r.x; }
	void ToString(mtlString &p_strOut) const
	{
		static const int Digits = 6; // +65535 is max (i.e. 6 digits)
		p_strOut.Free();
		p_strOut.SetSize(Digits);
		char *str = p_strOut.GetChars();
		int whole = mmlFIXED_DOWN(x);
		char sign;
		if (whole < 0) {
			whole = -whole;
			sign = '-';
		} else {
			sign = '+';
		}
		for (int i = 0; i < Digits-1; ++i) {
			int hi = (whole / 10) * 10;
			int lo = whole - hi;
			str[Digits - i - 1] = '0' + lo;
			whole /= 10;
		}

		str[0] = '0';
		int i = 0;
		for (; i < Digits-1; ++i) {
			if (str[i] == '0') { str[i] = ' '; }
			else { break; }
		}
		str[i-1] = sign;
	}
};

/*template < int p >
class fixed
{
private:
	int f;
public:
	fixed( void ) {} // do nothing
	fixed(float x) : f(int(x*(1<<p))) {}
	fixed(int x) : f(x<<p) {}
	operator int( void ) const { return f>>p; }
	fixed &operator+=(fixed f) { f += f.f; return *this; }
	fixed &operator-=(fixed f) { f -= f.f; return *this; }
	fixed &operator*=(fixed f) { (int)(((long long int)(f) * (long long int)(f.f)) >> p); return *this; }
	fixed &operator/=(fixed f) { (int)(((long long int)(f) << p) / (f.f)); return *this; }
	fixed operator+(fixed f) { return fixed(*this) + f; }
	fixed operator-(fixed f) { return fixed(*this) - f; }
	fixed operator*(fixed f) { return fixed(*this) * f; }
	fixed operator/(fixed f) { return fixed(*this) / f; }
	bool operator==(fixed f) { return f == f.f; }
	bool operator!=(fixed f) { return f != f.f; }
	bool operator<=(fixed f) { return f <= f.f; }
	bool operator>=(fixed f) { return f >= f.f; }
	bool operator<(fixed f) { return f < f.f; }
	bool operator>(fixed f) { return f > f.f; }
};*/

#endif
