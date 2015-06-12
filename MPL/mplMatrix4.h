#ifndef MPLMATRIX4_H
#define MPLMATRIX4_H

#include "mplFloat4.h"

namespace mpl
{

// a group of 4 row-by-col-dimensional matrices
template < int row, int col >
class matrix4
{
private:
	float4 e[row][col];

public:
	matrix4( void ) {}
	matrix4(const matrix4<row, col> &m);
	explicit matrix4(const float *f);

	float4 *operator[](int _row) { return e[_row]; }
	const float4 *operator[](int _row) const { return e[_row]; }

	// matrix operations

	matrix4<col, row> transpose( void ) const;
	matrix4 inverse( void ) const;

	float &unpack(int _row, int _col, int matrix_index) { return e[_row][_col][matrix_index]; }
	const float &unpack(int _row, int _col, int matrix_index) const { return e[_row][_col][matrix_index]; }
	void unpack(int matrix_index, float *out) const;
};

}

template < int row, int col >
mpl::matrix4<row, col>::matrix4(const mpl::matrix4<row, col> &m)
{
	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < col; ++c) {
			e[r][c] = m.e[r][c];
		}
	}
}

template < int row, int col >
mpl::matrix4<row, col>::matrix4(const float *f)
{
	for (int i = 0; i < 4; ++i) {
		for (int r = 0; r < row; ++r) {
			for (int c = 0; c < col; ++c) {
				e[r][c][i] = *f;
				++f;
			}
		}
	}
}

template < int row, int col >
mpl::matrix4<col, row> mpl::matrix4<row, col>::transpose( void ) const
{
	mpl::matrix4<col, row> m;
	for (int j = 0; j < row; ++j) {
		for (int i = 0; i < col; ++i) {
			m[i][j] = e[j][i];
		}
	}
	return m;
}

template < int row, int col >
mpl::matrix4<row, col> mpl::matrix4<row, col>::inverse( void ) const
{
	mpl::matrix4<col, row> dst;
	mpl::matrix4<col, row> src = transpose();
	mpl::float4 tmp[12];

	tmp[0] = src[2][2] * src[3][3];
	tmp[1] = src[2][3] * src[3][2];
	tmp[2] = src[2][1] * src[3][3];
	tmp[3] = src[2][3] * src[3][1];
	tmp[4] = src[2][1] * src[3][2];
	tmp[5] = src[2][2] * src[3][1];
	tmp[6] = src[2][0] * src[3][3];
	tmp[7] = src[2][3] * src[3][0];
	tmp[8] = src[2][0] * src[3][2];
	tmp[9] = src[2][2] * src[3][0];
	tmp[10] = src[2][0] * src[3][1];
	tmp[11] = src[2][1] * src[3][0];

	// calculate first 8 elements (cofactors)
	dst[0][0] = (tmp[0]*src[1][1] + tmp[3]*src[1][2] + tmp[4] *src[1][3]) - (tmp[1]*src[1][1] + tmp[2]*src[1][2] + tmp[5] *src[1][3]);
	dst[0][1] = (tmp[1]*src[1][0] + tmp[6]*src[1][2] + tmp[9] *src[1][3]) - (tmp[0]*src[1][0] + tmp[7]*src[1][2] + tmp[8] *src[1][3]);
	dst[0][2] = (tmp[2]*src[1][0] + tmp[7]*src[1][1] + tmp[10]*src[1][3]) - (tmp[3]*src[1][0] + tmp[6]*src[1][1] + tmp[11]*src[1][3]);
	dst[0][3] = (tmp[5]*src[1][0] + tmp[8]*src[1][1] + tmp[11]*src[1][2]) - (tmp[4]*src[1][0] + tmp[9]*src[1][1] + tmp[10]*src[1][2]);
	dst[1][0] = (tmp[1]*src[0][1] + tmp[2]*src[0][2] + tmp[5] *src[0][3]) - (tmp[0]*src[0][1] + tmp[3]*src[0][2] + tmp[4] *src[0][3]);
	dst[1][1] = (tmp[0]*src[0][0] + tmp[7]*src[0][2] + tmp[8] *src[0][3]) - (tmp[1]*src[0][0] + tmp[6]*src[0][2] + tmp[9] *src[0][3]);
	dst[1][2] = (tmp[3]*src[0][0] + tmp[6]*src[0][1] + tmp[11]*src[0][3]) - (tmp[2]*src[0][0] + tmp[7]*src[0][1] + tmp[10]*src[0][3]);
	dst[1][3] = (tmp[4]*src[0][0] + tmp[9]*src[0][1] + tmp[10]*src[0][2]) - (tmp[5]*src[0][0] + tmp[8]*src[0][1] + tmp[11]*src[0][2]);

	// calculate pairs for second 8 elements (cofactors)
	tmp[0] = src[0][2]*src[1][3];
	tmp[1] = src[0][3]*src[1][2];
	tmp[2] = src[0][1]*src[1][3];
	tmp[3] = src[0][3]*src[1][1];
	tmp[4] = src[0][1]*src[1][2];
	tmp[5] = src[0][2]*src[1][1];

	tmp[6]  = src[0][0]*src[1][3];
	tmp[7]  = src[0][3]*src[1][0];
	tmp[8]  = src[0][0]*src[1][2];
	tmp[9]  = src[0][2]*src[1][0];
	tmp[10] = src[0][0]*src[1][1];
	tmp[11] = src[0][1]*src[1][0];

	// calculate second 8 elements (cofactors)
	dst[2][0] = (tmp[0] *src[3][1] + tmp[3] *src[3][2] + tmp[4] *src[3][3]) - (tmp[1] *src[3][1] + tmp[2] *src[3][2] + tmp[5] *src[3][3]);
	dst[2][1] = (tmp[1] *src[3][0] + tmp[6] *src[3][2] + tmp[9] *src[3][3]) - (tmp[0] *src[3][0] + tmp[7] *src[3][2] + tmp[8] *src[3][3]);
	dst[2][2] = (tmp[2] *src[3][0] + tmp[7] *src[3][1] + tmp[10]*src[3][3]) - (tmp[3] *src[3][0] + tmp[6] *src[3][1] + tmp[11]*src[3][3]);
	dst[2][3] = (tmp[5] *src[3][0] + tmp[8] *src[3][1] + tmp[11]*src[3][2]) - (tmp[4] *src[3][0] + tmp[9] *src[3][1] + tmp[10]*src[3][2]);
	dst[3][0] = (tmp[2] *src[2][2] + tmp[5] *src[2][3] + tmp[1] *src[2][1]) - (tmp[4] *src[2][3] + tmp[0] *src[2][1] + tmp[3] *src[2][2]);
	dst[3][1] = (tmp[8] *src[2][3] + tmp[0] *src[2][0] + tmp[7] *src[2][2]) - (tmp[6] *src[2][2] + tmp[9] *src[2][3] + tmp[1] *src[2][0]);
	dst[3][2] = (tmp[6] *src[2][1] + tmp[11]*src[2][3] + tmp[3] *src[2][0]) - (tmp[10]*src[2][3] + tmp[2] *src[2][0] + tmp[7] *src[2][1]);
	dst[3][3] = (tmp[10]*src[2][2] + tmp[4] *src[2][0] + tmp[9] *src[2][1]) - (tmp[8] *src[2][1] + tmp[11]*src[2][2] + tmp[5] *src[2][0]);

	// calculate (inverse) determinant
	mpl::float4 det = float4(1.0f) / (src[0][0]*dst[0][0]+src[0][1]*dst[0][1]+src[0][2]*dst[0][2]+src[0][3]*dst[0][3]);

	// invert matrix
	dst *= det;

	return dst;
}

template < int row, int col >
void mpl::matrix4<row, col>::unpack(int matrix_index, float *out) const
{
	for (int i = 0; i < row*col; ++i) {
		out[i] = (((float4*)e)[i])[matrix_index];
	}
}

#endif // MPLMATRIX4_H
