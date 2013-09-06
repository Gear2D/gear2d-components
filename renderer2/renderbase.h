#ifndef rendererbase_h
#define rendererbase_h

#include <set>
#include <map>
#include <string>

using namespace std;

class render2;
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Window;

class renderbase {
  public:
    static initialize();
    static texture load(const string & filename);
    static void add(renderer2 * renderer);
    static void remove(renderer2 * renderer);
    static void update();
    
  private:
    static set<renderer2*> renderers;
    static map<string, SDL_Texture *> rawtextures;
    static set<zorder> renderorder;
    static SDL_Renderer * renderer;
    static SDL_Window * window;                             // TODO: add support for multiple windows
    static bool initialized;
};

typedef pair<int, texture*> zorder;

class texture {
  public:
    texture(SDL_Texture * raw);
    texture(texture && other);
    ~texture();
    
  public:
    float x, y, z;                                          // this texture position
    float w, h;                                             // texture size
    
  private:
    SDL_Texture * raw;                                      // raw texture to render
};

# endif