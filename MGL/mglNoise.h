#ifndef MGL_NOISE_H_INCLUDED__
#define MGL_NOISE_H_INCLUDED__

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
};

class mtlRandomizer
{
public:
	mtlRandomizer(unsigned int z = 0, unsigned int w = 0);
	void			SetSeeds(unsigned int z, unsigned int w);
	template < typename dist_t >
	void			SetDistribution( void ) { m_dist.New<dist_t>(); }
	float			GetDistribution(float x) { return m_dist->Evaluate(x); }
	unsigned int	GetRandomUint( void );
	unsigned int	GetRandomUint(unsigned int min, unsigned int max);
	int				GetRandomInt( void );
	int				GetRandomInt(int min, int max);
	float			GetRandomFloat( void );
	float			GetRandomFloat(float min, float max);

private:
	mtlShared<mtlDistribution>	m_dist;
	unsigned int				m_rseed_z;
	unsigned int				m_rseed_w;
};*/

class mglNoiseGenerator
{
public:
				  mglNoiseGenerator( void );
	void          SetNoiseSeed(unsigned int seed);
	void          SetRanomizerSeeds(unsigned int z, unsigned int w);
	template < typename dist_t >
	//void          SetRandomizerDistribution( void ) { m_rand.SetDistribution<dist_t>(); }
	void          SetScale(float scale);
	float         GetScale( void ) const;
	virtual float Noise(float x, float y, float z) const = 0;

protected:
	mtlRandom GetRandomizer( void ) const { return m_rand; }

private:
	//mtlRandomizer	m_rand;
	mtlRandom    m_rand;
	unsigned int m_seed;
	float        m_scale;
	mmlVector<2> m_offset;
};

class mglPerlinNoiseGenerator : public mglNoiseGenerator
{
private:
	float Lerp(float d, float u, float v) const;
	float Grad(int hash, float x, float y, float z) const;
	float Fade(float d) const;

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
public:
				  mglHalfspaceNoiseGenerator( void );
	void          SetHalfspaceDivisionCount(int halfspaces);
	int           GetHalfspaceDivisionCount( void ) const;
	void          SetColorShift(int colorShift);
	virtual float Noise(float x, float y, float z) const;
	void          Noise(unsigned char *input, int bpp, int w, int h) const;

private:
	int m_halfspaces;
	int m_colorShift;
};

#endif // MGL_NOISE_H_INCLUDED__
