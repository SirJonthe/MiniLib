#ifndef MPL_MATH_H_INCLUDED__
#define MPL_MATH_H_INCLUDED__

#include "mplWide.h"

namespace mpl
{
	//inline wide_float Floor(const wide_float &x);
	//inline wide_float Ceil(const wide_float &x)
	//{
	//}
	//inline mpl::wide_float Round(const mpl::wide_float &x)
	//{
	//	return mpl::Floor(x + mpl::wide_float(0.5f));
	//}
}

#define mmlFloor mpl::Floor
#define mmlCeil  mpl::Ceil
#define mmlRound mpl::Round

#endif // MPL_MATH_H_INCLUDED__
