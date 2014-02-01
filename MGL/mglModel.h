#ifndef MGL_MODEL_H_INCLUDED__
#define MGL_MODEL_H_INCLUDED__

#include "../MTL/mtlString.h"
#include "../MTL/mtlArray.h"
#include "../MTL/mtlList.h"
#include "../MTL/mtlAsset.h"
#include "../MML/mmlVector.h"
#include "mglTexture.h"
#include "mglTransform.h"
#include "mglPlane.h"

class mglModel;
class mglRasterizer;

// MAYBE:
	// separate the mesh and materials
	// have a model class that loads both data types

struct mglFacet
{
	int				v1, v2, v3;
	int				t1, t2, t3;
	int				n1, n2, n3;
	mmlVector<3>	normal;
};

class mglMaterial
{
	friend class mglModel;
private:
	const mglModel			*m_parent;
	mtlString				m_name;
	mtlAsset< mglTexture >	m_texture;
	mtlList< mglFacet >		m_facets;
	mmlVector<3>			m_diffuseColor;
public: 
							mglMaterial( void ) : m_parent(NULL), m_texture(""), m_diffuseColor(1.0f, 1.0f, 1.0f) {}
	const mglTexture		*GetTexture( void ) const { return m_texture.GetAsset(); }
	const mtlNode<mglFacet>	*GetFacets( void ) const { return m_facets.GetFront(); }
	int						GetFacetCount( void ) const { return m_facets.GetSize(); }
	const mtlString			&GetName( void ) const { return m_name; }
	const mglModel			*GetParentModel( void ) const { return m_parent; }
	const mmlVector<3>		&GetDiffuseColor( void ) const { return m_diffuseColor; }
};

class mglModel : public mtlAssetInterface
{
private:
	struct Edge
	{
		int v1, v2;
		int t1, t2;
		int n1, n2;
		//int f1, f2;
	};
private:
	mtlString					m_name;
	mtlArray< mmlVector<3> >	m_vertices;
	mtlArray< mmlVector<2> >	m_texCoords;
	mtlArray< mmlVector<3> >	m_normals;
	mtlArray< mglMaterial >		m_materials;
	mtlArray< Edge >			m_edges;
	mmlVector<3>				m_minBounds;
	mmlVector<3>				m_maxBounds;
	float						m_area;
	float						m_volume;
	int							m_facets;
	bool						m_closed;
private:
				mglModel(const mglModel&) {}
	mglModel	&operator=(const mglModel&) { return *this; }
private:
	bool	PreParseFile(const mtlString &p_fileContents);
	bool	ParseFile(const mtlString &p_fileContents);
	void	CountFacets( void );
	void	CreateEdgeList( void );
	void	CheckIfClosed( void );
	void	CalculateArea( void );
	float	CalculateVolume(int v1, int v2, int v3) const;
	void	CalculateVolume( void );
	void	CalculateFacetNormals( void );
	void	CalculateBounds( void );
public:
						mglModel( void ) {}
	virtual				~mglModel( void ) {}
	virtual void		Free( void );
	virtual bool		Load(const mtlDirectory &p_filename);
	float				GetArea( void ) const { return m_area; }
	float				GetVolume( void ) const { return m_volume; }
	bool				IsClosed( void ) const { return m_closed; }
	const mtlString		&GetName( void ) const { return m_name; }
	const mmlVector<3>	&GetVertex(int i) const { return m_vertices[i]; }
	int					GetVertexCount( void ) const { return m_vertices.GetSize(); }
	const mmlVector<2>	&GetTexCoord(int i) const { return m_texCoords[i]; }
	int					GetTexCoordCount( void ) const { return m_texCoords.GetSize(); }
	const mmlVector<3>	&GetNormal(int i) const { return m_normals[i]; }
	int					GetNormalCount( void ) const { return m_normals.GetSize(); }
	const mglMaterial	&GetMaterial(int i) const { return m_materials[i]; }
	int					GetMaterialCount( void ) const { return m_materials.GetSize(); }
	int					GetFacetCount( void ) const { return m_facets; }
	mmlVector<3>		GetFacetNormal(int v1, int v2, int v3) const;
	float				GetFacetArea(int v1, int v2, int v3) const;
	const mmlVector<3>	&GetMinBounds( void ) const { return m_minBounds; }
	const mmlVector<3>	&GetMaxBounds( void ) const { return m_maxBounds; }
	//virtual void		Render(const mglRasterizer *p_raster, const mglTransform &p_objectTransform, const mglTransform &p_cameraTransform) const;
	//bool				Debug_SaveData(const mtlDirectory &p_filename) const;
};

class mglStaticModel : public mglModel
{
public:
	struct Poly
	{
		mmlVector<5>		a, b, c;
		mmlVector<3>		normal;
		const mglMaterial	*material;
	};
	struct Node
	{
		mtlList< Poly >	polys;
		mglPlane		plane;
		Node			*parent;
		Node			*front;
		Node			*back;
		explicit Node(Node *p_parent) : polys(), parent(p_parent), front(NULL), back(NULL) {}
		~Node( void )
		{
			delete front;
			delete back;
		}
	};
private:
	Node	*m_root;
	int		m_depth;
private:
	mtlNode<Poly>	*FindBestSplittingPolygon(Node *node);
	void			SplitGeometryRecursively(Node *node, int depth);
	void			GenerateBSP( void );
public:
								mglStaticModel( void ) : mglModel(), m_root(NULL) {}
								~mglStaticModel( void ) { delete m_root; }
	void						Free( void );
	bool						Load(const mtlDirectory &p_filename);
	const mglStaticModel::Node	*GetBinarySpaceTree( void ) const;
	int							GetTreeDepth( void ) const { return m_depth; }
};

#endif
