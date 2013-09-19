#ifndef renderer2_h
#define renderer2_h

#include "gear2d.h"
#include <string>
#include <map>
#include "texture.h"

using namespace gear2d;

class renderer2 : public component::base {
  public:
    renderer2();
    virtual std::string family();
    virtual std::string type();
    virtual void setup(object::signature & sig);
    virtual void update(timediff dt);
    virtual ~renderer2();
    
  private:
    link<std::string> surfacelist;
    std::map<std::string, texture> textures;
};

#endif