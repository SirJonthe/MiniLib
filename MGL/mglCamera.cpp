#include "mglEngine.h"
#include "mglCamera.h"
#include "mglFramebuffer.h"
#include "mglRasterizer.h"

mglCamera::mglCamera(const mtlChars &p_name, mglRasterizer *p_rasterizer) : mglObject(p_name), m_rasterizer(p_rasterizer)
{
	SetPerspective(90.0f, 0.01f, 300.0f);
}

mglCamera::~mglCamera( void )
{
	delete m_rasterizer;
}

void mglCamera::SetPerspective(float p_vFov, float p_near, float p_far)
{
	if (m_rasterizer != NULL) {
		m_rasterizer->SetProjectionMatrix(p_vFov, p_near, p_far);
	}
}

void mglCamera::RenderView( void )
{
	if (m_rasterizer == NULL || m_rasterizer->framebuffer == NULL || GetEngine() == NULL) { return; }
	m_rasterizer->SetCameraTransform(transform);
	const mtlNode<mglObject*> *obj = GetEngine()->GetObjects().GetFront();
	while (obj != NULL) {
		if (obj->GetItem()->IsEnabled() && obj->GetItem()->visible && obj->GetItem()->model.GetAsset() != NULL && (mask & obj->GetItem()->mask) == mask) {
			m_rasterizer->Render(*obj->GetItem()->model.GetAsset(), obj->GetItem()->transform.GetWorldTransformMatrix());
		}
		obj = obj->GetNext();
	}
}
