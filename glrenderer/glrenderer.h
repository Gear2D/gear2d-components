#ifndef G2D_GLRENDERER_H
#define G2D_GLRENDERER_H

#include "gear2d.h"
#include "SDL.h"
#include "glimage.h"

class glimage;
using namespace std;
using namespace gear2d;

/**
 * @file glrenderer.h
 * @b family renderer
 * @b type glrenderer
 * @author Leonardo Guilherme de Freitas
 * @addtogroup components
 * @class renderer
 * 
 * Renderer component that can work with images,
 * meshes&textures, primitives, etc, using opengl as backend.
 *
 * @b parameters
 * @li <tt>renderer.w</tt>: Width of the render screen. Global.	Shall be set at the game config file. Defaults to 800. @b int 
 * @li <tt>renderer.h</tt>: Height of the render screen. Global. Shall be set at the game config file. Defaults to 600. @b int
 * @li <tt>camera.{x|y|z}</tt>: Camera x, y and z position for rendering calculations. Global. Does nothing on object signature. Defaults to 0. @b float
 * @li <tt>renderer.surfaces</tt>: list of surfaces to load and work with, in the form of id=file separated by spaces. Loaded at setup and whenever changed.  @b string
 * 		@warning double warning on this one. If you want to load a new surface, change renderer.surfaces BEFORE setting its other attributes or the
 *		binding will fail so bad you won't be able to hide. Be prepared for evil if you do.
 * @li <tt>\<id\>.position.bind</tt>: Tells if the position of this image shall follow spatial info (x, y). @b bool. Defaults to 1 (true)
 * @li <tt>\<id\>.position.absolute</tt>: Tells if camera should be taken in account when rendering this. @b bool. 1 (true)
 * @li <tt>\<id\>.position.{x|y|z}</tt>: Specifics about the position of the image. If position.bind is specified, it will become an offset. @b float
 * @li <tt>\<id\>.position.{w|h}</tt>: Width and height of the image. Note that this does not scale the image, see "scale" below. @b int
 * @li <tt>\<id\>.clip.{x|y|w|h}</tt>: Clipping frame for the image. Defaults to whole image. @b float
 * @li <tt>\<id\>.alpha</tt>: Alpha value for the surface. 0 is transparent, 1 is opaque. @b float
 * @li <tt>\<id\>.render</tt>: Sets if this surface shall be rendered. 0 is false (don't render) and 1 is true (render). Defaults to true.
 * @li <tt>\<id\>.zoom</tt>: Scale this image zoom by a factor. @b float. Natural scale is 1.0. Less than 1 will shrink, bigger than 1 will enlarge. Defaults to 1.0.
 * @li <tt>renderer.texts</tt>: list of ids to be treated like text
 * @li <tt>\<id\>.text</tt>: Text to be rendered. This only works if @c <id> was declared at renderer.texts
 * @li <tt>\<id\>.blended</tt>: Shall the text be rendered in blend mode (alpha blending)? Defaults to 0 @b bool. Do not use for text that changes much. 
 * @li <tt>\<id\>.font</tt>: Font-file to be used. @b string Defaults to arial.ttf
 * @li <tt>\<id\>.font.size</tt>: Size to be rendered, in pixels
 * @li <tt>\<id\>.font.{r|g|b}</tt>: Red/green/blue color to be displayed, ranging from 0 to 1. @b float Defaults to 0
 * @li <tt>\<id\>.font.style</tt>: List of styles to be displayed, separated by white-space (bold italic underline strikethru) @b string
 * 
 * 
 * @b globals:
 * @li <tt>imgpath</tt> Path where to look for image files. Shall be set at the main config file
 *
 */


class glrenderer : public component::base {
	public:
		/* texturedef = textureid, texturedepth */

		
	public:
        glrenderer();
        virtual ~glrenderer();
		virtual component::family family() { return "renderer"; }
		virtual component::type type() { return "glrenderer"; }
		virtual string depends() { return "spatial/space2d"; }
		
		virtual void setup(object::signature & sig);
		virtual void update(timediff dt);
		
		/* loads a list of surfaces */
		void loadsurfaces(string surfacelist, string imgpath = "");
		
		/* prepare an glimage based on a texture definition */
		glimage * prepare(string id, texturedef tex);
		
	private:
		/* basic component initialization */
		static void initialize(int w, int h, int bpp);
		
		/* initialize opengl things */
		static void initializegl(int w, int h);
		
		/* render everyone */
		static void render();
		
		/* loads a texture def based on a image file */
		static texturedef getraw(string path, bool reload);
		
		
	/* links */
	private:
		gear2d::link<string> surfaces;
		gear2d::link<string> textlist;
		
		map<string, glimage*> surfacebyid;
// 		map<string, textdef*> textdefbyid;
		
		
	/* hooks */
	public:
		virtual void handlesurfaces(parameterbase::id pid, component::base * lastwrite, object::id owner);
		virtual void handletexts(parameterbase::id pid, component::base * lastwrite, object::id owner);

		
	private:
		static bool initialized;
		static SDL_Surface * screen;
		static int texturesize;
		static int texturesize3d;
		static int texturedepth3d;
		static list<glrenderer *> renderers;
		static int rendervotes;
		
		static int glvectorsize;
		static float * glvertices;
		static float * gltextures;
		
		static map<string, texturedef> rawbyfile;
};

#endif