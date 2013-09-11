#include "texture.h"
#include "SDL.h"

texture::texture(const std::string & id, SDL_Texture * raw)
  : raw(raw)
  , w(0)
  , h(0)
  , x(0)
  , y(0)
  , z(0)
  , id(id) {
    if (raw == nullptr) return;
    SDL_QueryTexture(raw, NULL, NULL, &w, &h);
}

texture::texture(const texture & other) 
  : raw(other.raw)
  , w(other.w)
  , h(other.h)
  , x(other.x)
  , y(other.y)
  , z(other.z)
  , id(other.id) {
}

bool texture::operator<(const texture & other) {
  return (z < other.z);
}

texture & texture::operator=(const texture & other) {
  raw = other.raw;
  w = other.w;
  h = other.h;
  x = other.x;
  y = other.y;
  z = other.z;
  id = other.id;
  return *this;
}


texture::~texture() {

}
