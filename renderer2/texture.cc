#include "texture.h"
#include "SDL.h"

texture::texture()
  { }

texture::texture(const texture & other) 
  : raw(other.raw)
  , w(other.w)
  , h(other.h)
  , x(other.x)
  , y(other.y)
  , z(other.z)
  , id(other.id)
  , bind(other.bind)
  , rotation(other.rotation)
  , alpha(other.alpha)
{
  modinfo("renderer2");
  trace("Making a copy to", this, other.id, "from", &other, "just for you :(");
}

bool texture::operator<(const texture & other) {
  return (z < other.z);
}

texture & texture::operator=(const texture & other) {
  modinfo("renderer2");
  trace("Making a copy to", this, other.id, "from", &other, "just for you :(");
  raw = other.raw;
  w = other.w;
  h = other.h;
  x = other.x;
  y = other.y;
  z = other.z;
  id = other.id;
  bind = other.bind;
  
  
  return *this;
}


texture::~texture() {

}
