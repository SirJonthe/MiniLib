#include <ctime>
#include <cstdlib>
#include "mglEngine.h"
#include "mglObject.h"

void mglEngine::UpdateObjects( void )
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		mglObject *obj = node->value;
		node = node->GetNext();
		if (obj->IsEnabled()) {
			obj->OnUpdate();
		}
	}
}

void mglEngine::CollideObjects( void )
{
	mtlNode<mglObject*> *aNode = m_objects.GetFront();
	while (aNode != NULL) {
		mglObject *a = aNode->value;
		if (a->IsEnabled() && a->GetCollider() != NULL) {
			mtlNode<mglObject*> *bNode = aNode->GetNext();
			while (bNode != NULL) {
				mglObject *b = bNode->value;
				if (b->IsEnabled() && b->GetCollider() != NULL) {
					// perform collision detection
				}
				bNode = bNode->GetNext();
			}
		}
		aNode = aNode->GetNext();
	}
}

void mglEngine::PreRenderObjects( void )
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		mglObject *obj = node->value;
		node = node->GetNext();
		if (obj->IsEnabled() && obj->visible) {
			obj->OnPreRender();
		}
	}
}

void mglEngine::FinishObjects( void )
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		mglObject *obj = node->value;
		node = node->GetNext();
		if (obj->IsEnabled() && obj->visible) {
			obj->OnFinish();
		}
	}
}

void mglEngine::PostRenderObjects( void )
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		mglObject *obj = node->value;
		node = node->GetNext();
		if (obj->IsEnabled() && obj->visible) {
			obj->OnPostRender();
		}
	}
}

void mglEngine::RemoveDestroyedObjects( void )
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		if (node->value->m_markedForDestruction) {
			node->value->OnDestroy();
			delete node->value;
			node = node->Remove();
		} else {
			node = node->GetNext();
		}
	}
}

mglEngine::mglEngine( void ) : m_objects(), m_timeDelta(0.0f), m_time(0.0f), m_frameLimit(0), m_done(false), m_clearScreen(false), m_clearColor(0)
{
	srand((unsigned int)time(0));
}

mglEngine::~mglEngine( void )
{
	DestroyAllObjects();
}

void mglEngine::Update( void )
{
	m_inputs.Free();
	UpdateInput();

	PreRender();

	if (m_clearScreen) {
		ClearScreen();
	}

	// update time as closely to update function as possible
	float prevTime = m_time;
	m_time = GetActualTime();
	m_timeDelta = m_time - prevTime;

	UpdateObjects();
	PreRenderObjects();
	FinishObjects();
	PostRenderObjects();

	PostRender();

	UpdateVideo();
	RemoveDestroyedObjects();

	if (m_frameLimit > 0) {
		const int timeToCompleteUpdate = int(GetTimeInMilliseconds() - int(m_time * 1000.0f));
		const int targetTimeToCompleteUpdate = 1000 / m_frameLimit;
		const int delay = targetTimeToCompleteUpdate - timeToCompleteUpdate;
		if (delay > 0) { Sleep((unsigned int)delay); }
	}
}

void mglEngine::AddObject(mglObject *p_object)
{
	if (p_object->m_engine != NULL) { return; }
	m_objects.PushBack(p_object);
	p_object->m_engine = this;
	p_object->OnCreate();
}

void mglEngine::DestroyAllObjects( void )
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		delete node->value;
		node = node->Remove();
	}
}

void mglEngine::GetObjectsByName(const mtlString &p_name, mtlList<mglObject*> &p_out)
{
	p_out.Free();
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		if (node->value->name.Compare(p_name)) {
			p_out.PushBack(node->value);
		}
		node = node->GetNext();
	}
}

int mglEngine::GetRandomInt(int min, int max) const
{
	return min + (int)((double)rand() / RAND_MAX * (min - max + 1));
}

int mglEngine::Run( void )
{
	m_done = false;
	while (!m_done) {
		Update();
	}
	return EXIT_SUCCESS;
}

void mglEngine::Kill(int state)
{
	exit(state);
}
