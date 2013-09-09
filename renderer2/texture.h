#ifndef texture_h
#define texture_h

struct SDL_Texture;

class texture {
  public:
    texture(SDL_Texture * raw);
    texture(texture && other);
    ~texture();
    
  public:
    float x, y, z;                                          // this texture position
    int w, h;                                               // texture size
    
  private:
    SDL_Texture * raw;                                      // raw texture to render
};

#endif