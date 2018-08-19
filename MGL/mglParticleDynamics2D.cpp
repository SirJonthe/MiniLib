#include "mglParticleDynamics2D.h"
#include "../MML/mmlMath.h"

#define XY(i,j) ((i)+(m_width+2)*(j))
#define TEST_XY(i,j) ((i) > -1 && (i) < m_width && (j) > -1 && (j) < m_height)
#define FOR2D for (int j = 1; j <= m_height; ++j) { for (int i = 1; i <= m_width; ++i) {
#define ENDFOR2D }}

void mglParticleDynamics2D::AddSource(float *x, float *s, float delta_time) const
{
	for (int i = 0; i < m_total_size; ++i) {
		 x[i] += delta_time * s[i];
	}
}

void mglParticleDynamics2D::SetBounds(int b, float *x) const
{
	// Add other edge modes
	// EdgeMode_Free

	// EdgeMode_Confined:
	if (m_edge_mode == EdgeMode_Confined) {
		for (int i = 1; i <= m_height; ++i) {
			x[XY(0,i)]         = b==1 ? -x[XY(1,i)]       : x[XY(1,i)];
			x[XY(m_width+1,i)] = b==1 ? -x[XY(m_width,i)] : x[XY(m_width,i)];
		}

		for (int i = 1; i <= m_width; ++i) {
			x[XY(i,0)]          = b==2 ? -x[XY(i,1)]        : x[XY(i,1)];
			x[XY(i,m_height+1)] = b==2 ? -x[XY(i,m_height)] : x[XY(i,m_height)];
		}

		x[XY(0,0)]                  = 0.5f * (x[XY(1,0)]                + x[XY(0,1)]);
		x[XY(0,m_height+1)]         = 0.5f * (x[XY(1,m_height+1)]       + x[XY(0  ,m_height)]);
		x[XY(m_width+1,0)]          = 0.5f * (x[XY(m_width,0  )]        + x[XY(m_width+1,1)]);
		x[XY(m_width+1,m_height+1)] = 0.5f * (x[XY(m_width,m_height+1)] + x[XY(m_width+1,m_height)]);
	}
	// EdgeMode_Wrap:
	else if (m_edge_mode == EdgeMode_Wrap) {
		for (int i = 1; i <= m_width; ++i) {
			x[XY(i, 0)] = x[XY(i, m_height)];
			x[XY(i, m_height+1)] = x[XY(i, 1)];
		}
		for (int i = 1; i <= m_height; ++i) {
			x[XY(0, i)] = x[XY(m_width, i)];
			x[XY(m_width+1, i)] = x[XY(1, i)];
		}
		x[XY(0, 0)] = x[XY(m_width, m_height)];
		x[XY(0, m_height+1)] = x[XY(m_width, 1)];
		x[XY(m_width+1, 0)] = x[XY(1, m_height)];
		x[XY(m_width+1, m_height+1)] = x[XY(1, 1)];
	}
}

void mglParticleDynamics2D::LinSolve(int b, float *x, const float *x0, float a, float c) const
{
	for (int k = 0; k < 20; ++k) {
		FOR2D
			x[XY(i,j)] = (x0[XY(i,j)] + a * (x[XY(i-1,j)] + x[XY(i+1,j)] + x[XY(i,j-1)] + x[XY(i,j+1)])) / c;
		ENDFOR2D
		SetBounds(b, x);
	}
}

void mglParticleDynamics2D::Diffuse(int b, float *x, const float *x0, float diff, float delta_time) const
{
	float a = delta_time * diff * (float)m_width * (float)m_height;
	LinSolve(b, x, x0, a, 1.0f + 4.0f * a);
}

void mglParticleDynamics2D::Advect(int b, float *d, const float *d0, const float *u, const float *v, float delta_time) const
{
	const float delta_time_x0 = delta_time * (float)m_width;
	const float delta_time_y0 = delta_time * (float)m_height;
	FOR2D
		float x = mmlClamp(0.5f, i - delta_time_x0 * u[XY(i,j)], (float)m_width + 0.5f);
		float y = mmlClamp(0.5f, j - delta_time_y0 * v[XY(i,j)], (float)m_height + 0.5f);
		int i0 = (int)x;
		int i1 = i0 + 1;
		int j0 = (int)y;
		int j1 = j0 + 1;
		float s1 = x - i0;
		float s0 = 1.0f - s1;
		float t1 = y - j0;
		float t0 = 1.0f - t1;
		d[XY(i,j)] = s0 * (t0 * d0[XY(i0,j0)] + t1 * d0[XY(i0,j1)])+
					 s1 * (t0 * d0[XY(i1,j0)] + t1 * d0[XY(i1,j1)]);
	ENDFOR2D
	SetBounds(b, d);
}

void mglParticleDynamics2D::Project(float *u, float *v, float *p, float *div) const
{
	const int max_dim = mmlMax(m_width, m_height);
	FOR2D
		div[XY(i,j)] = -0.5f * (u[XY(i+1,j)] - u[XY(i-1,j)] + v[XY(i,j+1)] - v[XY(i,j-1)]) / max_dim;
		p[XY(i,j)] = 0.0f;
	ENDFOR2D
	SetBounds(0, div);
	SetBounds(0, p);

	LinSolve(0, p, div, 1.0f, 4.0f);

	FOR2D
		u[XY(i,j)] -= 0.5f * (float)m_width * (p[XY(i+1,j)] - p[XY(i-1,j)]);
		v[XY(i,j)] -= 0.5f * (float)m_height * (p[XY(i,j+1)] - p[XY(i,j-1)]);
	ENDFOR2D
	SetBounds(1, u);
	SetBounds(2, v);
}

void mglParticleDynamics2D::UpdateVelocity(float *u, float *v, float *u0, float *v0, float visc, float delta_time) const
{
	AddSource(u, u0, delta_time);
	AddSource(v, v0, delta_time);
	mmlSwap(u0, u);
	Diffuse(1, u, u0, visc, delta_time);
	mmlSwap(v0, v);
	Diffuse(2, v, v0, visc, delta_time);
	Project(u, v, u0, v0);
	mmlSwap(u0, u);
	mmlSwap(v0, v);
	Advect(1, u, u0, u0, v0, delta_time);
	Advect(2, v, v0, u0, v0, delta_time);
	Project(u, v, u0, v0);
}

void mglParticleDynamics2D::UpdateDensity(float *x, float *x0, float *u, float *v, float diff, float delta_time) const
{
	AddSource(x, x0, delta_time);
	mmlSwap(x0, x);
	Diffuse(0, x, x0, diff, delta_time);
	mmlSwap(x0, x);
	Advect(0, x, x0, u, v, delta_time);
}

void mglParticleDynamics2D::SetForce(int x, int y, float x0, float y0, float force)
{
	if (TEST_XY(x, y)) {
		mmlVector<2> v;
		v[0] = x0;
		v[1] = y0;
		v.Normalize();
		SetForce(x, y, v[0] * force, v[1] * force);
	}
}

void mglParticleDynamics2D::AddForce(int x, int y, float x0, float y0, float force)
{
	if (TEST_XY(x, y)) {
		mmlVector<2> v;
		v[0] = x0;
		v[1] = y0;
		v.Normalize();
		AddForce(x, y, v[0] * force, v[1] * force);
	}
}

void mglParticleDynamics2D::Reset( void )
{
	for (int i = 0; i < m_total_size; i++) {
		m_u[i] = m_v[i] = m_u_prev[i] = m_v_prev[i] = m_dens[i] = m_dens_prev[i] = 0.0f;
	}
}

void mglParticleDynamics2D::Create(int width, int height)
{
	m_total_size = (width+2)*(height+2);
	m_dens.Create(m_total_size);
	m_u.Create(m_total_size);
	m_v.Create(m_total_size);
	m_dens_prev.Create(m_total_size);
	m_u_prev.Create(m_total_size);
	m_v_prev.Create(m_total_size);
	m_width = width;
	m_height = height;
	Reset();
}

void mglParticleDynamics2D::Create(int N)
{
	Create(N, N);
}

void mglParticleDynamics2D::Destroy( void )
{
	m_dens.Free();
	m_u.Free();
	m_v.Free();
	m_dens_prev.Free();
	m_u_prev.Free();
	m_v_prev.Free();
	m_total_size = m_width = m_height = 0;
}

void mglParticleDynamics2D::Update(float time_delta)
{
	UpdateVelocity(m_u, m_v, m_u_prev, m_v_prev, m_viscosity, time_delta);
	UpdateDensity(m_dens, m_dens_prev, m_u, m_v, m_diffusion, time_delta);
	for (int i = 0; i < m_total_size; ++i) {
		m_dens_prev[i] = m_u_prev[i] = m_v_prev[i] = 0.0f;
	}
}

void mglParticleDynamics2D::SetForce(int x, int y, float vel_x, float vel_y)
{
	++x; ++y;
	m_u_prev[XY(x, y)] = vel_x;
	m_v_prev[XY(x, y)] = vel_y;
}

void mglParticleDynamics2D::SetSpin(int x, int y, int radius, float force)
{
	float error = (float)-radius;
	float x0 = (float)radius - 0.5f;
	float y0 = 0.5f;
	float cx = (float)x - 0.5f;
	float cy = (float)y - 0.5f;

	while (x0 >= y0) {
		SetForce((int)(cx + x0), (int)(cy + y0), -y0, x0, force);
		SetForce((int)(cx + y0), (int)(cy + x0), -x0, y0, force);

		if (x0 != 0.0f) {
			SetForce((int)(cx - x0), (int)(cy + y0), -y0, -x0, force);
			SetForce((int)(cx + y0), (int)(cy - x0),  x0,  y0, force);
		}

		if (y0 != 0.0f) {
			SetForce((int)(cx + x0), (int)(cy - y0),  y0,  x0, force);
			SetForce((int)(cx - y0), (int)(cy + x0), -x0, -y0, force);
		}

		if (x0 != 0.0f && y0 != 0.0f) {
			SetForce((int)(cx - x0), (int)(cy - y0), y0, -x0, force);
			SetForce((int)(cx - y0), (int)(cy - x0), x0, -y0, force);
		}

		error += y0;
		++y0;
		error += y0;

		if (error >= 0) {
			--x0;
			error -= x0;
			error -= x0;
		}
	}
}

void mglParticleDynamics2D::SetExpansion(int x, int y, int radius, float force)
{
	float error = (float)-radius;
	float x0 = (float)radius - 0.5f;
	float y0 = 0.5f;
	float cx = (float)x - 0.5f;
	float cy = (float)y - 0.5f;

	while (x0 >= y0) {
		SetForce((int)(cx + x0), (int)(cy + y0), x0, y0, force);
		SetForce((int)(cx + y0), (int)(cy + x0), y0, x0, force);

		if (x0 != 0.0f) {
			SetForce((int)(cx - x0), (int)(cy + y0), -x0,  y0, force);
			SetForce((int)(cx + y0), (int)(cy - x0),  y0, -x0, force);
		}

		if (y0 != 0.0f) {
			SetForce((int)(cx + x0), (int)(cy - y0),  x0, -y0, force);
			SetForce((int)(cx - y0), (int)(cy + x0), -y0,  x0, force);
		}

		if (x0 != 0.0f && y0 != 0.0f) {
			SetForce((int)(cx - x0), (int)(cy - y0), -x0, -y0, force);
			SetForce((int)(cx - y0), (int)(cy - x0), -y0, -x0, force);
		}

		error += y0;
		++y0;
		error += y0;

		if (error >= 0.0f) {
			--x0;
			error -= x0;
			error -= x0;
		}
	}
}

void mglParticleDynamics2D::SetVortex(int x, int y, int scale, int quadrant_count, int orientation, float force)
{
	int c0 = 0;
	int c1 = 1;
	int quad = orientation & 3;

	for (int i = 0; i < quadrant_count; ++i) {

		int t = c1;
		c1 = c1 + c0;
		c0 = t;

		float error = (float)-c0;
		float x0 = (float)c0 - 0.5f;
		float y0 = 0.5f;
		float cx = (float)x - 0.5f;
		float cy = (float)y - 0.5f;

		while (x0 >= y0) {
			if (quad == 3) {
				SetForce((int)(cx + x0), (int)(cy + y0), -y0, x0, force);
				SetForce((int)(cx + y0), (int)(cy + x0), -x0, y0, force);
			}

			if (x0 != 0.0f) {
				if (quad == 2) SetForce((int)(cx - x0), (int)(cy + y0), -y0, -x0, force);
				if (quad == 0) SetForce((int)(cx + y0), (int)(cy - x0),  x0,  y0, force);
			}

			if (y0 != 0.0f) {
				if (quad == 0) SetForce((int)(cx + x0), (int)(cy - y0),  y0,  x0, force);
				if (quad == 2) SetForce((int)(cx - y0), (int)(cy + x0), -x0, -y0, force);
			}

			if (quad == 1 && x0 != 0.0f && y0 != 0.0f) {
				SetForce((int)(cx - x0), (int)(cy - y0), y0, -x0, force);
				SetForce((int)(cx - y0), (int)(cy - x0), x0, -y0, force);
			}

			error += y0;
			++y0;
			error += y0;

			if (error >= 0.0f) {
				--x0;
				error -= x0;
				error -= x0;
			}
		}

		quad = (quad + 1) & 3;
		switch (quad) {
		case 0: x -= (c1 - c0) * scale; break;
		case 1: y += (c1 - c0) * scale; break;
		case 2: x += (c1 - c0) * scale; break;
		case 3: y -= (c1 - c0) * scale; break;
		}
	}
}

void mglParticleDynamics2D::SetDensity(int x, int y, float dens)
{
	m_dens_prev[XY(x+1, y+1)] = dens;
}

void mglParticleDynamics2D::AddForce(int x, int y, float vel_x, float vel_y)
{
	++x; ++y;
	m_u_prev[XY(x, y)] += vel_x;
	m_v_prev[XY(x, y)] += vel_y;
}

void mglParticleDynamics2D::AddSpin(int x, int y, int radius, float force)
{
	float error = (float)-radius;
	float x0 = (float)radius - 0.5f;
	float y0 = 0.5f;
	float cx = (float)x - 0.5f;
	float cy = (float)y - 0.5f;

	while (x0 >= y0) {
		AddForce((int)(cx + x0), (int)(cy + y0), -y0, x0, force);
		AddForce((int)(cx + y0), (int)(cy + x0), -x0, y0, force);

		if (x0 != 0.0f) {
			AddForce((int)(cx - x0), (int)(cy + y0), -y0, -x0, force);
			AddForce((int)(cx + y0), (int)(cy - x0),  x0,  y0, force);
		}

		if (y0 != 0.0f) {
			AddForce((int)(cx + x0), (int)(cy - y0),  y0,  x0, force);
			AddForce((int)(cx - y0), (int)(cy + x0), -x0, -y0, force);
		}

		if (x0 != 0.0f && y0 != 0.0f) {
			AddForce((int)(cx - x0), (int)(cy - y0), y0, -x0, force);
			AddForce((int)(cx - y0), (int)(cy - x0), x0, -y0, force);
		}

		error += y0;
		++y0;
		error += y0;

		if (error >= 0) {
			--x0;
			error -= x0;
			error -= x0;
		}
	}
}

void mglParticleDynamics2D::AddExpansion(int x, int y, int radius, float force)
{
	float error = (float)-radius;
	float x0 = (float)radius - 0.5f;
	float y0 = 0.5f;
	float cx = (float)x - 0.5f;
	float cy = (float)y - 0.5f;

	while (x0 >= y0) {
		AddForce((int)(cx + x0), (int)(cy + y0), x0, y0, force);
		AddForce((int)(cx + y0), (int)(cy + x0), y0, x0, force);

		if (x0 != 0.0f) {
			AddForce((int)(cx - x0), (int)(cy + y0), -x0,  y0, force);
			AddForce((int)(cx + y0), (int)(cy - x0),  y0, -x0, force);
		}

		if (y0 != 0.0f) {
			AddForce((int)(cx + x0), (int)(cy - y0),  x0, -y0, force);
			AddForce((int)(cx - y0), (int)(cy + x0), -y0,  x0, force);
		}

		if (x0 != 0.0f && y0 != 0.0f) {
			AddForce((int)(cx - x0), (int)(cy - y0), -x0, -y0, force);
			AddForce((int)(cx - y0), (int)(cy - x0), -y0, -x0, force);
		}

		error += y0;
		++y0;
		error += y0;

		if (error >= 0) {
			--x0;
			error -= x0;
			error -= x0;
		}
	}
}

void mglParticleDynamics2D::AddVortex(int x, int y, int scale, int quadrant_count, int orientation, float force)
{
	int c0 = 0;
	int c1 = 1;
	int quad = orientation & 3;

	for (int i = 0; i < quadrant_count; ++i) {

		int t = c1;
		c1 = c1 + c0;
		c0 = t;

		float error = (float)-c0;
		float x0 = (float)c0 - 0.5f;
		float y0 = 0.5f;
		float cx = (float)x - 0.5f;
		float cy = (float)y - 0.5f;

		while (x0 >= y0) {
			if (quad == 3) {
				AddForce((int)(cx + x0), (int)(cy + y0), -y0, x0, force);
				AddForce((int)(cx + y0), (int)(cy + x0), -x0, y0, force);
			}

			if (x0 != 0.0f) {
				if (quad == 2) AddForce((int)(cx - x0), (int)(cy + y0), -y0, -x0, force);
				if (quad == 0) AddForce((int)(cx + y0), (int)(cy - x0),  x0,  y0, force);
			}

			if (y0 != 0.0f) {
				if (quad == 0) AddForce((int)(cx + x0), (int)(cy - y0),  y0,  x0, force);
				if (quad == 2) AddForce((int)(cx - y0), (int)(cy + x0), -x0, -y0, force);
			}

			if (quad == 1 && x0 != 0.0f && y0 != 0.0f) {
				AddForce((int)(cx - x0), (int)(cy - y0), y0, -x0, force);
				AddForce((int)(cx - y0), (int)(cy - x0), x0, -y0, force);
			}

			error += y0;
			++y0;
			error += y0;

			if (error >= 0.0f) {
				--x0;
				error -= x0;
				error -= x0;
			}
		}

		quad = (quad + 1) & 3;
		switch (quad) {
		case 0: x -= (c1 - c0) * scale; break;
		case 1: y += (c1 - c0) * scale; break;
		case 2: x += (c1 - c0) * scale; break;
		case 3: y -= (c1 - c0) * scale; break;
		}
	}
}

void mglParticleDynamics2D::AddDensity(int x, int y, float dens)
{
	m_dens_prev[XY(x, y)] += dens;
}

float mglParticleDynamics2D::GetDensity(int x, int y) const
{
	return m_dens[XY(x+1, y+1)];
}

mmlVector<2> mglParticleDynamics2D::GetVelocity(int x, int y) const
{
	mmlVector<2> v;
	++x; ++y;
	v[0] = m_u[XY(x, y)];
	v[1] = m_v[XY(x, y)];
	return v;
}
