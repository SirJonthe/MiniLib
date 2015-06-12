#include "mplArray.h"

inline static int mplIMin(int a, int b)
{
	return a < b ? a : b;
}

inline static int mplIMax(int a, int b)
{
	return a > b ? a : b;
}

/*mpl::float4 mpl::vertex_array::dot(int packed_index) const
{
	const float4 *v = m_data + (packed_index * (int)m_fmt);
	if (m_fmt == format_xyz) {
		return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	}
	return v[0]*v[0] + v[1]*v[1];
}

mpl::vertex_array::vertex_array( void ) : array<float4>(), m_fmt(format_xyz)
{}

mpl::vertex_array::vertex_array(int vtx_count, vertex_array::format_t fmt) : array<float4>(), m_fmt(format_xyz)
{
	create(vtx_count, fmt);
}

void mpl::vertex_array::create(int vtx_count, vertex_array::format_t fmt)
{
	m_fmt = fmt;
	int not_packed = (vtx_count & 3) == 0 ? 0 : 1; // vtx_count % 4
	array<float4>::create(((vtx_count >> 2) + not_packed) * (int)m_fmt); // ((vtx_count / 4) + not_packed) * (int)m_fmt
}

void mpl::vertex_array::copy(const mpl::vertex_array &v)
{
	create(v.vertex_count(), v.m_fmt);
#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < m_size; ++i) {
		m_data[i] = v.m_data[i];
	}
}

void mpl::vertex_array::copy(const float *v, int vtx_count, mpl::vertex_array::format_t fmt)
{
	// THIS METHOD IS BAD
	// USE SIMD INSTRUCTIONS TO SWIZZLE DATA OR MEMORY MIGHT STALL!!

	create(vtx_count, fmt);

	// swizzle: xy xy xy xy         -> xxxx yyyy
	// swizzle: xyz xyz xyz xyz     -> xxxx yyyy zzzz
	// swizzle: xyzw xyzw xyzw xyzw -> xxxx yyyy zzzz wwww

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < vtx_count; ++i) {
		int index = i & 3; // i % 4
		int component = (i >> 2) * (int)m_fmt; // i / 4 * m_fmt
		int src_base_index = i * (int)m_fmt;
		for (int j = 0; j < (int)m_fmt; ++j) {
			m_data[component + j][index] = v[src_base_index + j];
		}
	}

	// put zeroes in rest of unfilled 4-sized block
	// This piece of work is too small for OMP
	const int count = vertex_count();
	for (int i = vtx_count; i < count; ++i) {
		int index = i & 3; // i % 4
		int component = (i >> 2) * (int)m_fmt; // i / 4 * m_fmt
		for (int j = 0; j < (int)m_fmt; ++j) {
			m_data[component + j][index] = 0.0f;
		}
	}
}

mpl::float4 mpl::vertex_array::ungroup_vertex(int vertex_index) const
{
	float x;
	float y;
	float z = 0.0f;
	float w = 1.0f;
	switch (m_fmt) {
	//case format_xyzw:
	//	w = ungroup_w(vertex_index);
	case format_xyz:
		z = ungroup_z(vertex_index);
	default:
		x = ungroup_x(vertex_index);
		y = ungroup_y(vertex_index);
	}
	return float4(x, y, z, w);
}

void mpl::vertex_array::set_vertex(int vertex_index, const float *v)
{
	for (int i = 0; i < (int)m_fmt; ++i) {
		ungroup_n(i, vertex_index) = v[i];
	}
}

mpl::scalar_array &mpl::vertex_array::dot(const mpl::vertex_array &a, const mpl::vertex_array &b, mpl::scalar_array &dots)
{
	const int astride = (int)a.format();
	const int bstride = (int)b.format();
	const int fmt = mplIMin(astride, bstride);
	const int count = mplIMin(a.group_count(), b.group_count());
	dots.create(count);

	if (fmt == format_xy) {
#ifdef _OPENMP
		#pragma omp parallel for
#endif
		for (int i = 0; i < count; ++i) {
			int i0 = i * astride;
			int j0 = i * bstride;
			dots[i] = a.m_data[i0] * b.m_data[j0] + a.m_data[i0+1] * b.m_data[j0+1];
		}
	}

	else if (fmt == format_xyz) {
#ifdef _OPENMP
		#pragma omp parallel for
#endif
		for (int i = 0; i < count; ++i) {
			int i0 = i * astride;
			int j0 = i * bstride;
			dots[i] = a.m_data[i0] * b.m_data[j0] + a.m_data[i0+1] * b.m_data[j0+1] + a.m_data[i0+2] * b.m_data[j0+2];
		}
	}
	return dots;
}

mpl::scalar_array &mpl::vertex_array::dot(const mpl::vertex_array &a, const mpl::float4 &b, mpl::scalar_array &dots)
{
	const int count = a.group_count();
	dots.create(count);

	if (a.m_fmt == format_xy) {
		const float4 x = b[0];
		const float4 y = b[1];
#ifdef _OPENMP
		#pragma omp parallel for
#endif
		for (int i = 0; i < count; ++i) {
			int i0 = i<<1;
			dots[i] = a.m_data[i0] * x + a.m_data[i0+1] * y;
		}
	}

	else if (a.m_fmt == format_xyz) {
		const float4 x = b[0];
		const float4 y = b[1];
		const float4 z = b[2];
#ifdef _OPENMP
		#pragma omp parallel for
#endif
		for (int i = 0; i < count; ++i) {
			int i0 = i*3;
			dots[i] = a.m_data[i0] * x + a.m_data[i0+1] * y + a.m_data[i0+2] * z;
		}

	}
	return dots;
}

mpl::vertex_array &mpl::vertex_array::cross(const vertex_array &a, const vertex_array &b, vertex_array &crosses)
{
	if (a.format() != format_xyz || b.format() != format_xyz) {
		crosses.create(0, format_xyz);
		return crosses;
	}

	crosses.create(mplIMin(a.vertex_count(), b.vertex_count()), format_xyz);
	const int count = mplIMin(a.m_size, b.m_size);

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < count; i += 3) {
		int i0 = i;
		int i1 = i+1;
		int i2 = i+2;

		float4 ax = a.m_data[i0];
		float4 ay = a.m_data[i1];
		float4 az = a.m_data[i2];

		float4 bx = b.m_data[i0];
		float4 by = b.m_data[i1];
		float4 bz = b.m_data[i2];

		crosses.m_data[i0] = ay * bz - az * by;
		crosses.m_data[i1] = az * bx - ax * bz;
		crosses.m_data[i2] = ax * by - ay * bx;
	}
	return crosses;
}

mpl::float4 mpl::vertex_array::length(int packed_index) const
{
	return sqrt(dot(packed_index));
}

mpl::float4 mpl::vertex_array::sum(int packed_index) const
{
	const float4 *v = m_data + (packed_index * (int)m_fmt);
	if (m_fmt == format_xyz) {
		return v[0] + v[1] + v[2];
	}
	return v[0] + v[1];
}

mpl::vertex_array &mpl::vertex_array::operator +=(const mpl::vertex_array &r)
{
#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < m_size; ++i) {
		m_data[i] += r.m_data[i];
	}
	return *this;
}

mpl::vertex_array &mpl::vertex_array::operator +=(const mpl::float4 &r)
{
	float4 f[4];
	for (int i = 0; i < (int)m_fmt; ++i) {
		f[i] = r[i]; // expand the vector so that each register contains 4 copies of same value
	}

	const int count = vertex_count();

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < count; ++i) {
		for (int j = 0; j < (int)m_fmt; ++j) {
			m_data[i+j] += r[j];
		}
	}
	return *this;
}

mpl::vertex_array &mpl::vertex_array::operator -=(const mpl::vertex_array &r)
{
#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < m_size; ++i) {
		m_data[i] -= r.m_data[i];
	}
	return *this;
}

mpl::vertex_array &mpl::vertex_array::operator -=(const mpl::float4 &r)
{
	float4 f[4];
	for (int i = 0; i < (int)m_fmt; ++i) {
		f[i] = r[i]; // expand the vector so that each register contains 4 copies of same value
	}

	const int count = vertex_count();

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < count; ++i) {
		for (int j = 0; j < (int)m_fmt; ++j) {
			m_data[i+j] -= r[j];
		}
	}
	return *this;
}

mpl::vertex_array &mpl::vertex_array::operator *=(const mpl::vertex_array &r)
{
#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < m_size; ++i) {
		m_data[i] *= r.m_data[i];
	}
	return *this;
}

mpl::vertex_array &mpl::vertex_array::operator *=(float r)
{
#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < m_size; ++i) {
		m_data[i] *= r;
	}
	return *this;
}

void mpl::vertex_array::normalize( void )
{
	if (m_fmt == format_xy) {
#ifdef _OPENMP
		#pragma omp parallel for
#endif
		for (int i = 0; i < m_size; i += 2) {

			float4 *v = m_data + i;

			float4 a = v[0];
			float4 b = v[1];

			float4 invlen = inv_sqrt(a*a + b*b);

			v[0] = a * invlen;
			v[1] = b * invlen;
		}
	} else if (m_fmt == format_xyz) {
#ifdef _OPENMP
		#pragma omp parallel for
#endif
		for (int i = 0; i < m_size; i += 3) {

			float4 *v = m_data + i;

			float4 a = v[0];
			float4 b = v[1];
			float4 c = v[2];

			float4 invlen = inv_sqrt(a*a + b*b + c*c);

			v[0] = a * invlen;
			v[1] = b * invlen;
			v[2] = c * invlen;
		}
	}
}*/

/*mpl::matrix_array::matrix_array(int mtx_count, format_t format) : array<mpl::float4>()
{
	create(mtx_count, format);
}

void mpl::matrix_array::create(int mtx_count, format_t format)
{
	m_fmt = format;
	int not_packed = (mtx_count & 3) == 0 ? 0 : 1; // mtx_count % 4
	array<float4>::create(((mtx_count >> 2) + not_packed) * (int)m_fmt); // ((mtx_count / 4) + not_packed) * (int)m_fmt
}

void mpl::matrix_array::copy(const mpl::matrix_array &m)
{
	create(m.matrix_count(), m.m_fmt);
	int count = reg_count();
	for (int i = 0; i < count; ++i) {
		m_data[i] = m.m_data[i];
	}
}

void mpl::matrix_array::copy(const float *m, int mtx_count, format_t format)
{
	// THIS METHOD IS BAD
	// USE SIMD INSTRUCTIONS TO SWIZZLE DATA OR MEMORY MIGHT STALL!!

	create(mtx_count, format);

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int i = 0; i < mtx_count; ++i) {
		int index = i & 3; // i % 4
		int component = (i >> 2) * (int)m_fmt; // i / 4 * m_fmt
		int src_base_index = i * (int)m_fmt;
		for (int j = 0; j < (int)m_fmt; ++j) {
			m_data[component + j][index] = m[src_base_index + j];
		}
	}

	// put zeroes in rest of unfilled 4-sized block
	// This piece of work is too small for OMP
	const int count = matrix_count();
	for (int i = mtx_count; i < count; ++i) {
		int index = i & 3; // i % 4
		int component = (i >> 2) * (int)m_fmt; // i / 4 * m_fmt
		for (int j = 0; j < (int)m_fmt; ++j) {
			m_data[component + j][index] = 0.0f;
		}
	}
}

mpl::matrix_array &mpl::matrix_array::operator *=(const mpl::float4x4 &r)
{
	//r = transposed right param
	//for (int y = 0; y < left_rows; ++y) {
	//	for (int x = 0; x < right_columns; ++x) {
	//		m[y][x] = dot(l[y], tr[x]);
	//	}
	//}


	float4 t[16];
	const int count = reg_count();

	if (m_fmt == format_2x2) {
		t[0] = r[0][0];
		t[1] = r[1][0];

		t[2] = r[0][1];
		t[3] = r[1][1];

		for (int n = 0; n < count; n+=4) {
			float4 l[4] = { m_data[n], m_data[n+1], m_data[n+2], m_data[n+3] }; // copy the matrix
			m_data[n]   = ;
			m_data[n+1] = ;
			m_data[n+2] = ;
			m_data[n+3] = ;
		}

		for (int y = 0; y < 2; ++y) {
			for (int x = 0; x < 2; ++x) {
				mat[y][x] = mmlDot(l[y], r[x]);
				mat[y][x] = l[y][0]*r[x][0] + l[y][1]*r[x][1];
			}
		}

		m[0][0] = l[0][0]*r[0][0] + l[0][1]*r[1][0];
		m[0][1] = l[0][0]*r[0][1] + l[0][1]*r[1][1];
		m[1][0] = l[1][0]*r[0][0] + l[1][1]*r[1][0];
		m[1][1] = l[1][0]*r[0][1] + l[1][1]*r[1][1];
	}
	else if (m_fmt == format_2x3) {
		m[0] = r[0][0];
		m[1] = r[1][0];
		m[2] = r[3][0];

		m[3] = r[0][1];
		m[4] = r[1][1];
		m[5] = r[3][1];
	}
	else if (m_fmt == format_3x3) {
		m[0] = r[0][0];
		m[1] = r[1][0];
		m[2] = r[2][0];

		m[3] = r[0][1];
		m[4] = r[1][1];
		m[5] = r[2][1];

		m[6] = r[0][2];
		m[7] = r[1][2];
		m[8] = r[2][2];
	}
	else if (m_fmt == format_3x4) {
		m[0] = r[0][0];
		m[1] = r[1][0];
		m[2] = r[2][0];
		m[3] = r[3][0];

		m[4] = r[0][1];
		m[5] = r[1][1];
		m[6] = r[2][1];
		m[7] = r[3][1];

		m[7] = r[0][2];
		m[8] = r[1][2];
		m[9] = r[2][2];
		m[10] = r[3][2];
	}

	return *this;
}*/
