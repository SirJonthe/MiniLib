#ifndef MPL_MATH_H_INCLUDED__
#define MPL_MATH_H_INCLUDED__

#include "mplWide.h"

namespace mpl
{
	//inline mpl::wide_float Floor(const mpl::wide_float &x);
	//inline mpl::wide_float Ceil(const mpl::wide_float &x)
	//{
	//}
	//inline mpl::wide_float Round(const mpl::wide_float &x)
	//{
	//	return mpl::Floor(x + mpl::wide_float(0.5f));
	//}
}

mpl::wide_float mmlMax(const mpl::wide_float &a, const mpl::wide_float &b) { return mpl::wide_float::max(a, b); }
mpl::wide_float mmlMin(const mpl::wide_float &a, const mpl::wide_float &b) { return mpl::wide_float::min(a, b); }
mpl::wide_float mmlSqrt(const mpl::wide_float &a) { return mpl::wide_float::sqrt(a); }

mpl::wide_int mmlMax(const mpl::wide_int &a, const mpl::wide_int &b) { return mpl::wide_int::max(a, b); }
mpl::wide_int mmlMin(const mpl::wide_int &a, const mpl::wide_int &b) { return mpl::wide_int::min(a, b); }

mpl::wide_fixed mmlMax(const mpl::wide_fixed &a, const mpl::wide_fixed &b) { return mpl::wide_fixed::max(a, b); }
mpl::wide_fixed mmlMin(const mpl::wide_fixed &a, const mpl::wide_fixed &b) { return mpl::wide_fixed::min(a, b); }

// inline mpl::wide_float mmlFloor(const mpl::wide_float &x) { return mpl::Floor(x); }
// inline mpl::wide_float mmlCeil(const mpl::wide_float &x)  { return mpl::Ceil(x);  }
// inline mpl::wide_float mmlRound(const mpl::wide_float &x) { return mpl::Round(x); }

#endif // MPL_MATH_H_INCLUDED__
