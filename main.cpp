#include "MTL/mtlBinaryTree.h"

#include "MGL/mglObject.h"
#include "MGL/mglCamera.h"
#include "MGL/mglEngine.h"
#include "MGL/mglFramebuffer.h"
#include "MGL/mglRasterizer.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

#include "Aux/SDL_Engine.h"
#include "Aux/OGL_Engine.h"
#include "Aux/OGL_Rasterizer.h"

class ControllableObject : public mglObject
{
private:
	bool			m_mouseDown;
	bool			m_shiftDown;
	mmlVector<3>	m_moveInc;
protected:
	void OnUpdate( void );
public:
	ControllableObject(const mtlChars &p_name) : mglObject(p_name), m_mouseDown(false), m_shiftDown(false), m_moveInc(0.0f, 0.0f, 0.0f) { transform.position += mglTransform::globalAxis.forward * 20.0f; }
};

void ControllableObject::OnUpdate( void )
{
	const mtlNode<mglInput> *inputNode = GetEngine()->GetInput();
	while (inputNode != NULL) {
		mglInput input = inputNode->GetItem();
		switch (input.type) {
		case mglButtonInput:
			if (input.button.device == mglMouse && input.button.button == SDL_BUTTON_LEFT) {
				m_mouseDown = (input.button.state == mglButtonDown) ? true : false;
			} else if (input.button.device == mglKeyboard) {
				if (input.button.button == SDLK_ESCAPE) { GetEngine()->Break(); }
				else {
					const float speed = input.button.state == mglButtonDown ? 4.0f : 0.0f;
					if (input.button.button == SDLK_RIGHT) {
						m_moveInc[0] = mglTransform::globalAxis.right[0] * speed;
					} else if (input.button.button == SDLK_LEFT) {
						m_moveInc[0] = mglTransform::globalAxis.right[0] * -speed;
					} else if (input.button.button == SDLK_a) {
						m_moveInc[1] = mglTransform::globalAxis.up[1] * speed;
					} else if (input.button.button == SDLK_z) {
						m_moveInc[1] = mglTransform::globalAxis.up[1] * -speed;
					} else if (input.button.button == SDLK_UP) {
						m_moveInc[2] = mglTransform::globalAxis.forward[2] * speed;
					} else if (input.button.button == SDLK_DOWN) {
						m_moveInc[2] = mglTransform::globalAxis.forward[2] * -speed;
					} else if (input.button.button == SDLK_LSHIFT) {
						m_shiftDown = (input.button.state == mglButtonDown) ? true : false;
					}
				}
			}
			break;
		case mglMotionInput:
			if (m_mouseDown) {
				if (!m_shiftDown) {
					transform.RotateGlobal(mglTransform::globalAxis.up, float(input.motion.relX) * 0.01f);
				} else {
					transform.RotateGlobal(mglTransform::globalAxis.right, float(input.motion.relY) * 0.01f);
				}
			}
			break;
		default:
			std::cout << "Not gownah happe!" << std::endl;
			std::cout << input.type << std::endl;
		}
		inputNode = inputNode->GetNext();
	}
	transform.MoveGlobal(m_moveInc * GetEngine()->GetDeltaTime());
}

class TransformController : public mglObject
{
private:
	mglTransform	*m_transform;
	bool			m_connected;
protected:
	void OnUpdate( void );
public:
	TransformController(const mtlChars &p_name, mglTransform &p_transform) : mglObject(p_name), m_transform(&p_transform), m_connected(false) {}
};

void TransformController::OnUpdate( void )
{
	const mtlNode<mglInput> *input = GetEngine()->GetInput();
	while (input != NULL) {
		mglInput i = input->GetItem();
		if (i.type == mglButtonInput && i.button.device == mglKeyboard && i.button.state == mglButtonDown && i.button.button == SDLK_SPACE) {
				if (!m_connected) {
					m_transform->SetParentTransform(&transform, true);
				} else {
					m_transform->SetParentTransform(NULL, true);
				}
				m_connected = !m_connected;
		}
		input = input->GetNext();
	}
}

void Unit_StringAppend( void );
void Unit_StringOverwrite( void );
void Unit_Clipping( void );
void Unit_Directory( void );
void Unit_QuatToMatrix( void );
void Unit_MergeLists( void );
void Unit_BinaryTree( void );
void SimpleScene(int argc, char **argv);
void SimpleSceneOGL(int argc, char **argv);
void TexturedTriangle(int argc, char **argv);
void DrawLine(int argc, char **argv);

// TODO: Implement SIMD parallel rasterizer, render in blocks of X pixels
// TODO: Implement CPU parallel rasterizer, one CPU per SIMD block scanline (local cache coherency)
	// std::thread or OpenMP?
	// http://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
// TODO: Implement fixed point rasterizer (fstp seems expensive), may not be compatible with SIMD
// TODO: group together the least important lights into a directional light
// TODO: transform lights to object space, light model (prevents having to transform model normals)
// TODO: stop splitting BSP when we get a fully convex subspace (possible within reasonable time consumption?)
// TODO: Transform with constraints (e.g. can't move along Z, can't rotate around Y and so on)
// TODO: Refactor renderer (completely generic rendering design), only provides interface for rendering primitives
// TODO: Model renders itself, uses renderer interface - PROBLEM: how to render stencil shadows? Separate rendering function?
// TODO: Texture and Model need virtual API interface functions
// TODO: Fix mtlString::Append()
// TODO: Make mtlParser more powerful (see header)

// BUG: mtlString::Insert does not work when appending, does it work for other cases?
// BUG: mtlString::Overwrite fails to write a larger string than is already available
// BUG: Multiple materials in a material file crashes model loader (maybe even multiple materials regardless of separate files?)
// BUG: Trailing newline may prevent or cause a failed read in model/material loader
// BUG: BSP models are sometimes missing polygons
	// could these polygons be the splitting polygons?
// BUG: BSP does not always work when very close to geometry or viewed from very oblique angles
// BUG: rotating by global axis might actually be rotating by local axis
// BUG: mouse down event generates uninitialized value somewhere
// BUG: preservation of transforms does not work

// http://devmaster.net/posts/11968/cracks-in-geometry-in-software-rasterizer
// http://chrishecker.com/Miscellaneous_Technical_Articles

int main(int argc, char **argv)
{
	std::cout << "Unit tests:" << std::endl;
	Unit_Clipping();
	Unit_StringAppend();
	Unit_StringOverwrite();
	Unit_Directory();
	Unit_QuatToMatrix();
	Unit_MergeLists();
	Unit_BinaryTree();
	std::cout << "completed" << std::endl;

	//SimpleSceneOGL(argc, argv);
	DrawLine(argc,argv);
	return 0;
}

void Unit_StringAppend( void )
{
	std::cout << "\tTesting string append...";
	mtlString str1;
	str1.Copy("Hello");
	mtlString str2;
	str2.Copy(", World");
	mtlString str3;
	str3.Copy(str1);
	str3.Append(str2);
	if (str3.GetSize() == str1.GetSize() + str2.GetSize()) {
		std::cout << str3 << std::endl;
	} else {
		std::cout << "failed: " << str3.GetSize() << " vs " << str1.GetSize() << "+" << str2.GetSize() << std::endl;
	}
}

void Unit_StringOverwrite( void )
{
	std::cout << "\tTesting string overwrite...";
	mtlString str1;
	str1.Copy("Hello");
	mtlString str2;
	str2.Copy("J");
	mtlString str3;
	str3.Copy(str1);
	str3.Overwrite(str2, 0);
	mtlString str4;
	str4.Copy(str1);
	str4.Overwrite("A big write", 0);
	if (str3.Compare("Jello") && str4.Compare("A big write")) {
		std::cout << "success" << std::endl;
	} else {
		std::cout << "failed: " << str3 << ", " << str4 << std::endl;
	}
}

void Unit_Clipping( void )
{
	mmlVector<3> a(0.0f,  50.0f,  50.0f);
	mmlVector<3> b(0.0f, -50.0f,  50.0f);
	mmlVector<3> c(0.0f,   0.0f, -50.0f);
	mmlVector<3> out1[4];
	mmlVector<3> out2[4];
	mglPlane plane(mmlVector<3>(0.0f, 0.0f, 0.0f), mglTransform::globalAxis.forward);
	plane.Clip(a, b, c, out1);
	std::cout << "\tTesting plane edge case...";
	if (plane.Clip(out1[0], out1[1], out1[2], out2) == 3 && plane.Clip(out1[0], out1[2], out1[3], out2) == 3) {
		plane.ReverseClip(a, b, c, out1);
		if (plane.Clip(out1[0], out1[1], out1[2], out2) == 0 && plane.Clip(out1[0], out1[2], out1[3], out2) == 0) {
			std::cout << "success" << std::endl;
		} else {
			std::cout << "failed" << std::endl;
		}
	} else {
		std::cout << "failed" << std::endl;
	}
}

/*void Unit_TextureCache(Platform &platform)
{
	mglTexture texture;
	texture.Create(512);
 
	mmlVector<4> a(400.0f, 0.0f, 0.0f, 0.0f);
	mmlVector<4> b(800.0f, 300.0f, 1.0f, 0.0f);
	mmlVector<4> c(400.0f, 600.0f, 1.0f, 1.0f);
	mmlVector<4> d(0.0f, 300.0f, 0.0f, 1.0f);
 
	const int numFrames = 100;
	const float angleInc = mmlPI / numFrames;
	const float COS = cos(angleInc);
	const float SIN = sin(angleInc);
	mmlMatrix<2,2> m;
	m[0][0] = COS; m[0][1] = -SIN;
	m[1][0] = SIN; m[1][1] =  COS;
	mmlVector<2> t;
	t[0] = 400.0f;
	t[1] = 300.0f;
 
	mglSWMonoRasterizer raster(platform.GetVideo());
 
 
	for (int i = 0; i < numFrames; ++i) {
		
		raster.Debug_RenderTriangle(a, b, c, &texture);
		raster.Debug_RenderTriangle(a, c, d, &texture);
 
		mmlVector<2>::Cast(&a) -= t;
		mmlVector<2>::Cast(&a) *= m;
		mmlVector<2>::Cast(&a) += t;
		
		mmlVector<2>::Cast(&b) -= t;
		mmlVector<2>::Cast(&b) *= m;
		mmlVector<2>::Cast(&b) += t;
		
		mmlVector<2>::Cast(&c) -= t;
		mmlVector<2>::Cast(&c) *= m;
		mmlVector<2>::Cast(&c) += t;
		
		mmlVector<2>::Cast(&d) -= t;
		mmlVector<2>::Cast(&d) *= m;
		mmlVector<2>::Cast(&d) += t;
	}
 
	platform.UpdateVideo();
}*/

void Unit_Directory( void )
{
	mtlDirectory dir1("~/Applications/MiniLib.app");
	mtlDirectory dir2("~/Applications/MiniLib.app/");
	mtlDirectory dir3("~/Applications/MiniLib.app/Contents/MacOS/MiniLib");
	mtlDirectory dir4("~/Applications/MiniLib.app/Contents/MacOS/MiniLib.ub");
	mtlDirectory dir5("~/Applications/MiniLib.app/.config.ini");
	mtlDirectory dir6("~/Applications/MiniLib.app/.config");
	mtlDirectory dir7("MiniLib.app");
	mtlDirectory dir8("/MiniLib.app");
	mtlDirectory dir9(".config");
	mtlDirectory dir10(".config.ini");
	
	std::cout << "\tTesting mtlDirectory..." << std::endl;
	
	if (!dir1.GetFolders().Compare("~/Applications/") || !dir1.GetFilename().Compare("MiniLib") || !dir1.GetExtension().Compare("app")) {
		std::cout << "\t\t" << dir1.GetDirectory() << ": failed 1" << std::endl;
		std::cout << "\t\t\t" << dir1.GetFolders() << " | " << dir1.GetFilename() << " | " << dir1.GetExtension() << std::endl;
	}
	if (!dir2.GetFolders().Compare("~/Applications/MiniLib.app/") || !dir2.GetFilename().Compare("") || !dir2.GetExtension().Compare("")) {
		std::cout << "\t\t" << dir2.GetDirectory() << ": failed 2" << std::endl;
		std::cout << "\t\t\t" << dir2.GetFolders() << " | " << dir2.GetFilename() << " | " << dir2.GetExtension() << std::endl;
	}
	if (!dir3.GetFolders().Compare("~/Applications/MiniLib.app/Contents/MacOS/") || !dir3.GetFilename().Compare("MiniLib") || !dir3.GetExtension().Compare("")) {
		std::cout << "\t\t" << dir3.GetDirectory() << ": failed 3" << std::endl;
		std::cout << "\t\t\t" << dir3.GetFolders() << " | " << dir3.GetFilename() << " | " << dir3.GetExtension() << std::endl;
	}
	if (!dir4.GetFolders().Compare("~/Applications/MiniLib.app/Contents/MacOS/") || !dir4.GetFilename().Compare("MiniLib") || !dir4.GetExtension().Compare("ub")) {
		std::cout << "\t\t" << dir4.GetDirectory() << ": failed 4" << std::endl;
		std::cout << "\t\t\t" << dir4.GetFolders() << " | " << dir4.GetFilename() << " | " << dir4.GetExtension() << std::endl;
	}
	if (!dir5.GetFolders().Compare("~/Applications/MiniLib.app/") || !dir5.GetFilename().Compare(".config") || !dir5.GetExtension().Compare("ini")) {
		std::cout << "\t\t" << dir5.GetDirectory() << ": failed 5" << std::endl;
		std::cout << "\t\t\t" << dir5.GetFolders() << " | " << dir5.GetFilename() << " | " << dir5.GetExtension() << std::endl;
	}
	if (!dir6.GetFolders().Compare("~/Applications/MiniLib.app/") || !dir6.GetFilename().Compare(".config") || !dir6.GetExtension().Compare("")) {
		std::cout << "\t\t" << dir6.GetDirectory() << ": failed 6" << std::endl;
		std::cout << "\t\t\t" << dir6.GetFolders() << " | " << dir6.GetFilename() << " | " << dir6.GetExtension() << std::endl;
	}
	if (!dir7.GetFolders().Compare("") || !dir7.GetFilename().Compare("MiniLib") || !dir7.GetExtension().Compare("app")) {
		std::cout << "\t\t" << dir7.GetDirectory() << ": failed 7" << std::endl;
		std::cout << "\t\t\t" << dir7.GetFolders() << " | " << dir7.GetFilename() << " | " << dir7.GetExtension() << std::endl;
	}
	if (!dir8.GetFolders().Compare("/") || !dir8.GetFilename().Compare("MiniLib") || !dir8.GetExtension().Compare("app")) {
		std::cout << "\t\t" << dir8.GetDirectory() << ": failed 8" << std::endl;
		std::cout << "\t\t\t" << dir8.GetFolders() << " | " << dir8.GetFilename() << " | " << dir8.GetExtension() << std::endl;
	}
	if (!dir9.GetFolders().Compare("") || !dir9.GetFilename().Compare(".config") || !dir9.GetExtension().Compare("")) {
		std::cout << "\t\t" << dir9.GetDirectory() << ": failed 9" << std::endl;
		std::cout << "\t\t\t" << dir9.GetFolders() << " | " << dir9.GetFilename() << " | " << dir9.GetExtension() << std::endl;
	}
	if (!dir10.GetFolders().Compare("") || !dir10.GetFilename().Compare(".config") || !dir10.GetExtension().Compare("ini")) {
		std::cout << "\t\t" << dir10.GetDirectory() << ": failed 10" << std::endl;
		std::cout << "\t\t\t" << dir10.GetFolders() << " | " << dir10.GetFilename() << " | " << dir10.GetExtension() << std::endl;
	}
	
	std::cout << "\tdone" << std::endl;
}

void Unit_QuatToMatrix( void )
{
	mglTransform transform;
	transform.position = mmlVector<3>(40.0f, 14.0f, 73.0f);
	transform.rotation.RotateByAxisAngle(mglTransform::globalAxis.up, 14.3f);
	transform.rotation.RotateByAxisAngle(mglTransform::globalAxis.right, 29.2f);
	transform.rotation.RotateByAxisAngle(mglTransform::globalAxis.forward, 1.7f);
	mmlMatrix<4,4> matrix = transform.GetWorldTransformMatrix();
	
	mmlVector<3> pointT = transform.position;
	mmlVector<3> pointM = mmlVector<3>(0.0f, 0.0f, 0.0f) * matrix;
	
	std::cout << "t(" << pointT[0] << ";" << pointT[1] << ";" << pointT[2] << ") : m(" << pointM[0] << ";" << pointM[1] << ";" << pointM[2] << ")" << std::endl;
}

void Unit_MergeLists( void )
{
	mtlList<int> l1;
	l1.AddLast(3);
	l1.AddLast(7);
	l1.AddLast(9);
	l1.AddLast(19);
	l1.AddLast(67);
	
	mtlList<int> l2;
	l2.AddLast(1);
	l2.AddLast(2);
	l2.AddLast(4);
	l2.AddLast(8);
	l2.AddLast(10);
	l2.AddLast(77);
	
	mtlList<int> l3;
	
	mtlNode<int> *n1 = l1.GetFirst();
	mtlNode<int> *n2 = l2.GetFirst();
	
	while (true) { // assumes neither lists return NULL for a start
		
		if (n1->GetItem() < n2->GetItem()) {
			l3.AddFirst(n1->GetItem());
			n1 = n1->GetNext();
			if (n1 == NULL) {
				while (n2 != NULL) {
					l3.AddFirst(n2->GetItem());
					n2 = n2->GetNext();
				}
				return;
			}
		} else {
			l3.AddFirst(n2->GetItem());
			n2 = n2->GetNext();
			if (n2 == NULL) {
				while (n1 != NULL) {
					l3.AddFirst(n1->GetItem());
					n1 = n1->GetNext();
				}
				return;
			}
		}
	}
	
	mtlNode<int> *n3 = l3.GetFirst();
	while (n3 != NULL) {
		std::cout << n3->GetItem() << " ";
		n3 = n3->GetNext();
	}
}

void Unit_BinaryTree( void )
{
	std::cout << "Testing binary tree...";
	
	mtlBinaryTree<int> tree;
	
	for (int i = 0; i < 10; ++i) {
		int item = rand()%10;
		tree.Insert(item);
		std::cout << item << " ";
	}
	
	int find = rand()%10;
	const mtlBranch<int> *n = tree.GetRoot()->Find(find);
	if (n == NULL) { std::cout << ": (" << find << ")"; }
	else { std::cout << ": [" << find << "]"; }
	std::cout << std::endl;
}

void SimpleScene(int argc, char **argv)
{
	SDL_Engine engine;
	if (!engine.InitSystems(argc, argv) || !engine.SetVideo(800, 600, false)) {
		std::cout << "Failed to init engine" << std::endl;
		return;
	}
	mglTexturedRasterizer *raster = new mglTexturedRasterizer(engine.GetVideo());
	//mglFlatRasterizer *raster = new mglFlatRasterizer(engine.GetVideo());
	engine.AddObject(new mglCamera("game_camera", raster));
	mglObject *obj = new ControllableObject("game_object");
	engine.AddObject(obj);
	std::cout << "Loading model...";
	if (!obj->model.Load<mglModel>("../Models/Ship/mod.obj")) {
		std::cout << "failed: " << obj->model.GetError() << std::endl;
		engine.CloseSystems();
		return;
	}
	std::cout << "done" << std::endl;
	
	engine.SetWindowCaption(argv[0]);
	
	engine.SetScreenClearColor(0);
	
	engine.Run();
	
	// MGL actually allows trailing memory to leak, since the OS will handle it anyway
	// do this to avoid memory checkers from detecting leaked memory (in order):
	engine.DestroyAllObjects(); // frees all engine-registered objects and unlocks references to models
	mtlAsset<mglModel>::Purge(); // frees all models and unlocks references to textures
	mtlAsset<mglStaticModel>::Purge(); // frees all BSP models and unlocks references to textures
	mtlAsset<mglTexture>::Purge(); // frees all textures
	
	engine.CloseSystems();
}

void SimpleSceneOGL(int argc, char **argv)
{
	OGL_Engine engine;
	if (!engine.InitSystems(argc, argv)) {
		std::cout << "Failed to init engine" << std::endl;
		return;
	}
	std::cout << "Video init...";
	if (!engine.SetVideo(800, 600, false)) {
		std::cout << "Failed to init video" << std::endl;
		return;
	}
	std::cout << "done" << std::endl;
	OGL_Rasterizer *raster = new OGL_Rasterizer(engine.GetVideo());
	engine.AddObject(new mglCamera("game_camera", raster));
	mglObject *obj = new ControllableObject("game_object");
	engine.AddObject(obj);
	std::cout << "Loading model...";
	if (!obj->model.Load<mglModel>("../Models/Ship/mod.obj")) {
		std::cout << "failed: " << obj->model.GetError() << std::endl;
		engine.CloseSystems();
		return;
	}
	std::cout << "done" << std::endl;
	
	TransformController transformController("game_transform", obj->transform);
	engine.AddObject(&transformController);
	transformController.transform.position = mmlVector<3>(15.0f, 17.0f, 20.0f);
	transformController.transform.RotateGlobal(mglTransform::globalAxis.up, 100.0f);
	transformController.transform.RotateGlobal(mglTransform::globalAxis.right, 20.0f);
	transformController.transform.RotateGlobal(mglTransform::globalAxis.forward, 1.0f);
	
	engine.SetWindowCaption(argv[0]);
	
	engine.SetScreenClearColor(0);
	
	engine.Run();
	
	// MGL actually allows trailing memory to leak, since the OS will handle it anyway
	// do this to avoid memory checkers from detecting leaked memory (in order):
	engine.DestroyAllObjects(); // frees all engine-registered objects and unlocks references to models
	mtlAsset<mglModel>::Purge(); // frees all models and unlocks references to textures
	mtlAsset<mglStaticModel>::Purge(); // frees all BSP models and unlocks references to textures
	mtlAsset<mglTexture>::Purge(); // frees all textures
	
	engine.CloseSystems();
}

void TexturedTriangle(int argc, char **argv)
{
	SDL_Engine engine;
	if (!engine.InitSystems(argc, argv) || !engine.SetVideo(800, 600, false)) {
		std::cout << "Failed to init engine" << std::endl;
		return;
	}
	mglTexturedRasterizer *raster = new mglTexturedRasterizer(engine.GetVideo());
	mtlAsset<mglTexture> texture;
	std::cout << "Loading texture...";
	if (!texture.Load<mglModel>("../Models/Ship/tex.tga")) {
		std::cout << "failed: " << texture.GetError() << std::endl;
		engine.CloseSystems();
		return;
	}
	std::cout << "done" << std::endl;
	
	engine.SetWindowCaption(argv[0]);
	
	const mmlVector<4> a(  0.0f,   0.0f, 0.0f, 1.0f);
	const mmlVector<4> b(200.0f,   0.0f, 1.0f, 1.0f);
	const mmlVector<4> c(200.0f, 200.0f, 1.0f, 0.0f);
	const mmlVector<4> d(  0.0f, 200.0f, 0.0f, 0.0f);
	
	raster->Debug_RenderTriangle(a, b, c, texture.GetAsset());
	raster->Debug_RenderTriangle(a, c, d, texture.GetAsset());
	
	SDL_Flip(SDL_GetVideoSurface());
	
	SDL_Event event;
	bool quit = false;
	while (!quit && SDL_WaitEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
			case SDL_KEYDOWN:
				quit = true;
				break;
		}
	}
	
	engine.CloseSystems();
}

void DrawLine(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Failed to init SDL" << std::endl;
		return;
	}
	if (SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE|SDL_DOUBLEBUF) == NULL) {
		std::cout << "Failed to init video" << std::endl;
		return;
	}
	
	bool quit = false;
	bool update = true;
	int x = 0, y = 0;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_MOUSEMOTION:
					x = event.motion.x;
					y = event.motion.y;
					update = true;
					break;
			}
		}
		if (update) {
			Uint32 *pixels = (Uint32*)SDL_GetVideoSurface()->pixels;
			int area = SDL_GetVideoSurface()->w*SDL_GetVideoSurface()->h;
			for (int i = 0; i < area; ++i) {
				pixels[i] = 0;
			}
			
			mglRay line;
			line.origin = mmlVector<3>(float(SDL_GetVideoSurface()->w >> 1), float(SDL_GetVideoSurface()->h >> 1), 0.0f);
			const mmlVector<3> mouse(float(x), float(y), 0.0f);
			line.direction = mmlNormalize(mouse - line.origin);
			mglDifferentialAnalyzer dda(line);
			
			int counter = 0;
			do {
				int xn = dda.GetX(), yn = dda.GetY();
				if (xn >= 0 && xn < SDL_GetVideoSurface()->w && yn >= 0 && yn < SDL_GetVideoSurface()->h) {
					pixels[yn * SDL_GetVideoSurface()->w + xn] = 0xff00ffff;
				}
				dda.Step();
				++counter;
			} while (dda.GetX() != x && dda.GetY() != y && counter < 1000);
			
			SDL_Flip(SDL_GetVideoSurface());
			update = false;
		}
	}
	
	SDL_FreeSurface(SDL_GetVideoSurface());
	SDL_Quit();
}
