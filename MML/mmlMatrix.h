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
template < int rows, int columns > // rows is rows (V), columns is columns (->), p_row is current row, p_column is current column
class mmlMatrix
{
public:
	static const int Rows = rows;
	static const int Columns = columns;
private:
	mmlVector<columns> e[rows];
public:
	//
	// index operators
	//
	const mmlVector<columns> &operator[](int p_row) const { return e[p_row]; }
	mmlVector<columns> &operator[](int p_row) { return e[p_row]; }
	//
	// conversion operators
	//
	operator float * ( void ) { return e[0]; }
	operator const float * const ( void ) const { return e[0]; }
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
	mmlMatrix(const mmlMatrix<rows,columns> &mat) {
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
	explicit mmlMatrix(const mmlMatrix<m2,n2> &mat) {
		const int minm = rows < m2 ? rows : m2;
		const int minn = columns < n2 ? columns : n2;
		int p_row = 0;
		for (; p_row < minm; ++p_row) {
			int p_column = 0;
			for (; p_column < minn; ++p_column) {
				e[p_row][p_column] = mat[p_row][p_column];
			}
			for (; p_column < columns; ++p_column) {
				e[p_row][p_column] = 0.f;
			}
		}
		for (; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = 0.f;
			}
		}
	}
	//
	// initializer
	//
	mmlMatrix(double e00, ...) {
		va_list vl;
		va_start(vl, e00);
		e[0][0] = (float)e00;
		for (int p_column = 1; p_column < columns; ++p_column) { e[0][p_column] = (float)va_arg(vl, double); }
		for (int p_row = 1; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = (float)va_arg(vl, double);
			}
		}
		va_end(vl);
	}
	//
	// conversion
	//
	mmlMatrix(const float * const mat) {
		for (int p_row = 0; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				e[p_row][p_column] = mat[p_row*rows + p_column];
			}
		}
	}
public:
	static mmlMatrix<rows, columns> &Cast(void *ptr)
	{
		return *(mmlMatrix<rows, columns>*)ptr;
	}
	static const mmlMatrix<rows, columns> &Cast(const void *ptr)
	{
		return *(mmlMatrix<rows, columns>*)ptr;
	}
	/*//
	// Trace
	//
	float Trace( void ) const
	{
		return mmlTrace(*this);
	}
	//
	// Det
	//
	float Det( void ) const
	{
		return mmlDet(*this);
	}*/
	//
	// Stack
	//
	void Stack(const mmlMatrix<rows,columns> &mat)
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
				e[p_row][p_column] = (float)(p_row == p_column);
			}
		}
	}
	//
	// IdentityMatrix
	//
	static mmlMatrix<rows,rows> IdentityMatrix( void )
	{
		mmlMatrix<rows,rows> ident;
		ident.SetIdentity();
		return ident;
	}
	//
	// ScaleMatrix
	//
	static mmlMatrix<rows, columns> ScaleMatrix(float scale)
	{
		mmlMatrix<rows,columns> m;
		for (int p_row = 0; p_row < rows; ++p_row) {
			for (int p_column = 0; p_column < columns; ++p_column) {
				m.e[p_row][p_column] = (float)(p_row == p_column) * scale;
			}
		}
		return m;
	}
};

//
// mmlTransp
//
template < int rows, int columns >
mmlMatrix<columns,rows> mmlTransp(const mmlMatrix<rows,columns> &mat)
{
	mmlMatrix<columns,rows> transp;
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
template < int rows, int columns >
mmlMatrix<rows-1,columns-1> mmlCollapse(const mmlMatrix<rows,columns> &mat, int p_row, int p_column)
{
	mmlMatrix<rows-1,columns-1> min;

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
template < int rows >
float mmlTrace(const mmlMatrix<rows,rows> &mat)
{
	float trace = 0.f;
	for (int p_row = 0; p_row < rows; ++p_row) {
		trace += mat[p_row][p_row];
	}
	return trace;
}

//
// mmlDet
//
template < int rows >
float mmlDet(const mmlMatrix<rows,rows> &mat)
{
	// Stopping conditions
	if (rows < 1) { return 0.f; }
	if (rows == 1) { return mat[0][0]; }

	// Transform to upper triangular matrix
	mmlMatrix<rows,rows> upperTri = mat;
	int counti = 0;
	for(int p_row = 0; p_row < rows-1; ++p_row) {
		// Elementary Row Operation I
		if(upperTri[p_row][p_row] == 0.f) {
			for(int k = p_row; k < rows; ++k) {
				if(upperTri[k][p_row] != 0.f) {
					for(int p_column = 0; p_column < rows; ++p_column) {
						float temp = upperTri[p_row][p_column];
						upperTri[p_row][p_column] = upperTri[k][p_column];
						upperTri[k][p_column] = temp;
					}
					k = rows;
				}
			}
			++counti;
		}
		// Elementary Row Operation III
		if(upperTri[p_row][p_row] != 0.f) {
			for(int k = p_row + 1; k < rows; ++k) {
				float factor = -1.f * upperTri[k][p_row] /  upperTri[p_row][p_row];
				for(int p_column = p_row; p_column < rows; ++p_column) {
					upperTri[k][p_column] = upperTri[k][p_column] + (factor * upperTri[p_row][p_column]);
				}
			}
		}
	}

	// Calculate determinant
	float det = 1.f;
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
template < int rows >
mmlMatrix<rows,rows> mmlInv(const mmlMatrix<rows,rows> &mat)
{
	// Hasn't checked the determinant. If det=0 then there is no inverse.
	mmlMatrix<rows,rows> inv = mat;
	//if (rows <= 0) { return; }  // sanity check
	//if (rows == 1) { return; }  // must be of dimension >= 2
	for (int p_row = 1; p_row < rows; ++p_row) { inv[0][p_row] /= inv[0][0]; } // normalize row 0
	for (int p_row = 1; p_row < rows; ++p_row) { 
		for (int p_column = p_row; p_column < rows; ++p_column) { // do a column of L
			float sum = 0.f;
			for (int k = 0; k < p_row; ++k) { 
				sum += inv[p_column][k] * inv[k][p_row];
			}
			inv[p_column][p_row] -= sum;
		}
		if (p_row == rows-1) continue;
		for (int p_column = p_row+1; p_column < rows; ++p_column) {  // do a row of U
			float sum = 0.f;
			for (int k = 0; k < p_row; ++k) {
				sum += inv[p_row][k]*inv[k][p_column];
			}
			inv[p_row][p_column] = (inv[p_row][p_column]-sum) / inv[p_row][p_row];
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) { // invert L
		for (int p_column = p_row; p_column < rows; ++p_column) {
			float x = 1.f;
			if (p_row != p_column) {
				x = 0.f;
				for (int k = p_row; k < p_column; ++k) {
					x -= inv[p_column][k]*inv[k][p_row];
				}
			}
			inv[p_column][p_row] = x / inv[p_column][p_column];
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) {  // invert U
		for (int p_column = p_row; p_column < rows; ++p_column) {
			if (p_row == p_column) { continue; }
			float sum = 0.f;
			for (int k = p_row; k < p_column; ++k) {
				sum += inv[k][p_column]*( (p_row==k) ? 1.f : inv[p_row][k] );
			}
			inv[p_row][p_column] = -sum;
		}
	}
	for (int p_row = 0; p_row < rows; ++p_row) {   // final inversion
		for (int p_column = 0; p_column < rows; ++p_column) {
			float sum = 0.f;
			for (int k = ((p_row>p_column)?p_row:p_column); k < rows; ++k) {
				sum += ((p_column==k)?1.f:inv[p_column][k])*inv[k][p_row];
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
template < int rows >
mmlMatrix<rows,rows> &operator*=(mmlMatrix<rows,rows> &l, mmlMatrix<rows,rows> r)
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
template < int rows, int columns >
mmlMatrix<rows,columns> &operator*=(mmlMatrix<rows,columns> &l, float r)
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
template < int left_rows, int shared, int right_columns >
mmlMatrix<left_rows,right_columns> operator*(const mmlMatrix<left_rows,shared> &l, const mmlMatrix<shared,right_columns> &r)
{
	const mmlMatrix<right_columns,shared> tr = mmlTransp(r); // transpose right
	mmlMatrix<left_rows,right_columns> mat;
	for (int p_row = 0; p_row < left_rows; ++p_row) {
		for (int p_column = 0; p_column < right_columns; ++p_column) {
			mat[p_row][p_column] = mmlDot(l[p_row], tr[p_column]);
		}
	}
	return mat;
}
template < int rows, int columns >
mmlMatrix<rows,columns> operator+(mmlMatrix<rows,columns> l, float r) { return (l*=r); }
template < int rows, int columns >
mmlMatrix<rows,columns> operator+(float l, mmlMatrix<rows,columns> r) { return (r*=l); }

//
// mmlVector/mmlMatrix operators
//
template < int rows, int columns >
mmlVector<rows> operator*(const mmlVector<columns> &vec, const mmlMatrix<rows,columns> &mat)
{
	mmlVector<rows> rvec;
	for (int p_row = 0; p_row < rows; ++p_row) {
		rvec[p_row] = mmlDot(vec, mat[p_row]);
	}
	return rvec;
}
template < int rows >
mmlVector<rows> &operator*=(mmlVector<rows> &vec, const mmlMatrix<rows,rows> &mat)
{
	vec = vec * mat;
	return vec;
}
// additional math functionality that is used often
inline mmlVector<3> operator*(const mmlVector<3> &v, const mmlMatrix<4,4> &m)
{
	return mmlVector<3>(
		mmlDot(v, *(mmlVector<3>*)(&m[0])) + m[0][3],
		mmlDot(v, *(mmlVector<3>*)(&m[1])) + m[1][3],
		mmlDot(v, *(mmlVector<3>*)(&m[2])) + m[2][3]
		);
}
inline mmlVector<3> &operator*=(mmlVector<3> &v, const mmlMatrix<4,4> &m)
{
	v = v * m;
	return v;
}

//
// mmlTranslationMatrix
//
inline mmlMatrix<4,4> mmlTranslationMatrix(float x, float y, float z)
{
	return mmlMatrix<4,4>(
		1.f, 0.f, 0.f, x,
		0.f, 1.f, 0.f, y,
		0.f, 0.f, 1.f, z,
		0.f, 0.f, 0.f, 1.f
		);
}

//
// mmlScaleMatrix
//
inline mmlMatrix<4,4> mmlScaleMatrix(float x, float y, float z)
{
	return mmlMatrix<4,4>(
		x,   0.f, 0.f, 0.f,
		0.f, y,   0.f, 0.f,
		0.f, 0.f, z,   0.f,
		0.f, 0.f, 0.f, 1.f
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
inline mmlMatrix<3,3> mmlTextureSpaceMatrix(const mmlVector<5> &va, const mmlVector<5> &vb, const mmlVector<5> &vc)
{
	mmlVector<3> t, b, n;

	// tangent
	const float eu21 = vb[3] - va[3];
	if (eu21 != 0.0f) {
		const mmlVector<3> exyz21 = mmlVector<3>::Cast(&(vb[0])) - mmlVector<3>::Cast(&(va[0]));
		t = mmlNormalize(exyz21 * (1.0f / eu21));
	} else {
		const float eu31 = vc[3] - va[3];
		const mmlVector<3> exyz31 = mmlVector<3>::Cast(&(vc[0])) - mmlVector<3>::Cast(&(va[0]));
		t = mmlNormalize(exyz31 * (1.0f / eu31));
	}

	// normal
	n = mmlSurfaceNormal(mmlVector<3>::Cast(&(va[0])), mmlVector<3>::Cast(&(vb[0])), mmlVector<3>::Cast(&(vc[0])));

	// binormal
	b = mmlCross(t, n);

	// TBN matrix that transforms directly to tangent space from world space
	return mmlMatrix<3,3>(
		t[0], t[1], t[2],
		b[0], b[1], b[2],
		n[0], n[1], n[2]
	);
}

//
// mmlXRotationMatrix
//
inline mmlMatrix<4,4> mmlXRotationMatrix(float xrad)
{
	const float SIN = sin(xrad);
	const float COS = cos(xrad);
	return mmlMatrix<4,4>(
		1.f, 0.f,  0.f, 0.f,
		0.f, COS, -SIN, 0.f,
		0.f, SIN,  COS, 0.f,
		0.f, 0.f,  0.f, 1.f
		);
}

//
// mmlYRotationMatrix
//
inline mmlMatrix<4,4> mmlYRotationMatrix(float yrad)
{
	const float SIN = sin(yrad);
	const float COS = cos(yrad);
	return mmlMatrix<4,4>(
		 COS, 0.f, SIN,	0.f,
		 0.f, 1.f, 0.f, 0.f,
		-SIN, 0.f, COS,	0.f,
		 0.f, 0.f, 0.f, 1.f
		);
}

//
// mmlZRotationMatrix
//
inline mmlMatrix<4,4> mmlZRotationMatrix(float zrad)
{
	const float SIN = sin(zrad);
	const float COS = cos(zrad);
	return mmlMatrix<4,4>(
		COS, -SIN, 0.f, 0.f,
		SIN,  COS, 0.f, 0.f,
		0.f,  0.f, 1.f, 0.f,
		0.f,  0.f, 0.f, 1.f
		);
}

inline mmlMatrix<2,2> mml2DRotationMatrix(float rad)
{
	const float SIN = sin(rad);
	const float COS = cos(rad);
	mmlMatrix<2,2> m;
	m[0][0] = COS; m[0][1] = -SIN;
	m[1][0] = SIN; m[1][1] = COS;
	return m;
}

//
// mmlEulerRotationMatrix
//
inline mmlMatrix<4,4> mmlEulerRotationMatrix(float head, float pitch, float roll)
{
	const float
		SINH = sin(head),
		SINP = sin(pitch),
		SINR = sin(roll),
		COSH = cos(head),
		COSP = cos(pitch),
		COSR = cos(roll);
	return mmlMatrix<4,4>(
		(COSR*COSH) -(SINR*SINP*SINH), -SINR*COSP, (COSR*SINH) +(SINR*SINP*COSH), 0.f,
		(SINR*COSH) +(COSR*SINP*SINH),  COSR*COSP, (SINR*SINH) -(COSR*SINP*COSH), 0.f,
		-COSP*SINH,                     SINP,      COSP*COSH,                     0.f,
		0.f,                            0.f,       0.f,                           1.f
		);
}

// http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/the-theory-of-stencil-shadow-volumes-r1873
// Should apparently be used for stencil shadows using z-fail method
/*inline mmlMatrix<4,4> mmlInfPerspectiveMatrix(float fovh, float fovv, float near, float far)
{
	return mmlMatrix<4,4>(
		1.f/tan(fovh/2.f), 0.f, 0.f, 0.f,
		0.f, 1.f/tan(fovv/2.f), 0.f, 0.f,
		0.f, 0.f, far/(far-near), 1.f,
		0.f, 0.f, (-far*near)/(far-near), 0.f
		);
}*/

inline mmlMatrix<4,4> mmlAxisRotationMatrix4(const mmlVector<3> &p_axis, float p_rot)
{
	const float x = p_axis[0];
	const float y = p_axis[1];
	const float z = p_axis[2];
	const float c = cosf(p_rot);
	const float s = sinf(p_rot);

	return mmlMatrix<4,4>(
		(x * x) * (1.0f - c) + c,       (x * y) * (1.0f - c) + (z * s), (x * z) * (1.0f - c) - (y * s), 0.f,
		(y * x) * (1.0f - c) - (z * s), (y * y) * (1.0f - c) + c,       (y * z) * (1.0f - c) + (x * s), 0.f,
		(z * x) * (1.0f - c) + (y * s), (z * y) * (1.0f - c) - (x * s), (z * z) * (1.0f - c) + c,       0.f,
		0.f,                            0.f,                            0.f,                            1.f
	);
}

inline mmlMatrix<3,3> mmlAxisRotationMatrix3(const mmlVector<3> &p_axis, float p_rot)
{
	const float x = p_axis[0];
	const float y = p_axis[1];
	const float z = p_axis[2];
	const float c = cosf(p_rot);
	const float s = sinf(p_rot);

	return mmlMatrix<3,3>(
		(x * x) * (1.0f - c) + c,       (x * y) * (1.0f - c) + (z * s), (x * z) * (1.0f - c) - (y * s),
		(y * x) * (1.0f - c) - (z * s), (y * y) * (1.0f - c) + c,       (y * z) * (1.0f - c) + (x * s),
		(z * x) * (1.0f - c) + (y * s), (z * y) * (1.0f - c) - (x * s), (z * z) * (1.0f - c) + c
	);
}

inline mmlMatrix<4,4> mmlViewMatrix(const mmlVector<3> &p_xAxis, const mmlVector<3> &p_yAxis, const mmlVector<3> &p_zAxis, const mmlVector<3> &p_viewPos)
{
	return mmlMatrix<4,4>(
		 p_xAxis[0],                  p_yAxis[0],                  p_zAxis[0],                 0.f,
		 p_xAxis[1],                  p_yAxis[1],                  p_zAxis[1],                 0.f,
		 p_xAxis[2],                  p_yAxis[2],                  p_zAxis[2],                 0.f,
		-mmlDot(p_xAxis, p_viewPos), -mmlDot(p_yAxis, p_viewPos), -mmlDot(p_zAxis, p_viewPos), 1.f
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
inline mmlMatrix<4,4> mmlToOpenGLTransform3D(const mmlMatrix<4,4> &p_transform)
{
	return mmlMatrix<4,4>(
		p_transform[0][0], p_transform[0][1], p_transform[0][2], 0.0f,
		p_transform[1][0], p_transform[1][1], p_transform[1][2], 0.0f,
		p_transform[2][0], p_transform[2][1], p_transform[2][2], 0.0f,
		p_transform[0][3], p_transform[1][3], p_transform[2][3], 1.0f
	);
}

inline mmlMatrix<4,4> mmlToOpenGLTransform2D(const mmlMatrix<3,3> &p_transform)
{
	return mmlMatrix<4,4>(
		p_transform[0][0], p_transform[0][1], 0.0f, 0.0f,
		p_transform[1][0], p_transform[1][1], 0.0f, 0.0f,
		0.0f,              0.0f,              1.0f, 0.0f,
		p_transform[0][2], p_transform[1][2], 0.0f, 1.0f
	);
}

inline mmlMatrix<3,3> mmlBasis(const mmlMatrix<4,4> &p_matrix)
{
	return mmlMatrix<3,3>(p_matrix);
}

#endif
