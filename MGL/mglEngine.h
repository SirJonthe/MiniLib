#ifndef MGL_ENGINE_H_INCLUDED__
#define MGL_ENGINE_H_INCLUDED__

#include "../MTL/mtlList.h"
#include "../MTL/mtlString.h"
#include "../MML/mmlVector.h"
#include "mglFramebuffer.h"
#include "mglInput.h"
#include "mglObject.h"

// THINK:
// - How do you re-enable disabled objects?
// - Disabled object's functions are never called, so it can't re-enable itself.
// - Disabled objects are not returned from GetObject functions (except GetObjects()).
// - Right now, m_enabled is synonomous with m_markedForDestruction with the exception
//   of them being removed from object list.

// TODO: GetRandomInt using own randomization function (better control over what happens in program)
// TODO: GetRandomFloat (0 - 1)
// TODO: GetRandomFloat (x - y)

class mglEngine
{
private:
	mtlList<mglObject*>	m_objects;
	//mtlList<mglLight*>	m_lights;
	double				m_timeDelta;
	double				m_time;
	unsigned int		m_frameLimit;
	bool				m_done;
	bool				m_updating;
	bool				m_clearScreen;
	unsigned int		m_clearColor;
protected:
	mtlList<mglInput>	m_inputs;
	mtlString			m_error;
private:
				mglEngine(const mglEngine&) {}
	mglEngine	&operator=(const mglEngine&) { return *this; }
protected:
	virtual unsigned int	GetTimeInMilliseconds( void ) const = 0;													/* Gets time since program start from API (ms) */
	virtual void			UpdateVideo( void ) = 0;																	/* Updates video content */
	virtual void			Sleep(unsigned int p_milliseconds) = 0;														/* Free resources to OS for specified amount of time */
	virtual void			UpdateInput( void ) = 0;																	/* Build m_inputState list, update mouse positions (abs/rel), process window events */
	virtual void			PreRender( void ) {}																		/* Prepare for rendering */
	virtual void			PostRender( void ) {}																		/* Cleans up after rendering */
	virtual void			ClearScreen( void ) = 0;																	/* Clears the screen with m_clearColor */
public:
	virtual bool					InitSystems(int argc, char**argv, unsigned int flags = 0) = 0;						/* Init API(s) */
	virtual void					CloseSystems( void ) = 0;															/* Destroy video and close the API subsystem(s) */
	virtual const mglFramebuffer<>	*GetVideo( void ) = 0;																/* Get API video buffer */
	virtual bool					SetVideo(int p_width, int p_height, bool p_fullscreen, unsigned int flags = 0) = 0;	/* Set API video buffer (must be 32 bits) and set texture format */
	virtual void					SystemLog(const char *p_message) = 0;												/* Log a system message to preferred output */
	virtual void					HideCursor( void ) = 0;																/* Hides cursor */
	virtual void					ShowCursor( void ) = 0;																/* Shows cursor */
	virtual void					SetCursorPosition(int x, int y) = 0;												/* Set cursor position */
	virtual void					SetWindowCaption(const char *caption) = 0;											/* Set a window caption */
private:
	void UpdateTime( void );
	void UpdateObjects( void );
	void CollideObjects( void );
	void PreRenderObjects( void );
	void FinishObjects( void );
	void PostRenderObjects( void );
	void RemoveDestroyedObjects( void );
protected:
	unsigned int GetClearColor( void ) const { return m_clearColor; }
public:
								mglEngine( void );
	virtual						~mglEngine( void );
	void						Update( void );
	void						AddObject(mglObject *p_object);
	void						DestroyAllObjects( void );
	double						GetTime( void ) const { return m_time; }
	double						GetDeltaTime( void ) const { return m_timeDelta; }
	double						GetActualTime( void ) const { return float(GetTimeInMilliseconds()) / 1000.0; }
	const mtlString				&GetError( void ) const { return m_error; }
	//void						GetObjectsByRay(const mglRay &p_ray, mtlList<mglObject*> &p_out);
	//void						GetObjectsByRay(const mglRay &p_ray, float p_length, mtlList<mglObject*> &p_out);
	//void						GetObjectsByCone(const mglRay &p_ray, float p_dotRange, mtlList<mglObject*> &p_out);
	//void						GetObjectsByCone(const mglRay &p_ray, float p_length, float p_dotRange, mtlList<mglObject*> &p_out);
	void						GetObjectsByName(const mtlChars &p_name, mtlList<mglObject*> &p_out);
	mglObject					*GetFirstObjectByName(const mtlChars &p_name);
	mglObject					*GetLastObjectByName(const mtlChars &p_name);
	template < typename type_t >
	void						GetObjectsByType(mtlList<type_t*> &p_out);
	template < typename type_t >
	void						GetFirstObjectByType(type_t *p_type);
	template < typename type_t >
	void						GetLastObjectByType(type_t *p_type);
	int							GetRandomInt(int min, int max) const;
	void						SetFrameLimit(unsigned int p_frameLimit) { m_frameLimit = p_frameLimit; }
	void						DisableFrameLimit( void ) { SetFrameLimit(0); }
	int							Run( void );
	void						Break( void ) { m_done = true; }
	void						Kill(int state = 1);
	const mtlList<mglObject*>	&GetObjects( void ) const { return m_objects; }
	const mtlNode<mglInput>		*GetInput( void ) const { return m_inputs.GetFront(); }
	int							GetInputQueueSize( void ) const { return m_inputs.GetSize(); }
	void						DisableScreenClear( void ) { m_clearScreen = false; }
	void						SetScreenClearColor(unsigned int color) { m_clearScreen = true; m_clearColor = color; }
};

// provide functions that narrow down a list of objects
// where you can call GetObjectsByName and then narrow down
// that return value further by calling GetObjectsByRay
// meaning you trace a ray against all objects called something
// specific and then return a list of the objects named that *and*
// that collided with the given ray

template < typename type_t >
void mglEngine::GetObjectsByType(mtlList<type_t*> &p_out)
{
	p_out.Free();
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		type_t *t;
		if (node->GetItem()->m_enabled && node->GetItem()->IsType(t)) {
			p_out.PushBack(t);
		}
		node = node->GetNext();
	}
}

template < typename type_t >
void mglEngine::GetFirstObjectByType(type_t *p_type)
{
	mtlNode<mglObject*> *node = m_objects.GetFront();
	while (node != NULL) {
		if (node->GetItem()->m_enabled && node->GetItem()->IsType(p_type)) { return; }
		node = node->GetNext();
	}
}

template < typename type_t >
void mglEngine::GetLastObjectByType(type_t *p_type)
{
	mtlNode<mglObject*> *node = m_objects.GetBack();
	while (node != NULL) {
		if (node->GetItem()->m_enabled && node->GetItem()->IsType(p_type)) { return; }
		node = node->GetPrev();
	}
}

#endif // MGL_ENGINE_H_INCLUDED__
