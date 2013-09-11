#ifndef texture_h
#define texture_h

#include <string>
#include "gear2d.h"

using namespace gear2d;

struct SDL_Texture;

class texture {
  public:
    texture(const std::string & id = std::string(), SDL_Texture * raw = nullptr);
    texture(const texture & other);
    texture & operator=(const texture & other);
    bool operator<(const texture & other);
    ~texture();
    
  public:
    link<float> x, y, z;                                    // this texture position
    link<int> w, h;                                         // texture size
    std::string id;                                         // surface id
    
  private:
    SDL_Texture * raw;                                      // raw texture to render
};

#endif