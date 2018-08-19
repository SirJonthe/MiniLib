#ifndef MGL_PARTICLEDYNAMICS2D_H_INCLUDED
#define MGL_PARTICLEDYNAMICS2D_H_INCLUDED

#include "../MTL/mtlArray.h"
#include "../MML/mmlVector.h"

class mglParticleDynamics2D
{
public:
	enum EdgeMode
	{
		EdgeMode_Free,     // fluid is allowed to flow outside of buffer edges
		EdgeMode_Confined, // fluid behaves like it is inside a box (bounded by buffer edges)
		EdgeMode_Wrap      // fluid that goes outside of an edge reenters at opposite edge
	};

private:
	mtlArray<float> m_dens; // I can implement this as a templated vector
	mtlArray<float> m_u;
	mtlArray<float> m_v;
	mtlArray<float> m_dens_prev;
	mtlArray<float> m_u_prev;
	mtlArray<float> m_v_prev;
	int             m_total_size;
	int             m_width;
	int             m_height;
	float           m_diffusion;
	float           m_viscosity;
	EdgeMode        m_edge_mode;

private:
	void AddSource(float *x, float *s, float delta_time) const;
	void SetBounds(int b, float *x) const;
	void LinSolve(int b, float *x, const float *x0, float a, float c) const;
	void Diffuse(int b, float *x, const float *x0, float diff, float delta_time) const;
	void Advect(int b, float *d, const float *d0, const float *u, const float *v, float delta_time) const;
	void Project(float *u, float *v, float *p, float *div) const;
	void UpdateVelocity(float *u, float *v, float *u0, float *v0, float visc, float delta_time) const;
	void UpdateDensity(float *x, float *x0, float *u, float *v, float diff, float delta_time) const;
	void SetForce(int x, int y, float x0, float y0, float force);
	void AddForce(int x, int y, float x0, float y0, float force);

public:
	mglParticleDynamics2D( void ) : m_total_size(0), m_width(0), m_height(0), m_diffusion(0.0f), m_viscosity(0.0f), m_edge_mode(EdgeMode_Confined) {}

	void Reset( void );

	void Create(int width, int height);
	void Create(int N);
	void Destroy( void );

	void Update(float delta_time = 0.1f);

	int GetWidth( void ) const  { return m_width; }
	int GetHeight( void ) const { return m_height; }

	void SetForce(int x, int y, float vel_x, float vel_y);
	void SetSpin(int x, int y, int radius, float force);
	void SetExpansion(int x, int y, int radius, float force);
	void SetVortex(int x, int y, int scale, int quadrant_count, int orientation, float force);
	void SetDensity(int x, int y, float dens);

	void AddForce(int x, int y, float vel_x, float vel_y);
	void AddSpin(int x, int y, int radius, float force);
	void AddExpansion(int x, int y, int radius, float force);
	void AddVortex(int x, int y, int scale, int quadrant_count, int orientation, float force);
	void AddDensity(int x, int y, float dens);

	float        GetDensity(int x, int y) const;
	mmlVector<2> GetVelocity(int x, int y) const;

	float GetViscosity( void ) const { return m_viscosity; }
	void  SetViscosity(float visc)   { m_viscosity = visc; }

	float GetDiffusion( void ) const { return m_diffusion; }
	void  SetDiffusion(float diff)   { m_diffusion = diff; }

	EdgeMode GetEdgeMode( void ) const       { return m_edge_mode; }
	void     SetEdgeMode(EdgeMode edge_mode) { m_edge_mode = edge_mode; }
};

#endif
