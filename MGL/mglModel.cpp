#include "../MTL/mtlParser.h"
#include "../MTL/mtlDirectory.h"
#include "mglModel.h"
#include "mglPlane.h"

struct FacetIndex { int v, t, n; };

bool mglModel::PreParseFile(const mtlString &p_fileContents)
{
	mtlParser parser;
	parser.SetBuffer(p_fileContents);
	mtlSubstring param;

	int v = 0, vt = 1, vn = 1, mtl = 0; // create a default texture coordinate and normal to avoid crashes when model does not contain any coords/normals
	mtlList<mtlString> materials;
	bool missingDefaultMaterial = true;

	while (!parser.IsEndOfFile()) {
		param = parser.ReadWord();
		mtlSubstring line = parser.ReadLine();
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
			mtlNode<mtlString> *node = materials.GetFront();
			while (node != NULL) {
				if (node->value.Compare(line)) {
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
				m_error.Copy("Unsupported param: ");
			} else {
				m_error.Copy("Unknown param: ");
			}
			m_error.Append(param);
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
	mtlSubstring param;
	mglMaterial *currentMaterial = NULL;
	int currentV = 0, currentT = 1, currentN = 1, currentM = -1;
	m_texCoords[0] = mmlVector<2>(0.0f, 0.0f); // default tex coord (for models that don't contain tex coords)
	m_normals[0] = mmlVector<3>(0.0f, 0.0f, 0.0f); // default normal (for models that don't contain normals)

	while (!parser.IsEndOfFile()) {
		param = parser.ReadWord();
		mtlParser line(parser.ReadLine());
		
		if (param.GetSize() == 0 || param.GetChars()[0] == '#') { continue; }
		
		if (param.Compare("f")) {
			mtlList<FacetIndex> facet;
			while (!line.IsEndOfFile()) {
				param = line.ReadWord();
				mtlList<mtlSubstring> facetPoint;
				param.SplitByChar(facetPoint, "/");
				mtlNode<mtlSubstring> *facetIndexStr = facetPoint.GetBack();
				FacetIndex facetIndex = { -1, 0, 0 };
				switch (facetPoint.GetSize()) {
					case 3:
						if (!facetIndexStr->value.ToInt(facetIndex.n)) {
							m_error.Copy("Failed to convert to int");
							return false;
						}
						if (facetIndex.n < 0) {
							facetIndex.n += currentN - 1;
							if (facetIndex.n < 0) {
								m_error.Copy("Relative index out of range");
								return false;
							}
						}
						facetIndexStr = facetIndexStr->GetPrev();
					case 2:
						if (facetIndexStr->value.GetSize() > 0) { // texture index *can* be empty in order to specify [vertex]//[normal]
							if (!facetIndexStr->value.ToInt(facetIndex.t)) {
								m_error.Copy("Failed to convert to int");
								return false;
							}
							if (facetIndex.t < 0) {
								facetIndex.t += currentN - 1;
								if (facetIndex.t < 0) {
									m_error.Copy("Relative index out of range");
									return false;
								}
							}
						}
						facetIndexStr = facetIndexStr->GetPrev();
					case 1:
						if (!facetIndexStr->value.ToInt(facetIndex.v)) {
							m_error.Copy("Failed to convert to int");
							return false;
						}
						if (facetIndex.v < 0) {
							facetIndex.v += currentN - 1;
							if (facetIndex.v < 0) {
								m_error.Copy("Relative index out of range");
								return false;
							}
						} else {
							facetIndex.v -= 1;
						}
						facetIndexStr = facetIndexStr->GetPrev();
						break;
					default:
						m_error.Copy("Facet format error");
						return false;
				}
				facet.PushBack(facetIndex);
			}
			if (facet.GetSize() >= 3) {
				if (currentMaterial == NULL) { // for models that start declaring facets before specifying material
					++currentM;
					currentMaterial = &m_materials[0];
				}
				mtlNode<FacetIndex> *i = facet.GetFront()->GetNext();
				mtlNode<FacetIndex> *j = i->GetNext();
				while (j != NULL) {
					mglFacet f;
					f.v1 = facet.GetFront()->value.v;
					f.t1 = facet.GetFront()->value.t;
					f.n1 = facet.GetFront()->value.n;
					f.v2 = i->value.v;
					f.t2 = i->value.t;
					f.n2 = i->value.n;
					f.v3 = j->value.v;
					f.t3 = j->value.t;
					f.n3 = j->value.n;
					currentMaterial->m_facets.PushBack(f);
					i = i->GetNext();
					j = j->GetNext();
				}
			} else {
				m_error.Copy("Incomplete facet");
				return false;
			}
		} else if (param.Compare("v")) {
			int n = 0;
			while (!line.IsEndOfFile()) {
				param = line.ReadWord();
				if (n > 4) {
					m_error.Copy("Too many arguments in v");
					return false;
				} else if (n < 3) {
					if (!param.ToFloat(m_vertices[currentV][n])) {
						m_error.Copy("Failed to convert to float");
						return false;
					}
				}
				++n;
			}
			if (n < 3) {
				m_error.Copy("Too few arguments in v");
				return false;
			}
			++currentV;
		} else if (param.Compare("vt")) {
			int n = 0;
			while (!line.IsEndOfFile()) {
				param = line.ReadWord();
				if (n > 3) {
					m_error.Copy("Too many arguments in vt");
					return false;
				} else if (n < 2) {
					if (!param.ToFloat(m_texCoords[currentT][n])) {
						m_error.Copy("Failed to convert to float");
						return false;
					}
				}
				++n;
			}
			if (n < 2) {
				m_error.Copy("Too few arguments in vt");
				return false;
			}
			++currentT;
		} else if (param.Compare("vn")) {
			int n = 0;
			while (!line.IsEndOfFile()) {
				param = line.ReadWord();
				if (n > 3) {
					m_error.Copy("Too many arguments in vn");
					return false;
				} else {
					if (!param.ToFloat(m_normals[currentN][n])) {
						m_error.Copy("Failed to convert to float");
						return false;
					}
				}
				++n;
			}
			if (n < 3) {
				m_error.Copy("Too few arguments in vn");
				return false;
			}
			++currentN;
		} else if (param.Compare("usemtl")) {
			param = line.ReadLine();
			int m = 0;
			for (; m < m_materials.GetSize(); ++m) {
				if (m_materials[m].m_name.Compare(param)) {
					currentMaterial = &m_materials[m];
					break;
				}
			}
			if (m == m_materials.GetSize()) {
				m_error.Copy("Undefined material");
				return false;
			}
		} else if (param.Compare("mtllib")) {
			param = line.ReadLine();
			mtlString mtlContents;
			if (!mtlParser::BufferFile(param, mtlContents)) {
				m_error.Copy("Material file unreadable");
				return false;
			}
			mtlParser mtlFile(mtlContents);
			int newmtl = -1;
			while (!mtlFile.IsEndOfFile()) {
				mtlParser mtlLine(mtlFile.ReadLine());
				while (!mtlLine.IsEndOfFile()) {
					param = mtlLine.ReadWord();
					if (param.Compare("newmtl")) {
						param = mtlLine.ReadLine();
						m_materials[++currentM].m_name.Copy(param);
						++newmtl;
					} else if (param.Compare("Kd")) {
						int n = 0;
						while (!mtlLine.IsEndOfFile()) {
							param = mtlLine.ReadWord();
							if (n >= 3) {
								m_error.Copy("Too many arguments in Kd");
								return false;
							} else if (n < 3) {
								if (!param.ToFloat(m_materials[currentM].m_diffuseColor[n])) {
									m_error.Copy("Failed to convert to float");
									return false;
								}
							}
							++n;
						}
						if (n < 3) {
							m_error.Copy("Too few arguments in Kd");
							return false;
						}
					} else if (param.Compare("map_Kd")) {
						if (newmtl < 0) {
							m_error.Copy("Missing material name");
							return false;
						}
						mtlChars path = mtlLine.ReadLine();
						if (!m_materials[currentM].m_texture.Load(path)) {
							m_error.Copy("Texture load failed: ");
							m_error.Append(m_materials[currentM].m_texture.GetError());
							return false;
						}
					} else {
						if (param.Compare("Ka") || param.Compare("Kd") || param.Compare("Ks") ||
							param.Compare("Tf") || param.Compare("illum") || param.Compare("d") ||
							param.Compare("Ns") || param.Compare("sharpness") || param.Compare("Ni") ||
							param.Compare("map_Ka") || param.Compare("map_Ks") || param.Compare("map_Ns") ||
							param.Compare("map_d") || param.Compare("disp") || param.Compare("decal") ||
							param.Compare("bump")) {
							m_error.Copy("Unsupported .mtl param: ");
						} else {
							m_error.Copy("Unknown .mtl param: ");
						}
						m_error.Append(param);
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

void mglModel::CountFacets( void )
{
	m_facets = 0;
	for (int i = 0; i < m_materials.GetSize(); ++i) {
		m_facets += m_materials[i].GetFacetCount();
	}
}

void mglModel::CreateEdgeList( void )
{
	m_edges.Create(m_facets * 3);
	int e = 0;
	for (int i = 0; i < m_materials.GetSize(); ++i) {
		const mtlNode<mglFacet> *f = m_materials[i].GetFacets();
		while (f != NULL) {
			m_edges[e].v1 = f->value.v1;
			m_edges[e].v2 = f->value.v3;
			m_edges[e].t1 = f->value.t1;
			m_edges[e].t2 = f->value.t3;
			m_edges[e].n1 = f->value.n1;
			m_edges[e].n2 = f->value.n3;
			
			m_edges[e+1].v1 = f->value.v2;
			m_edges[e+1].v2 = f->value.v1;
			m_edges[e+1].t1 = f->value.t2;
			m_edges[e+1].t2 = f->value.t1;
			m_edges[e+1].n1 = f->value.n2;
			m_edges[e+1].n2 = f->value.n1;
			
			m_edges[e+2].v1 = f->value.v3;
			m_edges[e+2].v2 = f->value.v2;
			m_edges[e+2].t1 = f->value.t3;
			m_edges[e+2].t2 = f->value.t2;
			m_edges[e+2].n1 = f->value.n3;
			m_edges[e+2].n2 = f->value.n2;
			
			f = f->GetNext();
			e += 3;
		}
	}
}

void mglModel::CheckIfClosed( void )
{	
	// Every edge in every face must equal some other edge in some other face
	// If not then that means that the edge is just 'dangling' free without a connected face
	
	mtlList<Edge> edgeList;
	for (int i = 0; i < m_edges.GetSize(); ++i) {
		edgeList.PushBack(m_edges[i]);
	}
	while (edgeList.GetSize() > 0) {
		mtlNode<Edge> *front = edgeList.GetFront();
		mtlNode<Edge> *cmp = front->GetNext();
		bool matchFound = false;
		while (cmp != NULL) {
			// does not treat the following situations properly
			// 1) edges that are shared between 3 or more facets
			// 2) facets that are specified multiple times with different materials
			// 3) facets that are connected to reverse facing facets
			if (cmp->value.v1 == front->value.v2 && cmp->value.v2 == front->value.v1) {
				cmp = cmp->Remove();
				matchFound = true;
			} else {
				cmp = cmp->GetNext();
			}
		}
		if (matchFound) {
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
	for (int m = 0; m < m_materials.GetSize(); ++m) {
		const mtlNode<mglFacet> *facet = m_materials[m].GetFacets();
		while (facet != NULL) {
			m_area += GetFacetArea(facet->value.v1, facet->value.v2, facet->value.v3);
			facet = facet->GetNext();
		}
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
		for (int m = 0; m < m_materials.GetSize(); ++m) {
			const mtlNode<mglFacet> *facet = m_materials[m].GetFacets();
			while (facet != NULL) {
				m_volume += CalculateVolume(facet->value.v1, facet->value.v2, facet->value.v3);
				facet = facet->GetNext();
			}
		}
	} else {
		m_volume = -1.0f;
	}
}

void mglModel::CalculateFacetNormals( void )
{
	for (int i = 0; i < m_materials.GetSize(); ++i) {
		mtlNode<mglFacet> *facet = m_materials[i].m_facets.GetFront();
		while (facet != NULL) {
			facet->value.normal = mmlSurfaceNormal(m_vertices[facet->value.v1], m_vertices[facet->value.v2], m_vertices[facet->value.v3]);
			facet = facet->GetNext();
		}
	}
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
	m_facets = 0;
	m_closed = false;
	m_error.Copy("");
}

bool mglModel::Load(const mtlDirectory &p_filename)
{
	Free();
	m_error.Copy("");
	
	if (!p_filename.GetExtension().Compare("obj")) {
		m_error.Copy("Not an .obj file");
		return false;
	}
	
	mtlString fileContents;
	if (!mtlParser::BufferFile(p_filename.GetDirectory(), fileContents)) {
		m_error.Copy("File unreadable");
		return false;
	}
	
	if (!PreParseFile(fileContents) || !ParseFile(fileContents)) {
		return false;
	}

	CountFacets();
	CreateEdgeList();
	CheckIfClosed();
	CalculateArea();
	CalculateVolume();
	CalculateFacetNormals();
	CalculateBounds();
	return true;
}

mmlVector<3> mglModel::GetFacetNormal(int v1, int v2, int v3) const
{
	return mmlSurfaceNormal(m_vertices[v1], m_vertices[v2], m_vertices[v3]);
}

float mglModel::GetFacetArea(int v1, int v2, int v3) const
{
	float a = (m_vertices[v2] - m_vertices[v1]).Len();
	float b = (m_vertices[v3] - m_vertices[v2]).Len();
	float c = (m_vertices[v3] - m_vertices[v1]).Len();
	float s = (a + b + c) * 0.5f;
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

/*void mglModel::Render(const mglRasterizer *p_raster, const mglTransform &p_objectTransform, const mglTransform &p_cameraTransform) const
{
	// rasterizer simply renders triangles, nothing more
		// removes the responsibility from the rasterizer to know how to render a model (becomes problematic when there are many model structures)
	// allows model to make some quality settings (texture samling, lighting model) that only affect the model currently rendered
}*/

mtlNode<mglStaticModel::Poly> *mglStaticModel::FindBestSplittingPolygon(mglStaticModel::Node *node)
{
	/*mtlNode<Poly> *splitPoly = node->polys.GetFront();
	mtlNode<Poly> *best = NULL;
	int bestFront = 0;
	int bestBack = 0;
	int bestTotal = 0;
	while (splitPoly != NULL) {
		mglPlane plane(mmlVector<3>::Cast(&splitPoly->value.a), splitPoly->value.normal);
		int numFront = 0;
		int numBack = 0;
		int numTotal = 0;
		mtlNode<Poly> *poly = node->polys.GetFront();
		while (poly != NULL) {
			if (poly != splitPoly) {
				switch (plane.DetermineClipping(mmlVector<3>::Cast(&poly->value.a), mmlVector<3>::Cast(&poly->value.b), mmlVector<3>::Cast(&poly->value.c))) {
				case mglCoinciding:
					break;
				case mglBehind:
					++numBack;
					++numTotal;
					break;
				case mglInFront:
					++numFront;
					++numTotal;
					break;
				case mglClipping:
					++numBack;
					++numFront;
					numTotal += 3;
					break;
				}
			}
			poly = poly->GetNext();
		}
		
		if (best == NULL || abs(numFront - numBack) < abs(bestFront - bestBack)) {
			best = splitPoly;
			bestFront = numFront;
			bestBack = numBack;
		}
		splitPoly = splitPoly->GetNext();
	}
	return best;*/
	return node->polys.GetFront();
}

void mglStaticModel::SplitGeometryRecursively(mglStaticModel::Node *node, int depth)
{
	if (node->polys.GetSize() <= 1) { return; } // end of the line
	m_depth = mmlMax2(m_depth, depth);
	node->front = new Node(node);
	node->back = new Node(node);
	mtlNode<Poly> *splitPoly = FindBestSplittingPolygon(node);
	node->plane = mglPlane(mmlVector<3>::Cast(&splitPoly->value.a), splitPoly->value.normal);
	const mglPlane backPlane(node->plane.GetPosition(), -node->plane.GetNormal());
	mtlNode<Poly> *poly = node->polys.GetFront();
	while (poly != NULL) {
		switch (node->plane.DetermineClipping(mmlVector<3>::Cast(&poly->value.a), mmlVector<3>::Cast(&poly->value.b), mmlVector<3>::Cast(&poly->value.c)))
		{
		case mglCoinciding:
			poly = poly->GetNext();
			break;
		case mglBehind:
			node->back->polys.PushBack(poly->value);
			poly = node->polys.Remove(poly);
			break;
		case mglInFront:
			node->front->polys.PushBack(poly->value);
			poly = node->polys.Remove(poly);
			break;
		case mglClipping:
			mmlVector<5> out[4];
			// get front section
			switch (node->plane.Clip(poly->value.a, poly->value.b, poly->value.c, out)) {
			case 4: {
				Poly p = { out[0], out[2], out[3], poly->value.normal, poly->value.material };
				node->front->polys.PushBack(p);
			}
			case 3: {
				Poly p = { out[0], out[1], out[2], poly->value.normal, poly->value.material };
				node->front->polys.PushBack(p);
			}
			}

			// get back section
			switch (backPlane.Clip(poly->value.a, poly->value.b, poly->value.c, out)) {
			case 4: {
				Poly p = { out[0], out[2], out[3], poly->value.normal, poly->value.material };
				node->back->polys.PushBack(p);
			}
			case 3: {
				Poly p = { out[0], out[1], out[2], poly->value.normal, poly->value.material };
				node->back->polys.PushBack(p);
			}
			}

			poly = node->polys.Remove(poly);
			break;
		}
	}
	if (node->front->polys.GetSize() > 0) {
		SplitGeometryRecursively(node->front, depth+1);
	} else {
		delete node->front;
		node->front = NULL;
	}
	if (node->back->polys.GetSize() > 0) {
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
			// dot(hyperplane.normal, splitPoly[n].centroid)
			// if dot < 0, then behind, dot > 0 then in front
	// traverse recursively to next node, pick a random polygon to split by, and split all other polygons by that splitter, sort "behind" and "front"
	// continue until you are satisfied or until there is only one polygon in a node

	// OPTIMIZATION:
		// stop splitting when polygon set is guaranteed not to overlap (including backface culling)

	if (GetVertexCount() <= 0) { return; }
	m_root = new Node(NULL);
	for (int i = 0; i < GetMaterialCount(); ++i) {
		const mtlNode<mglFacet> *facet = GetMaterial(i).GetFacets();
		while (facet != NULL) {
			const mmlVector<3> av = GetVertex(facet->value.v1);
			const mmlVector<3> bv = GetVertex(facet->value.v2);
			const mmlVector<3> cv = GetVertex(facet->value.v3);
			const mmlVector<2> at = GetTexCoord(facet->value.t1);
			const mmlVector<2> bt = GetTexCoord(facet->value.t2);
			const mmlVector<2> ct = GetTexCoord(facet->value.t3);
			Poly poly = {
				mmlVector<5>(av[0], av[1], av[2], at[0], at[1]),
				mmlVector<5>(bv[0], bv[1], bv[2], bt[0], bt[1]),
				mmlVector<5>(cv[0], cv[1], cv[2], ct[0], ct[1]),
				facet->value.normal,
				&GetMaterial(i)
			};
			m_root->polys.PushBack(poly);
			facet = facet->GetNext();
		}
	}
	if (m_root->polys.GetSize() > 0) {
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
