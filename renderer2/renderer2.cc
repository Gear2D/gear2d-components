#include "gear2d.h"
#include "log.h"

#include "renderer2.h"
#include "renderbase.h"
#include "texture.h"

#include <string>

using namespace gear2d;

class sigparser {
  private:
    object::signature & sig;
    component::base & com;

  public:
    sigparser(object::signature & sig, component::base * com)
      : sig(sig)
      , com(*com)
    { }

    template <typename datatype>
    link<datatype> init(std::string pid, const datatype & def = datatype()) {
      modinfo("renderer2");
      trace("Initializing from signature (as something)", pid);
      return com.fetch<datatype>(pid, def);
    }

    link<std::string> init(std::string pid, std::string def = std::string("")) {
      modinfo("renderer2");
      trace("Initializing from signature (as string)", pid);
      return com.fetch<std::string>(pid, def);
    }
};

renderer2::renderer2() {

}

renderer2::~renderer2() { 

}

std::string renderer2::family() { return "renderer"; }
std::string renderer2::type() { return "renderer2"; }

void renderer2::setup(object::signature & s) {
  modinfo("renderer2");
  sigparser sig(s, this);
  
  if (!renderbase::initialized) {
    int width; int height; bool fullscreen;
    width = eval<int>(s["renderer.w"], 640);
    height = eval<int>(s["renderer.h"], 480);
    fullscreen = eval<int>(s["renderer.fullscreen"], 0) == 1;
    renderbase::initialize(width, height, fullscreen);
  }

  renderbase::add(this);
  
  surfacelist = sig.init("renderer.surfaces");
  size_t pos = string::npos;
  
  /* iterate through surface list */
  for (std::string surfdef : split(surfacelist, ' ')) {
    pos = surfdef.find('=');
    
    // skip erroneous lines
    if (pos == string::npos) {
      trace("Skipping", surfdef, "as it is not a valid surface definition. Must be id=filename.",  log::error);
      continue;
    }
    
    /* get id=name */
    string id = surfdef.substr(0,  pos);
    string filename = surfdef.substr(pos+1);
    size_t s = id.size() + 1;
    string p(id + ". ");
    
    /* wire texture parameters with links */
    trace("Loading", filename, "as", id);
    SDL_Texture * raw = renderbase::load(filename);
    texture & t = textures[id];
    t.id = id;
    p[s] = 'x'; t.x = sig.init(p, .0f);
    p[s] = 'y'; t.y = sig.init(p, .0f);
    p[s] = 'z'; t.z = sig.init(p, .0f);
    p[s] = 'w'; t.w = sig.init(p, renderbase::querywidth(raw));
    p[s] = 'h'; t.h = sig.init(p, renderbase::queryheight(raw));
    t.bind = sig.init(id + ".bind", 0);
    t.objx = sig.init("x", .0f);
    t.objy = sig.init("y", .0f);
    t.objz = sig.init("z", .0f);
    t.raw = raw;

    renderbase::renderorder.insert(make_pair(t.z, &t));
  }
}

void renderer2::update(timediff dt) {
  renderbase::update();
}

g2dcomponent(renderer2)
