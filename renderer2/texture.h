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
    gear2d::link<float> x, y, z;                            // this texture position
    gear2d::link<int> w, h;                                 // texture size
    gear2d::link<int> bind;
    gear2d::link<bool> fixed;
    gear2d::link<float> objx, objy, objz;
    gear2d::link<float> rotation;
    gear2d::link<float> alpha;
    struct {
      gear2d::link<int> x;
      gear2d::link<int> y;
      gear2d::link<int> w;
      gear2d::link<int> h;
    } clip;
    
    bool bindclipw;
    bool bindcliph;
    int oldz;
    gear2d::link<bool> render;
};

#endif
