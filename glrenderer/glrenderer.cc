#include "gear2d.h"
#include "glrenderer.h"
#include "glimage.h"

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "resize++.h"

#include "glimage.h"

using namespace std;
using namespace gear2d;

/* static things */
int glrenderer::texturesize3d;
int glrenderer::texturedepth3d;
int glrenderer::texturesize;
list<glrenderer *> glrenderer::renderers;
SDL_Surface * glrenderer::screen = 0;
bool glrenderer::initialized = false;
int glrenderer::rendervotes = 0;

int glrenderer::glvectorsize = 0;
float * glrenderer::glvertices = 0;
float * glrenderer::gltextures = 0;


void glrenderer::initializegl(int w, int h) {
	glewInit();

	// Get max values
	int test;
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &test);
	glrenderer::texturesize3d = test;
	glrenderer::texturedepth3d = test;
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &test);
	glrenderer::texturesize = test;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &test);
	
	if (test < glrenderer::texturedepth3d) glrenderer::texturedepth3d = test;
	
	/* ZOMG */
	
	// Enable Texture Mapping
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_TEXTURE_2D_ARRAY_EXT);
	//glEnable(GL_TEXTURE_2D_ARRAY);
	glEnable(GL_TEXTURE_3D);
	
		// shading mathod: GL_SMOOTH or GL_FLAT
	//glShadeModel(GL_FLAT);

	// background color
	glClearColor(0.0, 0.0, 0.0, 0.0);

	/* Depth buffer setup */
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Perspective Calculations: GL_NICEST or GL_FASTEST  */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	// Enable Blending
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glEnable(GL_BLEND);
	glDisable(GL_BLEND);

	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	//glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far

	// Lighting initialization
	// Disable Lightning
	glDisable(GL_LIGHTING);
	//initLights();

	// Set The Perpective view
	// Make sure we're chaning the projection
	glMatrixMode(GL_PROJECTION);
	// Reset The View
	glLoadIdentity();
	// Set our perspective
	
	gluPerspective(50.0, (GLfloat)(((float)w) / ((float)h)), 0.1f, 100.0f);
}

/* hook for a renderer.surfaces change */
void glrenderer::handlesurfaces(parameterbase::id pid, component::base* lastwrite, object::id owner) {
	/* TODO :D*/
}

/* hook for a renderer.texts change */
void glrenderer::handletexts(parameterbase::id pid, component::base* lastwrite, object::id owner) {
	/* TODO :D*/
}

glrenderer::glrenderer() {
	renderers.push_back(this);
}

glrenderer::~glrenderer() {
	renderers.remove(this);
}

void glrenderer::setup(object::signature & sig) {
	/* init sdl, screen, gl, stuff, whatnot */
	int w = eval(sig["renderer.w"], 800);
	int h = eval(sig["renderer.h"], 600);
	int bpp = eval(sig["renderer.bpp"], 32);
	if (!initialized) initialize(w, h, bpp);
	write("renderer.w", w);
	write("renderer.h", h);
	write("renderer.bpp", bpp);
	if (!initialized) initialize(w, h, bpp);
	
	write("imgpath", sig["imgpath"]);
	
	/* TODO: text handling */
	
	surfaces = fetch<string>("renderer.surfaces");
	surfaces = sig["renderer.surfaces"];
	hook("renderer.surfaces", (component::call) &glrenderer::handlesurfaces);
	
	textlist = fetch<string>("renderer.texts");
	textlist = sig["renderer.texts"];
	hook("renderer.texts", (component::call) &glrenderer::handletexts);
	
	if ((string)surfaces != "") {
		loadsurfaces(surfaces, sig["imgpath"]);
	}
	// initialize with signature;
	// we do not need to rerun this because if the user loads a surface
	// after setup phase, probably he'll load parameters by himself.
	for (map<string, glimage*>::iterator i = surfacebyid.begin(); i != surfacebyid.end(); i++) {
		string id = i->first;
		glimage * s = i->second;
		
		// pid was set by bind() of by surface() in worst case.
		write(id + ".position.x", eval(sig[id + ".position.x"], 0.0f));
		write(id + ".position.y", eval(sig[id + ".position.y"], 0.0f));
		write(id + ".position.z", eval(sig[id + ".position.z"], 0.0f));
		write(id + ".position.w", eval(sig[id + ".position.w"], s->tex.w));
		write(id + ".position.h", eval(sig[id + ".position.h"], s->tex.h));
		write(id + ".position.rotation", eval(sig[id + ".position.rotation"], 0.0f));
		write(id + ".zoom", eval(sig[id + ".zoom"], 1.0f));
		write(id + ".clip.x", eval<int>(sig[id + ".clip.x"], 0));
		write(id + ".clip.y", eval<int>(sig[id + ".clip.y"], 0));
		write(id + ".clip.w", eval<int>(sig[id + ".clip.w"], s->tex.w));
		write(id + ".clip.h", eval<int>(sig[id + ".clip.h"], s->tex.h));
		write(id + ".bind", eval(sig[id + ".bind"], true));
		write(id + ".absolute", eval<bool>(sig[id + ".absolute"], true));
		write(id + ".alpha", eval<float>(sig[id + ".alpha"], 1.0f));
		write(id + ".render", eval<bool>(sig[id + ".render"], true));
	}	
}

void glrenderer::loadsurfaces(string surfacelist, string imgpath) {
	set<string> surfaceset;
	set<string>::iterator surfacedef;
	split(surfaceset, surfacelist, ' ');
	
	for (surfacedef = surfaceset.begin(); surfacedef != surfaceset.end(); surfacedef++) {
		int p = surfacedef->find('=');
		string id = surfacedef->substr(0, p);
		string file = surfacedef->substr(p+1);
		if (surfacebyid[id] != 0) continue;
		texturedef tex = getraw(imgpath + file, false);
		glimage * img = prepare(id, tex);
	}
}


texturedef glrenderer::getraw(string path, bool reload = false) {
	if (rawbyfile.find(path) != rawbyfile.end()) return rawbyfile[path];
	SDL_Surface * tmp = IMG_Load(path.c_str()), *resized = 0, *s = 0;
	if (tmp == 0) {
		throw evil(string("(Renderer component) Not able to load ") + path + ": " + SDL_GetError());
	}

	SDL_FreeSurface(tmp);
	
	int val = max(tmp->w, tmp->h);
	val--;
	val = (val >> 1) | val;
	val = (val >> 2) | val;
	val = (val >> 4) | val;
	val = (val >> 8) | val;
	val = (val >> 16) | val;
	val++;
	
	val = max(val, glrenderer::texturesize);
	
	resized = SDL_Resize(tmp, val, val);
	SDL_FreeSurface(tmp);
	
	
	if (resized->format->Amask != 0) {
		SDL_SetAlpha(resized, SDL_RLEACCEL | SDL_SRCALPHA, 255);
		s = SDL_DisplayFormatAlpha(resized);
	} else {
		SDL_SetColorKey(resized, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB(resized->format, 0xff, 0, 0xff));
		s = SDL_DisplayFormat(resized);
	}

	if (s == 0) {
		s = resized;
	} else {
		SDL_FreeSurface(resized);
	}
	
	texturedef tex(0, 0, s->w, s->h);
	
	glGenTextures(1, &tex.id);
	
	// load images as 2d texture array
	glBindTexture( GL_TEXTURE_2D, tex.id);

// 	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_LINEAR to turn on interpolation
// 	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use GL_NEAREST to turn off interpolation
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // 4-byte pixel alignment
	
	glTexImage2D(GL_TEXTURE_2D, 0, 4, val, val, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
	
	glBindTexture( GL_TEXTURE_2D, 0);
	
	rawbyfile[path] = tex;
	
	SDL_FreeSurface(s);
	
	return tex;
}

glimage * glrenderer::prepare(string id, texturedef tex) {
	glimage * s = new glimage(this, id, tex);
	surfacebyid[id] = s;
	
	// these will bind param names to our data
	bind(id + ".position.x", s->x);
	bind(id + ".position.y", s->y);
	bind(id + ".position.z", s->z);
	bind(id + ".position.w", s->w);
	bind(id + ".position.h", s->h);
	bind(id + ".position.rotation", s->rotation);
	bind(id + ".clip.x", s->clip.x);
	bind(id + ".clip.y", s->clip.y);
	bind(id + ".clip.w", s->clip.w);
	bind(id + ".clip.h", s->clip.h);
	bind(id + ".bind", s->bind);
	bind(id + ".render", s->render);
	bind(id + ".absolute", s->absolute);
	bind(id + ".alpha", s->alpha);
	bind(id + ".zoom", s->zoom);

	return s;
}


void glrenderer::update(timediff dt) {
	rendervotes++;
	if (rendervotes >= renderers.size()) {
		render();
		rendervotes = 0;
	}
}

void glrenderer::render() {
	if (glvectorsize == 0) glvectorsize = renderers.size();
	if (renderers.size() > glvectorsize) {
		glvectorsize <<= 2;
		glvertices = (float *) realloc(glvertices, glvectorsize * 12);
		gltextures = (float *) realloc(gltextures, glvectorsize * 8);
	}
}


void glrenderer::initialize(int w, int h, int bpp = 32) {
	if (initialized == true) return;
	
	/* Flags to pass to SDL_SetVideoMode */
	int videoflags;

	/* this holds some info about our display */
	const SDL_VideoInfo * videoinfo;
	
	/* initialize SDL */
	if (!SDL_WasInit(SDL_INIT_VIDEO)) {
		cerr << "(Rendering component) Video wasn't initialized...." << endl;
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
			throw evil(string("(Rendering component) Failed initializing SDL: ") + SDL_GetError());
		}
	}
	
	videoinfo = SDL_GetVideoInfo();
	
	if (videoinfo == 0) { throw evil(string("(Rendering component) Video query failed: ") + SDL_GetError()); }

	/* the flags to pass to SDL_SetVideoMode */
	videoflags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
	videoflags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
	videoflags |= SDL_HWPALETTE;       /* Store the palette in hardware */
	
	if (videoinfo->hw_available)
		videoflags |= SDL_HWSURFACE;
	else
		videoflags |= SDL_SWSURFACE;

	/* This checks if hardware blits can be done */
	if (videoinfo->blit_hw)
		videoflags |= SDL_HWACCEL;

	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	if (screen == 0) screen = SDL_SetVideoMode(w, h, bpp, videoflags);
	if (screen == 0) {
		throw evil(string("(Rendering component) Failed initializing video: ") + SDL_GetError());
	}
	
	if (!TTF_WasInit()) {
		if (TTF_Init() != 0) {
			throw evil(string("(Rendering component) Failed initializing font renderer: ") + TTF_GetError());
		}
	}
	
	/* init opengl stuff */
	initializegl(w, h);
	
	rendervotes = 0;
	
	initialized = true;
}

extern "C" {
	component::base * build() { return new glrenderer; }
}