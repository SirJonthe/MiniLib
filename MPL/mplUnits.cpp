#include <cassert>
#include "mplUnits.h"
#include "mplWide.h"

using namespace mpl;

#define FORWIDTH for (int i = 0; i < MPL_WIDTH; ++i)

template < typename wide_t >
void TestReadWrite( void )
{
	typename wide_t::vector_t out;
	FORWIDTH { out[i] = typename wide_t::scalar_t(i); }

	wide_t a = wide_t(out);

	FORWIDTH { out[i] = 0; }

	a.to_scalar(out);

	FORWIDTH {
		assert(out[i] == i);
	}
}

template < typename wide_t >
void TestCmov( void )
{
	typename wide_t::vector_t out;

	typename wide_t::vector_t as;
	FORWIDTH { as[i] = typename wide_t::scalar_t(int(i / 1.5f)); }
	wide_t a = wide_t(as);

	typename wide_t::vector_t bs;
	FORWIDTH { out[i] = typename wide_t::scalar_t(int((MPL_WIDTH - i - 1) / 1.5f)); }
	wide_t b = wide_t(bs);

	wide_t final = wide_t::cmov(a < b, a, b);
	final.to_scalar(out);
	FORWIDTH { assert(out[i] == (as[i] < bs[i] ? as[i] : bs[i])); }

	final = wide_t::cmov(a <= b, a, b);
	final.to_scalar(out);
	FORWIDTH { assert(out[i] == (as[i] <= bs[i] ? as[i] : bs[i])); }

	final = wide_t::cmov(a > b, a, b);
	final.to_scalar(out);
	FORWIDTH { assert(out[i] == (as[i] > bs[i] ? as[i] : bs[i])); }

	final = wide_t::cmov(a >= b, a, b);
	final.to_scalar(out);
	FORWIDTH { assert(out[i] == (as[i] >= bs[i] ? as[i] : bs[i])); }

	final = wide_t::cmov(a == b, a, b);
	final.to_scalar(out);
	FORWIDTH { assert(out[i] == (as[i] == bs[i] ? as[i] : bs[i])); }

	final = wide_t::cmov(a != b, a, b);
	final.to_scalar(out);
	FORWIDTH { assert(out[i] == (as[i] != bs[i] ? as[i] : bs[i])); }
}

template < typename wide_t >
void TestBool2Wide( void )
{
	typename wide_t::vector_t out;
	wide_t w;

	w = wide_t(true);
	w.to_scalar(out);
	FORWIDTH { assert(out[i] == typename wide_t::scalar_t(1)); }

	w = wide_t(false);
	w.to_scalar(out);
	FORWIDTH { assert(out[i] == typename wide_t::scalar_t(0)); }
}

void mpl::unit_test::DoUnitTests( void )
{
	TestReadWrite<wide_int>();
	TestReadWrite<wide_float>();
	TestCmov<wide_int>();
	TestCmov<wide_float>();
	TestBool2Wide<wide_int>();
	TestBool2Wide<wide_float>();
}
