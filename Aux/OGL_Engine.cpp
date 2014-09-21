//
//  OGL_Engine.cpp
//  MiniLib
//
//  Created by Jonathan Karlsson on 3/28/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#include "OGL_Engine.h"

void OGL_Engine::PreRender( void )
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void OGL_Engine::ClearScreen( void )
{
	glClear(GL_COLOR_BUFFER_BIT);
}

bool OGL_Engine::SetVideo(int p_width, int p_height, bool p_fullscreen, unsigned int flags)
{
	if (!SDL_Engine::SetVideo(p_width, p_height, p_fullscreen, SDL_OPENGL|flags)) { return false; }
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	
	return true;
}
