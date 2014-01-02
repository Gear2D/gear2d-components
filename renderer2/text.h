#ifndef TEXT_H
#define TEXT_H

#include "gear2d.h"
#include "SDL_ttf.h"
#include "texture.h"
#include <string>


struct text {
  struct {
    gear2d::link<std::string> name;
    gear2d::link<int> size;
  } font;

  struct {
    gear2d::link<float> r, g, b;
  } color;
  
  gear2d::link<std::string> style;
  gear2d::link<bool> blended;
  gear2d::link<std::string> text;
 
  bool bold, italic, underline, strikethrough;
  bool dirty; /* set to true if something changed */
  
  texture * rendered;
};

#endif