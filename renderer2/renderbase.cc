#include "renderbase.h"
#include "renderer2.h"

#include "SDL.h"
#include "SDL_image.h"

//set<renderer2*> renderbase::renderers;
map<string, SDL_Texture *> renderbase::rawtextures;
bool renderbase::initialized = false;
bool renderbase::error = false;
int renderbase::votesleft = 0;
int renderbase::starttime = 0;
std::string renderbase::imgpath = "";
std::set<zorder> renderbase::renderorder;
SDL_Renderer * renderbase::sdlrenderer;
SDL_Window * renderbase::sdlwindow;
std::set<renderer2*> renderbase::renderers;
bool renderbase::showfps = false;
SDL_Texture * renderbase::numberstex = nullptr;



unsigned char numbers_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x06,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x81, 0xd4, 0xcd, 0x0a, 0x00, 0x00, 0x00,
  0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0b, 0x13, 0x00, 0x00, 0x0b,
  0x13, 0x01, 0x00, 0x9a, 0x9c, 0x18, 0x00, 0x00, 0x00, 0x07, 0x74, 0x49,
  0x4d, 0x45, 0x07, 0xdd, 0x0a, 0x17, 0x11, 0x14, 0x2c, 0xe4, 0x98, 0xa0,
  0x27, 0x00, 0x00, 0x00, 0x1d, 0x69, 0x54, 0x58, 0x74, 0x43, 0x6f, 0x6d,
  0x6d, 0x65, 0x6e, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x72, 0x65,
  0x61, 0x74, 0x65, 0x64, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x47, 0x49,
  0x4d, 0x50, 0x64, 0x2e, 0x65, 0x07, 0x00, 0x00, 0x00, 0x75, 0x49, 0x44,
  0x41, 0x54, 0x38, 0xcb, 0x95, 0x93, 0x51, 0x0e, 0x80, 0x30, 0x08, 0x43,
  0x1f, 0xde, 0xff, 0xce, 0xf5, 0xc7, 0x68, 0x24, 0x03, 0x0a, 0x3f, 0xcb,
  0x1a, 0xa0, 0x5d, 0x07, 0xc1, 0x17, 0x02, 0x82, 0x7f, 0x64, 0x4c, 0xcf,
  0x19, 0xe9, 0x4e, 0xc2, 0x73, 0x6d, 0x95, 0xe7, 0xf6, 0x53, 0xd1, 0x3f,
  0x63, 0x2f, 0xa8, 0x84, 0xe9, 0x90, 0x73, 0x22, 0xdc, 0xd4, 0x76, 0x98,
  0xa3, 0xa9, 0xd4, 0xd2, 0x11, 0xba, 0x02, 0x64, 0x9a, 0x32, 0x61, 0x9d,
  0x8e, 0xd1, 0xd4, 0xcd, 0x43, 0x64, 0x3a, 0xe8, 0x88, 0x74, 0x79, 0x35,
  0xfc, 0xba, 0x00, 0xae, 0x61, 0x47, 0x30, 0x76, 0x88, 0xc3, 0x5c, 0xc7,
  0x30, 0xfb, 0x2c, 0x79, 0x18, 0x78, 0x46, 0xa7, 0x2b, 0x07, 0xba, 0x7d,
  0x70, 0xc7, 0xa3, 0xc3, 0xd8, 0xd6, 0xde, 0xa9, 0x06, 0x54, 0xc1, 0xc6,
  0x92, 0x23, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
  0x42, 0x60, 0x82
};
unsigned int numbers_png_len = 255;


void renderbase::add(renderer2 * renderer) {
  if (!initialized) initialize();
  renderers.insert(renderer);
}

void renderbase::remove(renderer2 * renderer) {
  if (!initialized) initialize();
  renderers.erase(renderer);
}

int renderbase::queryheight(SDL_Texture *texture) {
  int h;
  SDL_QueryTexture(texture, NULL, NULL, NULL, &h);
  return h;
}

int renderbase::querywidth(SDL_Texture *texture) {
  int w;
  SDL_QueryTexture(texture, NULL, NULL, &w, NULL);
  return w;
}

SDL_Texture * renderbase::load(const string & filename) {
  moderr("render2");
  std::string filepath = imgpath + filename;
  if (!initialized) initialize();
  SDL_Texture * tex = nullptr;
  auto i = rawtextures.find(filename);
  if (i == rawtextures.end()) {                             // texture not there.
    tex = IMG_LoadTexture(sdlrenderer, filepath.c_str());
    if (tex == nullptr) {
      trace("Could not load texture", filepath, ":", SDL_GetError(), log::error);
      // Let it go on.
    }
  }
  return tex;
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
  modinfo("renderbase");

  static int timemark = 0;
  static float dt = 0;
  dt = dt*0.75f + 0.25f*(SDL_GetTicks() - timemark);
  timemark = SDL_GetTicks();


  SDL_SetRenderDrawColor(sdlrenderer, 0xdc, 0x9c, 0x76, 255);
  SDL_RenderClear(sdlrenderer);
  
  for (zorder zpair : renderorder) {
    texture & t = *(zpair.second);
    SDL_Rect dest;
    if (!t.bind) {
      dest.x = t.x; dest.y = t.y; dest.w = t.w; dest.h = t.h;
    } else {
      dest.x = t.objx; dest.y = t.objy; dest.w = t.w; dest.h = t.h;
      //dest = { t.objx, t.objy, t.w, t.h };
    }
    
    SDL_Rect src;
    src.x = t.clip.x; src.y = t.clip.y; src.w = t.clip.w; src.h = t.clip.h;
    dest.w = src.w;
    dest.h = src.h;
    
    //SDL_Point center;
    //center.x = (dest.x + dest.w)/2.0f;
    //center.y = (dest.y + dest.y)/2.0f;
    
    int alpha = t.alpha * 255;
    SDL_SetTextureBlendMode(t.raw, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(t.raw, alpha);
    trace(t.id, "clip:", src.x, src.y, dest.w, dest.h);
    SDL_RenderCopyEx(sdlrenderer, t.raw, &src, &dest, t.rotation, NULL, SDL_FLIP_NONE);
    total++;
  }

  if (showfps) {
    SDL_Rect src = {0, 0, 5, 6};
    SDL_Rect dst = {0, 0, 5, 6};
    string stringdt = std::to_string((int)dt);

    for (char c : stringdt) {
      src.x = (c - '0') * 5;
      SDL_RenderCopy(sdlrenderer, numberstex, &src, &dst);
      dst.x += 5;
    }
  }

  SDL_RenderPresent(sdlrenderer);
  return total;
}


void renderbase::initialize(int width,  int height, bool fullscreen,const std::string & filepath) {
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

  imgpath = filepath;
  trim(imgpath);
  if (!imgpath.empty()) {
    if (imgpath[imgpath.size()-1] != '/') imgpath += '/';
  }

  /* load numbers texture */
  SDL_RWops * numbersrw = SDL_RWFromConstMem(numbers_png, numbers_png_len);
  numberstex = IMG_LoadTexture_RW(sdlrenderer, numbersrw, 1);

  trace("Finished renderer initialization", log::info);
  initialized = true;
}
