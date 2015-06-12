#ifndef MPL_VECTOR4_H_INCLUDED__
#define MPL_VECTOR4_H_INCLUDED__

#include "mplFloat4.h"

namespace mpl
{

template < int n >
class vector4
{
public:
	struct unpacked_t
	{
		float e[n];
	};

private:
	float4 e[n];

public:
	vector4( void ) {}
	vector4(const vector4 &v);
	vector4(const float4 &v);
	explicit vector4(const float *v);

	operator float4*( void ) { return e; }
	operator const float4*( void ) const { return e; }

	vector4 &operator +=(const vector4 &r);
	vector4 &operator -=(const vector4 &r);
	vector4 &operator *=(const vector4 &r);
	vector4 &operator *=(const float4 &r);
	vector4 &operator *=(float r);
	vector4 &operator /=(const vector4 &r);
	vector4 &operator /=(const float4 &r);
	vector4 &operator /=(float r);

	vector4 operator +(const vector4 &r) const;
	vector4 operator -(const vector4 &r) const;
	vector4 operator *(const vector4 &r) const;
	vector4 operator *(const float4 &r) const;
	vector4 operator *(float r) const;
	vector4 operator /(const vector4 &r) const;
	vector4 operator /(const float4 &r) const;
	vector4 operator /(float r) const;
	static float4 dot(const vector4<n> &l, const vector4<n> &r);

	void load_vector(const float *v, int vector_index);
	void load_vector(float val, int vector_index);
	void zero_vector(int vector_index);

	float4 length( void ) const;
	vector4 normalize( void ) const;
	vector4 sqrt( void ) const;
	vector4 inv_sqrt( void ) const;

	float &unpack(int i, int vector_index) { return e[i][vector_index]; }
	const float &unpack(int i, int vector_index) const { return e[i][vector_index]; }
	void unpack(int vector_index, float *out) const;
};

inline vector4<3> cross(const vector4<3> &l, const vector4<3> &r)
{
	mpl::vector4<3> out;
	out[0] = l[1]*r[2] - l[2]*r[1];
	out[1] = l[2]*r[0] - l[0]*r[2];
	out[2] = l[0]*r[1] - l[1]*r[0];
	return out;
}

}

template < int n >
mpl::vector4<n>::vector4(const mpl::vector4<n> &v)
{
	for (int i = 0; i < n; ++i) {
		e[i] = v[i];
	}
}

template < int n >
mpl::vector4<n>::vector4(const mpl::float4 &v)
{
	for (int i = 0; i < n; ++i) {
		e[i] = v[i];
	}
}

template < int n >
mpl::vector4<n>::vector4(const float *v)
{
	for (int i = 0; i < n; ++i) {
		e[i] = float4(v+i, n);
	}
}

template < int n >
mpl::vector4<n> &mpl::vector4<n>::operator +=(const mpl::vector4<n> &r)
{
	for (int i = 0; i < n; ++i) {
		e[i] += r.e[i];
	}
	return *this;
}

template < int n >
mpl::vector4<n> &mpl::vector4<n>::operator -=(const mpl::vector4<n> &r)
{
	for (int i = 0; i < n; ++i) {
		e[i] -= r.e[i];
	}
	return *this;
}

template < int n >
mpl::vector4<n> &mpl::vector4<n>::operator *=(const mpl::vector4<n> &r)
{
	for (int i = 0; i < n; ++i) {
		e[i] *= r.e[i];
	}
	return *this;
}

template < int n >
mpl::vector4<n> &mpl::vector4<n>::operator *=(const mpl::float4 &r)
{
	for (int i = 0; i < n; ++i) {
		e[i] *= r[i];
	}
	return *this;
}

template < int n >
mpl::vector4<n> &mpl::vector4<n>::operator *=(float r)
{
	for (int i = 0; i < n; ++i) {
		e[i] *= r;
	}
	return *this;
}

template < int n >
mpl::vector4<n> &mpl::vector4<n>::operator /=(const mpl::vector4<n> &r)
{
	for (int i = 0; i < n; ++i) {
		e[i] /= r.e[i];
	}
	return *this;
}

template < int n >
mpl::vector4<n> &mpl::vector4<n>::operator /=(const mpl::float4 &r)
{
	for (int i = 0; i < n; ++i) {
		e[i] /= r[i];
	}
	return *this;
}

template < int n >
mpl::vector4<n> &mpl::vector4<n>::operator /=(float r)
{
	for (int i = 0; i < n; ++i) {
		e[i] /= r;
	}
	return *this;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::operator +(const mpl::vector4<n> &r) const
{
	mpl::vector4<n> l;
	for (int i = 0; i < n; ++i) {
		l.e[i] = e[i] + r.e[i];
	}
	return l;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::operator -(const mpl::vector4<n> &r) const
{
	mpl::vector4<n> l;
	for (int i = 0; i < n; ++i) {
		l.e[i] = e[i] - r.e[i];
	}
	return l;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::operator *(const mpl::vector4<n> &r) const
{
	mpl::vector4<n> l;
	for (int i = 0; i < n; ++i) {
		l.e[i] = e[i] * r.e[i];
	}
	return l;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::operator *(const mpl::float4 &r) const
{
	mpl::vector4<n> l;
	for (int i = 0; i < n; ++i) {
		l.e[i] = e[i] * r;
	}
	return l;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::operator *(float r) const
{
	mpl::vector4<n> l;
	for (int i = 0; i < n; ++i) {
		l.e[i] = e[i] * r;
	}
	return l;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::operator /(const mpl::vector4<n> &r) const
{
	mpl::vector4<n> l;
	for (int i = 0; i < n; ++i) {
		l.e[i] = e[i] / r.e[i];
	}
	return l;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::operator /(const mpl::float4 &r) const
{
	mpl::vector4<n> l;
	for (int i = 0; i < n; ++i) {
		l.e[i] = e[i] / r;
	}
	return l;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::operator /(float r) const
{
	mpl::vector4<n> l;
	for (int i = 0; i < n; ++i) {
		l.e[i] = e[i] / r;
	}
	return l;
}

template < int n >
mpl::float4 mpl::vector4<n>::dot(const mpl::vector4<n> &l, const mpl::vector4<n> &r)
{
	mpl::float4 out = 0.0f;
	for (int i = 0; i < n; ++i) {
		out += l[i]*r[i];
	}
	return out;
}

template < int n >
void mpl::vector4<n>::load_vector(const float *v, int vector_index)
{
	for (int i = 0; i < n; ++i) {
		e[i][vector_index] = v[i];
	}
}

template < int n >
void mpl::vector4<n>::load_vector(float val, int vector_index)
{
	for (int i = 0; i < n; ++i) {
		e[i][vector_index] = val;
	}
}

template < int n >
void mpl::vector4<n>::zero_vector(int vector_index)
{
	for (int i = 0; i < n; ++i) {
		e[i][vector_index] = 0.0f;
	}
}

template < int n >
mpl::float4 mpl::vector4<n>::length( void ) const
{
	return sqrt(dot(*this, *this));
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::normalize( void ) const
{
	return *this * mpl::inv_sqrt(dot(*this, *this));
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::sqrt( void ) const
{
	mpl::vector4<n> out;
	for (int i = 0; i < n; ++i) {
		out.e[i] = sqrt(e[i]);
	}
	return out;
}

template < int n >
mpl::vector4<n> mpl::vector4<n>::inv_sqrt( void ) const
{
	mpl::vector4<n> out;
	for (int i = 0; i < n; ++i) {
		out.e[i] = inv_sqrt(e[i]);
	}
	return out;
}

template < int n >
void mpl::vector4<n>::unpack(int vector_index, float *out) const
{
	for (int i = 0; i < n; ++i) {
		out[i] = e[i][vector_index];
	}
}

#endif
