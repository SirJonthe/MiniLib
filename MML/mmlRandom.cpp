#include "mmlRandom.h"
#include <cstdlib>

mmlRandom::mmlRandom( void ) : m_state(0xe10df4c5d34db135), m_inc(1)
{}

mmlRandom::mmlRandom(unsigned long long seed, unsigned long long inc) : m_state(0), m_inc(1)
{
	SetSeed(seed, inc);
}

void mmlRandom::SetSeed(unsigned long long seed, unsigned long long inc)
{
	m_state = seed;
	m_inc = inc | 1;
	GetUint(); // Generate a dummy value to avoid an oft recurring initial value
}

unsigned int mmlRandom::GetUint( void )
{
	unsigned long long oldstate = m_state;
	// Advance internal state
	m_state = oldstate * 6364136223846793005ull + m_inc;
	// Calculate output function (XSH RR), uses old state for max ILP
	unsigned int xorshifted = (unsigned int)(((oldstate >> 18u) ^ oldstate) >> 27u);
	unsigned int rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

unsigned int mmlRandom::GetUint(unsigned int min, unsigned int max)
{
	unsigned int r = GetUint();
	if (max <= min) { return min; }
	return (r % (max-min)) + min;
}

int mmlRandom::GetInt( void )
{
	return (int)GetUint();
}

int mmlRandom::GetInt(int min, int max)
{
	int r = GetInt();
	if (max <= min) { return min; }
	return abs(r % (max-min)) + min;
}

// Floating-point conversion magic by John D. Cook (http://www.codeproject.com/Articles/25172/Simple-Random-Number-Generation)
float mmlRandom::GetFloat( void )
{
	return float((GetUint() + 1) * 2.328306435454494e-10);
}

float mmlRandom::GetFloat(float min, float max)
{
	return GetFloat() * (max - min) + min;
}
