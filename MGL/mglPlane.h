#ifndef MGL_PLANE_H_INCLUDED__
#define MGL_PLANE_H_INCLUDED__

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
	float GetSignedDistance(const mmlVector<3> &point) const { return mmlDot(point - m_position, m_normal); }
	mglClip DetermineSide(float dist) const;
	bool IsBehind(float dist) const;
	bool IsInFront(float dist) const;
	bool IsCoinciding(float dist) const;
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
	const mmlVector<3> &GetNormal( void ) const { return m_normal; }
};

// Clipping test:
	// Clip a CLIPPING triangle once and grab output
	// Try clipping one of the output triangles against the plane again
	// Output MUST be the same as input

/*template < int n >
int mglPlane::Clip(const mmlVector<n> &a, const mmlVector<n> &b, const mmlVector<n> &c, mmlVector<n> (&out)[4]) const
{	
	const mmlVector<n> *triangle[3] = { &a, &b, &c };
	int num = 0;
	float bdist = mmlDot(*(mmlVector<3>*)(triangle[2]) - m_position, m_normal); //previous distance
	for (int i = 0, j = 2; i < 3; j=i, ++i){
		float adist = mmlDot(*(mmlVector<3>*)(triangle[i]) - m_position, m_normal); // current distance
		if ((adist > 0.f) != (bdist > 0.f)) {
			//if (adist != 0.0f && bdist != 0.0f) {
			float s = adist / (adist - bdist);
			out[num++] = *triangle[i] + (*triangle[j] - *triangle[i]) * s;
			//}
		}
		if (adist >= 0.f) {
			out[num++] = *triangle[i];
		}

		bdist = adist; // do this to avoid recalculating same values every loop
	}
	return num;
}*/

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
	
	// HOT MESS, COMING THROUGH
	
	/*const float adist = mmlDot(mmlVector<3>::Cast(&a) - m_position, m_normal);
	const float bdist = mmlDot(mmlVector<3>::Cast(&b) - m_position, m_normal);
	const float cdist = mmlDot(mmlVector<3>::Cast(&c) - m_position, m_normal);
	
	//if (adist <= 0.0f && bdist <= 0.0f && cdist <= 0.0f) { return 0; }
	
	if (adist == 0.0f) {
		if (bdist == 0.0f) { // a and b coinciding
			if (cdist <= 0.0f) { // all three coinciding (or c is clipping), return 0
				return 0;
			} else { // c is not coinciding or clipping, return full triangle
				out[0] = a;
				out[1] = b;
				out[2] = c;
				return 3;
			}
		} else if (cdist == 0.0f) { // a and c coinciding
			if (bdist < 0.0f) { // we already know b is not coinciding, b is on negative side, clip entire triangle
				return 0;
			} else { // b is on positive side, return entire triangle
				out[0] = a;
				out[1] = b;
				out[2] = c;
				return 3;
			}
		} else { // only a is coinciding
			if (bdist * cdist > 0.0f) { // b and c are on the same side
				if (bdist < 0.0f) { // b and c are behind plane, return 0
					return 0;
				} else { // b and c are in front of plane, return full triangle
					out[0] = a;
					out[1] = b;
					out[2] = c;
					return 3;
				}
			}
		}
	} else if (bdist == 0.0f) { // b is coinciding, but not a
		if (cdist == 0.0f) { // b and c is coinciding
			if (adist < 0.0f) {// we already know a is not coinciding, a is on negative side, clip entire triangle
				return 0;
			} else { // a is on positive side, return entire triangle
				out[0] = a;
				out[1] = b;
				out[2] = c;
				return 3;
			}
		} else { // only b is coinciding
			if (adist * cdist > 0.0f) { // a and c are on the same side
				if (cdist < 0.0f) { // a and c are behind plane, return 0
					return 0;
				} else { // a and c are in front of plane, return full triangle
					out[0] = a;
					out[1] = b;
					out[2] = c;
					return 3;
				}
			}
		}
	} else if (cdist == 0.0f) { // only c coinciding
		if (adist * bdist > 0.0f) { // a and b are on the same side
			if (bdist < 0.0f) { // b and a are behind plane, return 0
				return 0;
			} else { // b and a are in front of plane, return full triangle
				out[0] = a;
				out[1] = b;
				out[2] = c;
				return 3;
			}
		}
	}
	
	int num = 0;
	if ((adist > 0.0f) != (cdist > 0.0f)) {
		float s = adist / (adist - cdist);
		out[num++] = a + (c - a) * s;
	}
	if (adist >= 0.0f) {
		out[num++] = a;
	}
	
	if ((bdist > 0.0f) != (adist > 0.0f)) {
		float s = bdist / (bdist - adist);
		out[num++] = b + (a - b) * s;
	}
	if (bdist >= 0.0f) {
		out[num++] = b;
	}
	
	if ((cdist > 0.0f) != (bdist > 0.0f)) {
		float s = cdist / (cdist - bdist);
		out[num++] = c + (b - c) * s;
	}
	if (cdist >= 0.0f) {
		out[num++] = c;
	}
	
	return num;*/

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

/*template < int n >
int mglPlane::Clip(const mmlVector<n> &a, const mmlVector<n> &b, const mmlVector<n> &c, mmlVector<n> (&result)[4]) const
{
	long positive = 0;
	long negative = 0;
	const float boundaryEpsilon = 0.001f;
	const signed char polygonExterior = -1;
	const signed char polygonBoundary = 0;
	const signed char polygonInterior = 1;

	const long vertexCount = 3;
	const mmlVector<n> vertex[vertexCount] = { a, b, c };
	signed char location[3];

	for (long a = 0; a < vertexCount; a++) {
		float d = plane * vertex[a];
		if (d > boundaryEpsilon) {
			location[a] = polygonInterior;
			positive++;
		} else {
			if (d < -boundaryEpsilon) {
				location[a] = polygonExterior;
				negative++;
			} else {
				location[a] = polygonBoundary;
			}
		}
	}

	if (negative == 0) {
		for (long a = 0; a < vertexCount; a++) result[a] = vertex[a];
		return (vertexCount);
	} else if (positive == 0) {
		return (0);
	}

	long count = 0;
	long previous = vertexCount - 1;
	for (long index = 0; index < vertexCount; index++) {
		long loc = location[index];
		if (loc == polygonExterior) {
			if (location[previous] == polygonInterior) {
				const mmlVector<n>& v1 = vertex[previous];
				const mmlVector<n>& v2 = vertex[index];
				mmlVector<n> dv = v2 - v1;

				float t = plane * v2 / (plane * dv);
				result[count++] = v2 - dv * t;
			}
		} else {
			const mmlVector<n>& v1 = vertex[index];
			if ((loc == polygonInterior) && (location[previous] == polygonExterior)) {
				const mmlVector<n>& v2 = vertex[previous];
				mmlVector<n> dv = v2 - v1;

				float t = plane * v2 / (plane * dv);
				result[count++] = v2 - dv * t;
			}

			result[count++] = v1;
		}

		previous = index;
	}

	return (count);
}*/

template < int n >
int mglPlane::ReverseClip(const mmlVector<n> &a, const mmlVector<n> &b, const mmlVector<n> &c, mmlVector<n> (&out)[4]) const
{
	mglPlane reversed;
	reversed.m_position = m_position;
	reversed.m_normal = -m_normal;
	return reversed.Clip(a, b, c, out);
}

#endif
