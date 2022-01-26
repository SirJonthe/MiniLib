#ifndef MML_RANDOM_INCLUDED
#define MML_RANDOM_INCLUDED

// Based on minimal PCG32 algorithm implementation by M.E. O'Neill (pcg-random.org)
// Licenced under Apache Licence 2.0

#include "mmlVector.h"

class mmlRandom
{
private:
	unsigned long long m_state;
	unsigned long long m_inc;

public:
				 mmlRandom( void );
	explicit     mmlRandom(unsigned long long seed, unsigned long long inc = 1);

	void         SetSeed(unsigned long long seed, unsigned long long inc = 1);

	unsigned int GetUint( void );
	unsigned int GetUint(unsigned int min, unsigned int max);

	int          GetInt( void );
	int          GetInt(int min, int max);

	float        GetFloat( void );
	float        GetFloat(float min, float max);

	template < int n, typename real_t >
	mmlVector<n,real_t> GetNormal( void );
};

template < int n, typename real_t >
mmlVector<n,real_t> mmlRandom::GetNormal( void )
{
	static constexpr real_t TAU = mmlTau<20,real_t>();
	mmlVector<n,real_t> v;
	real_t sum = real_t(0);
	do {
		sum = real_t(0);
		for (int i = 0; i < n - 1; ++i) {
			const real_t A = real_t(GetFloat());
			const real_t B = real_t(GetFloat());
			const real_t radius = mmlSqrt(real_t(-2) * log(A));
			const real_t angle = TAU * B;
			v[i] = radius * ( (i & 1) == 0 ? mmlCos(angle) : mmlSin(angle) );
			sum += v[i] * v[i];
		}
	} while (sum == real_t(0)); // Avoid 0!

	const real_t inv_len = real_t(1) / mmlSqrt(sum);
	for (int i = 0; i < n; ++i) {
		v[i] *= inv_len;
	}

	return v;
}

#endif
