#include "renderbase.h"
#include "renderer2.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

//set<renderer2*> renderbase::renderers;
map<string, SDL_Texture *> renderbase::rawtextures;
bool renderbase::initialized = false;
bool renderbase::error = false;
int renderbase::votesleft = 0;
int renderbase::starttime = 0;
std::set<zorder> renderbase::renderorder;
SDL_Renderer * renderbase::sdlrenderer;
SDL_Window * renderbase::sdlwindow;


void renderbase::add(texture & t) {
  if (!initialized) initialize();
  renderorder.emplace({t.z, &t});
}

void renderbase::remove(renderer2 * renderer) {
  if (!initialized) initialize();
  renderers.erase(renderer);
}

texture renderbase::load(const string & id, const string & filename) {
  moderr("render2");
  if (!initialized) initialize();
  SDL_Texture * tex = nullptr;
  auto i = rawtextures.find(filename);
  if (i == rawtextures.end()) {                             // texture not there.
    tex = IMG_LoadTexture(sdlrenderer, filename);
    if (tex == nullptr) {
      trace("Could not load texture", filename, ":", SDL_GetError());
      // Let it go on.
    }
  }
  return texture(id, tex);
}

int renderbase::update() {
  /* decrease votes */
  if (votesleft > 0) {
    if ((SDL_GetTicks() - starttime) < 30) {
      votesleft--;
      return 0;
    }
  }

  /* render! */
  int total = renderbase::render();
  starttime = SDL_GetTicks();
  votesleft = renderers.size();
  return total;
}

int renderbase::render() {
  int total = 0;
  
  for (zorder zpair : renderorder) {
    texture & t = *(zpair.second);
    SDL_Rect dest = { t.x, t.y, t.w, t.h };
    SDL_RenderCopyEx(sdlrenderer, t.raw, NULL, &dest, 0.0f, NULL, SDL_FLIP_NONE);
    total++;
  }

  return total;
}


void renderbase::initialize(int width,  int height, bool fullscreen) {
  moderr("render2");
  int flags = 0;
  if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
  error = 0;
  trace("Initializing renderer2 width =", width, "height =", height, "fullscreen =", fullscreen, log::info);
  if (!SDL_WasInit(SDL_INIT_EVERYTHING)) {
    trace("SDL was not initialized at all. Initializing SDL.");
    error = SDL_Init(SDL_INIT_VIDEO) != 0;
    if (error) {
      trace("Cannot initialize SDL:",  SDL_GetError());
      return;
    }
  } else {
    error = SDL_InitSubSystem(SDL_INIT_VIDEO) != 0;
    if (error) {
      trace("Could not initialize SDL video subsystem:", SDL_GetError());
      return;
    }
  }
  error = SDL_CreateWindowAndRenderer(width, height, flags, &sdlwindow, &sdlrenderer) != 0;
  if (error) {
    trace("Could not create window or renderer:", SDL_GetError());
    return;
  }
  trace("Finished renderer initialization", log::info);
  initialized = true;
}
