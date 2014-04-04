#include <ctime>
#include <cstdlib>
#include "mglEngine.h"

void mglEngine::UpdateTime( void )
{
	float prevTime = m_time;
	m_time = GetActualTime();
	m_timeDelta = m_time - prevTime;
}

void mglEngine::UpdateObjects( void )
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		mglObject *obj = node->GetItem();
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
		mglObject *a = aNode->GetItem();
		if (a->IsEnabled() && a->GetCollider() != NULL) {
			mtlNode<mglObject*> *bNode = aNode->GetNext();
			while (bNode != NULL) {
				mglObject *b = bNode->GetItem();
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
		mglObject *obj = node->GetItem();
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
		mglObject *obj = node->GetItem();
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
		mglObject *obj = node->GetItem();
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
		if (node->GetItem()->m_markedForDestruction) {
			node->GetItem()->OnDestroy();
			delete node->GetItem();
			node = node->Remove();
		} else {
			node = node->GetNext();
		}
	}
}

mglEngine::mglEngine( void ) : m_objects(), m_timeDelta(0.0f), m_time(0.0f), m_frameLimit(0), m_done(false), m_updating(false), m_clearScreen(false), m_clearColor(0)
{
	srand((unsigned int)time(0));
}

mglEngine::~mglEngine( void )
{
	m_updating = false; // if Kill is called this will be 'true' and DestroyAllObjects will not free any memory
	DestroyAllObjects();
}

void mglEngine::Update( void )
{
	m_updating = true;

	m_inputs.Free();
	UpdateInput();

	PreRender();

	if (m_clearScreen) {
		ClearScreen();
	}

	// update time as closely to update function as possible
	UpdateTime();

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

	m_updating = false;
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
	if (!m_updating) {
		while (node != NULL) {
			delete node->GetItem();
			node = node->GetNext();
		}
		m_objects.Free();
	} else {
		while (node != NULL) {
			node->GetItem()->Destroy();
			node = node->GetNext();
		}
	}
}

void mglEngine::GetObjectsByName(const mtlChars &p_name, mtlList<mglObject*> &p_out)
{
	p_out.Free();
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		if (node->GetItem()->m_enabled && node->GetItem()->name.Compare(p_name)) {
			p_out.PushBack(node->GetItem());
		}
		node = node->GetNext();
	}
}

mglObject *mglEngine::GetFirstObjectByName(const mtlChars &p_name)
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		if (node->GetItem()->name.Compare(p_name)) {
			return node->GetItem();
		}
		node = node->GetNext();
	}
	return NULL;
}

mglObject *mglEngine::GetLastObjectByName(const mtlChars &p_name)
{
	mtlNode<mglObject*> *node = m_objects.GetBack();
	while (node != NULL) {
		if (node->GetItem()->name.Compare(p_name)) {
			return node->GetItem();
		}
		node = node->GetPrev();
	}
	return NULL;
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
