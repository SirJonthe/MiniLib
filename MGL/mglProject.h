#ifndef MGL_PROJECT_H_INCLUDED__
#define MGL_PROJECT_H_INCLUDED__

inline float mglProject1D(float point, float cam, float dist, float scale = 1.0f)
{
	return ((point - cam) * scale) / dist;
}

inline mmlVector<2> mglProject2D(mmlVector<2> point, mmlVector<2> cam, float dist, float scale = 1.0f)
{
	return ((point - cam) * scale) / dist;
}

#endif // MGL_PROJECT_H_INCLUDED__
