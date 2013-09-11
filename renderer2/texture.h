#ifndef texture_h
#define texture_h

#include <string>
#include "gear2d.h"

using namespace gear2d;

struct SDL_Texture;

struct texture {
    texture();
    texture(const texture & other);
    texture & operator=(const texture & other);
    bool operator<(const texture & other);
    ~texture();

    std::string id;
    SDL_Texture * raw;                                      // raw texture to render
    link<float> x, y, z;                                    // this texture position
    link<int> w, h;                                         // texture size
    link<int> bind;
    link<float> objx, objy, objz;
};

#endif
