#ifndef texture_h
#define texture_h

#include <string>
#include "gear2d.h"


using namespace gear2d;

struct SDL_Texture;

struct texture {
    texture();
    ~texture();

    std::string id;
    SDL_Texture * raw;                                      // raw texture to render
    link<float> x, y, z;                                    // this texture position
    link<int> w, h;                                         // texture size
    link<int> bind;
    link<float> objx, objy, objz;
    link<float> rotation;
    link<float> alpha;
    struct {
      link<int> x;
      link<int> y;
      link<int> w;
      link<int> h;
    } clip;
    
    bool bindclipw;
    bool bindcliph;
    link<bool> render;
};

#endif
