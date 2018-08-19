#ifndef MGL_PLANE_H_INCLUDED
#define MGL_PLANE_H_INCLUDED

#include "../MML/mmlVector.h"

enum mglClip
{
	mglClipping,
	mglBehind,
	mglCoinciding,
	mglInFront
};

class mglPlane
{
private:
	mmlVector<3> m_position;
	mmlVector<3> m_normal;

private:
	float   GetSignedDistance(const mmlVector<3> &point) const { return mmlDot(point - m_position, m_normal); }
	mglClip DetermineSide(float dist) const;
	bool    IsBehind(float dist) const;
	bool    IsInFront(float dist) const;
	bool    IsCoinciding(float dist) const;

public:
	mglPlane( void ) {} // do nothing
	mglPlane(const mmlVector<3> &p_position, const mmlVector<3> &p_normal);
	mglPlane(const mmlVector<3> &a, const mmlVector<3> &b, const mmlVector<3> &c);

	template < int n > int Clip(const mmlVector<n> &a, const mmlVector<n> &b, const mmlVector<n> &c, mmlVector<n> (&out)[4]) const;
	template < int n > int ReverseClip(const mmlVector<n> &a, const mmlVector<n> &b, const mmlVector<n> &c, mmlVector<n> (&out)[4]) const;

	mglClip DetermineClipping(const mmlVector<3> &a, const mmlVector<3> &b, const mmlVector<3> &c) const;
	mglClip DetermineSide(const mmlVector<3> &point) const;

	bool IsBehind(const mmlVector<3> &point) const;
	bool IsInFront(const mmlVector<3> &point) const;
	bool IsCoinciding(const mmlVector<3> &point) const;

	const mmlVector<3> &GetPosition( void ) const { return m_position; }
	const mmlVector<3> &GetNormal( void )   const { return m_normal; }
};

template < int n >
int mglPlane::Clip(const mmlVector<n> &a, const mmlVector<n> &b, const mmlVector<n> &c, mmlVector<n> (&out)[4]) const
{
	// if coinciding, return NOTHING, since returning the full triangle
	// would mean returning the full triangle when calling ReverseClip.
	// This would have the effect that coinciding polygons are attached
	// to both front and back nodes when generating BSP trees.
	
	// if two edges are coinciding AND one is negative, OR
	// one edge is coinciding AND two edges are negative
	// return 0
	// if two edges are coinciding AND one is positive, OR
	// one edge is coinciding AND two are positive
	// return 3
	// All other cases
	// clip

	const mmlVector<n> in[3] = { a, b, c };
	float dist[3];
	mglClip side[3];
	int numOut = 0;
	for (int i = 0; i < 3; ++i) {
		dist[i] = GetSignedDistance(mmlVector<3>::Cast(&in[i]));
		side[i] = DetermineSide(dist[i]);
		if (side[i] <= int(mglCoinciding)) { ++numOut; }
	}
	if (numOut == 3) { // all verts are either behind, or on, plane
		return 0;
	}
	int numIn = 0;
	for (int i = 0, j = 2; i < 3; j=i, ++i) {
		if ((side[i] == int(mglBehind)) == (side[j] >= int(mglCoinciding))) {
			out[numIn++] = in[i] + (in[j] - in[i]) * (dist[i] / (dist[i] - dist[j]));
		}
		if (side[i] >= int(mglCoinciding)) {
			out[numIn++] = in[i];
		}
	}
	return numIn;
}

template < int n >
int mglPlane::ReverseClip(const mmlVector<n> &a, const mmlVector<n> &b, const mmlVector<n> &c, mmlVector<n> (&out)[4]) const
{
	mglPlane reversed;
	reversed.m_position = m_position;
	reversed.m_normal = -m_normal;
	return reversed.Clip(a, b, c, out);
}

#endif
