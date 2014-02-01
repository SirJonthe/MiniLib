#ifndef SDL_ENGINE_H
#define SDL_ENGINE_H

#ifdef _MSC_VER
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#include "SDL.h"
#elif defined(__GNUC__)
#include <SDL/SDL.h>
#endif

#include <iostream>

#include "MGL/mglFramebuffer.h"
#include "MGL/mglEngine.h"

std::ostream &operator<<(std::ostream &out, const mtlString &p_str);
std::ostream &operator<<(std::ostream &out, const mtlSubstring &p_str);

class SDL_Framebuffer : public mglFramebuffer<>
{
public:
			~SDL_Framebuffer( void ) { m_pixels = NULL; } // m_pixels wasn't allocated by new, so NULL the pointer before delete is called
	void	SetBuffer(SDL_Surface *surf);
};

class SDL_Engine : public mglEngine
{
private:
	SDL_Framebuffer m_video;
	SDL_Event		m_event;
protected:
	unsigned int	GetTimeInMilliseconds( void ) const { return (unsigned int)SDL_GetTicks(); }
	void			UpdateVideo( void ) { SDL_Flip(SDL_GetVideoSurface()); }
	void			Sleep(unsigned int p_milliseconds) { SDL_Delay(Uint32(p_milliseconds)); }
	void			UpdateInput( void );
	void			PreRender( void );
	void			PostRender( void );
	void			ClearScreen( void );
public:
	bool					InitSystems(int argc, char**argv, unsigned int flags = 0);
	void					CloseSystems( void );
	const mglFramebuffer<>	*GetVideo( void ) { return &m_video; }
	bool					SetVideo(int p_width, int p_height, bool p_fullscreen, unsigned int flags = 0);
	void					SystemLog(const char *message) { std::cout << message; }
	void					HideCursor( void ) { SDL_ShowCursor(0); }
	void					ShowCursor( void ) { SDL_ShowCursor(1); }
	void					SetCursorPosition(int x, int y) { SDL_WarpMouse(Uint16(x), Uint16(y)); }
	void					SetWindowCaption(const char *caption) { SDL_WM_SetCaption(caption, NULL); }
};

#endif // SDL_ENGINE_H
