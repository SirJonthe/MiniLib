#include "mtlRandom.h"
#include <cstdlib>

mtlRandom::mtlRandom( void ) : m_state(0xe10df4c5d34db135), m_inc(1)
{}

mtlRandom::mtlRandom(unsigned long long seed, unsigned long long inc) : m_state(0), m_inc(1)
{
	SetSeed(seed, inc);
}

void mtlRandom::SetSeed(unsigned long long seed, unsigned long long inc)
{
	m_state = seed;
	m_inc = inc | 1;
}

unsigned int mtlRandom::GetUint( void )
{
	unsigned long long oldstate = m_state;
	// Advance internal state
	m_state = oldstate * 6364136223846793005ull + m_inc;
	// Calculate output function (XSH RR), uses old state for max ILP
	unsigned int xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	unsigned int rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

unsigned int mtlRandom::GetUint(unsigned int min, unsigned int max)
{
	if (max == min) { return min; }
	unsigned int r = GetUint();
	return (r % (max-min)) + min;
}

int mtlRandom::GetInt( void )
{
	return (int)GetUint();
}

int mtlRandom::GetInt(int min, int max)
{
	if (max == min) { return min; }
	int r = GetInt();
	return abs(r % (max-min)) + min;
}

// Floating-point conversion magic by John D. Cook (http://www.codeproject.com/Articles/25172/Simple-Random-Number-Generation)
float mtlRandom::GetFloat( void )
{
	return float((GetUint() + 1) * 2.328306435454494e-10);
}

float mtlRandom::GetFloat(float min, float max)
{
	return GetFloat() * (max - min) + min;
}
