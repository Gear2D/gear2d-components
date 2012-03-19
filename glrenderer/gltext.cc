#include "gltext.h"

#include "FTGL/FTFont.h"
#include "FTGL/ftgl.h"

map<string, FTFont *> gltext::fonts;
gltext::gltext(component::base* parent, string id)
: parent(parent)
, id(id)
, x(parent->fetch<float>(id + ".position.x"))
, y(parent->fetch<float>(id + ".position.y"))
, z(parent->fetch<float>(id + ".position.z"))
, rotation(parent->fetch<float>(id + ".position.rotation"))
, zoom(parent->fetch<float>(id + ".zoom"))
, w(parent->fetch<int>(id + ".position.w"))
, h(parent->fetch<int>(id + ".position.h"))
, alpha(parent->fetch<float>(id + ".alpha"))
, bind(parent->fetch<bool>(id + ".bind"))
, absolute(parent->fetch<bool>(id + ".absolute"))
, render(parent->fetch<bool>(id + ".render"))
, text(parent->fetch<string>(id + ".text"))
, blended(parent->fetch<bool>(id + ".blended"))
, font(parent->fetch<string>(id + ".font"))
, fontsz(parent->fetch<int>(id + ".font.size"))
, r(parent->fetch<float>(id + ".font.r"))
, g(parent->fetch<float>(id + ".font.g"))
, b(parent->fetch<float>(id + ".font.b")) { }

void gltext::rendertext() {
	FTFont * f;
	if (fonts[(string)font] == 0) {
		fonts[(string)font] = new FTTextureFont(((string)font).c_str());
	}
	f = fonts[((string)font)];
	if (f->Error()) { return; }
	f->FaceSize(fontsz);
	f->Render((((string)text)).c_str(), -1, FTPoint(x, y - f->Ascender()));
// 	cout << "desc: " << f->Descender() << " asc: " << f->Ascender() << endl;
	
	
}
