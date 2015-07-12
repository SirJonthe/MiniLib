#ifndef MGL_PROJECT_H_INCLUDED__
#define MGL_PROJECT_H_INCLUDED__

inline float mglProject1D(float cam, float src, float dist, float scale = 1.0f)
{
	return ((src - cam) * scale) / dist;
}

#endif // MGL_PROJECT_H_INCLUDED__
