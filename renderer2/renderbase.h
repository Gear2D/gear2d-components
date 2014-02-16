#ifndef rendererbase_h
#define rendererbase_h

#include <set>
#include <map>
#include <string>

#include "texture.h"
#include "text.h"

using namespace std;

class renderer2;
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Window;

typedef pair<int, texture*> zorder;

class renderbase {
  public:
    static void initialize(int width = 0, int height = 0, bool fullscreen = false, const string & filepath = "");
    static SDL_Texture * load(const string & filename);
    static SDL_Texture * fromtext(text & source);
    static int querywidth(SDL_Texture * texture);
    static int queryheight(SDL_Texture * texture);
    static void add(renderer2 * renderer);
    static void remove(renderer2 * renderer);
    static int update(); /* may render. returns number of surfaces rendered */
    static int updatecamera(float x, float y, float w, float h);
    
  private:
    static int render();
    
  private:
    static map<string, SDL_Texture *> rawtextures;
    static SDL_Renderer * sdlrenderer;
    static SDL_Window * sdlwindow;                             // TODO: add support for multiple windows
    static int width, height;
    
    struct camerastruct {
      float x, y, w, h;
    };
    
    static camerastruct camera;
    
    static int votesleft;
    static int starttime;
    static bool error;
    static set<renderer2*> renderers;
    static SDL_Texture * numberstex;
    
  public:
    static bool initialized;    
    static set<zorder> renderorder;
    static std::string imgpath;
    static bool showfps;
    static int screenwidth;
    static int screenheight;
};
#endif
