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
//inline mpl::wide_float                       mmlWrap(mpl::wide_float value, const mpl::wide_float &max)       { return mpl::wide_float::mov_if_true(mpl::wide_float::mov_if_true(value, max - (-value % max), value < 0), value % max, value > max); }
//inline mpl::wide_int                         mmlWrap(mpl::wide_int value, const mpl::wide_int &max)           { return mpl::wide_int::mov_if_true(mpl::wide_int::mov_if_true(value, (max + 1) - (-value % max), value < 0), value % (max + 1), value > max); }
//template < int n > inline mpl::wide_fixed<n> mmlWrap(mpl::wide_fixed<n> value, const mpl::wide_fixed<n> &max) { return *(mpl::wide_fixed<n>*)&(mmlWrap(*(mpl::wide_int*)&value, *(const mpl::wide_int*)&max)); }

inline mpl::wide_int mmlRound(const mpl::wide_float &x) { return mpl::wide_int(x + 0.5f); }
inline mpl::wide_int mmlFloor(const mpl::wide_float &x) { return mpl::wide_int(x + (1<<15)) - (1<<15); }
inline mpl::wide_int mmlCeil(const mpl::wide_float &x)  { return mpl::wide_int(1<<15) - mpl::wide_int(mpl::wide_float(1<<15) - x); }

#endif // MPL_MATH_H_INCLUDED__
