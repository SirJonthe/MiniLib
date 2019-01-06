#include "mglPlane.h"

mglClip mglPlane::DetermineSide(float dist) const
{
	return mmlIsApproxEqual(dist, 0.0f) ? mglCoinciding : (dist > 0.0f ? mglInFront : mglBehind);
}

bool mglPlane::IsBehind(float dist) const
{
	return !mmlIsApproxEqual(dist, 0.0f) && dist < 0.0f;
}

bool mglPlane::IsInFront(float dist) const
{
	return !mmlIsApproxEqual(dist, 0.0f) && dist > 0.0f;
}

bool mglPlane::IsCoinciding(float dist) const
{
	return mmlIsApproxEqual(dist, 0.0f);
}

mglPlane::mglPlane(const mmlVector<3> &p_position, const mmlVector<3> &p_normal) :
m_position(p_position), m_normal(mmlNormalize(p_normal))
{}

mglPlane::mglPlane(const mmlVector<3> &a, const mmlVector<3> &b, const mmlVector<3> &c) :
m_position(a), m_normal(mmlSurfaceNormal(a, b, c))
{}

mglClip mglPlane::DetermineClipping(const mmlVector<3> &a, const mmlVector<3> &b, const mmlVector<3> &c) const
{
	const mglClip side[3] = { DetermineSide(a), DetermineSide(b), DetermineSide(c) };
	int behind = 0, inFront = 0, coinciding = 0;
	for (int i = 0; i < 3; ++i) {
		switch (side[i]) {
		case mglBehind:
			++behind;
			break;
		case mglInFront:
			++inFront;
			break;
		default:
			++coinciding;
			break;
		};
	}
	if (coinciding == 3) { return mglCoinciding; }
	if (behind == 3 || (inFront == 0 && coinciding > 0)) { return mglBehind; }
	if (inFront == 3 || (behind == 0 && coinciding > 0)) { return mglInFront; }
	return mglClipping;
}

mglClip mglPlane::DetermineSide(const mmlVector<3> &point) const
{
	return DetermineSide(GetSignedDistance(point));
}

bool mglPlane::IsBehind(const mmlVector<3> &point) const
{
	return IsBehind(GetSignedDistance(point));
}

bool mglPlane::IsInFront(const mmlVector<3> &point) const
{
	return IsInFront(GetSignedDistance(point));
}

bool mglPlane::IsCoinciding(const mmlVector<3> &point) const
{
	return IsCoinciding(GetSignedDistance(point));
}
