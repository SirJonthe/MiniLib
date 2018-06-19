// Copyright (c) Jonathan Karlsson 2011
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_MATRIX_H_INCLUDED__
#define MML_MATRIX_H_INCLUDED__

#include <stdarg.h>
#include "mmlVector.h"

//
// mmlMatrix
//
template < int rows, int columns, typename type_t = float > // rows is rows (V), columns is columns (->), p_row is current row, p_column is current column
class mmlMatrix
{
public:
	static const int Rows = rows;
	static const int Columns = columns;

private:
	mmlVector<columns,type_t> e[rows];

public:
	//
	// index operators
	//
	const mmlVector<columns,type_t> &operator[](int p_row) const { return e[p_row]; }
	mmlVector<columns,type_t> &operator[](int p_row) { return e[p_row]; }
	//
	// conversion operators
	//
	operator type_t * ( void ) { return e[0]; }
	operator const type_t * const ( void ) const { return e[0]; }

public:
	//
	// default
	//
	mmlMatrix( void ) {
		// don't initialize
	}
	//
	// copy
	//
	mmlMatrix(const mmlMatrix<rows,columns,type_t> &mat) {
		for (int p_row = 0; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = mat[p_row][p_column];
			}
		}
	}
	//
	// conversion
	//
	template < int m2, int n2>
	explicit mmlMatrix(const mmlMatrix<m2,n2,type_t> &mat) {
		const int minm = rows < m2 ? rows : m2;
		const int minn = columns < n2 ? columns : n2;
		int p_row = 0;
		for (; p_row < minm; ++p_row) {
			int p_column = 0;
			for (; p_column < minn; ++p_column) {
				e[p_row][p_column] = mat[p_row][p_column];
			}
			for (; p_column < columns; ++p_column) {
				e[p_row][p_column] = type_t(0.0);
			}
		}
		for (; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = type_t(0.0);
			}
		}
	}
	//
	// initializer
	//
	mmlMatrix(const typename mml::va_cast<type_t>::va_t &e00, ...) {
		va_list vl;
		va_start(vl, e00);
		e[0][0] = type_t(e00);
		for (int p_column = 1; p_column < columns; ++p_column) { e[0][p_column] = type_t(va_arg(vl, typename mml::va_cast<type_t>::va_t)); }
		for (int p_row = 1; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = type_t(va_arg(vl, typename mml::va_cast<type_t>::va_t));
			}
		}
		va_end(vl);
	}
	//
	// conversion
	//
	mmlMatrix(const type_t * const mat) {
		for (int p_row = 0; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = mat[p_row*rows + p_column];
			}
		}
	}

public:
	static mmlMatrix<rows, columns,type_t> &Cast(void *ptr)
	{
		return *(mmlMatrix<rows, columns,type_t>*)ptr;
	}
	static const mmlMatrix<rows, columns,type_t> &Cast(const void *ptr)
	{
		return *(mmlMatrix<rows, columns,type_t>*)ptr;
	}
	//
	// Stack
	//
	void Stack(const mmlMatrix<rows,columns,type_t> &mat)
	{
		*this = mat * (*this);
	}
	//
	// IdentityMatrix
	//
	void SetIdentity( void )
	{
		for (int p_row = 0; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = type_t(p_row == p_column ? 1 : 0);
			}
		}
	}
	//
	// IdentityMatrix
	//
	static mmlMatrix<rows,rows,type_t> IdentityMatrix( void )
	{
		mmlMatrix<rows,rows,type_t> ident;
		ident.SetIdentity();
		return ident;
	}
	//
	// ScaleMatrix
	//
	static mmlMatrix<rows, columns, type_t> ScaleMatrix(const type_t &scale)
	{
		mmlMatrix<rows,columns, type_t> m;
		for (int p_row = 0; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				m.e[p_row][p_column] = type_t(p_row == p_column ? 1 : 0) * scale;
			}
		}
		return m;
	}
};

//
// mmlTransp
//
template < int rows, int columns, typename type_t >
mmlMatrix<columns,rows,type_t> mmlTransp(const mmlMatrix<rows,columns,type_t> &mat)
{
	mmlMatrix<columns,rows,type_t> transp;
	for (int p_row = 0; p_row < rows; ++p_row) {
		for (int p_column = 0; p_column < columns; ++p_column) {
			transp[p_column][p_row] = mat[p_row][p_column];
		}
	}
	return transp;
}

//
// mmlCollapse
//
template < int rows, int columns, typename type_t >
mmlMatrix<rows-1,columns-1,type_t> mmlCollapse(const mmlMatrix<rows,columns,type_t> &mat, int p_row, int p_column)
{
	mmlMatrix<rows-1,columns-1,type_t> min;

	int i2 = 0;
	for (; i2 < p_row; ++i2) {
		int j2 = 0;
		for (; j2 < p_column; ++j2) { min[i2][j2] = mat[i2][j2]; }
		for (; j2 < columns-1; ++j2) { min[i2][j2] = mat[i2][j2+1]; }
	}
	for (; i2 < rows-1; ++i2) {
		int j2 = 0;
		for (; j2 < p_column; ++j2) { min[i2][j2] = mat[i2+1][j2]; }
		for (; j2 < columns-1; ++j2) { min[i2][j2] = mat[i2+1][j2+1]; }
	}

	return min;
}

//
// mmlTrace
//
template < int rows, typename type_t >
type_t mmlTrace(const mmlMatrix<rows,rows,type_t> &mat)
{
	type_t trace = type_t(0.0);
	for (int p_row = 0; p_row < rows; ++p_row) {
		trace += mat[p_row][p_row];
	}
	return trace;
}

//
// mmlDet
//
template < int rows, typename type_t >
type_t mmlDet(const mmlMatrix<rows,rows> &mat)
{
	// Stopping conditions
	if (rows < 1) { return type_t(0.0); }
	if (rows == 1) { return mat[0][0]; }

	// Transform to upper triangular matrix
	mmlMatrix<rows,rows,type_t> upperTri = mat;
	int counti = 0;
	for(int p_row = 0; p_row < rows-1; ++p_row) {
		// Elementary Row Operation I
		if(upperTri[p_row][p_row] == type_t(0.0)) {
			for(int k = p_row; k < rows; ++k) {
				if(upperTri[k][p_row] != type_t(0.0)) {
					for(int p_column = 0; p_column < rows; ++p_column) {
//						type_t temp = upperTri[p_row][p_column];
//						upperTri[p_row][p_column] = upperTri[k][p_column];
//						upperTri[k][p_column] = temp;
						mmlSwap(upperTri[p_row][p_column], upperTri[k][p_column]);
					}
					k = rows;
				}
			}
			++counti;
		}
		// Elementary Row Operation III
		if(upperTri[p_row][p_row] != type_t(0.0)) {
			for(int k = p_row + 1; k < rows; ++k) {
				type_t factor = -type_t(1.0) * upperTri[k][p_row] /  upperTri[p_row][p_row];
				for(int p_column = p_row; p_column < rows; ++p_column) {
					upperTri[k][p_column] = upperTri[k][p_column] + (factor * upperTri[p_row][p_column]);
				}
			}
		}
	}

	// Calculate determinant
	type_t det = type_t(1.0);
	for (int p_row = 0; p_row < rows; ++p_row) {
		det *= upperTri[p_row][p_row];
	}
	if (counti % 2 != 0) {
		det = -det;
	}

	return det;
}

//
// mmlInv
//
template < int rows, typename type_t >
mmlMatrix<rows,rows,type_t> mmlInv(const mmlMatrix<rows,rows,type_t> &mat)
{
	// Hasn't checked the determinant. If det=0 then there is no inverse.
	mmlMatrix<rows,rows,type_t> inv = mat;
	//if (rows <= 0) { return; }  // sanity check
	//if (rows == 1) { return; }  // must be of dimension >= 2
	for (int p_row = 1; p_row < rows; ++p_row) { inv[0][p_row] /= inv[0][0]; } // normalize row 0
	for (int p_row = 1; p_row < rows; ++p_row) { 
		for (int p_column = p_row; p_column < rows; ++p_column) { // do a column of L
			float sum = type_t(0.0);
			for (int k = 0; k < p_row; ++k) { 
				sum += inv[p_column][k] * inv[k][p_row];
			}
			inv[p_column][p_row] -= sum;
		}
		if (p_row == rows-1) { continue; }
		for (int p_column = p_row+1; p_column < rows; ++p_column) {  // do a row of U
			float sum = type_t(0.0);
			for (int k = 0; k < p_row; ++k) {
				sum += inv[p_row][k] * inv[k][p_column];
			}
			inv[p_row][p_column] = (inv[p_row][p_column]-sum) / inv[p_row][p_row];
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) { // invert L
		for (int p_column = p_row; p_column < rows; ++p_column) {
			float x = type_t(1.0);
			if (p_row != p_column) {
				x = type_t(0.0);
				for (int k = p_row; k < p_column; ++k) {
					x -= inv[p_column][k] * inv[k][p_row];
				}
			}
			inv[p_column][p_row] = x / inv[p_column][p_column];
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) {  // invert U
		for (int p_column = p_row; p_column < rows; ++p_column) {
			if (p_row == p_column) { continue; }
			float sum = type_t(0.0);
			for (int k = p_row; k < p_column; ++k) {
				sum += inv[k][p_column] * ((p_row==k) ? type_t(1.0) : inv[p_row][k]);
			}
			inv[p_row][p_column] = -sum;
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) {   // final inversion
		for (int p_column = 0; p_column < rows; ++p_column) {
			float sum = type_t(0.0);
			for (int k = ((p_row > p_column) ? p_row : p_column); k < rows; ++k) {
				sum += ((p_column==k) ? type_t(1.0) : inv[p_column][k]) * inv[k][p_row];
			}
			inv[p_column][p_row] = sum;
		}
	}
	return inv;
}

//
// mmlMatrix/mmlMatrix operators
//
template < int rows, int columns >
mmlMatrix<rows,columns> &operator+=(mmlMatrix<rows,columns> &l, const mmlMatrix<rows,columns> &r)
{
	for (int p_row = 0; p_row < rows; ++p_row) {
		l[p_row] += r[p_row];
	}
	return l;
}
template < int rows, int columns >
mmlMatrix<rows,columns> &operator-=(mmlMatrix<rows,columns> &l, const mmlMatrix<rows,columns> &r)
{
	for (int p_row = 0; p_row < rows; ++p_row) {
		l[p_row] -= r[p_row];
	}
	return l;
}
template < int rows, typename type_t >
mmlMatrix<rows,rows,type_t> &operator*=(mmlMatrix<rows,rows,type_t> &l, mmlMatrix<rows,rows,type_t> r)
{
	r = mmlTransp(r); // transpose right
	for (int p_row = 0; p_row < rows; ++p_row) {
		const mmlVector<rows> li = l[p_row]; // current l vector
		for (int p_column = 0; p_column < rows; ++p_column) {
			l[p_row][p_column] = mmlDot(li, r[p_column]);
		}
	}
	return l;
}
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns,type_t> &operator*=(mmlMatrix<rows,columns,type_t> &l, const type_t &r)
{
	for (int p_row = 0; p_row < rows; ++p_row) {
		l[p_row] *= r;
	}
	return l;
}
template < int rows, int columns >
mmlMatrix<rows,columns> operator+(mmlMatrix<rows,columns> l, const mmlMatrix<rows,columns> &r) { return (l+=r); }
template < int rows, int columns >
mmlMatrix<rows,columns> operator-(mmlMatrix<rows,columns> l, const mmlMatrix<rows,columns> &r) { return (l-=r); }
template < int left_rows, int shared, int right_columns, typename type_t >
mmlMatrix<left_rows,right_columns,type_t> operator*(const mmlMatrix<left_rows,shared,type_t> &l, const mmlMatrix<shared,right_columns,type_t> &r)
{
	const mmlMatrix<right_columns,shared,type_t> tr = mmlTransp(r); // transpose right
	mmlMatrix<left_rows,right_columns,type_t> mat;
	for (int p_row = 0; p_row < left_rows; ++p_row) {
		for (int p_column = 0; p_column < right_columns; ++p_column) {
			mat[p_row][p_column] = mmlDot(l[p_row], tr[p_column]);
		}
	}
	return mat;
}
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns,type_t> operator*(mmlMatrix<rows,columns,type_t> l, const type_t &r) { return (l*=r); }
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns> operator*(const type_t &l, mmlMatrix<rows,columns,type_t> r) { return (r*=l); }

//
// mmlVector/mmlMatrix operators
//
template < int rows, int columns, typename type_t >
mmlVector<rows,type_t> operator*(const mmlVector<columns,type_t> &vec, const mmlMatrix<rows,columns,type_t> &mat)
{
	mmlVector<rows,type_t> rvec;
	for (int p_row = 0; p_row < rows; ++p_row) {
		rvec[p_row] = mmlDot(vec, mat[p_row]);
	}
	return rvec;
}
template < int rows, typename type_t >
mmlVector<rows,type_t> &operator*=(mmlVector<rows,type_t> &vec, const mmlMatrix<rows,rows,type_t> &mat)
{
	vec = vec * mat;
	return vec;
}
// additional math functionality that is used often
template < typename type_t >
inline mmlVector<3,type_t> operator*(const mmlVector<3,type_t> &v, const mmlMatrix<4,4,type_t> &m)
{
	return mmlVector<3,type_t>(
		mmlDot(v, *(mmlVector<3,type_t>*)(&m[0])) + m[0][3],
		mmlDot(v, *(mmlVector<3,type_t>*)(&m[1])) + m[1][3],
		mmlDot(v, *(mmlVector<3,type_t>*)(&m[2])) + m[2][3]
		);
}
template < typename type_t >
inline mmlVector<3,type_t> &operator*=(mmlVector<3,type_t> &v, const mmlMatrix<4,4,type_t> &m)
{
	v = v * m;
	return v;
}

//
// mmlTranslationMatrix
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlTranslationMatrix(const type_t &x, const type_t &y, const type_t &z)
{
	return mmlMatrix<4,4,type_t>(
		type_t(1.0), type_t(0.0), type_t(0.0), x,
		type_t(0.0), type_t(1.0), type_t(0.0), y,
		type_t(0.0), type_t(0.0), type_t(1.0), z,
		type_t(0.0), type_t(0.0), type_t(0.0), type_t(1.0)
		);
}

//
// mmlScaleMatrix
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlScaleMatrix(const type_t &x, const type_t &y, const type_t &z)
{
	return mmlMatrix<4,4,type_t>(
		x,           type_t(0.0), type_t(0.0), type_t(0.0),
		type_t(0.0), y,           type_t(0.0), type_t(0.0),
		type_t(0.0), type_t(0.0), z,           type_t(0.0),
		type_t(0.0), type_t(0.0), type_t(0.0), type_t(1.0)
		);
}

//
// mmlNormalTransformMatrix
//
inline mmlMatrix<3,3> mmlNormalTransformMatrix(const mmlMatrix<4,4> &mat)
{
	return mmlMatrix<3,3>(mmlTransp(mmlInv(mat)));
}

//
// mmlTextureSpaceMatrix
// Input: 3 vertices making up a triangle in the format [X Y Z U V] (where U and V are texture coordinates)
// Output: a matrix that rotates a 3D vertex lying in the same space as the input vertices to texture space.
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlTextureSpaceMatrix(const mmlVector<5,type_t> &va, const mmlVector<5,type_t> &vb, const mmlVector<5,type_t> &vc)
{
	mmlVector<3> t, b, n;

	// tangent
	const float eu21 = vb[3] - va[3];
	if (eu21 != type_t(0.0)) {
		const mmlVector<3,type_t> exyz21 = mmlVector<3,type_t>::Cast(&(vb[0])) - mmlVector<3,type_t>::Cast(&(va[0]));
		t = mmlNormalize(exyz21 * (type_t(1.0) / eu21));
	} else {
		const float eu31 = vc[3] - va[3];
		const mmlVector<3,type_t> exyz31 = mmlVector<3,type_t>::Cast(&(vc[0])) - mmlVector<3,type_t>::Cast(&(va[0]));
		t = mmlNormalize(exyz31 * (type_t(1.0) / eu31));
	}

	// normal
	n = mmlSurfaceNormal(mmlVector<3,type_t>::Cast(&(va[0])), mmlVector<3,type_t>::Cast(&(vb[0])), mmlVector<3,type_t>::Cast(&(vc[0])));

	// binormal
	b = mmlCross(t, n);

	// TBN matrix that transforms directly to tangent space from world space
	return mmlMatrix<3,3,type_t>(
		t[0], t[1], t[2],
		b[0], b[1], b[2],
		n[0], n[1], n[2]
	);
}

//
// mmlXRotationMatrix
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlXRotationMatrix(const type_t &xrad)
{
	const type_t SIN = sin(xrad);
	const type_t COS = cos(xrad);
	return mmlMatrix<4,4>(
		type_t(1.0), type_t(0.0),  type_t(0.0), type_t(0.0),
		type_t(0.0), COS,       -SIN,       type_t(0.0),
		type_t(0.0), SIN,        COS,       type_t(0.0),
		type_t(0.0), type_t(0.0),  type_t(0.0), type_t(1.0)
		);
}

//
// mmlYRotationMatrix
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlYRotationMatrix(const type_t &yrad)
{
	const type_t SIN = sin(yrad);
	const type_t COS = cos(yrad);
	return mmlMatrix<4,4,type_t>(
		 COS,       type_t(0.0), SIN,       type_t(0.0),
		 type_t(0.0), type_t(1.0), type_t(0.0), type_t(0.0),
		-SIN,       type_t(0.0), COS,       type_t(0.0),
		 type_t(0.0), type_t(0.0), type_t(0.0), type_t(1.0)
		);
}

//
// mmlZRotationMatrix
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlZRotationMatrix(const type_t &zrad)
{
	const type_t SIN = sin(zrad);
	const type_t COS = cos(zrad);
	return mmlMatrix<4,4,type_t>(
		COS,      -SIN,       type_t(0.0), type_t(0.0),
		SIN,       COS,       type_t(0.0), type_t(0.0),
		type_t(0.0), type_t(0.0), type_t(1.0), type_t(0.0),
		type_t(0.0), type_t(0.0), type_t(0.0), type_t(1.0)
		);
}

template < typename type_t >
inline mmlMatrix<2,2,type_t> mml2DRotationMatrix(const type_t &rad)
{
	const type_t SIN = sin(rad);
	const type_t COS = cos(rad);
	mmlMatrix<2,2,type_t> m;
	m[0][0] = COS; m[0][1] = -SIN;
	m[1][0] = SIN; m[1][1] = COS;
	return m;
}

//
// mmlEulerRotationMatrix
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlEulerRotationMatrix(const type_t &head, const type_t &pitch, const type_t &roll)
{
	const type_t
		SINH = sin(head),
		SINP = sin(pitch),
		SINR = sin(roll),
		COSH = cos(head),
		COSP = cos(pitch),
		COSR = cos(roll);
	return mmlMatrix<4,4,type_t>(
		(COSR*COSH) -(SINR*SINP*SINH), -SINR*COSP, (COSR*SINH) +(SINR*SINP*COSH), type_t(0.0),
		(SINR*COSH) +(COSR*SINP*SINH),  COSR*COSP, (SINR*SINH) -(COSR*SINP*COSH), type_t(0.0),
		-COSP*SINH,                     SINP,      COSP*COSH,                     type_t(0.0),
		type_t(0.0),                      type_t(0.0), type_t(0.0),                     type_t(1.0)
		);
}

// http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/the-theory-of-stencil-shadow-volumes-r1873
// Should apparently be used for stencil shadows using z-fail method
/*inline mmlMatrix<4,4> mmlInfPerspectiveMatrix(float fovh, float fovv, float near, float far)
{
	return mmlMatrix<4,4>(
		type_t(1.0)/tan(fovh/2.f), type_t(0.0), type_t(0.0), type_t(0.0),
		type_t(0.0), type_t(1.0)/tan(fovv/2.f), type_t(0.0), type_t(0.0),
		type_t(0.0), type_t(0.0), far/(far-near), type_t(1.0),
		type_t(0.0), type_t(0.0), (-far*near)/(far-near), type_t(0.0)
		);
}*/

template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlAxisRotationMatrix4(const mmlVector<3,type_t> &p_axis, const type_t &p_rot)
{
	const type_t x = p_axis[0];
	const type_t y = p_axis[1];
	const type_t z = p_axis[2];
	const type_t c = cosf(p_rot);
	const type_t s = sinf(p_rot);

	return mmlMatrix<4,4,type_t>(
		(x * x) * (type_t(1.0) - c) + c,       (x * y) * (type_t(1.0) - c) + (z * s), (x * z) * (type_t(1.0) - c) - (y * s), type_t(0.0),
		(y * x) * (type_t(1.0) - c) - (z * s), (y * y) * (type_t(1.0) - c) + c,       (y * z) * (type_t(1.0) - c) + (x * s), type_t(0.0),
		(z * x) * (type_t(1.0) - c) + (y * s), (z * y) * (type_t(1.0) - c) - (x * s), (z * z) * (type_t(1.0) - c) + c,       type_t(0.0),
		type_t(0.0),                           type_t(0.0),                           type_t(0.0),                           type_t(1.0)
	);
}

template < typename type_t >
inline mmlMatrix<3,3> mmlAxisRotationMatrix3(const mmlVector<3,type_t> &p_axis, const type_t &p_rot)
{
	const type_t x = p_axis[0];
	const type_t y = p_axis[1];
	const type_t z = p_axis[2];
	const type_t c = cos(p_rot);
	const type_t s = sin(p_rot);

	return mmlMatrix<3,3,type_t>(
		(x * x) * (type_t(1.0) - c) + c,       (x * y) * (type_t(1.0) - c) + (z * s), (x * z) * (type_t(1.0) - c) - (y * s),
		(y * x) * (type_t(1.0) - c) - (z * s), (y * y) * (type_t(1.0) - c) + c,       (y * z) * (type_t(1.0) - c) + (x * s),
		(z * x) * (type_t(1.0) - c) + (y * s), (z * y) * (type_t(1.0) - c) - (x * s), (z * z) * (type_t(1.0) - c) + c
	);
}

template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlViewMatrix(const mmlVector<3,type_t> &p_xAxis, const mmlVector<3,type_t> &p_yAxis, const mmlVector<3,type_t> &p_zAxis, const mmlVector<3,type_t> &p_viewPos)
{
	return mmlMatrix<4,4,type_t>(
		 p_xAxis[0],                  p_yAxis[0],                  p_zAxis[0],                 type_t(0.0),
		 p_xAxis[1],                  p_yAxis[1],                  p_zAxis[1],                 type_t(0.0),
		 p_xAxis[2],                  p_yAxis[2],                  p_zAxis[2],                 type_t(0.0),
		-mmlDot(p_xAxis, p_viewPos), -mmlDot(p_yAxis, p_viewPos), -mmlDot(p_zAxis, p_viewPos), type_t(1.0)
	);
}

inline mmlMatrix<4,4> mmlViewMatrix(const mmlVector<3> &p_up, const mmlVector<3> &p_target, const mmlVector<3> p_viewPos)
{
	const mmlVector<3> zaxis = mmlNormalize(p_target - p_viewPos);
	const mmlVector<3> xaxis = mmlNormalize(mmlCross(p_up, zaxis));
	const mmlVector<3> yaxis = mmlCross(zaxis, xaxis);
	return mmlViewMatrix(xaxis, yaxis, zaxis, p_viewPos);
}

// Do not use this for perspective transform matrices
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlToOpenGLTransform3D(const mmlMatrix<4,4,type_t> &p_transform)
{
	return mmlMatrix<4,4,type_t>(
		p_transform[0][0], p_transform[0][1], p_transform[0][2], type_t(0.0),
		p_transform[1][0], p_transform[1][1], p_transform[1][2], type_t(0.0),
		p_transform[2][0], p_transform[2][1], p_transform[2][2], type_t(0.0),
		p_transform[0][3], p_transform[1][3], p_transform[2][3], type_t(1.0)
	);
}

template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlToOpenGLTransform2D(const mmlMatrix<3,3,type_t> &p_transform)
{
	return mmlMatrix<4,4,type_t>(
		p_transform[0][0], p_transform[0][1], type_t(0.0), type_t(0.0),
		p_transform[1][0], p_transform[1][1], type_t(0.0), type_t(0.0),
		type_t(0.0),         type_t(0.0),         type_t(1.0), type_t(0.0),
		p_transform[0][2], p_transform[1][2], type_t(0.0), type_t(1.0)
	);
}

inline mmlMatrix<3,3> mmlBasis(const mmlMatrix<4,4> &p_matrix)
{
	return mmlMatrix<3,3>(p_matrix);
}

#endif
