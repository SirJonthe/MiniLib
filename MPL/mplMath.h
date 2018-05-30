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

// inline mpl::wide_float mmlFloor(const mpl::wide_float &x) { return mpl::Floor(x); }
// inline mpl::wide_float mmlCeil(const mpl::wide_float &x)  { return mpl::Ceil(x);  }
// inline mpl::wide_float mmlRound(const mpl::wide_float &x) { return mpl::Round(x); }

#endif // MPL_MATH_H_INCLUDED__
