#include "renderbase.h"
#include "renderer2.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

set<renderer2*> renderbase::renderers;
map<string, SDL_Texture *> renderbase::rawtextures;
bool renderbase::initialized = false;


void renderbase::add(renderer2 * renderer) {
  if (!initialized) initialize();
  renderers.insert(renderer);
}

void renderbase::remove(renderer2 * renderer) {
  if (!initialized) initialize();
  renderers.erase(renderer);
}

texture renderbase::load(const string & filename) {
  if (!initialized) initialize();
  auto i = rawtextures.find(filename);
  SDL_Texture * texture = nullptr;
  if (i == rawtextures.end()) {
  }
}

renderbase::initialize() {
  SDL_CreateWindowAndRenderer();
}
