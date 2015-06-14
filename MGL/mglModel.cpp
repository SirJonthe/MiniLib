#include "../MTL/mtlParser.h"
#include "../MTL/mtlDirectory.h"
#include "mglModel.h"
#include "mglPlane.h"

struct FacetIndex { int v, t, n; };

bool mglModel::PreParseFile(const mtlString &p_fileContents)
{
	mtlParser parser;
	parser.SetBuffer(p_fileContents);
	mtlChars param;

	int v = 0, vt = 1, vn = 1, mtl = 0; // create a default texture coordinate and normal to avoid crashes when model does not contain any coords/normals
	mtlList<mtlString> materials;
	bool missingDefaultMaterial = true;

	while (!parser.IsEnd()) {
		param = parser.ReadWord();
		mtlChars line = parser.ReadLine();
		if (param.GetSize() == 0 || param.GetChars()[0] == '#') { continue; }
		if (param.Compare("f")) {
			if (missingDefaultMaterial) {
				++mtl;
				missingDefaultMaterial = false;
			}
		} else if (param.Compare("v")) {
			++v;
		} else if (param.Compare("vt")) {
			++vt;
		} else if (param.Compare("vn")) {
			++vn;
		} else if (param.Compare("usemtl")) {
			missingDefaultMaterial = false;
			mtlItem<mtlString> *node = materials.GetFirst();
			while (node != NULL) {
				if (node->GetItem().Compare(line)) {
					break;
				}
				node = node->GetNext();
			}
			if (node == NULL) {
				++mtl;
			}
		} else if (param.Compare("mtllib") || param.Compare("o")) {
			// do nothing
			// or maybe parse mtllib for newmtl to count materials...?
		} else {
			mtlString error;
			if (
					param.Compare("vp") || param.Compare("deg") || param.Compare("bmat") ||
					param.Compare("step") || param.Compare("cstype") || param.Compare("p") ||
					param.Compare("l") || param.Compare("curv") || param.Compare("curv2") ||
					param.Compare("surf") || param.Compare("parm") || param.Compare("trim") ||
					param.Compare("hole") || param.Compare("scrv") || param.Compare("sp") ||
					param.Compare("end") || param.Compare("con") || param.Compare("g") ||
					param.Compare("s") || param.Compare("mg") || param.Compare("bevel") ||
					param.Compare("c_interp") || param.Compare("d_interp") || param.Compare("lod") ||
					param.Compare("trace_obj") || param.Compare("ctech") || param.Compare("stech") ||
					param.Compare("maplib") || param.Compare("usemap") || param.Compare("shadow_obj")
					) {
				error.Copy("Unsupported param: ");
			} else {
				error.Copy("Unknown param: ");
			}
			error.Append(param);
			SetError(error);
			return false;
		}
	}
	m_vertices.Create(v);
	m_texCoords.Create(vt);
	m_normals.Create(vn);
	m_materials.Create(mtl);

	return true;
}

bool mglModel::ParseFile(const mtlString &p_fileContents)
{
	// NOTE: I think comments are handled improperly
		// Will not handle comments on same line as proper statements
	mtlParser parser;
	parser.SetBuffer(p_fileContents);
	mtlChars param;
	mglMaterialIndex *currentMaterial = NULL;
	int currentV = 0, currentT = 1, currentN = 1, currentM = -1;
	m_texCoords[0] = mmlVector<2>(0.0f, 0.0f); // default tex coord (for models that don't contain tex coords)
	m_normals[0] = mmlVector<3>(0.0f, 0.0f, 0.0f); // default normal (for models that don't contain normals)
	mtlString error;

	while (!parser.IsEnd()) {
		param = parser.ReadWord();
		mtlParser line(parser.ReadLine());
		
		if (param.GetSize() == 0 || param.GetChars()[0] == '#') { continue; }
		
		if (param.Compare("f")) {
			mtlList<FacetIndex> facet;
			while (!line.IsEnd()) {
				param = line.ReadWord();
				mtlList<mtlChars> facetPoint;
				param.SplitByChar(facetPoint, "/");
				mtlItem<mtlChars> *facetIndexStr = facetPoint.GetLast();
				FacetIndex facetIndex = { -1, 0, 0 };
				switch (facetPoint.GetSize()) {
					case 3:
						if (!facetIndexStr->GetItem().ToInt(facetIndex.n)) {
							SetError("Failed to convert to int");
							return false;
						}
						if (facetIndex.n < 0) {
							facetIndex.n += currentN - 1;
							if (facetIndex.n < 0) {
								SetError("Relative index out of range");
								return false;
							}
						}
						facetIndexStr = facetIndexStr->GetPrev();
					case 2:
						if (facetIndexStr->GetItem().GetSize() > 0) { // texture index *can* be empty in order to specify [vertex]//[normal]
							if (!facetIndexStr->GetItem().ToInt(facetIndex.t)) {
								error.Copy("Failed to convert to int");
								return false;
							}
							if (facetIndex.t < 0) {
								facetIndex.t += currentN - 1;
								if (facetIndex.t < 0) {
									SetError("Relative index out of range");
									return false;
								}
							}
						}
						facetIndexStr = facetIndexStr->GetPrev();
					case 1:
						if (!facetIndexStr->GetItem().ToInt(facetIndex.v)) {
							SetError("Failed to convert to int");
							return false;
						}
						if (facetIndex.v < 0) {
							facetIndex.v += currentN - 1;
							if (facetIndex.v < 0) {
								SetError("Relative index out of range");
								return false;
							}
						} else {
							facetIndex.v -= 1;
						}
						facetIndexStr = facetIndexStr->GetPrev();
						break;
					default:
						SetError("Facet format error");
						return false;
				}
				facet.AddLast(facetIndex);
			}
			if (facet.GetSize() >= 3) {
				if (currentMaterial == NULL) { // for models that start declaring facets before specifying material
					++currentM;
					currentMaterial = &m_materials[0];
				}
				mtlItem<FacetIndex> *i = facet.GetFirst()->GetNext();
				mtlItem<FacetIndex> *j = i->GetNext();
				while (j != NULL) {
					mglFacet f;
					f.v1 = facet.GetFirst()->GetItem().v;
					f.t1 = facet.GetFirst()->GetItem().t;
					f.n1 = facet.GetFirst()->GetItem().n;
					f.v2 = i->GetItem().v;
					f.t2 = i->GetItem().t;
					f.n2 = i->GetItem().n;
					f.v3 = j->GetItem().v;
					f.t3 = j->GetItem().t;
					f.n3 = j->GetItem().n;
					currentMaterial->m_facets.AddLast(f);
					i = i->GetNext();
					j = j->GetNext();
				}
			} else {
				SetError("Incomplete facet");
				return false;
			}
		} else if (param.Compare("v")) {
			int n = 0;
			while (!line.IsEnd()) {
				param = line.ReadWord();
				if (n > 4) {
					SetError("Too many arguments in v");
					return false;
				} else if (n < 3) {
					if (!param.ToFloat(m_vertices[currentV][n])) {
						SetError("Failed to convert to float");
						return false;
					}
				}
				++n;
			}
			if (n < 3) {
				SetError("Too few arguments in v");
				return false;
			}
			++currentV;
		} else if (param.Compare("vt")) {
			int n = 0;
			while (!line.IsEnd()) {
				param = line.ReadWord();
				if (n > 3) {
					SetError("Too many arguments in vt");
					return false;
				} else if (n < 2) {
					if (!param.ToFloat(m_texCoords[currentT][n])) {
						SetError("Failed to convert to float");
						return false;
					}
				}
				++n;
			}
			if (n < 2) {
				SetError("Too few arguments in vt");
				return false;
			}
			++currentT;
		} else if (param.Compare("vn")) {
			int n = 0;
			while (!line.IsEnd()) {
				param = line.ReadWord();
				if (n > 3) {
					SetError("Too many arguments in vn");
					return false;
				} else {
					if (!param.ToFloat(m_normals[currentN][n])) {
						SetError("Failed to convert to float");
						return false;
					}
				}
				++n;
			}
			if (n < 3) {
				SetError("Too few arguments in vn");
				return false;
			}
			++currentN;
		} else if (param.Compare("usemtl")) {
			param = line.ReadLine();
			int m = 0;
			for (; m < m_materials.GetSize(); ++m) {
				if (m_materials[m].m_properties.m_name.Compare(param)) {
					currentMaterial = &m_materials[m];
					break;
				}
			}
			if (m == m_materials.GetSize()) {
				SetError("Undefined material");
				return false;
			}
		} else if (param.Compare("mtllib")) {
			param = line.ReadLine();
			mtlString mtlContents;
			if (!mtlParser::BufferFile(param, mtlContents)) {
				SetError("Material file unreadable");
				return false;
			}
			mtlParser mtlFile(mtlContents);
			int newmtl = -1;
			while (!mtlFile.IsEnd()) {
				mtlParser mtlLine(mtlFile.ReadLine());
				while (!mtlLine.IsEnd()) {
					param = mtlLine.ReadWord();
					if (param.Compare("newmtl")) {
						param = mtlLine.ReadLine();
						m_materials[++currentM].m_properties.m_name.Copy(param);
						++newmtl;
					} else if (param.Compare("Kd")) {
						int n = 0;
						while (!mtlLine.IsEnd()) {
							param = mtlLine.ReadWord();
							if (n >= 3) {
								SetError("Too many arguments in Kd");
								return false;
							} else if (n < 3) {
								if (!param.ToFloat(m_materials[currentM].m_properties.m_diffuseColor[n])) {
									SetError("Failed to convert to float");
									return false;
								}
							}
							++n;
						}
						if (n < 3) {
							SetError("Too few arguments in Kd");
							return false;
						}
					} else if (param.Compare("map_Kd")) {
						if (newmtl < 0) {
							SetError("Missing material name");
							return false;
						}
						mtlChars path = mtlLine.ReadLine();
						m_materials[currentM].m_properties.m_diffuseMap = mtlAsset<mglTexture>::Load(path);
						if (m_materials[currentM].m_properties.m_diffuseMap.IsNull()) {
							SetError("Diffuse map load failed");
							return false;
						}
					} else if (param.Compare("bump")) {
						if (newmtl < 0) {
							SetError("Missing material name");
							return false;
						}
						mtlChars path = mtlLine.ReadLine();
						m_materials[currentM].m_properties.m_normalMap = mtlAsset<mglTexture>::Load(path);
						if (m_materials[currentM].m_properties.m_normalMap.IsNull()) {
							SetError("Normal map load failed");
							return false;
						}
					} else {
						mtlString error;
						if (param.Compare("Ka") || param.Compare("Kd") || param.Compare("Ks") ||
							param.Compare("Tf") || param.Compare("illum") || param.Compare("d") ||
							param.Compare("Ns") || param.Compare("sharpness") || param.Compare("Ni") ||
							param.Compare("map_Ka") || param.Compare("map_Ks") || param.Compare("map_Ns") ||
							param.Compare("map_d") || param.Compare("disp") || param.Compare("decal")) {
							error.Copy("Unsupported .mtl param: ");
						} else {
							error.Copy("Unknown .mtl param: ");
						}
						error.Append(param);
						SetError(error);
						return false;
					}
				}
			}
			mtlFile.SetBuffer(mtlContents);
		} else if (param.Compare("o")) {
			m_name.Copy(line.ReadLine());
		}
	}
	return true;
}

void mglModel::CalculateBounds( void )
{
	if (m_vertices.GetSize() == 0 || m_materials.GetSize() == 0) {
		m_maxBounds = m_minBounds = mmlVector<3>(0.0f, 0.0f, 0.0f);
	} else {
		m_maxBounds = m_minBounds = m_vertices[0];
		for (int i = 1; i < m_vertices.GetSize(); ++i) {
			m_maxBounds = mmlMax(m_maxBounds, m_vertices[i]);
			m_minBounds = mmlMin(m_minBounds, m_vertices[i]);
		}
	}
}

void mglModel::CalculateFacetNormals( void )
{
	for (int i = 0; i < m_materials.GetSize(); ++i) {
		mtlItem<mglFacet> *facet = m_materials[i].m_facets.GetFirst();
		while (facet != NULL) {
			facet->GetItem().normal = mmlSurfaceNormal(m_vertices[facet->GetItem().v1], m_vertices[facet->GetItem().v2], m_vertices[facet->GetItem().v3]);
			facet = facet->GetNext();
		}
	}
}


void mglModel::CreateEdgeListAndMainFacetList( void )
{
	int facets = 0;
	for (int i = 0; i < m_materials.GetSize(); ++i) {
		facets += m_materials[i].GetFacetCount();
	}
	
	m_facets.Create(facets);
	m_edges.Create(facets * 3);

	int f = 0;
	int e = 0;
	for (int m = 0; m < m_materials.GetSize(); ++m) {
		mtlItem<mglFacet> *facet = m_materials[m].m_facets.GetFirst();
		while (facet != NULL) {

			facet->GetItem().e1 = e;
			facet->GetItem().e2 = e+1;
			facet->GetItem().e3 = e+2;

			m_facets[f] = facet->GetItem();

			m_edges[e].v1 = m_facets[f].v1;
			m_edges[e].v2 = m_facets[f].v3;
			m_edges[e].t1 = m_facets[f].t1;
			m_edges[e].t2 = m_facets[f].t3;
			m_edges[e].n1 = m_facets[f].n1;
			m_edges[e].n2 = m_facets[f].n3;

			m_edges[e+1].v1 = m_facets[f].v2;
			m_edges[e+1].v2 = m_facets[f].v1;
			m_edges[e+1].t1 = m_facets[f].t2;
			m_edges[e+1].t2 = m_facets[f].t1;
			m_edges[e+1].n1 = m_facets[f].n2;
			m_edges[e+1].n2 = m_facets[f].n1;

			m_edges[e+2].v1 = m_facets[f].v3;
			m_edges[e+2].v2 = m_facets[f].v2;
			m_edges[e+2].t1 = m_facets[f].t3;
			m_edges[e+2].t2 = m_facets[f].t2;
			m_edges[e+2].n1 = m_facets[f].n3;
			m_edges[e+2].n2 = m_facets[f].n2;

			++f;
			e += 3;
			facet = facet->GetNext();
		}
	}
}

void mglModel::CheckIfClosed( void )
{	
	// Every edge in every face must equal some other edge in some other face
	// If not then that means that the edge is just 'dangling' free without a connected face
	
	mtlList<mglFacetEdge> edgeList;
	for (int i = 0; i < m_edges.GetSize(); ++i) {
		edgeList.AddLast(m_edges[i]);
	}
	while (edgeList.GetSize() > 0) {
		mtlItem<mglFacetEdge> *front = edgeList.GetFirst();
		mtlItem<mglFacetEdge> *cmp = front->GetNext();
		while (cmp != NULL) {
			// does not treat the following situations properly
			// 1) edges that are shared between 3 or more facets
			// 2) facets that are specified multiple times with different materials
			// 3) facets that are connected to reverse facing facets
			if (cmp->GetItem().v1 == front->GetItem().v2 && cmp->GetItem().v2 == front->GetItem().v1) {
				cmp->Remove();
				break;
			}
			cmp = cmp->GetNext();
		}
		if (cmp != NULL) {
			front->Remove();
		} else {
			break;
		}
	}
	m_closed = (edgeList.GetSize() == 0);
}

void mglModel::CalculateArea( void )
{
	m_area = 0.0f;
	for (int i = 0; i < GetFacetCount(); ++i) {
		m_area += GetFacetArea(m_facets[i].v1, m_facets[i].v2, m_facets[i].v3);
	}
}

float mglModel::CalculateVolume(int v1, int v2, int v3) const
{
	const mmlVector<3> p1 = m_vertices[v1];
	const mmlVector<3> p2 = m_vertices[v2];
	const mmlVector<3> p3 = m_vertices[v3];
	float v321 = p3[0] * p2[1] * p1[2];
    float v231 = p2[0] * p3[1] * p1[2];
    float v312 = p3[0] * p1[1] * p2[2];
    float v132 = p1[0] * p3[1] * p2[2];
    float v213 = p2[0] * p1[1] * p3[2];
    float v123 = p1[0] * p2[1] * p3[2];
    return (1.0f/6.0f) * (-v321 + v231 + v312 - v132 - v213 + v123);
}

void mglModel::CalculateVolume( void )
{
	if (m_closed) {
		m_volume = 0.0f;
		for (int i = 0; i < GetFacetCount(); ++i) {
			m_volume += CalculateVolume(m_facets[i].v1, m_facets[i].v2, m_facets[i].v3);
		}
	} else {
		m_volume = -1.0f;
	}
}

void mglModel::CalculateMetadata( void )
{
	CalculateBounds();
	CalculateFacetNormals();
	CreateEdgeListAndMainFacetList();
	CheckIfClosed();
	CalculateArea();
	CalculateVolume();
}

void mglModel::Free( void )
{
	m_name.Copy("");
	m_vertices.Free();
	m_texCoords.Free();
	m_normals.Free();
	m_materials.Free();
	m_edges.Free();
	m_area = 0.0f;
	m_volume = -1.0f;
	m_facets.Free();
	m_closed = false;
	SetError("");
}

bool mglModel::Load(const mtlDirectory &p_filename)
{
	Free();
	
	if (!p_filename.GetExtension().Compare("obj")) {
		SetError("Not an .obj file");
		return false;
	}
	
	mtlString fileContents;
	if (!mtlParser::BufferFile(p_filename.GetDirectory(), fileContents)) {
		SetError("File unreadable");
		return false;
	}
	
	if (!PreParseFile(fileContents) || !ParseFile(fileContents)) {
		return false;
	}
	
	CalculateMetadata();
	return true;
}

mmlVector<3> mglModel::GetFacetNormal(int v1, int v2, int v3) const
{
	return mmlSurfaceNormal(m_vertices[v1], m_vertices[v2], m_vertices[v3]);
}

float mglModel::GetFacetArea(int v1, int v2, int v3) const
{
	const float a = (m_vertices[v2] - m_vertices[v1]).Len();
	const float b = (m_vertices[v3] - m_vertices[v2]).Len();
	const float c = (m_vertices[v3] - m_vertices[v1]).Len();
	const float s = (a + b + c) * 0.5f;
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

/*void mglModel::Render(const mglRasterizer *p_raster, const mglTransform &p_objectTransform, const mglTransform &p_cameraTransform) const
{
	// rasterizer simply renders triangles, nothing more
		// removes the responsibility from the rasterizer to know how to render a model (becomes problematic when there are many model structures)
	// allows model to make some quality settings (texture samling, lighting model) that only affect the model currently rendered
}*/

mtlItem<mglStaticModel::Triangle> *mglStaticModel::FindBestSplittingTriangle(mglStaticModel::Node *node)
{
	return node->triangles.GetFirst();
}

void mglStaticModel::SplitGeometryRecursively(mglStaticModel::Node *node, int depth)
{
	if (node->triangles.GetSize() <= 1) { return; } // end of the line
	m_depth = mmlMax2(m_depth, depth);
	node->front = new Node(node);
	node->back = new Node(node);
	mtlItem<Triangle> *splitTri = FindBestSplittingTriangle(node);
	node->plane = mglPlane(mmlVector<3>::Cast(&splitTri->GetItem().a), splitTri->GetItem().normal);
	const mglPlane backPlane(node->plane.GetPosition(), -node->plane.GetNormal());
	mtlItem<Triangle> *tri = node->triangles.GetFirst();
	while (tri != NULL) {
		switch (node->plane.DetermineClipping(mmlVector<3>::Cast(&tri->GetItem().a), mmlVector<3>::Cast(&tri->GetItem().b), mmlVector<3>::Cast(&tri->GetItem().c)))
		{
		case mglCoinciding:
			tri = tri->GetNext();
			break;
		case mglBehind:
			node->back->triangles.AddLast(tri->GetItem());
			tri = node->triangles.Remove(tri);
			break;
		case mglInFront:
			node->front->triangles.AddLast(tri->GetItem());
			tri = node->triangles.Remove(tri);
			break;
		case mglClipping:
			mmlVector<5> out[4];
			// get front section
			switch (node->plane.Clip(tri->GetItem().a, tri->GetItem().b, tri->GetItem().c, out)) {
			case 4: {
				Triangle t = { out[0], out[2], out[3], tri->GetItem().normal, tri->GetItem().material };
				node->front->triangles.AddLast(t);
			}
			case 3: {
				Triangle t = { out[0], out[1], out[2], tri->GetItem().normal, tri->GetItem().material };
				node->front->triangles.AddLast(t);
			}
			}

			// get back section
			switch (backPlane.Clip(tri->GetItem().a, tri->GetItem().b, tri->GetItem().c, out)) {
			case 4: {
				Triangle t = { out[0], out[2], out[3], tri->GetItem().normal, tri->GetItem().material };
				node->back->triangles.AddLast(t);
			}
			case 3: {
				Triangle t = { out[0], out[1], out[2], tri->GetItem().normal, tri->GetItem().material };
				node->back->triangles.AddLast(t);
			}
			}

			tri = node->triangles.Remove(tri);
			break;
		}
	}
	if (node->front->triangles.GetSize() > 0) {
		SplitGeometryRecursively(node->front, depth+1);
	} else {
		delete node->front;
		node->front = NULL;
	}
	if (node->back->triangles.GetSize() > 0) {
		SplitGeometryRecursively(node->back, depth+1);
	} else {
		delete node->back;
		node->back = NULL;
	}
}

void mglStaticModel::GenerateBSP( void )
{
	// add all polygons to the root node
	// pick a random polygon to split by
	// sort polygons in "behind splitting" or "in front of splitting" in respective nodes
	// four cases:
		// wholly in front of splitting plane - sort to list "in front of spltting plane"
		// wholly behind splitting plane - sort to list "behind splitting plane"
		// intersecting splitting plane - split polygon and add one polygon to front list and one to back list (determine which one)
		// coinciding with splitting plane - add at the list of polygons at splitting plane node
		// determine if a polygon is in front of a hyperplane
			// split polygon by the hyperplane
			// now you have one or two polygons
			// dot(hyperplane.normal, splitTri[n].centroid)
			// if dot < 0, then behind, dot > 0 then in front
	// traverse recursively to next node, pick a random polygon to split by, and split all other polygons by that splitter, sort "behind" and "front"
	// continue until you are satisfied or until there is only one polygon in a node

	// OPTIMIZATION:
		// stop splitting when polygon set is guaranteed not to overlap (including backface culling)

	if (GetVertexCount() <= 0) { return; }
	m_root = new Node(NULL);
	for (int i = 0; i < GetMaterialCount(); ++i) {
		const mtlItem<mglFacet> *facet = GetMaterial(i).GetFacets();
		while (facet != NULL) {
			const mmlVector<3> av = GetVertex(facet->GetItem().v1);
			const mmlVector<3> bv = GetVertex(facet->GetItem().v2);
			const mmlVector<3> cv = GetVertex(facet->GetItem().v3);
			const mmlVector<2> at = GetTexCoord(facet->GetItem().t1);
			const mmlVector<2> bt = GetTexCoord(facet->GetItem().t2);
			const mmlVector<2> ct = GetTexCoord(facet->GetItem().t3);
			Triangle tri = {
				mmlVector<5>(av[0], av[1], av[2], at[0], at[1]),
				mmlVector<5>(bv[0], bv[1], bv[2], bt[0], bt[1]),
				mmlVector<5>(cv[0], cv[1], cv[2], ct[0], ct[1]),
				facet->GetItem().normal,
				&GetMaterial(i).GetProperties()
			};
			m_root->triangles.AddLast(tri);
			facet = facet->GetNext();
		}
	}
	if (m_root->triangles.GetSize() > 0) {
		SplitGeometryRecursively(m_root, 1);
	} else {
		delete m_root;
		m_root = NULL;
	}
	// BalanceBSP(); // don't know how to do thing, but it might be necessary for performance
}

void mglStaticModel::Free( void )
{
	mglModel::Free();
	delete m_root;
	m_root = NULL;
	m_depth = 0;
}

bool mglStaticModel::Load(const mtlDirectory &p_filename)
{
	if (!mglModel::Load(p_filename)) {
		return false;
	}
	GenerateBSP();
	return true;
}

const mglStaticModel::Node *mglStaticModel::GetBinarySpaceTree( void ) const
{
	return m_root;
}
