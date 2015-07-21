#include <cstdlib>
#include "mglNoise.h"
#include "../MML/mmlVector.h"

/*mtlRandomizer::mtlRandomizer(unsigned int z, unsigned int w) : m_dist()
{
	SetSeeds(z, w);
	SetDistribution<mtlLinearDistribution>();
}

void mtlRandomizer::SetSeeds(unsigned int z, unsigned int w)
{
	// must be non-zero
	m_rseed_z = z == 0 ? 0xfeedface : z;
	m_rseed_w = w == 0 ? 0xdeadbeef : w;
}

unsigned int mtlRandomizer::GetRandomUint( void )
{
	m_rseed_z = 36969 * (m_rseed_z & 65535) + (m_rseed_z >> 16);
	m_rseed_w = 18000 * (m_rseed_w & 65535) + (m_rseed_w >> 16);
	return (m_rseed_z << 16) + m_rseed_w;
}

unsigned int mtlRandomizer::GetRandomUint(unsigned int min, unsigned int max)
{
	unsigned int r = GetRandomUint();
	return (r % (max-min)) + min;
}

int mtlRandomizer::GetRandomInt( void )
{
	return (int)GetRandomUint();
}

int mtlRandomizer::GetRandomInt(int min, int max)
{
	int r = GetRandomInt();
	return abs(r % (max-min)) + min;
}

float mtlRandomizer::GetRandomFloat( void )
{
	return float((GetRandomUint() + 1) * 2.328306435454494e-10);
}

float mtlRandomizer::GetRandomFloat(float min, float max)
{
	float r = GetRandomFloat();
	return (r * (max-min)) + min;
}*/

mglNoiseGenerator::mglNoiseGenerator( void ) : m_rand(), m_seed(0), m_scale(1.0f), m_offset(0.0f, 0.0f, 0.0f)
{}

void mglNoiseGenerator::SetNoiseSeed(unsigned int seed)
{
	m_seed = seed;
}

void mglNoiseGenerator::SetRanomizerSeeds(unsigned int z, unsigned int w)
{
	m_rand.SetSeed(z, w);
}

void mglNoiseGenerator::SetScale(float scale)
{
	m_scale = mmlMax2(scale, 1.0f);
}

float mglNoiseGenerator::GetScale( void ) const
{
	return m_scale;
}

unsigned char mglPerlinNoiseGenerator::Perm(unsigned int i) const
{
	static const unsigned char p[256] = {
		151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
		140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
		247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
		 57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
		 74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
		 60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
		 65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
		200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
		 52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
		207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
		119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
		129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
		218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
		 81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
		184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
		222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
	};

	return p[i & 255];
}

float mglPerlinNoiseGenerator::Lerp(float d, float u, float v) const
{
	//return u + d * (v - u);
	return mmlLerp(u, v, d);
}

float mglPerlinNoiseGenerator::Fade(float d) const
{
	return d * d * d * (d * (d * 6.0f - 15.0f) + 10.0f);
}

float mglPerlinNoiseGenerator::Grad(int hash, float x, float y, float z) const
{
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float mglPerlinNoiseGenerator::Noise(float x, float y, float z) const
{
	float scale = 1.0f / GetScale();
	mmlVector<3> offset = GetOffset();
	x = (x + offset[0]) * scale;
	y = (y + offset[1]) * scale;
	z = (z + offset[2]) * scale;

	// right now noise is generated around 0,0 as center
	// alter this to make x, y center so that scaling scales from center

	int X = ((int)floor(x)) & 255;
	int Y = ((int)floor(y)) & 255;
	int Z = ((int)floor(z)) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	float u = Fade(x);
	float v = Fade(y);
	float w = Fade(z);

	int A = Perm(X) + Y;
	int AA = Perm(A) + Z;
	int AB = Perm(A + 1) + Z;
	int B = Perm(X + 1) + Y;
	int BA = Perm(B) + Z;
	int BB = Perm(B + 1) + Z;

	float res = Lerp(w, Lerp(v, Lerp(u, Grad(Perm(AA), x, y, z), Grad(Perm(BA), x-1, y, z)), Lerp(u, Grad(Perm(AB), x, y-1, z), Grad(Perm(BB), x-1, y-1, z))),	Lerp(v, Lerp(u, Grad(Perm(AA+1), x, y, z-1), Grad(Perm(BA+1), x-1, y, z-1)), Lerp(u, Grad(Perm(AB+1), x, y-1, z-1), Grad(Perm(BB+1), x-1, y-1, z-1))));
	//return rand.GetDistribution((res + 1.0f) / 2.0f);
	return (res + 1.0f) / 2.0f;
}

mglHalfspaceNoiseGenerator::mglHalfspaceNoiseGenerator( void ) : m_halfspaces(1), m_colorShift(3)
{}

void mglHalfspaceNoiseGenerator::SetHalfspaceDivisionCount(int halfspaces)
{
	m_halfspaces = halfspaces;
}

int mglHalfspaceNoiseGenerator::GetHalfspaceDivisionCount( void ) const
{
	return m_halfspaces;
}

void mglHalfspaceNoiseGenerator::SetColorShift(int colorShift)
{
	m_colorShift = colorShift;
}

float mglHalfspaceNoiseGenerator::Noise(float x, float y, float z) const
{
	return 0.0f;
}

void mglHalfspaceNoiseGenerator::Noise(unsigned char *input, int bpp, int w, int h) const
{
	short *ext = new short[bpp*w*h];
	for (int i = 0; i < w*h*bpp; ++i) {
		ext[i] = 0x7F;
	}
	mtlRandom rand = GetRandomizer();
	for (int i = 0; i < m_halfspaces; ++i) {
		int color_offset = rand.GetInt(-m_colorShift+1, m_colorShift);
		mmlVector<2> p1(rand.GetFloat() * (float)w, rand.GetFloat() * (float)h);
		mmlVector<2> p2(rand.GetFloat() * (float)w, rand.GetFloat() * (float)h);
		mmlVector<2> tan = p2 - p1;
		mmlVector<2> normal(-tan[1], tan[0]);
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				mmlVector<2> frag = mmlVector<2>((float)x, (float)y) - p1;
				if (mmlDot(frag, normal) > 0.0f) {
					int index = (y * w + x) * bpp;
					short color = ext[index] + color_offset;
					for (int j = 0; j < bpp; ++j) {
						ext[index+j] = color;
					}
				}
			}
		}
	}

	for (int i = 0; i < w*h*bpp; ++i) {
		input[i] = (unsigned char)mmlClamp((short)0, ext[i], (short)255);
	}

	delete [] ext;
}
