#ifndef G2D_GLIMAGE_H
#define G2D_GLIMAGE_H

#include "SDL.h"
#include "gear2d.h"

using namespace gear2d;
using namespace std;

/* Vulgo SDL_Surface * */
struct texturedef {
	unsigned int w, h;
	unsigned int realw, realh;
	unsigned int id, depth;
	unsigned int count;
	texturedef(int id = 0, int depth = 0, int w = 0, int h = 0, int realw = 0, int realh = 0) : w(w), h(h), realw(realw), realh(realh), count(0) { };
};


// this handles surface parameters
struct glimage {
		// id of this surface
		string id;
		component::base * parent;
		
		// parameters to be referenced
		float x;
		float y;
		float z;
		float rotation;
		float zoom;
		int w;
		int h;
		float alpha;
		bool bind;
		bool absolute;
		bool render;
		SDL_Rect clip;
		
		// raw surface
		texturedef & tex;
		
		
		bool operator<(const glimage & other) const {
			return (tex.id < other.tex.id || (!(other.tex.id < tex.id) && tex.depth < other.tex.depth));
		}
		
		bool operator==(const glimage & other) const {
			if (this == &other) return true;
			if (
				id == other.id &&
				z == other.z &&
				x == other.x &&
				y == other.y
			) { return true; }
			return false;
		}
		
		glimage(component::base * parent, string id, texturedef & tex)
		: parent(parent)
		, id(id)
		, x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, rotation(0.0f)
		, zoom(1.0f)
		, w(tex.realw)
		, h(tex.realh)
		, alpha(1.0f)
		, bind(true)
		, absolute(false)
		, tex(tex)
		{
			tex.count++;
		}
		
		virtual ~glimage() { tex.count--; }
};

/* compares two pointers of glimage */
struct glimagecmp {
	bool operator()(glimage * lhs, glimage * rhs) {
		return (*lhs < * rhs);
	}
};

#endif