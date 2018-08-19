#ifndef MML_RANDOM_INCLUDED
#define MML_RANDOM_INCLUDED

// Based on minimal PCG32 algorithm implementation by M.E. O'Neill (pcg-random.org)
// Licenced under Apache Licence 2.0

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
};

#endif
