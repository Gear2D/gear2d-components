#include "texture.h"
#include "SDL.h"

texture::texture(SDL_Texture * raw)
  : raw(raw)
  , w(0)
  , h(0)
  , x(0)
  , y(0)
  , z(0) {
    if (raw == nullptr) return;
    SDL_QueryTexture(raw, NULL, NULL, w, h);
}

texture::texture(texture && other) 
  : raw(other.raw)
  , w(other.w)
  , h(other.h)
  , x(other.x)
  , y(other.y)
  , z(other.z) {
  other.raw = nullptr;
}

texture::~texture() {

}
