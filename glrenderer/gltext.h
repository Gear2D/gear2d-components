#ifndef GLTEXT_H
#define GLTEXT_H

#include <string>
#include "gear2d.h"

#include "FTGL/ftgl.h"

using namespace gear2d;
using namespace std;

/* we all hate copy and paste */

struct gltext {
	
	static map<string, FTFont *> fonts;
	
	// id of this surface
	string id;
	component::base * parent;
	
	gear2d::link<float> x;
	gear2d::link<float> y;
	gear2d::link<float> z;
	gear2d::link<float> rotation;
	gear2d::link<float> zoom;
	gear2d::link<int> w;
	gear2d::link<int> h;
	gear2d::link<float> alpha;
	gear2d::link<bool> bind;
	gear2d::link<bool> absolute;
	gear2d::link<bool> render;
	
	gear2d::link<string> text;
	
	gear2d::link<bool> blended;
	gear2d::link<string> font;
	gear2d::link<int> fontsz;
	gear2d::link<float> r;
	gear2d::link<float> g;
	gear2d::link<float> b;
	string styles;
	
	
	
	
	gltext(component::base * parent, string id);
	
	virtual ~gltext() { }
	
	virtual void rendertext();
};

#endif