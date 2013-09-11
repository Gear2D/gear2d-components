#ifndef rendererbase_h
#define rendererbase_h

#include <set>
#include <map>
#include <string>

#include "texture.h"

using namespace std;

class renderer2;
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Window;

typedef pair<int, texture*> zorder;

class renderbase {
  public:
    static void initialize(int width = 640, int height = 480, bool fullscreen = false);
    static texture load(const string & id, const string & filename);
    static void add(renderer2 * renderer);
    static void remove(renderer2 * renderer);
    static int update(); /* may render. returns number of surfaces rendered */
    
  private:
    static int render();
    
  private:
    static set<renderer2*> renderers;
    static map<string, SDL_Texture *> rawtextures;
    static set<zorder> renderorder;
    static SDL_Renderer * sdlrenderer;
    static SDL_Window * sdlwindow;                             // TODO: add support for multiple windows
    static int width, height;
    
    static int votesleft;
    static int starttime;
    static bool error;
    
  public:
    static bool initialized;    
};
#endif
