//
//  OGL_Engine.h
//  MiniLib
//
//  Created by Jonathan Karlsson on 3/28/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#ifndef OGL_ENGINE_H_INCLUDED__
#define OGL_ENGINE_H_INCLUDED__

#include "SDL_Engine.h"

#ifdef _MSC_VER
//#pragma comment(lib, "SDL.lib")
#include "SDL_opengl.h"
#elif defined(__GNUC__)
#include <SDL/SDL_opengl.h>
#endif

class OGL_Engine : public SDL_Engine
{
protected:
	void UpdateVideo( void ) { SDL_GL_SwapBuffers(); }
	void PreRender( void );
	void PostRender( void ) {}
	void ClearScreen( void );
public:
	bool	SetVideo(int p_width, int p_height, bool p_fullscreen, unsigned int flags = 0); // OPEN_GL
};

#endif
