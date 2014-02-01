#include "SDL_Engine.h"

std::ostream &operator<<(std::ostream &out, const mtlString &p_str)
{
	if (p_str.GetChars() == NULL) {
		return out;
	}
	return out << p_str.GetChars();
}

std::ostream &operator<<(std::ostream &out, const mtlSubstring &p_str)
{
	for (int i = 0; i < p_str.GetSize(); ++i) {
		out << p_str.GetChars()[i];
	}
	return out;
}

void SDL_Framebuffer::SetBuffer(SDL_Surface *surf)
{
	if (surf != NULL) {
		m_pixels = (unsigned int*)surf->pixels;
		m_width = (int)surf->w;
		m_height = (int)surf->h;
	} else {
		m_pixels = NULL;
		m_width = 0;
		m_height = 0;
	}
}

void SDL_Engine::UpdateInput( void )
{
	mglInput state;
	state.motion.type = (mglInputType)0;
	state.motion.device = (mglInputDevice)0;
	state.motion.absX = 0;
	state.motion.absY = 0;
	state.motion.relX = 0;
	state.motion.relY = 0;
	m_event.type = 0;
	while (SDL_PollEvent(&m_event)) {
		switch (m_event.type) {
		case SDL_QUIT:
			Kill(EXIT_SUCCESS);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			state.type = mglButtonInput;
			state.button.device = mglMouse;
			state.button.button = (int)m_event.button.button;
			state.button.state = (m_event.type == SDL_MOUSEBUTTONDOWN) ? mglButtonDown : mglButtonUp;
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			state.type = mglButtonInput;
			state.button.device = mglKeyboard;
			state.button.button = (int)m_event.key.keysym.sym;
			state.button.state = (m_event.type == SDL_KEYDOWN) ? mglButtonDown : mglButtonUp;
			break;
		case SDL_MOUSEMOTION:
			state.type = mglMotionInput;
			state.motion.device = mglMouse;
			state.motion.absX = (int)m_event.motion.x;
			state.motion.absY = (int)m_event.motion.y;
			state.motion.relX = (int)m_event.motion.xrel;
			state.motion.relY = (int)m_event.motion.yrel;
			break;
		default: continue;
		}
		m_inputs.PushBack(state);
	}
}

void SDL_Engine::PreRender( void )
{
	if (SDL_MUSTLOCK(SDL_GetVideoSurface()) && SDL_GetVideoSurface()->locked == 0) {
		SDL_LockSurface(SDL_GetVideoSurface());
	}
}

void SDL_Engine::PostRender( void )
{
	if (SDL_MUSTLOCK(SDL_GetVideoSurface())) {
		while (SDL_GetVideoSurface()->locked > 0) {
			SDL_UnlockSurface(SDL_GetVideoSurface());
		}
	}
}

void SDL_Engine::ClearScreen( void )
{
	const int area = m_video.GetArea();
	unsigned int *pixels = m_video.GetPixels();
	unsigned int color = GetClearColor();
	for (int i = 0; i < area; ++i) {
		pixels[i] = color;
	}
}

bool SDL_Engine::InitSystems(int, char**, unsigned int flags)
{
	SystemLog("SDL init...");
	if (SDL_Init(SDL_INIT_VIDEO|flags)) {
		m_error.CopyPtr(SDL_GetError());
		SystemLog(m_error.GetChars());
		SystemLog("\n");
		return false;
	}
	SystemLog("done\n");
	return true;
}

void SDL_Engine::CloseSystems( void )
{
	SystemLog("Closing SDL...");
	SDL_FreeSurface(SDL_GetVideoSurface());
	m_video.SetBuffer(NULL);
	SDL_Quit();
	SystemLog("done\n");
}

bool SDL_Engine::SetVideo(int p_width, int p_height, bool p_fullscreen, unsigned int flags)
{
	if (m_video.GetPixels() != NULL) {
		SDL_FreeSurface(SDL_GetVideoSurface());
		m_video.SetBuffer(NULL);
	}
	SystemLog("SDL video init...");
	SDL_Surface *surf = SDL_SetVideoMode(p_width, p_height, 32, (p_fullscreen ? SDL_FULLSCREEN : 0)|SDL_HWSURFACE|SDL_DOUBLEBUF|flags);
	if (surf == NULL) {
		m_error.CopyPtr(SDL_GetError());
		SystemLog(m_error.GetChars());
		SystemLog("\n");
		return false;
	}
	SystemLog("done\n");

	m_video.SetBuffer(surf);

	SDL_PixelFormat *format = surf->format;
	mglTexture::nativeFormat.RMask = format->Rmask;
	mglTexture::nativeFormat.GMask = format->Gmask;
	mglTexture::nativeFormat.BMask = format->Bmask;
	mglTexture::nativeFormat.AMask = format->Amask;
	mglTexture::nativeFormat.RShift = format->Rshift;
	mglTexture::nativeFormat.GShift = format->Gshift;
	mglTexture::nativeFormat.BShift = format->Bshift;
	mglTexture::nativeFormat.AShift = format->Ashift;
	if (mglTexture::nativeFormat.AMask == 0) {
		mglTexture::nativeFormat.AMask = ~(format->Rmask | format->Gmask | format->Bmask);
		if (format->Rshift == 0 || format->Gshift == 0 || format->Bshift == 0) {
			if (format->Rshift == 8 || format->Gshift == 8 || format->Bshift == 8) {
				if (format->Rshift == 16 || format->Gshift == 16 || format->Bshift == 16) {
					mglTexture::nativeFormat.AShift = 24;
				} else { mglTexture::nativeFormat.AShift = 16; }
			} else { mglTexture::nativeFormat.AShift = 8; }
		} else { mglTexture::nativeFormat.AShift = 0; }
	}
	return true;
}
