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

#include <limits>


using namespace std;
using namespace gear2d;

/* static things */
map<string, texturedef> glrenderer::rawbyfile;
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

glrenderer::renderset glrenderer::renderqueue;

double glrenderer::viewleft;
double glrenderer::viewright;
double glrenderer::viewtop;
double glrenderer::viewbottom;


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
	glEnable(GL_TEXTURE_2D);
	
		// shading mathod: GL_SMOOTH or GL_FLAT
	//glShadeModel(GL_FLAT);

	// background color
	glClearColor(0.0, 0.0, 0.0, 0.0);

	/* Depth buffer setup */
	glClearDepth(1000.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
// 	glDisable(GL_DEPTH_TEST);

	// Perspective Calculations: GL_NICEST or GL_FASTEST  */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
// 	glDisable(GL_BLEND);

	glDisable(GL_CULL_FACE);
// 	glFrontFace(GL_CCW);
// 	glEnable(GL_CULL_FACE);

	//glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1000.0f);                         // 0 is near, 1 is far

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
	viewleft = -(w/2.0f);
	viewright = w/2.0f;
	viewtop = -h/2.0f;
	viewbottom = h/2.0f;
	
	glOrtho(viewleft, viewright, viewbottom, viewtop, -1000.0f, 1000.0);
// 	gluPerspective(50.0, (GLfloat)(((float)w) / ((float)h)), 0.1f, 100.0f);
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

/* destroys the glimage, which decreases the count of texturedefs */
glrenderer::~glrenderer() {
	renderers.remove(this);
	while (!surfacebyid.empty()) {
		glimage * img = (surfacebyid.begin()->second);
		renderqueue.erase(img);
		surfacebyid.erase(surfacebyid.begin());
		
		delete img;
	}
}

void glrenderer::setup(object::signature & sig) {
	/* init sdl, screen, gl, stuff, whatnot */
	int w = eval(sig["renderer.w"], 800);
	int h = eval(sig["renderer.h"], 600);
	int bpp = eval(sig["renderer.bpp"], 32);
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
		write(id + ".position.w", eval(sig[id + ".position.w"], s->w));
		write(id + ".position.h", eval(sig[id + ".position.h"], s->h));
		write(id + ".position.rotation", eval(sig[id + ".position.rotation"], 0.0f));
		write(id + ".zoom", eval(sig[id + ".zoom"], 1.0f));
		write(id + ".clip.x", eval<int>(sig[id + ".clip.x"], 0));
		write(id + ".clip.y", eval<int>(sig[id + ".clip.y"], 0));
		write(id + ".clip.w", eval<int>(sig[id + ".clip.w"], s->w));
		write(id + ".clip.h", eval<int>(sig[id + ".clip.h"], s->h));
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
		texturedef & tex = getraw(imgpath + file, false);
		glimage * img = prepare(id, tex);
		renderqueue.insert(img);
	}
}


texturedef & glrenderer::getraw(string path, bool reload = false) {
	if (rawbyfile.find(path) != rawbyfile.end()) return rawbyfile[path];
	SDL_Surface * tmp = IMG_Load(path.c_str()), *resized = 0, *s = 0;
	if (tmp == 0) {
		throw evil(string("(Renderer component) Not able to load ") + path + ": " + SDL_GetError());
	}
	
	
	int val = max(tmp->w, tmp->h);
	val--;
	val = (val >> 1) | val;
	val = (val >> 2) | val;
	val = (val >> 4) | val;
	val = (val >> 8) | val;
	val = (val >> 16) | val;
	val++;
	
	val = max(val, glrenderer::texturesize);
	
	
	resized = SDL_Resize(tmp, val, val, false);
	if (resized->format->Amask != 0) {
		SDL_SetAlpha(resized, SDL_RLEACCEL | SDL_SRCALPHA, 255);
	} else {
		SDL_SetColorKey(resized, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB(resized->format, 0xff, 0, 0xff));
	}
	s = SDL_DisplayFormatAlpha(resized);
	if (s == 0) { s = resized; }
	texturedef tex(0, 0, s->w, s->h, tmp->w, tmp->h);
	SDL_FreeSurface(tmp);
	
	
	glGenTextures(1, &tex.id);
	
	// load images as 2d texture array
	glBindTexture( GL_TEXTURE_2D, tex.id);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_LINEAR to turn on interpolation
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use GL_NEAREST to turn off interpolation
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // 4-byte pixel alignment
	
	glTexImage2D(GL_TEXTURE_2D, 0, 4, val, val, 0, GL_BGRA, GL_UNSIGNED_BYTE, s->pixels);
	
	glBindTexture( GL_TEXTURE_2D, 0);
	
	rawbyfile[path] = tex;
	
	SDL_FreeSurface(s);
	
	return rawbyfile[path];
}

glimage * glrenderer::prepare(string id, texturedef & tex) {
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

void glrenderer::populate() {
	if (glvectorsize == 0) {
		if (glvectorsize == 0) glvectorsize = renderers.size();
		glvectorsize = 1024;
// 		glvertices = (float *) realloc(glvertices, glvectorsize * 12);
// 		gltextures = (float *) realloc(gltextures, glvectorsize * 8);
		glvertices = (float *) malloc(glvectorsize * 12);
		gltextures = (float *) malloc(glvectorsize * 8);
		
	}
	
	int v = 0;
	int t = 0;
	for (renderset::iterator i = renderqueue.begin(); i != renderqueue.end(); i++) {
		glimage * img = *i;
		
		/* clock-wise */
		
		/* vertices */
		float x = img->x - screen->w/2;
		float y = img->y - screen->h/2;
		float z = img->z;
		if (img->bind) {
			x += img->parent->read<float>("x");
			y += img->parent->read<float>("y");
			z += img->parent->read<float>("z");
		}
		
// 		printf("aaa %d %d %d %d\n", img->w, img->tex.realw, img->tex.w, img->tex.id);
		glvertices[v++] = x;
		glvertices[v++] = y;
		glvertices[v++] = z;
		
		/* clip/attach point */
		gltextures[t++] = 0;
		gltextures[t++] = 0;
		
		
		/* vertices */
		glvertices[v++] = x + img->w;
		glvertices[v++] = y;
		glvertices[v++] = z;
		
		/* clip/attach point */
		gltextures[t++] = 1;
		gltextures[t++] = 0;
		
		/* vertices */
		glvertices[v++] = x + img->w;
		glvertices[v++] = y + img->h;
		glvertices[v++] = z;
		
		/* clip/attach point */
		gltextures[t++] = 1;
		gltextures[t++] = 1;
		
		/* vertices */
		glvertices[v++] = x;
		glvertices[v++] = y + img->h;
		glvertices[v++] = z;
		
		/* clip/attach point */
		gltextures[t++] = 0;
		gltextures[t++] = 1;
		
		/* TODO: clipping */
	}
}

void glrenderer::render() {
	populate();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(viewleft, viewright, viewbottom, viewtop, -1000.0f, 1000.0);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glTexCoordPointer(2, GL_FLOAT, 0, gltextures);
	glVertexPointer(3, GL_FLOAT, 0, glvertices);
	
	texturedef tex = (*renderqueue.begin())->tex;
	int start = -4;
	for (renderset::iterator i = renderqueue.begin(); i != renderqueue.end(); ++i) {
		if (tex.id == (*i)->tex.id) { start += 4; continue; }
		
		/* new texture id */
// 		printf("casa do xapeu: %d\n", tex.id);
		glBindTexture(GL_TEXTURE_2D, tex.id);
		glDrawArrays(GL_QUADS, start, tex.count * 4);
// 		printf("%d\n", start);
		start += 4;
	
		tex = (*i)->tex;
	}
	/* last texture */
// 	printf("%d\n", start);
// 	printf("casa do xapeu: %d\n", tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);
	glDrawArrays(GL_QUADS, start, tex.count * 4);

	/* house cleaning? */
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	SDL_GL_SwapBuffers();
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