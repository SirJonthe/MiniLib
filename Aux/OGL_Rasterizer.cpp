//
//  OGL_Rasterizer.cpp
//  MiniLib
//
//  Created by Jonathan Karlsson on 3/28/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#include <iostream>
#include "OGL_Rasterizer.h"

mmlMatrix<4,4> OGL_Rasterizer::ConvertToOGLMatrix(const mmlMatrix<4, 4> &m1) const
{
	mmlMatrix<4,4> m;
	m[0][0] = m1[0][0]; m[0][1] = m1[0][1]; m[0][2] = m1[0][2]; m[0][3] = 0.0f;
	m[1][0] = m1[1][0]; m[1][1] = m1[1][1]; m[1][2] = m1[1][2]; m[1][3] = 0.0f;
	m[2][0] = m1[2][0]; m[2][1] = m1[2][1]; m[2][2] = m1[2][2]; m[2][3] = 0.0f;
	m[3][0] = m1[0][3]; m[3][1] = m1[1][3]; m[3][2] = m1[2][3]; m[3][3] = 1.0f;
	return m;
}

void OGL_Rasterizer::UpdateAPIProjectionMatrix( void )
{
	const float y = m_projection[1][1];
	m_projection[1][1] = -y;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_projection[0][0]);
	m_projection[1][1] = y;
}

void OGL_Rasterizer::Render(const mglModel &p_model, const mmlMatrix<4,4> &p_objToWorld)
{
	const mmlMatrix<4,4> objToWorld = ConvertToOGLMatrix(p_objToWorld);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&objToWorld[0][0]);
	
	for (int m = 0; m < p_model.GetMaterialCount(); ++m) {
		const mglMaterialIndex *material = &p_model.GetMaterial(m);
		
		// bind texture
		/*bool hasTexture = false;
		if (material->GetProperties().GetDiffuseMap() != NULL) {
			OGL_TextureComponent *APIComponent = dynamic_cast<OGL_TextureComponent*>(material->GetProperties().GetDiffuseMap()->GetAPIComponent());
			if (APIComponent != NULL && APIComponent->index > -1) {
				glBindTexture(, APIComponent->index);
				hasTexture = true;
			} else {
				glBindTexture(, -1);
			}
		}*/
		// set material properties
		mmlVector<4> diffuseColor;
		diffuseColor[0] = material->GetProperties().GetDiffuseColor()[0];
		diffuseColor[1] = material->GetProperties().GetDiffuseColor()[1];
		diffuseColor[2] = material->GetProperties().GetDiffuseColor()[2];
		diffuseColor[3] = material->GetProperties().GetDiffuseColor()[3];
		//glMaterialfv(GL_FRONT, GL_DIFFUSE, &diffuseColor[0]);
		
		const mmlVector<3> view = mmlVector<3>(0.0f, 0.0f, -1.0f) * mmlMatrix<3,3>(mmlInv(p_objToWorld));
		
		const mtlNode<mglFacet> *facet = material->GetFacets();
		glBegin(GL_TRIANGLES);
		/*if (hasTexture) {
			while (facet != NULL) {
				
				const mglFacet i = facet->GetItem();
				
				glTexCoord2fv(p_model.GetTexCoord(i.t1));
				glNormal3fv(p_model.GetNormal(i.n1));
				glVertex3fv(p_model.GetNormal(i.v1));
				
				glTexCoord2fv(p_model.GetTexCoord(i.t2));
				glNormal3fv(p_model.GetNormal(i.n2));
				glVertex3fv(p_model.GetNormal(i.v2));
				
				glTexCoord2fv(p_model.GetTexCoord(i.t3));
				glNormal3fv(p_model.GetNormal(i.n3));
				glVertex3fv(p_model.GetNormal(i.v3));
				
				facet = facet->GetNext();
			}
		} else {*/
			while (facet != NULL) {
				
				const mglFacet i = facet->GetItem();
				
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3fv(&p_model.GetVertex(i.v3)[0]);
				glColor3f(0.0f, 1.0f, 0.0f);
				glVertex3fv(&p_model.GetVertex(i.v2)[0]);
				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3fv(&p_model.GetVertex(i.v1)[0]);
				
				facet = facet->GetNext();
			}
		//}
		glEnd();
	}
}
