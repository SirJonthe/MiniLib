#include "mglRay.h"

void mglDifferentialAnalyzer3::SetInitialState(const mglRay3 &p_ray)
{
	m_origin = p_ray.origin;
	m_direction = p_ray.direction;
	m_xyz[0] = int( p_ray.origin[0] );
	m_xyz[1] = int( p_ray.origin[1] );
	m_xyz[2] = int( p_ray.origin[2] );
	for (int i = 0; i < 3; ++i) {
		m_delta[i] = (m_direction * (1.0f / m_direction[i])).Len();
		if (m_direction[i] < 0.0f) {
			m_step[i] = -1;
			m_lengths[i] = (p_ray.origin[i] - m_xyz[i]) * m_delta[i];
		} else {
			m_step[i] = 1;
			m_lengths[i] = (m_xyz[i] + 1.0f - p_ray.origin[i]) * m_delta[i];
		}
	}
	//m_side = -1; // this breaks calling GetDistance()
	//m_side = 0; // this also breaks calling GetDistance() since it will be 0
	Step();
}

void mglDifferentialAnalyzer3::Step( void )
{
	m_side = 0;
	for (int i = 1; i < 3; ++i) {
		if (m_lengths[m_side] > m_lengths[i]) {
			m_side = i;
		}
	}
	m_lengths[m_side] += m_delta[m_side];
	m_xyz[m_side] += m_step[m_side];
}

mmlVector<2> mglDifferentialAnalyzer3::GetImpactUV( void ) const
{
	mmlVector<3> impact = GetImpactPosition();
	float u = impact[(m_side + 1) % 3];
	float v = impact[(m_side + 2) % 3];
	return mmlVector<2>(u - floor(u), v - floor(v));
}

void mglDifferentialAnalyzer2::SetInitialState(const mglRay2 &p_ray)
{
	m_origin = p_ray.origin;
	m_direction = p_ray.direction;
	m_xyz[0] = int( p_ray.origin[0] );
	m_xyz[1] = int( p_ray.origin[1] );
	for (int i = 0; i < 2; ++i) {
		m_delta[i] = (m_direction * (1.0f / m_direction[i])).Len();
		if (m_direction[i] < 0.0f) {
			m_step[i] = -1;
			m_lengths[i] = (p_ray.origin[i] - m_xyz[i]) * m_delta[i];
		} else {
			m_step[i] = 1;
			m_lengths[i] = (m_xyz[i] + 1.0f - p_ray.origin[i]) * m_delta[i];
		}
	}
	//m_side = -1; // this breaks calling GetDistance()
	//m_side = 0; // this also breaks calling GetDistance() since it will be 0
	Step();
}

void mglDifferentialAnalyzer2::Step( void )
{
	m_side = 0;
	for (int i = 1; i < 2; ++i) {
		if (m_lengths[m_side] > m_lengths[i]) {
			m_side = i;
		}
	}
	m_lengths[m_side] += m_delta[m_side];
	m_xyz[m_side] += m_step[m_side];
}

float mglDifferentialAnalyzer3::GetImpactU( void ) const
{
	mmlVector<3> impact = GetImpactPosition();
	float u = impact[(m_side + 1) % 2];
	return u - floor(u);
}
