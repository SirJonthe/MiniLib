#ifndef MGL_GEOMETRY_H_INCLUDED__
#define MGL_GEOMETRY_H_INCLUDED__

#include "../MTL/mtlArray.h"
#include "../MML/mmlVector.h"
#include "mglIndex.h"

template < int n >
class mglGeometry
{
public:
	enum Format
	{
		Triangles,
		TriangleStrip,
		TriangleIndex
	};

private:
	mtlArray< mmlVector<n> >	m_vert;
	mtlArray< mglIndex<int,3> >	m_index;
	Format						m_format;
};

typedef mglGeometry<2> mglGeometry2;
typedef mglGeometry<3> mglGeometry3;

#endif // MGL_GEOMETRY_H_INCLUDED__
