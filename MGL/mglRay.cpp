#include "mglRay.h"

void mglDifferentialAnalyzer::SetInitialState(const mglRay &p_ray)
{
	m_direction = p_ray.direction;
	m_xyz[0] = int( p_ray.position[0] );
	m_xyz[1] = int( p_ray.position[1] );
	m_xyz[2] = int( p_ray.position[2] );
	for (int i = 0; i < 3; ++i) {
		m_delta[i] = (m_direction * (1.0f / m_direction[i])).Len();
		if (m_direction[i] < 0.0f) {
			m_step[i] = -1;
			m_position[i] = (p_ray.position[i] - m_xyz[i]) * m_delta[i];
		} else {
			m_step[i] = 1;
			m_position[i] = (m_xyz[i] + 1.0f - p_ray.position[i]) * m_delta[i];
		}
	}
	m_side = -1;
}

void mglDifferentialAnalyzer::Step( void )
{
	m_side = 0;
	for (int i = 1; i < 3; ++i) {
		if (m_position[m_side] > m_position[i]) {
			m_side = i;
		}
	}
	m_position[m_side] += m_delta[m_side];
	m_xyz[m_side] += m_step[m_side];
}
