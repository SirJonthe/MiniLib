// Copyright (c) Jonathan Karlsson 2011
// Code may be used freely for non-commercial purposes.
// Author retains the right to use code for commercial purposes.
// Author retains his moral rights under the applicable copyright laws.

#ifndef MML_MATRIX_H_INCLUDED
#define MML_MATRIX_H_INCLUDED

#include <stdarg.h>
#include "mmlVector.h"

#define VAT typename mml::va_cast<type_t>::va_t

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
	operator const type_t *( void ) const { return e[0]; }

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
	template < int m2, int n2, typename type2_t >
	explicit mmlMatrix(const mmlMatrix<m2,n2,type_t> &mat) {
		const int minm = rows < m2 ? rows : m2;
		const int minn = columns < n2 ? columns : n2;
		int p_row = 0;
		for (; p_row < minm; ++p_row) {
			int p_column = 0;
			for (; p_column < minn; ++p_column) {
				e[p_row][p_column] = type_t(mat[p_row][p_column]);
			}
			for (; p_column < columns; ++p_column) {
				e[p_row][p_column] = type_t(0);
			}
		}
		for (; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = type_t(0);
			}
		}
	}
	//
	// initializer
	//
	mmlMatrix(const VAT e00, ...) {
		va_list vl;
		va_start(vl, e00);
		e[0][0] = type_t(e00);
		for (int p_column = 1; p_column < columns; ++p_column) { e[0][p_column] = type_t(va_arg(vl, VAT)); }
		for (int p_row = 1; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = type_t(va_arg(vl, VAT));
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
				e[p_row][p_column] = mat[p_row*columns + p_column];
			}
		}
	}
	//
	// assignment operators
	//
	mmlMatrix<rows,columns,type_t> &operator=(const mmlMatrix<rows,columns,type_t> &m) {
		for (int p_row = 0; p_row < rows; ++p_row) {
			e[p_row] = m[p_row];
		}
		return *this;
	}

public:
	static mmlMatrix<rows,columns,type_t> &Cast(void *ptr)
	{
		return *reinterpret_cast<mmlMatrix<rows, columns,type_t>*>(ptr);
	}
	static const mmlMatrix<rows,columns,type_t> &Cast(const void *ptr)
	{
		return *reinterpret_cast<const mmlMatrix<rows, columns,type_t>*>(ptr);
	}

	//
	// Identity
	//
	static mmlMatrix<rows,rows,type_t> Identity( void )
	{
		mmlMatrix<rows,rows,type_t> I;
		for (int p_row = 0; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				I.e[p_row][p_column] = type_t(p_row == p_column ? 1 : 0);
			}
		}
		return I;
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
	type_t trace = type_t(0);
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
	if (rows < 1) { return type_t(0); }
	if (rows == 1) { return mat[0][0]; }

	// Transform to upper triangular matrix
	mmlMatrix<rows,rows,type_t> upperTri = mat;
	int counti = 0;
	for(int p_row = 0; p_row < rows-1; ++p_row) {
		// Elementary Row Operation I
		if(upperTri[p_row][p_row] == type_t(0)) {
			for(int k = p_row; k < rows; ++k) {
				if(upperTri[k][p_row] != type_t(0)) {
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
		if(upperTri[p_row][p_row] != type_t(0)) {
			for(int k = p_row + 1; k < rows; ++k) {
				type_t factor = -type_t(1) * upperTri[k][p_row] /  upperTri[p_row][p_row];
				for(int p_column = p_row; p_column < rows; ++p_column) {
					upperTri[k][p_column] = upperTri[k][p_column] + (factor * upperTri[p_row][p_column]);
				}
			}
		}
	}

	// Calculate determinant
	type_t det = type_t(1);
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
	if (inv[0][0] == type_t(0)) {
		for (int row = 1; row < rows; ++row) {
			if (inv[row][0] != type_t(0)) {
				mmlSwap(inv[0], inv[row]);
				break;
			}
		}
	}
	for (int p_row = 1; p_row < rows; ++p_row) { inv[0][p_row] /= inv[0][0]; } // normalize row 0
	for (int p_row = 1; p_row < rows; ++p_row) { 
		for (int p_column = p_row; p_column < rows; ++p_column) { // do a column of L
			type_t sum = type_t(0);
			for (int k = 0; k < p_row; ++k) { 
				sum += inv[p_column][k] * inv[k][p_row];
			}
			inv[p_column][p_row] -= sum;
		}
		if (p_row == rows-1) { continue; }
		for (int p_column = p_row+1; p_column < rows; ++p_column) { // do a row of U
			type_t sum = type_t(0);
			for (int k = 0; k < p_row; ++k) {
				sum += inv[p_row][k] * inv[k][p_column];
			}
			inv[p_row][p_column] = (inv[p_row][p_column]-sum) / inv[p_row][p_row];
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) { // invert L
		for (int p_column = p_row; p_column < rows; ++p_column) {
			type_t x = type_t(1);
			if (p_row != p_column) {
				x = type_t(0);
				for (int k = p_row; k < p_column; ++k) {
					x -= inv[p_column][k] * inv[k][p_row];
				}
			}
			inv[p_column][p_row] = x / inv[p_column][p_column];
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) { // invert U
		for (int p_column = p_row; p_column < rows; ++p_column) {
			if (p_row == p_column) { continue; }
			type_t sum = type_t(0);
			for (int k = p_row; k < p_column; ++k) {
				sum += inv[k][p_column] * ((p_row==k) ? type_t(1) : inv[p_row][k]);
			}
			inv[p_row][p_column] = -sum;
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) { // final inversion
		for (int p_column = 0; p_column < rows; ++p_column) {
			type_t sum = type_t(0);
			for (int k = ((p_row > p_column) ? p_row : p_column); k < rows; ++k) {
				sum += ((p_column==k) ? type_t(1) : inv[p_column][k]) * inv[k][p_row];
			}
			inv[p_column][p_row] = sum;
		}
	}
	return inv;
}

//
// mmlMatrix/mmlMatrix operators
//
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns,type_t> &operator+=(mmlMatrix<rows,columns,type_t> &l, const mmlMatrix<rows,columns,type_t> &r)
{
	for (int p_row = 0; p_row < rows; ++p_row) {
		l[p_row] += r[p_row];
	}
	return l;
}
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns,type_t> &operator-=(mmlMatrix<rows,columns,type_t> &l, const mmlMatrix<rows,columns,type_t> &r)
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
		const mmlVector<rows,type_t> li = l[p_row]; // current l vector
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
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns,type_t> operator+(mmlMatrix<rows,columns,type_t> l, const mmlMatrix<rows,columns,type_t> &r) { return (l+=r); }
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns,type_t> operator-(mmlMatrix<rows,columns,type_t> l, const mmlMatrix<rows,columns,type_t> &r) { return (l-=r); }
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
mmlMatrix<rows,columns,type_t> operator*(const type_t &l, mmlMatrix<rows,columns,type_t> r) { return (r*=l); }

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
//	return mmlVector<3,type_t>(
//		mmlDot(v, mmlVector<3,type_t>::Cast(&m[0])) + m[0][3],
//		mmlDot(v, mmlVector<3,type_t>::Cast(&m[1])) + m[1][3],
//		mmlDot(v, mmlVector<3,type_t>::Cast(&m[2])) + m[2][3]
//	);

	// NOTE: Updated to convert homogeneous coordinates to Cartesian.
	mmlVector<3,type_t> o;
	o[0] = mmlDot(v, mmlVector<3,type_t>::Cast(&m[0])) + m[0][3];
	o[1] = mmlDot(v, mmlVector<3,type_t>::Cast(&m[1])) + m[1][3];
	o[2] = mmlDot(v, mmlVector<3,type_t>::Cast(&m[2])) + m[2][3];
	const type_t w = mmlDot(v, mmlVector<3,type_t>::Cast(&m[3])) + m[3][3];
	if (mmlIsApproxEqual(w, 1.0f) == false) {
		o /= w;
	}
	return o;
}

template < typename type_t >
inline mmlVector<3,type_t> &operator*=(mmlVector<3,type_t> &v, const mmlMatrix<4,4,type_t> &m)
{
	v = v * m;
	return v;
}

//
// mmlTransform
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlTransform(const mmlMatrix<3,3,type_t> &basis, const mmlVector<3,type_t> &pos)
{
	return mmlMatrix<4,4,type_t>(
		VAT(basis[0][0]), VAT(basis[0][1]), VAT(basis[0][2]), VAT(pos[0]),
		VAT(basis[1][0]), VAT(basis[1][1]), VAT(basis[1][2]), VAT(pos[1]),
		VAT(basis[2][0]), VAT(basis[2][1]), VAT(basis[2][2]), VAT(pos[2]),
		VAT(0),           VAT(0),           VAT(0),           VAT(1)
	);
}

//
// mmlBasis
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlBasis(const mmlMatrix<4,4,type_t> &transform)
{
	return mmlMatrix<3,3,type_t>(
		VAT(transform[0][0]), VAT(transform[0][1]), VAT(transform[0][2]),
		VAT(transform[1][0]), VAT(transform[1][1]), VAT(transform[1][2]),
		VAT(transform[2][0]), VAT(transform[2][1]), VAT(transform[2][2])
	);
}

//
// mmlRotation
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlRotation(const mmlMatrix<4,4,type_t> &transform)
{
	mmlMatrix<3,3> basis = mmlBasis(transform);
	basis[0] = mmlNormalize(basis[0]);
	basis[1] = mmlNormalize(basis[1]);
	basis[2] = mmlNormalize(basis[2]);
	return basis;
}

//
// mmlTranslation
//
template < typename type_t >
inline mmlVector<3,type_t> mmlTranslation(const mmlMatrix<4,4,type_t> &transform)
{
	return mmlVector<3,type_t>(VAT(transform[0][3]), VAT(transform[1][3]), VAT(transform[2][3]));
}

//
// mmlTranslation
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlTranslate(const type_t &x, const type_t &y, const type_t &z)
{
	return mmlMatrix<4,4,type_t>(
		VAT(1), VAT(0), VAT(0), VAT(x),
		VAT(0), VAT(1), VAT(0), VAT(y),
		VAT(0), VAT(0), VAT(1), VAT(z),
		VAT(0), VAT(0), VAT(0), VAT(1)
	);
}

//
// mmlTranslation
//
template < typename type_t >
inline mmlMatrix<4,4,type_t> mmlTranslate(const mmlVector<3,type_t> &translation)
{
	return mmlTranslate(translation[0], translation[1], translation[2]);
}

//
// mmlScale
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlScale(const type_t &x, const type_t &y, const type_t &z)
{
	return mmlMatrix<3,3,type_t>(
		VAT(x), VAT(0), VAT(0),
		VAT(0), VAT(y), VAT(0),
		VAT(0), VAT(0), VAT(z)
	);
}

//
// mmlScale
//
template < typename type_t >
inline mmlVector<3,type_t> mmlScale(const mmlMatrix<3,3,type_t> &transform)
{
	return mmlVector<3>(
		VAT(transform[0].Len()),
		VAT(transform[1].Len()),
		VAT(transform[2].Len())
	);
}

//
// mmlNormalTransform
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlNormalTransform(const mmlMatrix<4,4,type_t> &mat)
{
	return mmlMatrix<3,3,type_t>(mmlTransp(mmlInv(mat)));
}

//
// mmlTextureSpaceMatrix
// Input: 3 vertices making up a triangle in the format [X Y Z U V] (where U and V are texture coordinates)
// Output: a matrix that rotates a 3D vertex lying in the same space as the input vertices to texture space.
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlTextureSpaceMatrix(const mmlVector<5,type_t> &va, const mmlVector<5,type_t> &vb, const mmlVector<5,type_t> &vc)
{
	mmlVector<3,type_t> t, b, n;

	// tangent
	const type_t eu21 = vb[3] - va[3];
	if (eu21 != type_t(0)) {
		const mmlVector<3,type_t> exyz21 = mmlVector<3,type_t>::Cast(&(vb[0])) - mmlVector<3,type_t>::Cast(&(va[0]));
		t = mmlNormalize(exyz21 * (type_t(1) / eu21));
	} else {
		const type_t eu31 = vc[3] - va[3];
		const mmlVector<3,type_t> exyz31 = mmlVector<3,type_t>::Cast(&(vc[0])) - mmlVector<3,type_t>::Cast(&(va[0]));
		t = mmlNormalize(exyz31 * (type_t(1) / eu31));
	}

	// normal
	n = mmlSurfaceNormal(mmlVector<3,type_t>::Cast(&(va[0])), mmlVector<3,type_t>::Cast(&(vb[0])), mmlVector<3,type_t>::Cast(&(vc[0])));

	// binormal
	b = mmlCross(t, n);

	// TBN matrix that transforms directly to tangent space from world space
	return mmlMatrix<3,3,type_t>(
		VAT(t[0]), VAT(t[1]), VAT(t[2]),
		VAT(b[0]), VAT(b[1]), VAT(b[2]),
		VAT(n[0]), VAT(n[1]), VAT(n[2])
	);
}

//
// mmlRotateX
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlRotateX(const type_t &xrad)
{
	const VAT SIN = sin(xrad);
	const VAT COS = cos(xrad);
	return mmlMatrix<3,3,type_t>(
		VAT(1), VAT(0),  VAT(0),
		VAT(0), COS,    -SIN,
		VAT(0), SIN,     COS
	);
}

//
// mmlRotateY
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlRotateY(const type_t &yrad)
{
	const VAT SIN = sin(yrad);
	const VAT COS = cos(yrad);
	return mmlMatrix<3,3,type_t>(
		  COS,    VAT(0), SIN,
		  VAT(0), VAT(1), VAT(0),
		 -SIN,    VAT(0), COS
	);
}

//
// mmlRotateZ
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlRotateZ(const type_t &zrad)
{
	const VAT SIN = sin(zrad);
	const VAT COS = cos(zrad);
	return mmlMatrix<3,3,type_t>(
		COS,    -SIN,   VAT(0),
		SIN,     COS,   VAT(0),
		VAT(0), VAT(0), VAT(1)
	);
}

//
// mmlRotate
//
template < typename type_t >
inline mmlMatrix<2,2,type_t> mmlRotate(const type_t &rad)
{
	const VAT SIN = sin(rad);
	const VAT COS = cos(rad);
	return mmlMatrix<2,2,type_t>(
		COS, -SIN,
		SIN,  COS
	);
}

//
// mmlEuler
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlEuler(const type_t &head, const type_t &pitch, const type_t &roll)
{
	const VAT
		SINH = sin(head),
		SINP = sin(pitch),
		SINR = sin(roll),
		COSH = cos(head),
		COSP = cos(pitch),
		COSR = cos(roll);
	return mmlMatrix<3,3,type_t>(
		(COSR*COSH) -(SINR*SINP*SINH), -SINR*COSP, (COSR*SINH) +(SINR*SINP*COSH),
		(SINR*COSH) +(COSR*SINP*SINH),  COSR*COSP, (SINR*SINH) -(COSR*SINP*COSH),
		-COSP*SINH,                     SINP,      COSP*COSH
	);
}

//
// mmlAxisAngle
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlAxisAngle(const mmlVector<3,type_t> &p_axis, const type_t &p_rot)
{
	const VAT SIN    = sin(p_rot);
	const VAT COS    = cos(p_rot);
	const VAT ICOS   = VAT(1) - COS;
	const VAT uxSIN  = p_axis[0]*SIN;
	const VAT uySIN  = p_axis[1]*SIN;
	const VAT uzSIN  = p_axis[2]*SIN;
	const VAT uxICOS = p_axis[0]*ICOS;
	const VAT uyICOS = p_axis[1]*ICOS;
	const VAT uzuxICOS = p_axis[2]*uxICOS;
	const VAT uyuxICOS = p_axis[1]*uxICOS;
	const VAT uzuyICOS = p_axis[2]*uyICOS;
	return mmlMatrix<3,3,type_t>(
		COS + p_axis[0]*uxICOS, uyuxICOS - uzSIN, uzuxICOS + uySIN,
		uyuxICOS + uzSIN, COS + p_axis[1]*uyICOS, uzuyICOS - uxSIN,
		uzuxICOS - uySIN, uzuyICOS + uxSIN, COS + p_axis[2]*p_axis[2]*ICOS
	);
}

//
// mmlAnyTransform
//
template < typename type_t >
inline mmlMatrix<3,3,type_t> mmlAnyTransform(const mmlVector<3,type_t> &normal)
{
	// Related to the TBN (mmlTextureSpaceMatrix), I guess?
	const mmlVector<3,type_t> T = mmlAnyTangent(normal); // X axis
	const mmlVector<3,type_t> B = mmlCross(T, normal); // Y axis
	mmlMatrix<3,3,type_t> o;
	o[0][0] = T[0];
	o[1][0] = T[1];
	o[2][0] = T[2];
	o[0][1] = B[0];
	o[1][1] = B[1];
	o[2][1] = B[2];
	o[0][2] = normal[0];
	o[1][2] = normal[1];
	o[2][2] = normal[2];
	return o;
}

//
// mmlNormalize
//
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns,type_t> mmlNormalize(const mmlMatrix<rows,columns,type_t> &m)
{
	mmlMatrix<rows,columns,type_t> o;
	for (int i = 0; i < rows; ++i) {
		o[i] = mmlNormalize(m[i]);
	}
	return o;
}


//
// mmlOrtho
// Make sure matrix axis are orthogonal to each other (uses X as base).
//
template < int rows, int columns, typename type_t >
mmlMatrix<3,3,type_t> mmlOrtho(const mmlMatrix<3,3,type_t> &m)
{
	mmlMatrix<3,3,type_t> o;
	mmlVector<3> x;
	x[0] = m[0][0];
	x[1] = m[1][0];
	x[2] = m[2][0];

	mmlVector<3> y;
	y[0] = m[0][1];
	y[1] = m[1][1];
	y[2] = m[2][1];

	mmlVector<3> z = mmlCross(x, y);
	y = mmlCross(z, x);

	o[0][0] = x[0];
	o[1][0] = x[1];
	o[2][0] = x[2];

	o[0][1] = y[0];
	o[1][1] = y[1];
	o[2][1] = y[2];

	o[0][2] = z[0];
	o[1][2] = z[1];
	o[2][2] = z[2];

	return o;
}

//
// mmlOrthoNormalize
// Make sure matrix axis are orthogonal to each other (uses X as base) and normalizes axis.
//
template < int rows, int columns, typename type_t >
mmlMatrix<rows,columns,type_t> mmlOrthoNormalize(const mmlMatrix<rows,columns,type_t> &m)
{
	mmlMatrix<3,3,type_t> o;
	mmlVector<3> x;
	x[0] = m[0][0];
	x[1] = m[1][0];
	x[2] = m[2][0];
	x.NormalizeIf();

	mmlVector<3> y;
	y[0] = m[0][1];
	y[1] = m[1][1];
	y[2] = m[2][1];

	mmlVector<3> z = mmlNormalize(mmlCross(x, y));
	y = mmlNormalize(mmlCross(z, x));

	o[0][0] = x[0];
	o[1][0] = x[1];
	o[2][0] = x[2];

	o[0][1] = y[0];
	o[1][1] = y[1];
	o[2][1] = y[2];

	o[0][2] = z[0];
	o[1][2] = z[1];
	o[2][2] = z[2];

	return o;
}

template < typename type_t >
mmlMatrix<3,3,type_t> mmlSkew(const mmlVector<3,type_t> &v)
{
	mmlMatrix<3,3,type_t> o;
	o[0][0] =  type_t(0); o[0][1] = -v[2];      o[0][2] =  v[1];
	o[1][0] =  v[2];      o[1][1] =  type_t(0); o[1][2] = -v[0];
	o[2][0] = -v[1];      o[2][1] =  v[0];      o[2][2] =  type_t(0);
	return o;
}

#undef VAT

#endif
