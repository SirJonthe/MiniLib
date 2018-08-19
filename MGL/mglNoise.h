#ifndef MGL_NOISE_H_INCLUDED
#define MGL_NOISE_H_INCLUDED

#include "mglIndex.h"
#include "../MTL/mtlPointer.h"
#include "../MML/mmlMath.h"
#include "../MML/mmlVector.h"
#include "../MTL/mtlRandom.h"

/*class mtlDistribution
{
public:
	virtual       ~mtlDistribution( void ) {}
	virtual float  Evaluate(float x) const = 0;
};

class mtlLinearDistribution : public mtlDistribution
{
public:
	virtual float Evaluate(float x) const { return x; }
};

class mtlSineStepDistribution : public mtlDistribution
{
public:
	mtlSineStepDistribution( void ) : m_steps(1.0f) {}
	void			SetStepCount(float steps) { m_steps = steps; }
	virtual float	Evaluate(float x) const { return x + sin(m_steps*x*mmlPI)/(m_steps*mmlPI); }

private:
	float m_steps;
};*/

class mglNoiseGenerator
{
private:
	mtlRandom    m_rand;
	unsigned int m_seed;
	float        m_scale;
	mmlVector<3> m_offset;

protected:
	mtlRandom GetRandomizer( void ) const { return m_rand; }

public:
				  mglNoiseGenerator( void );
	void          SetNoiseSeed(unsigned int seed);
	void          SetRanomizerSeeds(unsigned int z, unsigned int w);
	//template < typename dist_t >
	//void          SetRandomizerDistribution( void ) { m_rand.SetDistribution<dist_t>(); }
	void          SetScale(float scale);
	float         GetScale( void ) const;
	virtual float Noise(float x, float y, float z) const = 0;
	void          SetOffset(float x, float y, float z) { m_offset = mmlVector<3>(x, y, z); }
	mmlVector<3>  GetOffset( void ) const { return m_offset; }
};

class mglPerlinNoiseGenerator : public mglNoiseGenerator
{
private:
	unsigned char Perm(unsigned int i)                      const;
	float         Lerp(float d, float u, float v)           const;
	float         Grad(int hash, float x, float y, float z) const;
	float         Fade(float d)                             const;

public:
	virtual float Noise(float x, float y, float z) const;
};

class mglBillowNoiseGenerator : public mglNoiseGenerator
{
};

class mglVoronoiNoiseGenerator : public mglNoiseGenerator
{
};

class mglHalfspaceNoiseGenerator : public mglNoiseGenerator
{
private:
	int m_halfspaces;
	int m_colorShift;

public:
				  mglHalfspaceNoiseGenerator( void );
	void          SetHalfspaceDivisionCount(int halfspaces);
	int           GetHalfspaceDivisionCount( void ) const;
	void          SetColorShift(int colorShift);
	virtual float Noise(float x, float y, float z) const;
	void          Noise(unsigned char *input, int bpp, int w, int h) const;
};

#endif // MGL_NOISE_H_INCLUDED
