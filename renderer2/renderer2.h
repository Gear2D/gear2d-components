#ifndef renderer2_h
#define renderer2_h

#include "gear2d.h"
#include <string>
#include <map>
#include "texture.h"
#include "text.h"

using namespace gear2d;

class renderer2 : public component::base {
  public:
    renderer2();
    virtual std::string family();
    virtual std::string type();
    virtual std::string depends();
    virtual void setup(object::signature & sig);
    virtual void update(timediff dt);
    virtual ~renderer2();
    virtual void destroyed();

  private /* handlers */:
     void surfaceschanged(std::string pid, gear2d::component::base * lastwrite, gear2d::object * owner);
     void zchanged(std::string pid, gear2d::component::base * lastwrite, gear2d::object * owner);
     void textstylechanged(std::string pid, gear2d::component::base * lastwrite, gear2d::object * owner);
     void textchanged(std::string pid, gear2d::component::base * lastwrite, gear2d::object * owner);
     
  private:
    void loadsurfaces(const std::string & surfacelist);
    void loadtexts(const std::string & textlist);
    texture * wiretexture(const std::string & id, SDL_Texture * raw); /* loads a single texture */
    
  private:
    gear2d::link<std::string> surfacelist;
    gear2d::link<std::string> textlist;
    std::map<std::string, texture *> textures;
    std::map<std::string, text *> texts;
    sigparser sig;
};

#endif
