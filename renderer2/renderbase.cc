#include "renderbase.h"
#include "renderer2.h"
#include "text.h"

#include "SDL.h"
#include "SDL_image.h"

#include <unordered_map>

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
int renderbase::screenwidth;
int renderbase::screenheight;
renderbase::camerastruct renderbase::camera;



unsigned char numbers_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x07,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x54, 0x41, 0x2e, 0x1c, 0x00, 0x00, 0x00,
  0x06, 0x62, 0x4b, 0x47, 0x44, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xa0,
  0xbd, 0xa7, 0x93, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00,
  0x00, 0x0b, 0x13, 0x00, 0x00, 0x0b, 0x13, 0x01, 0x00, 0x9a, 0x9c, 0x18,
  0x00, 0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45, 0x07, 0xde, 0x01, 0x0b,
  0x02, 0x04, 0x0b, 0xfe, 0xbb, 0xb1, 0xac, 0x00, 0x00, 0x00, 0x1d, 0x69,
  0x54, 0x58, 0x74, 0x43, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x64, 0x20, 0x77,
  0x69, 0x74, 0x68, 0x20, 0x47, 0x49, 0x4d, 0x50, 0x64, 0x2e, 0x65, 0x07,
  0x00, 0x00, 0x00, 0xd6, 0x49, 0x44, 0x41, 0x54, 0x38, 0xcb, 0x9d, 0x54,
  0xcb, 0x0e, 0xc3, 0x30, 0x0c, 0x82, 0xa9, 0xff, 0xff, 0xcb, 0xec, 0x30,
  0x5b, 0x62, 0x16, 0xce, 0xda, 0xf9, 0xd4, 0x12, 0x07, 0x3f, 0x03, 0x51,
  0x26, 0x49, 0x00, 0x40, 0x92, 0xf5, 0x0f, 0x00, 0xc2, 0xb7, 0xb1, 0xdd,
  0x03, 0x3e, 0x31, 0x90, 0x64, 0xf1, 0xa4, 0x3b, 0xc9, 0x1e, 0xf1, 0x1f,
  0xce, 0x22, 0x0f, 0xad, 0x38, 0xd5, 0x27, 0x24, 0x79, 0xa2, 0x5f, 0xb8,
  0x93, 0x0f, 0x1c, 0x8e, 0x0d, 0x7c, 0xbd, 0x13, 0xfc, 0xa3, 0x6f, 0xe5,
  0x23, 0xf7, 0xdf, 0x62, 0x3b, 0x96, 0xf0, 0xab, 0x89, 0x3c, 0x89, 0x9a,
  0x36, 0x2d, 0xa0, 0x9f, 0x21, 0xf8, 0xc7, 0x04, 0x7c, 0x4b, 0xfc, 0xee,
  0x29, 0xe9, 0xc9, 0xef, 0x3e, 0x5b, 0x73, 0xb7, 0x7c, 0xd2, 0xf7, 0x6b,
  0xdd, 0xad, 0x0f, 0x21, 0x53, 0x72, 0x0b, 0x21, 0x1b, 0x9b, 0x09, 0x7b,
  0xc3, 0x52, 0x1c, 0x9b, 0x82, 0x12, 0x5e, 0x4d, 0xa3, 0xc7, 0x98, 0x0d,
  0x49, 0x71, 0x1d, 0x6f, 0xbb, 0xf0, 0xa7, 0x05, 0x72, 0x6d, 0xcd, 0xb8,
  0xb1, 0xf6, 0xeb, 0x54, 0x27, 0x1e, 0x34, 0x42, 0xcb, 0x54, 0xe9, 0x3d,
  0xec, 0xb8, 0xd7, 0x7c, 0x1f, 0xfd, 0x66, 0x4e, 0x45, 0xce, 0x15, 0x3f,
  0x15, 0xd3, 0x7a, 0x60, 0x4f, 0xe5, 0x67, 0xf1, 0x1e, 0x63, 0x2b, 0x76,
  0x6e, 0x5c, 0xd2, 0x87, 0x88, 0x6f, 0x2a, 0xbd, 0x28, 0x35, 0x6f, 0xaa,
  0x2d, 0x82, 0xe2, 0xdf, 0x51, 0xea, 0xa7, 0x6a, 0x7f, 0x52, 0xf5, 0xa8,
  0xd2, 0x6f, 0x40, 0x33, 0x05, 0x1d, 0x6c, 0x72, 0xe9, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
  };
unsigned int numbers_png_len = 370;


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
  int err = SDL_QueryTexture(texture, NULL, NULL, NULL, &h);
  if (err) {
    moderr("renderer2");
    trace("Error querying width of a texture:", SDL_GetError());
  }
  return h;
}

int renderbase::querywidth(SDL_Texture *texture) {
  int w;
  int err = SDL_QueryTexture(texture, NULL, NULL, &w, NULL);
  if (err) {
    moderr("renderer2");
    trace("Error querying width of a texture:", SDL_GetError());
  }
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
      trace("Could not load texture", filepath, ":", SDL_GetError());
      // Let it go on.
    }
    rawtextures[filename] = tex;
  } else {
    tex = i->second;
  }
  return tex;
}

SDL_Texture * renderbase::fromtext(text & source) {
  static std::map<std::string, TTF_Font *> fonts;
  static char buf[5];
  sprintf(buf, "%4.4d", (int)source.font.size);
  string fontname = (string)source.font.face;
  string fontid = fontname + buf;
  TTF_Font * f;
  SDL_Texture * raw; SDL_Surface * tmpsurface;
  auto it = fonts.find(fontid);
  if (it == fonts.end()) {
    f = TTF_OpenFont(fontname.c_str(), source.font.size);
    if (f == nullptr) {
      moderr("renderer2");
      trace("Cannot open font", fontname , "@", buf, "Error:", TTF_GetError());
      return nullptr;
    }
    
    fonts.insert(it, { fontid, f });
  } else {
    f = it->second;
  }
  
  SDL_Color color = {
    source.color.r * 255, 
    source.color.g * 255, 
    source.color.b * 255
  };
  
  string & style = (string&)source.style;
  int styleflags = TTF_STYLE_NORMAL
    | source.bold ? TTF_STYLE_BOLD : 0
    | source.italic ? TTF_STYLE_ITALIC : 0
    | source.underline ? TTF_STYLE_UNDERLINE : 0
    | source.strikethrough ? TTF_STYLE_STRIKETHROUGH : 0
    ;
    
  TTF_SetFontStyle(f, styleflags);
  
  TTF_SetFontHinting(f, 3);
  
  if (source.blended) {
    tmpsurface = TTF_RenderText_Blended(f, ((std::string)source.text).c_str(), color);
  } else {
    tmpsurface = TTF_RenderText_Solid(f, ((std::string)source.text).c_str(), color);
  }
  raw = SDL_CreateTextureFromSurface(sdlrenderer, tmpsurface);
  if (raw == nullptr) {
    moderr("renderer2");
    trace("Could not create text texture:", SDL_GetError());
  }
  
  return raw;
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
  static int timemark = 0;
  static float dt = 0;
  dt = dt*0.75f + 0.25f*(SDL_GetTicks() - timemark);
  timemark = SDL_GetTicks();


  SDL_RenderClear(sdlrenderer);
  
  for (zorder zpair : renderorder) {
    texture & t = *(zpair.second);
    if (!t.render) continue;
    SDL_Rect dest;
    dest.x = t.x; dest.y = t.y; dest.w = t.w; dest.h = t.h;
    if (t.bind) {
      dest.x += t.objx;
      dest.y += t.objy;
    }  
    
    SDL_Rect src;
    src.x = t.clip.x; src.y = t.clip.y; src.w = t.clip.w; src.h = t.clip.h;
    if (t.bindclipw) dest.w = src.w;
    if (t.bindcliph) dest.h = src.h;
    
    //SDL_Point center;
    //center.x = (dest.x + dest.w)/2.0f;
    //center.y = (dest.y + dest.y)/2.0f;
    
    int alpha = t.alpha * 255;
    SDL_SetTextureBlendMode(t.raw, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(t.raw, alpha);
    SDL_RenderCopyEx(sdlrenderer, t.raw, &src, &dest, t.rotation, NULL, SDL_FLIP_NONE);
    total++;
  }

  showfps = true;
  if (showfps) {
    SDL_Rect src = {0, 0, 6, 7};
    SDL_Rect dst = {0, 0, 6*3, 7*3};
    char buf[5];
    sprintf(buf, "%.4d", (int)dt);
    string stringdt = buf;

    for (char c : stringdt) {
      src.x = (c - '0') * 6;
      SDL_RenderCopy(sdlrenderer, numberstex, &src, &dst);
      dst.x += dst.w * 2;
      
    }
  }

  SDL_RenderPresent(sdlrenderer);
  return total;
}


void renderbase::initialize(int width,  int height, bool fullscreen,const std::string & filepath) {
  moderr("render2");
  int flags = 0;
  if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  error = 0;
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
  
  if (!TTF_WasInit()) {
    error = TTF_Init();
    if (error) {
      trace("Cannot initialize SDL_ttf:", TTF_GetError());
      return;
    }
  }
  
  SDL_DisplayMode displaymode;
  SDL_GetCurrentDisplayMode(0, &displaymode);

  /* the value passed in width and height defines the logical size, 
   * so we use it to define logical size later  */
  if (width == 0) width = displaymode.w;
  if (height == 0) height = displaymode.h;
  screenwidth = width;
  screenheight = height;
  camera = { 0, 0, width, height };
  
  error = SDL_CreateWindowAndRenderer(screenwidth, screenheight, flags, &sdlwindow, &sdlrenderer) != 0;
  if (error) {
    trace("Could not create window or renderer:", SDL_GetError());
    return;
  }
  
  SDL_SetRenderDrawColor(sdlrenderer, 0, 0, 0, 0);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(sdlrenderer, camera.w, camera.h);
  
  // TODO: make camera.x and y affect position of stuff

  
  imgpath = filepath;
  trim(imgpath);
  if (!imgpath.empty()) {
    if (imgpath[imgpath.size()-1] != '/') imgpath += '/';
  }

  /* load numbers texture */
  SDL_RWops * numbersrw = SDL_RWFromConstMem(numbers_png, numbers_png_len);
  numberstex = IMG_LoadTexture_RW(sdlrenderer, numbersrw, 1);

  trace.i("Finished renderer initialization");
  initialized = true;
}

int renderbase::updatecamera(float x, float y, float w, float h) {
  camera = { x, y, w, h };
  SDL_RenderSetLogicalSize(sdlrenderer, camera.w, camera.h);
}

