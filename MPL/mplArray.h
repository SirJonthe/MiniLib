#ifndef MPL_ARRAY_H_INCLUDED__
#define MPL_ARRAY_H_INCLUDED__

#include "mplFloat4.h"
#include "mplVector4.h"

// move some of the shared interface up to array
// implement vertex_array as vector4
// implement matrix_array as matrix4

namespace mpl
{

template < typename type_t >
class array
{
protected:
	type_t	*m_data;
	int		m_size;
	int		m_pool;

private:
	array(const array&) : m_data(0), m_size(0), m_pool(0) {}
	array &operator=(const array&) { return *this; }

public:
	array( void ) : m_data(0), m_size(0), m_pool(0) {}
	~array( void ) { delete [] m_data; }

	operator type_t*( void ) { return m_data; }
	operator const type_t*( void ) const { return m_data; }

	void create(int size);
	void destroy( void );
	int size( void ) const { return m_size; }
};

//template < int dim = 1 >
//class packed_array : private array<float4, dim>
//{
// // Both vertex_array and matrix_array share basically all of their code (except operators)
//};

class scalar_array;

template < int n >
class vertex_array : private array< vector4<n> >
{
private:
	vertex_array(const vertex_array<n> &v) {}
	vertex_array &operator=(const vertex_array<n> &v) { return *this; }

public:
	vertex_array( void );
	explicit vertex_array(int vtx_count);

	operator const vector4<n>*( void ) const { return this->m_data; }
	operator vector4<n>*( void ) { return this->m_data; }

	void create(int vtx_count);
	void destroy( void );
	void copy(const vertex_array<n> &v);
	void copy(float *v, int vtx_count);

	vertex_array &operator +=(const vertex_array<n> &r);
	vertex_array &operator -=(const vertex_array<n> &r);
	vertex_array &operator *=(const vertex_array<n> &r);
	vertex_array &operator *=(const scalar_array &r);
	vertex_array &operator *=(const vector4<n> &r);
	vertex_array &operator *=(const float4 &r);
	vertex_array &operator *=(float r);
	vertex_array &operator /=(const vertex_array<n> &r);
	vertex_array &operator /=(const scalar_array &r);
	vertex_array &operator /=(const vector4<n> &r);
	vertex_array &operator /=(const float4 &r);
	vertex_array &operator /=(float r);

	void normalize( void );
	void cross(const vertex_array<n> &a, const vertex_array<n> &b);

	int vertex_count( void ) const { return this->m_size << 2; }
	int group_count( void ) const { return this->m_size; }
};

class scalar_array : private array<float4>
{
public:
	operator float4*( void ) { return m_data; }
	operator const float4*( void ) const { return m_data; }

	//float ungroup_scalar(int scalar_index) const;
	//float &ungroup_scalar(int scalar_index);
	void set_scalar(int scalar_index, float scalar) { m_data[scalar_index] = scalar; }

	void create(int size) { array<float4>::create(size); }
	void destroy( void ) { array<float4>::destroy(); }

	int reg_count( void ) const { return m_size; }
	int count( void ) const { return m_size << 2; }

	template < int n >
	void dot(const vertex_array<n> &a, const vertex_array<n> &b);
};

/*class vertex_array : private array<float4>
{
public:
	enum format_t
	{
		format_xy	= 2,
		format_xyz	= 3
		//format_xyzw	= 4
	};

private:
	// data layout:
	// [xxxx] [yyyy] [xxxx] etc.
	// or...
	// [xxxx] [yyyy] [zzzz] [xxxx] etc.
	// or...
	// [xxxx] [yyyy] [zzzz] [wwww] [xxxx] etc.
	format_t m_fmt;

private:
	vertex_array(const vertex_array&) : array<float4>(), m_fmt(format_xyz) {}
	vertex_array &operator=(const vertex_array&) { return *this; }

	float4 dot(int packed_index) const;

	float &ungroup_n(int n, int vertex_index) { return m_data[(vertex_index >> 2) * (int)m_fmt + n][vertex_index & 3];; }
	float ungroup_n(int n, int vertex_index) const { return m_data[(vertex_index >> 2) * (int)m_fmt + n][vertex_index & 3];; }

public:
	vertex_array( void );
	vertex_array(int vtx_count, vertex_array::format_t fmt);

	void create(int vtx_count, vertex_array::format_t fmt);
	void destroy( void ) { array<float4>::destroy(); }
	void copy(const vertex_array &v);
	void copy(const float *v, int vtx_count, vertex_array::format_t fmt);

	float4 ungroup_vertex(int vertex_index) const;
	void set_vertex(int vertex_index, const float *v);

	static scalar_array &dot(const vertex_array &a, const vertex_array &b, scalar_array &dots);
	static scalar_array &dot(const vertex_array &a, const float4 &b, scalar_array &dots);
	static vertex_array &cross(const vertex_array &a, const vertex_array &b, vertex_array &crosses);
	float4 length(int group_index) const;
	float4 sum(int group_index) const;

	vertex_array &operator+=(const vertex_array &r);
	vertex_array &operator+=(const float4 &r);
	vertex_array &operator-=(const vertex_array &r);
	vertex_array &operator-=(const float4 &r);
	vertex_array &operator*=(const vertex_array &r);
	vertex_array &operator*=(float r);

	void normalize( void );

	operator float4*( void ) { return m_data; } // works on component basis
	operator const float4*( void ) const { return m_data; } // works on component basis

	vertex_array::format_t format( void ) const { return m_fmt; }

	int vertex_count( void ) const { return (m_size / (int)m_fmt) << 2; } // number of vertices in the array (does not vary with format)
	int reg_count( void ) const { return m_size; } // number of physical elements in the array (varias with format)
	int group_count( void ) const { return (m_size / (int)m_fmt); } // number of groups of 4 verteces (does not vary with format)

	float ungroup_x(int vertex_index) const { return m_data[(vertex_index >> 2) * (int)m_fmt][vertex_index & 3]; }
	float ungroup_y(int vertex_index) const { return m_data[(vertex_index >> 2) * (int)m_fmt + 1][vertex_index & 3]; }
	float ungroup_z(int vertex_index) const { return m_data[(vertex_index >> 2) * (int)m_fmt + 2][vertex_index & 3]; }
	//float ungroup_w(int vertex_index) const { return m_data[(vertex_index >> 2) * (int)m_fmt + 3][vertex_index & 3]; }

	float &ungroup_x(int vertex_index) { return m_data[(vertex_index >> 2) * (int)m_fmt][vertex_index & 3]; }
	float &ungroup_y(int vertex_index) { return m_data[(vertex_index >> 2) * (int)m_fmt + 1][vertex_index & 3]; }
	float &ungroup_z(int vertex_index) { return m_data[(vertex_index >> 2) * (int)m_fmt + 2][vertex_index & 3]; }
	//float &ungroup_w(int vertex_index) { return m_data[(vertex_index >> 2) * (int)m_fmt + 3][vertex_index & 3]; }
};*/

/*class matrix_array : private array<float4>
{
public:
	enum format_t
	{
		format_2x2 = 4,	// 2d rotation
		format_2x3 = 6,	// 2d rotation and translation
		format_3x3 = 9,	// 3d rotation
		format_3x4 = 12	// 3d rotation and translation
	};

private:
	format_t m_fmt;

private:
	matrix_array(const matrix_array&) : array<float4>(), m_fmt(format_3x4) {}
	matrix_array &operator=(const matrix_array&) { return *this; }

public:
	matrix_array( void ) : array<float4>(), m_fmt(format_3x4) {}
	matrix_array(int mtx_count, format_t format);

	operator float4*( void ) { return m_data; } // works on component basis
	operator const float4*( void ) const { return m_data; } // works on component basis

	format_t format( void ) const { return m_fmt; }

	void create(int mtx_count, format_t format);
	void destroy( void ) { array<float4>::destroy(); }
	void copy(const matrix_array &m);
	void copy(const float *m, int mtx_count, format_t format);

	int matrix_count( void ) const { return (m_size / (int)m_fmt) << 2; } // number of matrices in the array (does not vary with format)
	int reg_count( void ) const { return m_size; } // number of physical elements in the array (varias with format)
	int group_count( void ) const { return (m_size / (int)m_fmt); } // number of groups of 4 matrices (does not vary with format)

	//float4x4 ungroup_matrix(int matrix_index) const;

	float &ungroup(int x, int y, int matrix_index) { return m_data[(matrix_index >> 2) * (int)m_fmt + (y << 2) + x][matrix_index & 3]; }
	float ungroup(int x, int y, int matrix_index) const { return m_data[(matrix_index >> 2) * (int)m_fmt + (y << 2) + x][matrix_index & 3]; }

	matrix_array &operator *=(const float4x4 &r);
};*/

// color_array
// format_bw = 1
// format_rgb = 3
// format_rgba = 4

}

template < typename type_t >
void mpl::array<type_t>::create(int size)
{
	if (size > m_pool) {
		m_pool = size;
		delete [] m_data;
		m_data = new type_t[m_pool];
	}
	m_size = (size > -1) ? size : 0;
}

template < typename type_t >
void mpl::array<type_t>::destroy( void )
{
	delete [] m_data;
	m_data = 0;
	m_size = 0;
	m_pool = 0;
}

template < int n >
mpl::vertex_array<n>::vertex_array(int vtx_count) : mpl::array< mpl::vector4<n> >()
{
	create(vtx_count);
}

template < int n >
void mpl::vertex_array<n>::create(int vtx_count)
{
	array< vector4<n> >::create((vtx_count >> 2) + (((vtx_count & 3) == 0) ? 0 : 1));
}

template < int n >
void mpl::vertex_array<n>::destroy( void )
{
	array< vector4<n> >::destroy();
}

template < int n >
void mpl::vertex_array<n>::copy(float *v, int vtx_count)
{
	create(vtx_count);
	for (int i = 0; i < vtx_count; ++i) {
		this->m_data[i>>2].load_vector(v, i&3);
		v+=n;
	}
	// only a maximum of 3 iterations
	for (int i = vtx_count; i < vertex_count(); ++i) {
		this->m_data[i>>2].zero_vector(i&3);
	}
}

template < int n >
void mpl::vertex_array<n>::copy(const vertex_array<n> &v)
{
	create(v.m_size);
	for (int i = 0; i < this->m_size; ++i) {
		this->m_data[i] = v.m_data[i];
	}
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator +=(const mpl::vertex_array<n> &r)
{
	const int size = r.m_size < this->m_size ? r.m_size : this->m_size;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < size; ++i) {
		this->m_data[i] += r.m_data[i];
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator -=(const mpl::vertex_array<n> &r)
{
	const int size = r.m_size < this->m_size ? r.m_size : this->m_size;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < size; ++i) {
		this->m_data[i] -= r.m_data[i];
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator *=(const mpl::vertex_array<n> &r)
{
	const int size = r.m_size < this->m_size ? r.m_size : this->m_size;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < size; ++i) {
		this->m_data[i] *= r.m_data[i];
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator *=(const mpl::scalar_array &r)
{
	const int size = r.m_size < this->m_size ? r.m_size : this->m_size;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < size; ++i) {
		this->m_data[i] *= r.m_data[i];
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator *=(const mpl::vector4<n> &r)
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < this->m_size; ++i) {
		this->m_data[i] *= r;
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator *=(const mpl::float4 &r)
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < this->m_size; ++i) {
		this->m_data[i] *= r;
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator *=(float r)
{
	return *this *= float4(r);
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator /=(const mpl::vertex_array<n> &r)
{
	const int size = r.m_size < this->m_size ? r.m_size : this->m_size;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < size; ++i) {
		this->m_data[i] /= r.m_data[i];
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator /=(const mpl::scalar_array &r)
{
	const int size = r.m_size < this->m_size ? r.m_size : this->m_size;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < size; ++i) {
		this->m_data[i] /= r.m_data[i];
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator /=(const mpl::vector4<n> &r)
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < this->m_size; ++i) {
		this->m_data[i] /= r;
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator /=(const mpl::float4 &r)
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < this->m_size; ++i) {
		this->m_data[i] /= r;
	}
	return *this;
}

template < int n >
mpl::vertex_array<n> &mpl::vertex_array<n>::operator /=(float r)
{
	return *this *= float4(r);
}

template < int n >
void mpl::vertex_array<n>::normalize( void )
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < this->m_size; ++i) {
		this->m_data[i] = this->m_data[i].normalize();
	}
}

template < int n >
void mpl::vertex_array<n>::cross(const vertex_array<n> &a, const vertex_array<n> &b)
{
	const int size = a.m_size < b.m_size ? a.m_size : b.m_size;
	create(size);
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < size; ++i) {
		this->m_data[i] = mpl::cross(a.m_data[i], b.m_data[i]);
	}
}

template < int n >
void mpl::scalar_array::dot(const vertex_array<n> &a, const vertex_array<n> &b)
{
	const int size = a.group_count() < b.group_count() ? a.group_count() : b.group_count();
	create(size);
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < size; ++i) {
		m_data[i] = vector4<n>::dot(a[i], b[i]);
	}
}

#endif
