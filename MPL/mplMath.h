#ifndef MPL_MATH_H_INCLUDED__
#define MPL_MATH_H_INCLUDED__

#include "mplWide.h"

inline mpl::wide_float                       mmlMax(const mpl::wide_float &a, const mpl::wide_float &b)       { return mpl::wide_float::max(a, b); }
inline mpl::wide_float                       mmlMin(const mpl::wide_float &a, const mpl::wide_float &b)       { return mpl::wide_float::min(a, b); }
inline mpl::wide_float                       mmlSqrt(const mpl::wide_float &a)                                { return mpl::wide_float::sqrt(a); }
inline mpl::wide_int                         mmlMax(const mpl::wide_int &a, const mpl::wide_int &b)           { return mpl::wide_int::max(a, b); }
inline mpl::wide_int                         mmlMin(const mpl::wide_int &a, const mpl::wide_int &b)           { return mpl::wide_int::min(a, b); }
template < int n > inline mpl::wide_fixed<n> mmlMax(const mpl::wide_fixed<n> &a, const mpl::wide_fixed<n> &b) { return mpl::wide_fixed<n>::max(a, b); }
template < int n > inline mpl::wide_fixed<n> mmlMin(const mpl::wide_fixed<n> &a, const mpl::wide_fixed<n> &b) { return mpl::wide_fixed<n>::min(a, b); }

inline mpl::wide_int mmlRound(const mpl::wide_float &x) { return mpl::wide_int(x + 0.5f); }
inline mpl::wide_int mmlFloor(const mpl::wide_float &x) { return mpl::wide_int(x + (1<<15)) - (1<<15); }
inline mpl::wide_int mmlCeil(const mpl::wide_float &x)  { return mpl::wide_int(1<<15) - mpl::wide_int(mpl::wide_float(1<<15) - x); }

#endif // MPL_MATH_H_INCLUDED__
